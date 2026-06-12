#pragma once
#include <gmock/gmock.h>
#include "repositories/IRepository.h"
#include "models/ProductionJob.h"

class MockProductionRepository : public IRepository<ProductionJob> {
public:
    MOCK_METHOD(bool, save, (ProductionJob&), (override));
    MOCK_METHOD(std::optional<ProductionJob>, findById, (const std::string&), (const, override));
    MOCK_METHOD(std::vector<ProductionJob>,   findAll,  (), (const, override));
    MOCK_METHOD(bool, remove, (const std::string&), (override));
    MOCK_METHOD(bool, load,  (), (override));
    MOCK_METHOD(bool, flush, (), (override));
};
