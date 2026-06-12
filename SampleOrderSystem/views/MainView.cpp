#include "views/MainView.h"
#include <iostream>
#include <string>
#include <limits>

namespace {
    int readInt(const std::string& prompt) {
        std::cout << prompt;
        std::string s; std::cin >> s;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        try { return std::stoi(s); } catch (...) { return -1; }
    }
}

MainView::MainView(SampleView* sampleView, OrderView* orderView,
                   ProductionView* productionView, ReleaseView* releaseView,
                   MonitoringView* monitoringView)
    : m_sampleView(sampleView)
    , m_orderView(orderView)
    , m_productionView(productionView)
    , m_releaseView(releaseView)
    , m_monitoringView(monitoringView) {}

void MainView::run() {
    while (true) {
        std::cout << "\n==============================\n"
                  << "  시료 생산주문관리 시스템\n"
                  << "==============================\n"
                  << "  1. 시료 관리\n"
                  << "  2. 주문 관리\n"
                  << "  3. 생산 관리\n"
                  << "  4. 출고 관리\n"
                  << "  5. 재고 모니터링\n"
                  << "  0. 종료\n";
        int ch = readInt("  선택: ");
        if      (ch == 1) m_sampleView->run();
        else if (ch == 2) m_orderView->run();
        else if (ch == 3) m_productionView->run();
        else if (ch == 4) m_releaseView->run();
        else if (ch == 5) m_monitoringView->run();
        else if (ch == 0) { std::cout << "  종료합니다.\n"; break; }
        else std::cout << "  잘못된 입력입니다.\n";
    }
}
