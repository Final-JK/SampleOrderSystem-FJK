#include "repositories/SampleRepository.h"
#include <fstream>
#include <filesystem>
#include <algorithm>
#include <cstdio>
#include <nlohmann/json.hpp>

SampleRepository::SampleRepository(const std::string& filePath)
    : m_filePath(filePath) {}

std::string SampleRepository::generateId() {
    char buf[16];
    std::snprintf(buf, sizeof(buf), "S-%03d", m_nextId++);
    return buf;
}

bool SampleRepository::load() {
    m_data.clear();
    std::ifstream ifs(m_filePath);
    if (!ifs.is_open()) return true;  // 파일 없으면 빈 배열로 초기화
    try {
        nlohmann::json j;
        ifs >> j;
        m_data = j.get<std::vector<Sample>>();
    } catch (...) {
        return false;
    }
    for (const auto& s : m_data) {
        if (s.m_sampleId.size() > 2 && s.m_sampleId.rfind("S-", 0) == 0) {
            try {
                int n = std::stoi(s.m_sampleId.substr(2));
                if (n >= m_nextId) m_nextId = n + 1;
            } catch (...) {}
        }
    }
    return true;
}

bool SampleRepository::flush() {
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

bool SampleRepository::save(Sample& entity) {
    if (entity.m_sampleId.empty()) {
        entity.m_sampleId = generateId();
        m_data.push_back(entity);
    } else {
        auto it = std::find_if(m_data.begin(), m_data.end(),
            [&](const Sample& s) { return s.m_sampleId == entity.m_sampleId; });
        if (it != m_data.end()) {
            *it = entity;
        } else {
            m_data.push_back(entity);
        }
    }
    return flush();
}

std::optional<Sample> SampleRepository::findById(const std::string& id) const {
    auto it = std::find_if(m_data.begin(), m_data.end(),
        [&](const Sample& s) { return s.m_sampleId == id; });
    if (it != m_data.end()) return *it;
    return std::nullopt;
}

std::vector<Sample> SampleRepository::findAll() const {
    return m_data;
}

bool SampleRepository::remove(const std::string& id) {
    auto it = std::find_if(m_data.begin(), m_data.end(),
        [&](const Sample& s) { return s.m_sampleId == id; });
    if (it == m_data.end()) return false;
    m_data.erase(it);
    return flush();
}
