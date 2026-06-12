#include "repositories/ProductionRepository.h"
#include <fstream>
#include <filesystem>
#include <algorithm>
#include <cstdio>
#include <ctime>
#include <chrono>
#include <nlohmann/json.hpp>

// DateTimeHelper는 4단계(utils) 산출물이므로 계층 역전 방지를 위해 로컬 헬퍼 사용
static std::string todayCompact() {
    auto now = std::chrono::system_clock::now();
    std::time_t t = std::chrono::system_clock::to_time_t(now);
    std::tm tm{};
#ifdef _WIN32
    localtime_s(&tm, &t);
#else
    localtime_r(&t, &tm);
#endif
    char buf[9];
    std::strftime(buf, sizeof(buf), "%Y%m%d", &tm);
    return buf;
}

ProductionRepository::ProductionRepository(const std::string& filePath)
    : m_filePath(filePath) {}

std::string ProductionRepository::generateId() {
    char buf[32];
    std::snprintf(buf, sizeof(buf), "JOB-%s-%04d",
        todayCompact().c_str(), m_nextSeq++);
    return buf;
}

bool ProductionRepository::load() {
    m_data.clear();
    std::ifstream ifs(m_filePath);
    if (!ifs.is_open()) return true;
    try {
        nlohmann::json j;
        ifs >> j;
        m_data = j.get<std::vector<ProductionJob>>();
    } catch (...) {
        return false;
    }
    // 오늘 날짜 기준 최대 시퀀스 추출
    const std::string today = "JOB-" + todayCompact() + "-";
    for (const auto& p : m_data) {
        if (p.m_jobId.rfind(today, 0) == 0) {
            try {
                int n = std::stoi(p.m_jobId.substr(today.size()));
                if (n >= m_nextSeq) m_nextSeq = n + 1;
            } catch (...) {}
        }
    }
    return true;
}

bool ProductionRepository::flush() {
    auto parent = std::filesystem::path(m_filePath).parent_path();
    if (!parent.empty()) std::filesystem::create_directories(parent);

    nlohmann::json j = m_data;
    const std::string tmpPath = m_filePath + ".tmp";
    {
        std::ofstream ofs(tmpPath);
        if (!ofs.is_open()) return false;
        ofs << j.dump(2);
    }
    std::error_code ec;
    std::filesystem::rename(tmpPath, m_filePath, ec);
    return !ec;
}

bool ProductionRepository::save(ProductionJob& entity) {
    if (entity.m_jobId.empty()) {
        entity.m_jobId = generateId();
        m_data.push_back(entity);
    } else {
        auto it = std::find_if(m_data.begin(), m_data.end(),
            [&](const ProductionJob& p) { return p.m_jobId == entity.m_jobId; });
        if (it != m_data.end()) {
            *it = entity;
        } else {
            m_data.push_back(entity);
        }
    }
    return flush();
}

std::optional<ProductionJob> ProductionRepository::findById(const std::string& id) const {
    auto it = std::find_if(m_data.begin(), m_data.end(),
        [&](const ProductionJob& p) { return p.m_jobId == id; });
    if (it != m_data.end()) return *it;
    return std::nullopt;
}

std::vector<ProductionJob> ProductionRepository::findAll() const {
    return m_data;
}

bool ProductionRepository::remove(const std::string& id) {
    auto it = std::find_if(m_data.begin(), m_data.end(),
        [&](const ProductionJob& p) { return p.m_jobId == id; });
    if (it == m_data.end()) return false;
    m_data.erase(it);
    return flush();
}
