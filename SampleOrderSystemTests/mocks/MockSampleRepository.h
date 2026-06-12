#pragma once
#include <gmock/gmock.h>
#include "repositories/IRepository.h"
#include "models/Sample.h"

class MockSampleRepository : public IRepository<Sample> {
public:
    MOCK_METHOD(bool, save, (Sample&), (override));
    MOCK_METHOD(std::optional<Sample>, findById, (const std::string&), (const, override));
    MOCK_METHOD(std::vector<Sample>,   findAll,  (), (const, override));
    MOCK_METHOD(bool, remove, (const std::string&), (override));
    MOCK_METHOD(bool, load,  (), (override));
    MOCK_METHOD(bool, flush, (), (override));
};
