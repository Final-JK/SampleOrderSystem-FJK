#include "utils/ConsoleHelper.h"
#include <cstdint>

int ConsoleHelper::displayWidth(const std::string& s) {
    int width = 0;
    size_t i = 0;
    while (i < s.size()) {
        unsigned char c = static_cast<unsigned char>(s[i]);
        uint32_t cp = 0;
        if (c < 0x80) {
            cp = c; i += 1;
        } else if ((c & 0xE0) == 0xC0 && i + 1 < s.size()) {
            cp = (c & 0x1F) << 6 | (static_cast<unsigned char>(s[i+1]) & 0x3F);
            i += 2;
        } else if ((c & 0xF0) == 0xE0 && i + 2 < s.size()) {
            cp = (c & 0x0F) << 12
               | (static_cast<unsigned char>(s[i+1]) & 0x3F) << 6
               | (static_cast<unsigned char>(s[i+2]) & 0x3F);
            i += 3;
        } else {
            i += 1; width += 1; continue;  // 불완전 시퀀스
        }

        bool isWide =
            (cp >= 0x1100 && cp <= 0x11FF) ||   // Hangul Jamo
            (cp >= 0x2E80 && cp <= 0x303F) ||   // CJK Radicals / Kangxi
            (cp >= 0x3040 && cp <= 0x33FF) ||   // Hiragana/Katakana/CJK Symbols
            (cp >= 0x3400 && cp <= 0x4DBF) ||   // CJK Extension A
            (cp >= 0x4E00 && cp <= 0x9FFF) ||   // CJK Unified Ideographs
            (cp >= 0xA000 && cp <= 0xA4CF) ||   // Yi
            (cp >= 0xAC00 && cp <= 0xD7AF) ||   // Hangul Syllables
            (cp >= 0xF900 && cp <= 0xFAFF) ||   // CJK Compatibility Ideographs
            (cp >= 0xFF00 && cp <= 0xFFEF);     // Halfwidth/Fullwidth Forms

        width += isWide ? 2 : 1;
    }
    return width;
}

std::string ConsoleHelper::padRight(const std::string& s, int targetWidth) {
    int w = displayWidth(s);
    if (w >= targetWidth) return s;
    return s + std::string(targetWidth - w, ' ');
}

std::string ConsoleHelper::padLeft(const std::string& s, int targetWidth) {
    int w = displayWidth(s);
    if (w >= targetWidth) return s;
    return std::string(targetWidth - w, ' ') + s;
}
