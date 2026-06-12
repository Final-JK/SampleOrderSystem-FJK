#include "models/Sample.h"

void to_json(nlohmann::json& j, const Sample& s) {
    j = {
        {"sample_id",            s.m_sampleId},
        {"name",                 s.m_name},
        {"avg_production_time",  s.m_avgProductionTime},
        {"yield_rate",           s.m_yieldRate},
        {"stock",                s.m_stock},
        {"stock_updated_at",     s.m_stockUpdatedAt}
    };
}

void from_json(const nlohmann::json& j, Sample& s) {
    s.m_sampleId           = j.value("sample_id",           "");
    s.m_name               = j.value("name",                "");
    s.m_avgProductionTime  = j.value("avg_production_time", 0.0);
    s.m_yieldRate          = j.value("yield_rate",          0.0);
    s.m_stock              = j.value("stock",               0);
    s.m_stockUpdatedAt     = j.value("stock_updated_at",    "");
}
