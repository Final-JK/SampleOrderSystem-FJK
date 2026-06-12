#pragma once
#include <string>

class DateTimeHelper {
public:
    static std::string nowIso();        // "2026-06-12T09:00:00"
    static std::string todayCompact();  // "20260612"
};
