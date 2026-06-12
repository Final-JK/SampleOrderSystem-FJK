#include "views/MonitoringView.h"
#include "utils/ConsoleHelper.h"
#include <iostream>
#include <string>
#include <algorithm>
#include <limits>

MonitoringView::MonitoringView(SampleController* sampleCtrl, OrderController* orderCtrl)
    : m_sampleCtrl(sampleCtrl), m_orderCtrl(orderCtrl) {}

void MonitoringView::showData() const {
    using CH = ConsoleHelper;

    auto samples = m_sampleCtrl->getAllSamples();
    auto orders  = m_orderCtrl->getAllOrders();

    if (samples.empty()) { std::cout << "  등록된 시료가 없습니다.\n"; return; }

    // 상태별 주문 건수 (REJECTED 제외)
    int cntReserved = 0, cntConfirmed = 0, cntProducing = 0, cntRelease = 0;
    for (const auto& o : orders) {
        switch (o.m_status) {
            case OrderStatus::RESERVED:  ++cntReserved;  break;
            case OrderStatus::CONFIRMED: ++cntConfirmed; break;
            case OrderStatus::PRODUCING: ++cntProducing; break;
            case OrderStatus::RELEASE:   ++cntRelease;   break;
            default: break;
        }
    }
    std::cout << "\n[주문 현황]\n"
              << "  대기(RESERVED): "   << cntReserved
              << "  확정(CONFIRMED): "  << cntConfirmed
              << "  생산중(PRODUCING): " << cntProducing
              << "  출고(RELEASE): "    << cntRelease << "\n\n";

    // 시료별 CONFIRMED 주문 수량 합산
    auto confirmedSum = [&](const std::string& sampleId) {
        int sum = 0;
        for (const auto& o : orders)
            if (o.m_sampleId == sampleId && o.m_status == OrderStatus::CONFIRMED)
                sum += o.m_quantity;
        return sum;
    };

    // 재고 기준 ASCII 바 (최대 재고 기준 20칸)
    int maxStock = 0;
    for (const auto& s : samples) maxStock = std::max(maxStock, s.m_stock);
    if (maxStock == 0) maxStock = 1;

    auto makeBar = [&](int stock) {
        int filled = (stock * 20) / maxStock;
        return std::string(filled, '#') + std::string(20 - filled, '.');
    };

    std::cout << '\n'
              << CH::padRight("ID", 8)
              << CH::padRight("이름", 24)
              << CH::padRight("재고", 6)
              << CH::padRight("확정량", 8)
              << CH::padRight("상태", 8)
              << "바\n"
              << std::string(76, '-') << '\n';

    for (const auto& s : samples) {
        int confirmed = confirmedSum(s.m_sampleId);
        std::string status;
        if      (s.m_stock == 0)             status = "고갈";
        else if (s.m_stock < confirmed)      status = "부족";
        else                                 status = "여유";

        std::cout << CH::padRight(s.m_sampleId, 8)
                  << CH::padRight(s.m_name, 24)
                  << CH::padRight(std::to_string(s.m_stock), 6)
                  << CH::padRight(std::to_string(confirmed), 8)
                  << CH::padRight(status, 8)
                  << '[' << makeBar(s.m_stock) << "]\n";
    }
}

void MonitoringView::run() const {
    while (true) {
        showData();
        std::cout << "\n  Enter: 새로고침   0: 돌아가기\n  선택: ";
        std::string input;
        std::getline(std::cin, input);
        if (input == "0") break;
    }
}
