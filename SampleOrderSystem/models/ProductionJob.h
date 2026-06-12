#pragma once
#include <string>
#include <nlohmann/json.hpp>

enum class JobStatus { RUNNING, COMPLETED };

std::string jobStatusToString(JobStatus s);
JobStatus jobStatusFromString(const std::string& s);

struct ProductionJob {
    std::string m_jobId;
    std::string m_orderId;
    std::string m_sampleId;
    int m_shortage{0};
    int m_actualProduction{0};
    double m_totalTimeMins{0.0};
    JobStatus m_status{JobStatus::RUNNING};
    std::string m_createdAt;
    std::string m_completedAt;  // 생산 완료 시각 (RUNNING 상태면 빈 문자열)
};

void to_json(nlohmann::json& j, const ProductionJob& p);
void from_json(const nlohmann::json& j, ProductionJob& p);
