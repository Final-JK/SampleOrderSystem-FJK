#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "mocks/MockOrderRepository.h"
#include "mocks/MockSampleRepository.h"
#include "mocks/MockProductionRepository.h"
#include "controllers/OrderController.h"

using ::testing::Return;
using ::testing::_;

class OrderControllerTest : public ::testing::Test {
protected:
    MockOrderRepository      mockOrderRepo;
    MockSampleRepository     mockSampleRepo;
    MockProductionRepository mockProductionRepo;
    OrderController ctrl{&mockOrderRepo, &mockSampleRepo, &mockProductionRepo};

    Sample makeStockedSample(int stock) {
        Sample s; s.m_sampleId = "S-001"; s.m_name = "웨이퍼";
        s.m_stock = stock; s.m_yieldRate = 0.9; s.m_avgProductionTime = 1.0;
        return s;
    }
    Order makeReservedOrder(int qty) {
        Order o; o.m_orderId = "ORD-0001"; o.m_sampleId = "S-001";
        o.m_quantity = qty; o.m_status = OrderStatus::RESERVED;
        return o;
    }
};

// approve: 재고 충분 → CONFIRMED + 재고 차감
TEST_F(OrderControllerTest, Approve_SufficientStock_Confirmed) {
    auto order  = makeReservedOrder(100);
    auto sample = makeStockedSample(300);

    EXPECT_CALL(mockOrderRepo,  findById("ORD-0001")).WillOnce(Return(order));
    EXPECT_CALL(mockSampleRepo, findById("S-001"))   .WillOnce(Return(sample));

    Sample savedSample;
    Order  savedOrder;
    EXPECT_CALL(mockSampleRepo, save(_)).WillOnce([&](Sample& s) { savedSample = s; return true; });
    EXPECT_CALL(mockOrderRepo,  save(_)).WillOnce([&](Order&  o) { savedOrder  = o; return true; });

    EXPECT_TRUE(ctrl.approve("ORD-0001"));
    EXPECT_EQ(savedOrder.m_status,  OrderStatus::CONFIRMED);
    EXPECT_EQ(savedSample.m_stock,  200);  // 300 - 100
}

// approve: 재고 부족 → PRODUCING + ProductionJob 생성
TEST_F(OrderControllerTest, Approve_InsufficientStock_Producing) {
    auto order  = makeReservedOrder(300);
    auto sample = makeStockedSample(100);

    EXPECT_CALL(mockOrderRepo,      findById("ORD-0001")).WillOnce(Return(order));
    EXPECT_CALL(mockSampleRepo,     findById("S-001"))   .WillOnce(Return(sample));
    EXPECT_CALL(mockProductionRepo, save(_))             .WillOnce(Return(true));

    Order savedOrder;
    EXPECT_CALL(mockOrderRepo, save(_)).WillOnce([&](Order& o) { savedOrder = o; return true; });

    EXPECT_TRUE(ctrl.approve("ORD-0001"));
    EXPECT_EQ(savedOrder.m_status, OrderStatus::PRODUCING);
}

// approve: 존재하지 않는 주문 → false
TEST_F(OrderControllerTest, Approve_NotFound_ReturnsFalse) {
    EXPECT_CALL(mockOrderRepo, findById("INVALID")).WillOnce(Return(std::nullopt));
    EXPECT_FALSE(ctrl.approve("INVALID"));
}

// reject: RESERVED → REJECTED
TEST_F(OrderControllerTest, Reject_ReservedOrder_Rejected) {
    auto order = makeReservedOrder(100);
    EXPECT_CALL(mockOrderRepo, findById("ORD-0001")).WillOnce(Return(order));
    Order savedOrder;
    EXPECT_CALL(mockOrderRepo, save(_)).WillOnce([&](Order& o) { savedOrder = o; return true; });

    EXPECT_TRUE(ctrl.reject("ORD-0001"));
    EXPECT_EQ(savedOrder.m_status, OrderStatus::REJECTED);
}

// reject: RESERVED 아닌 상태 → false
TEST_F(OrderControllerTest, Reject_NonReservedOrder_ReturnsFalse) {
    Order o = makeReservedOrder(100);
    o.m_status = OrderStatus::CONFIRMED;
    EXPECT_CALL(mockOrderRepo, findById("ORD-0001")).WillOnce(Return(o));
    EXPECT_FALSE(ctrl.reject("ORD-0001"));
}

// createOrder: 시료 없으면 false
TEST_F(OrderControllerTest, CreateOrder_SampleNotFound_ReturnsFalse) {
    EXPECT_CALL(mockSampleRepo, findById("X-999")).WillOnce(Return(std::nullopt));
    EXPECT_FALSE(ctrl.createOrder("X-999", "고객사", 100));
}

// createOrder: 정상 등록
TEST_F(OrderControllerTest, CreateOrder_Valid_ReturnsTrue) {
    Sample s; s.m_sampleId = "S-001";
    EXPECT_CALL(mockSampleRepo, findById("S-001")).WillOnce(Return(s));
    EXPECT_CALL(mockOrderRepo,  save(_))          .WillOnce(Return(true));
    EXPECT_TRUE(ctrl.createOrder("S-001", "삼성전자", 200));
}
