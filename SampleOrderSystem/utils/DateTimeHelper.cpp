#include "utils/DateTimeHelper.h"
#include <chrono>
#include <ctime>
#include <cstdio>

static std::tm localNow() {
    auto now = std::chrono::system_clock::now();
    std::time_t t = std::chrono::system_clock::to_time_t(now);
    std::tm tm{};
#ifdef _WIN32
    localtime_s(&tm, &t);
#else
    localtime_r(&t, &tm);
#endif
    return tm;
}

std::string DateTimeHelper::nowIso() {
    std::tm tm = localNow();
    char buf[20];
    std::strftime(buf, sizeof(buf), "%Y-%m-%dT%H:%M:%S", &tm);
    return buf;
}

std::string DateTimeHelper::todayCompact() {
    std::tm tm = localNow();
    char buf[9];
    std::strftime(buf, sizeof(buf), "%Y%m%d", &tm);
    return buf;
}
