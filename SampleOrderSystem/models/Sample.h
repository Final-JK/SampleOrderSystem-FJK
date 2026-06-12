#pragma once
#include <string>
#include <nlohmann/json.hpp>

struct Sample {
    std::string m_sampleId;
    std::string m_name;
    double m_avgProductionTime{0.0};
    double m_yieldRate{0.0};
    int m_stock{0};
    std::string m_stockUpdatedAt;   // 재고 마지막 갱신 시각 (ISO 8601)
};

void to_json(nlohmann::json& j, const Sample& s);
void from_json(const nlohmann::json& j, Sample& s);
