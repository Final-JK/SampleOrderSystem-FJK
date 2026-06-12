#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "mocks/MockProductionRepository.h"
#include "mocks/MockOrderRepository.h"
#include "mocks/MockSampleRepository.h"
#include "controllers/ProductionController.h"

using ::testing::Return;
using ::testing::_;

class ProductionControllerTest : public ::testing::Test {
protected:
    MockProductionRepository mockProductionRepo;
    MockOrderRepository      mockOrderRepo;
    MockSampleRepository     mockSampleRepo;
    ProductionController ctrl{&mockProductionRepo, &mockOrderRepo, &mockSampleRepo};

    ProductionJob makeRunningJob() {
        ProductionJob j;
        j.m_jobId = "JOB-0001"; j.m_orderId = "ORD-0001"; j.m_sampleId = "S-001";
        j.m_shortage = 200; j.m_actualProduction = 230;
        j.m_status = JobStatus::RUNNING;
        return j;
    }
    Order makeProducingOrder(int qty) {
        Order o; o.m_orderId = "ORD-0001"; o.m_sampleId = "S-001";
        o.m_quantity = qty; o.m_status = OrderStatus::PRODUCING;
        return o;
    }
    Sample makeSample(int stock) {
        Sample s; s.m_sampleId = "S-001"; s.m_stock = stock;
        return s;
    }
};

// completeJob: COMPLETED + 재고 반영 + 주문 CONFIRMED
TEST_F(ProductionControllerTest, CompleteJob_UpdatesAllEntities) {
    auto job    = makeRunningJob();      // actualProduction=230
    auto order  = makeProducingOrder(50); // quantity=50
    auto sample = makeSample(10);

    EXPECT_CALL(mockProductionRepo, findById("JOB-0001")).WillOnce(Return(job));
    EXPECT_CALL(mockOrderRepo,      findById("ORD-0001")).WillOnce(Return(order));
    EXPECT_CALL(mockSampleRepo,     findById("S-001"))   .WillOnce(Return(sample));

    ProductionJob savedJob; Order savedOrder; Sample savedSample;
    EXPECT_CALL(mockProductionRepo, save(_)).WillOnce([&](ProductionJob& j) { savedJob    = j; return true; });
    EXPECT_CALL(mockSampleRepo,     save(_)).WillOnce([&](Sample&        s) { savedSample = s; return true; });
    EXPECT_CALL(mockOrderRepo,      save(_)).WillOnce([&](Order&         o) { savedOrder  = o; return true; });

    EXPECT_TRUE(ctrl.completeJob("JOB-0001"));
    EXPECT_EQ(savedJob.m_status,    JobStatus::COMPLETED);
    EXPECT_EQ(savedOrder.m_status,  OrderStatus::CONFIRMED);
    EXPECT_EQ(savedSample.m_stock,  10 + 230 - 50);  // stock += actualProduction - quantity
    EXPECT_FALSE(savedJob.m_completedAt.empty());
}

// completeJob: RUNNING 아닌 Job → false
TEST_F(ProductionControllerTest, CompleteJob_AlreadyCompleted_ReturnsFalse) {
    ProductionJob j = makeRunningJob();
    j.m_status = JobStatus::COMPLETED;
    EXPECT_CALL(mockProductionRepo, findById("JOB-0001")).WillOnce(Return(j));
    EXPECT_FALSE(ctrl.completeJob("JOB-0001"));
}

// completeJob: 존재하지 않는 Job → false
TEST_F(ProductionControllerTest, CompleteJob_NotFound_ReturnsFalse) {
    EXPECT_CALL(mockProductionRepo, findById("INVALID")).WillOnce(Return(std::nullopt));
    EXPECT_FALSE(ctrl.completeJob("INVALID"));
}

// getRunningJobs: RUNNING만 필터링
TEST_F(ProductionControllerTest, GetRunningJobs_ReturnsOnlyRunning) {
    ProductionJob j1 = makeRunningJob();
    ProductionJob j2 = makeRunningJob(); j2.m_jobId = "JOB-0002"; j2.m_status = JobStatus::COMPLETED;
    EXPECT_CALL(mockProductionRepo, findAll()).WillOnce(Return(std::vector<ProductionJob>{j1, j2}));
    auto result = ctrl.getRunningJobs();
    EXPECT_EQ(result.size(), 1u);
    EXPECT_EQ(result[0].m_status, JobStatus::RUNNING);
}
