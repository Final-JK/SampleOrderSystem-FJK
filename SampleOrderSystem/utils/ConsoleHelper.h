#pragma once
#include <string>

class ConsoleHelper {
public:
    static int displayWidth(const std::string& s);   // CJK=2, ASCII=1
    static std::string padRight(const std::string& s, int targetWidth);
    static std::string padLeft(const std::string& s, int targetWidth);
};
