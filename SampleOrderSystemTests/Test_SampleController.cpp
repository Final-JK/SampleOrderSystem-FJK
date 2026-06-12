#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "mocks/MockSampleRepository.h"
#include "controllers/SampleController.h"

using ::testing::Return;
using ::testing::_;
using ::testing::DoAll;
using ::testing::SetArgReferee;

class SampleControllerTest : public ::testing::Test {
protected:
    MockSampleRepository mockRepo;
    SampleController     ctrl{&mockRepo};

    Sample makeSample(const std::string& id, const std::string& name, int stock = 100) {
        Sample s;
        s.m_sampleId = id; s.m_name = name; s.m_stock = stock;
        s.m_avgProductionTime = 1.0; s.m_yieldRate = 0.9;
        return s;
    }
};

TEST_F(SampleControllerTest, GetAllSamples_ReturnsList) {
    std::vector<Sample> expected = { makeSample("S-001", "웨이퍼A"), makeSample("S-002", "웨이퍼B") };
    EXPECT_CALL(mockRepo, findAll()).WillOnce(Return(expected));
    auto result = ctrl.getAllSamples();
    EXPECT_EQ(result.size(), 2u);
    EXPECT_EQ(result[0].m_sampleId, "S-001");
}

TEST_F(SampleControllerTest, SearchByName_FiltersCorrectly) {
    std::vector<Sample> all = { makeSample("S-001", "실리콘 웨이퍼"), makeSample("S-002", "갈륨비소") };
    EXPECT_CALL(mockRepo, findAll()).WillOnce(Return(all));
    auto result = ctrl.searchByName("웨이퍼");
    EXPECT_EQ(result.size(), 1u);
    EXPECT_EQ(result[0].m_sampleId, "S-001");
}

TEST_F(SampleControllerTest, SearchByName_NoMatch_ReturnsEmpty) {
    std::vector<Sample> all = { makeSample("S-001", "실리콘 웨이퍼") };
    EXPECT_CALL(mockRepo, findAll()).WillOnce(Return(all));
    EXPECT_TRUE(ctrl.searchByName("없는시료").empty());
}

TEST_F(SampleControllerTest, AddSample_CallsSave) {
    EXPECT_CALL(mockRepo, save(_)).WillOnce(Return(true));
    EXPECT_TRUE(ctrl.addSample("새 시료", 1.5, 0.9, 200));
}

TEST_F(SampleControllerTest, AddSample_SaveFails_ReturnsFalse) {
    EXPECT_CALL(mockRepo, save(_)).WillOnce(Return(false));
    EXPECT_FALSE(ctrl.addSample("새 시료", 1.5, 0.9, 200));
}
