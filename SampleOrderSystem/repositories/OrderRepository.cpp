#include "repositories/OrderRepository.h"
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

OrderRepository::OrderRepository(const std::string& filePath)
    : m_filePath(filePath) {}

std::string OrderRepository::generateId() {
    char buf[32];
    std::snprintf(buf, sizeof(buf), "ORD-%s-%04d",
        todayCompact().c_str(), m_nextSeq++);
    return buf;
}

bool OrderRepository::load() {
    m_data.clear();
    std::ifstream ifs(m_filePath);
    if (!ifs.is_open()) return true;
    try {
        nlohmann::json j;
        ifs >> j;
        m_data = j.get<std::vector<Order>>();
    } catch (...) {
        return false;
    }
    // 오늘 날짜 기준 최대 시퀀스 추출
    const std::string today = "ORD-" + todayCompact() + "-";
    for (const auto& o : m_data) {
        if (o.m_orderId.rfind(today, 0) == 0) {
            try {
                int n = std::stoi(o.m_orderId.substr(today.size()));
                if (n >= m_nextSeq) m_nextSeq = n + 1;
            } catch (...) {}
        }
    }
    return true;
}

bool OrderRepository::flush() {
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

bool OrderRepository::save(Order& entity) {
    if (entity.m_orderId.empty()) {
        entity.m_orderId = generateId();
        m_data.push_back(entity);
    } else {
        auto it = std::find_if(m_data.begin(), m_data.end(),
            [&](const Order& o) { return o.m_orderId == entity.m_orderId; });
        if (it != m_data.end()) {
            *it = entity;
        } else {
            m_data.push_back(entity);
        }
    }
    return flush();
}

std::optional<Order> OrderRepository::findById(const std::string& id) const {
    auto it = std::find_if(m_data.begin(), m_data.end(),
        [&](const Order& o) { return o.m_orderId == id; });
    if (it != m_data.end()) return *it;
    return std::nullopt;
}

std::vector<Order> OrderRepository::findAll() const {
    return m_data;
}

bool OrderRepository::remove(const std::string& id) {
    auto it = std::find_if(m_data.begin(), m_data.end(),
        [&](const Order& o) { return o.m_orderId == id; });
    if (it == m_data.end()) return false;
    m_data.erase(it);
    return flush();
}
