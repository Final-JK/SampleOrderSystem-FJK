#pragma once
#include <gmock/gmock.h>
#include "repositories/IRepository.h"
#include "models/Order.h"

class MockOrderRepository : public IRepository<Order> {
public:
    MOCK_METHOD(bool, save, (Order&), (override));
    MOCK_METHOD(std::optional<Order>, findById, (const std::string&), (const, override));
    MOCK_METHOD(std::vector<Order>,   findAll,  (), (const, override));
    MOCK_METHOD(bool, remove, (const std::string&), (override));
    MOCK_METHOD(bool, load,  (), (override));
    MOCK_METHOD(bool, flush, (), (override));
};
