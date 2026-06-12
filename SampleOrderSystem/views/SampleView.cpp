#include "views/SampleView.h"
#include "utils/ConsoleHelper.h"
#include <iostream>
#include <string>
#include <cstdio>
#include <limits>

namespace {
    int readInt(const std::string& prompt) {
        std::cout << prompt;
        std::string s; std::cin >> s;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        try { return std::stoi(s); } catch (...) { return -1; }
    }
    std::string readLine(const std::string& prompt) {
        std::cout << prompt;
        std::string s; std::getline(std::cin, s);
        return s;
    }
}

SampleView::SampleView(SampleController* ctrl) : m_ctrl(ctrl) {}

void SampleView::showList() const {
    using CH = ConsoleHelper;
    auto samples = m_ctrl->getAllSamples();
    if (samples.empty()) { std::cout << "  등록된 시료가 없습니다.\n"; return; }
    // ID(8) + 이름(24) + 생산시간(12) + 수율(8) + 재고(last,max4) = 56
    std::cout << '\n'
              << CH::padRight("ID", 8)
              << CH::padRight("이름", 24)
              << CH::padRight("생산시간", 12)
              << CH::padRight("수율", 8)
              << "재고\n"
              << std::string(56, '-') << '\n';
    for (const auto& s : samples) {
        char timeBuf[16]; std::snprintf(timeBuf, sizeof(timeBuf), "%.1fh", s.m_avgProductionTime);
        char rateBuf[8];  std::snprintf(rateBuf, sizeof(rateBuf), "%.0f%%", s.m_yieldRate * 100);
        std::cout << CH::padRight(s.m_sampleId, 8)
                  << CH::padRight(s.m_name, 24)
                  << CH::padRight(timeBuf, 12)
                  << CH::padRight(rateBuf, 8)
                  << s.m_stock << '\n';
    }
}

void SampleView::showSearch() const {
    std::string kw = readLine("  검색어: ");
    if (kw.empty()) return;
    using CH = ConsoleHelper;
    auto results = m_ctrl->searchByName(kw);
    if (results.empty()) { std::cout << "  검색 결과 없음.\n"; return; }
    std::cout << '\n'
              << CH::padRight("ID", 8)
              << CH::padRight("이름", 24)
              << "재고\n"
              << std::string(36, '-') << '\n';
    for (const auto& s : results)
        std::cout << CH::padRight(s.m_sampleId, 8) << CH::padRight(s.m_name, 24) << s.m_stock << '\n';
}

void SampleView::showAdd() {
    std::string name = readLine("  시료명: ");
    if (name.empty()) return;
    double avgTime = 0.0, yieldRate = 0.0;
    int stock = 0;
    try {
        std::cout << "  평균 생산시간(h): "; std::cin >> avgTime;
        std::cout << "  수율(0.0~1.0): ";    std::cin >> yieldRate;
        std::cout << "  초기 재고: ";        std::cin >> stock;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    } catch (...) { std::cout << "  입력 오류.\n"; return; }
    std::cout << (m_ctrl->addSample(name, avgTime, yieldRate, stock) ? "  등록 완료.\n" : "  등록 실패.\n");
}

void SampleView::run() {
    while (true) {
        std::cout << "\n[시료 관리]\n"
                  << "  1. 전체 목록\n"
                  << "  2. 이름 검색\n"
                  << "  3. 시료 등록\n"
                  << "  0. 돌아가기\n";
        int ch = readInt("  선택: ");
        if      (ch == 1) showList();
        else if (ch == 2) showSearch();
        else if (ch == 3) showAdd();
        else if (ch == 0) break;
        else std::cout << "  잘못된 입력입니다.\n";
    }
}
