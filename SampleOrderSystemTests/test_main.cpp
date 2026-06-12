#ifdef _WIN32
#include <windows.h>
#endif

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <iostream>

namespace {
    void initConsole() {
#ifdef _WIN32
        UINT originalCp = GetConsoleOutputCP();
        if (originalCp != CP_UTF8) {
            std::cout
                << "========================================\n"
                << "[WARNING] Console encoding is not UTF-8\n"
                << "          Current Code Page: " << originalCp << "\n"
                << "========================================\n"
                << "\n"
                << "Why Korean characters in test output may appear garbled:\n"
                << "  This test suite uses UTF-8 encoded Korean strings in\n"
                << "  assertion messages and test names.\n"
                << "  Your console is using Code Page " << originalCp << ", which\n"
                << "  cannot render UTF-8 multi-byte sequences correctly.\n"
                << "\n"
                << "How to fix:\n"
                << "  Option 1 (recommended): Run before launching:\n"
                << "              chcp 65001\n"
                << "  Option 2: Use Windows Terminal (defaults to UTF-8).\n"
                << "  Option 3: Enable system-wide UTF-8:\n"
                << "              Settings > Time & Language > Language\n"
                << "              > Administrative Language Settings\n"
                << "              > Change system locale\n"
                << "              > Check 'Beta: Use Unicode UTF-8 for worldwide\n"
                << "                language support'\n"
                << "\n"
                << "  Switching to UTF-8 now (Code Page 65001)...\n"
                << "========================================\n\n";
        }
        SetConsoleOutputCP(CP_UTF8);
        SetConsoleCP(CP_UTF8);
#endif
    }
}

int main(int argc, char** argv) {
    initConsole();
    ::testing::InitGoogleMock(&argc, argv);
    return RUN_ALL_TESTS();
}
