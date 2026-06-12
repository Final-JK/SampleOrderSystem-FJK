#include "views/ProductionView.h"
#include "utils/ConsoleHelper.h"
#include <iostream>
#include <string>
#include <cstdio>

namespace {
    void printJobs(const std::vector<ProductionJob>& jobs) {
        using CH = ConsoleHelper;
        if (jobs.empty()) { std::cout << "  목록이 비어 있습니다.\n"; return; }
        std::cout << '\n'
                  << CH::padRight("JOB ID", 22)
                  << CH::padRight("주문 ID", 22)
                  << CH::padRight("부족량", 8)
                  << CH::padRight("생산량", 8)
                  << CH::padRight("예상(분)", 10)
                  << "상태\n"
                  << std::string(74, '-') << '\n';
        for (const auto& j : jobs) {
            char timeBuf[16]; std::snprintf(timeBuf, sizeof(timeBuf), "%.1f", j.m_totalTimeMins);
            std::cout << CH::padRight(j.m_jobId, 22)
                      << CH::padRight(j.m_orderId, 22)
                      << CH::padRight(std::to_string(j.m_shortage), 8)
                      << CH::padRight(std::to_string(j.m_actualProduction), 8)
                      << CH::padRight(timeBuf, 10)
                      << jobStatusToString(j.m_status) << '\n';
        }
    }
}

ProductionView::ProductionView(ProductionController* ctrl) : m_ctrl(ctrl) {}

void ProductionView::showAll()     const { printJobs(m_ctrl->getAllJobs()); }
void ProductionView::showRunning() const { printJobs(m_ctrl->getRunningJobs()); }

void ProductionView::showComplete() {
    showRunning();
    std::string id = ConsoleHelper::readLine("  완료 처리할 JOB ID: ");
    if (id.empty()) return;
    std::cout << (m_ctrl->completeJob(id) ? "  생산 완료 처리됨.\n" : "  처리 실패.\n");
}

void ProductionView::run() {
    while (true) {
        std::cout << "\n[생산 관리]\n"
                  << "  1. 전체 생산 목록\n"
                  << "  2. 진행 중 목록\n"
                  << "  3. 생산 완료 처리\n"
                  << "  0. 돌아가기\n";
        int ch = ConsoleHelper::readInt("  선택: ");
        if      (ch == 1) showAll();
        else if (ch == 2) showRunning();
        else if (ch == 3) showComplete();
        else if (ch == 0) break;
        else std::cout << "  잘못된 입력입니다.\n";
    }
}
