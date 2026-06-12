#include "views/OrderView.h"
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

    void printOrders(const std::vector<Order>& orders) {
        using CH = ConsoleHelper;
        if (orders.empty()) { std::cout << "  목록이 비어 있습니다.\n"; return; }
        std::cout << '\n'
                  << CH::padRight("주문ID", 22)
                  << CH::padRight("시료ID", 8)
                  << CH::padRight("고객사", 20)
                  << CH::padRight("수량", 6)
                  << "상태\n"
                  << std::string(60, '-') << '\n';
        for (const auto& o : orders)
            std::cout << CH::padRight(o.m_orderId, 22)
                      << CH::padRight(o.m_sampleId, 8)
                      << CH::padRight(o.m_customer, 20)
                      << CH::padRight(std::to_string(o.m_quantity), 6)
                      << orderStatusToString(o.m_status) << '\n';
    }
}

OrderView::OrderView(OrderController* ctrl) : m_ctrl(ctrl) {}

void OrderView::showAll() const    { printOrders(m_ctrl->getAllOrders()); }
void OrderView::showReserved() const { printOrders(m_ctrl->getReservedOrders()); }

void OrderView::showCreate() {
    std::string sampleId = readLine("  시료 ID: ");
    std::string customer = readLine("  고객사: ");
    int qty = readInt("  주문 수량: ");
    if (sampleId.empty() || customer.empty() || qty <= 0) { std::cout << "  입력 오류.\n"; return; }
    std::cout << (m_ctrl->createOrder(sampleId, customer, qty) ? "  주문 등록 완료.\n" : "  등록 실패 (시료 ID 확인).\n");
}

void OrderView::showApprove() {
    showReserved();
    std::string id = readLine("  주문 ID: ");
    if (id.empty()) return;
    int action = readInt("  1.승인  2.거절: ");
    bool ok = false;
    if      (action == 1) ok = m_ctrl->approve(id);
    else if (action == 2) ok = m_ctrl->reject(id);
    else { std::cout << "  잘못된 입력.\n"; return; }
    std::cout << (ok ? "  처리 완료.\n" : "  처리 실패.\n");
}

void OrderView::run() {
    while (true) {
        std::cout << "\n[주문 관리]\n"
                  << "  1. 전체 주문 목록\n"
                  << "  2. 대기 주문 목록\n"
                  << "  3. 주문 등록\n"
                  << "  4. 승인/거절\n"
                  << "  0. 돌아가기\n";
        int ch = readInt("  선택: ");
        if      (ch == 1) showAll();
        else if (ch == 2) showReserved();
        else if (ch == 3) showCreate();
        else if (ch == 4) showApprove();
        else if (ch == 0) break;
        else std::cout << "  잘못된 입력입니다.\n";
    }
}
