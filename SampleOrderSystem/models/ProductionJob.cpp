#include "models/ProductionJob.h"

std::string jobStatusToString(JobStatus s) {
    return s == JobStatus::RUNNING ? "RUNNING" : "COMPLETED";
}

JobStatus jobStatusFromString(const std::string& s) {
    return s == "COMPLETED" ? JobStatus::COMPLETED : JobStatus::RUNNING;
}

void to_json(nlohmann::json& j, const ProductionJob& p) {
    j = {
        {"job_id",            p.m_jobId},
        {"order_id",          p.m_orderId},
        {"sample_id",         p.m_sampleId},
        {"shortage",          p.m_shortage},
        {"actual_production", p.m_actualProduction},
        {"total_time_min",    p.m_totalTimeMins},
        {"status",            jobStatusToString(p.m_status)},
        {"created_at",        p.m_createdAt},
        {"completed_at",      p.m_completedAt}
    };
}

void from_json(const nlohmann::json& j, ProductionJob& p) {
    p.m_jobId            = j.value("job_id",            "");
    p.m_orderId          = j.value("order_id",          "");
    p.m_sampleId         = j.value("sample_id",         "");
    p.m_shortage         = j.value("shortage",          0);
    p.m_actualProduction = j.value("actual_production", 0);
    p.m_totalTimeMins    = j.value("total_time_min",    0.0);
    p.m_status           = jobStatusFromString(j.value("status", "RUNNING"));
    p.m_createdAt        = j.value("created_at",        "");
    p.m_completedAt      = j.value("completed_at",      "");
}
