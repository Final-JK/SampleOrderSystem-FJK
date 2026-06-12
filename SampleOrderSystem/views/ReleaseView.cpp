#include "views/ReleaseView.h"
#include "utils/ConsoleHelper.h"
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
    std::string readLine(const std::string& prompt) {
        std::cout << prompt;
        std::string s; std::getline(std::cin, s);
        return s;
    }
}

ReleaseView::ReleaseView(ReleaseController* ctrl) : m_ctrl(ctrl) {}

void ReleaseView::showConfirmed() const {
    using CH = ConsoleHelper;
    auto orders = m_ctrl->getConfirmedOrders();
    if (orders.empty()) { std::cout << "  출고 대기 주문이 없습니다.\n"; return; }
    std::cout << '\n'
              << CH::padRight("주문ID", 22)
              << CH::padRight("시료ID", 8)
              << CH::padRight("고객사", 16)
              << "수량\n"
              << std::string(52, '-') << '\n';
    for (const auto& o : orders)
        std::cout << CH::padRight(o.m_orderId, 22)
                  << CH::padRight(o.m_sampleId, 8)
                  << CH::padRight(o.m_customer, 16)
                  << o.m_quantity << '\n';
}

void ReleaseView::showRelease() {
    showConfirmed();
    std::string id = readLine("  출고 처리할 주문 ID: ");
    if (id.empty()) return;
    std::cout << (m_ctrl->release(id) ? "  출고 처리 완료.\n" : "  처리 실패.\n");
}

void ReleaseView::run() {
    while (true) {
        std::cout << "\n[출고 관리]\n"
                  << "  1. 출고 대기 목록\n"
                  << "  2. 출고 처리\n"
                  << "  0. 돌아가기\n";
        int ch = readInt("  선택: ");
        if      (ch == 1) showConfirmed();
        else if (ch == 2) showRelease();
        else if (ch == 0) break;
        else std::cout << "  잘못된 입력입니다.\n";
    }
}
