#include "views/MainView.h"
#include "utils/ConsoleHelper.h"
#include <iostream>
#include <string>

MainView::MainView(SampleView* sampleView, OrderView* orderView,
                   ProductionView* productionView, ReleaseView* releaseView,
                   MonitoringView* monitoringView,
                   SampleController* sampleCtrl, OrderController* orderCtrl,
                   ProductionController* productionCtrl)
    : m_sampleView(sampleView)
    , m_orderView(orderView)
    , m_productionView(productionView)
    , m_releaseView(releaseView)
    , m_monitoringView(monitoringView)
    , m_sampleCtrl(sampleCtrl)
    , m_orderCtrl(orderCtrl)
    , m_productionCtrl(productionCtrl) {}

void MainView::showSummary() const {
    auto samples = m_sampleCtrl->getAllSamples();
    auto orders  = m_orderCtrl->getAllOrders();
    auto running = m_productionCtrl->getRunningJobs();

    int totalStock = 0;
    for (const auto& s : samples) totalStock += s.m_stock;

    std::cout << "  시료: " << samples.size() << "종"
              << "  총 재고: " << totalStock << "ea"
              << "  주문: " << orders.size() << "건"
              << "  생산 진행: " << running.size() << "건\n";
}

void MainView::run() {
    while (true) {
        std::cout << "\n==============================\n"
                  << "  시료 생산주문관리 시스템\n"
                  << "==============================\n";
        showSummary();
        std::cout << "------------------------------\n"
                  << "  1. 시료 관리\n"
                  << "  2. 주문 관리\n"
                  << "  3. 생산 관리\n"
                  << "  4. 출고 관리\n"
                  << "  5. 재고 모니터링\n"
                  << "  0. 종료\n";
        int ch = ConsoleHelper::readInt("  선택: ");
        if      (ch == 1) m_sampleView->run();
        else if (ch == 2) m_orderView->run();
        else if (ch == 3) m_productionView->run();
        else if (ch == 4) m_releaseView->run();
        else if (ch == 5) m_monitoringView->run();
        else if (ch == 0) { std::cout << "  종료합니다.\n"; break; }
        else std::cout << "  잘못된 입력입니다.\n";
    }
}
