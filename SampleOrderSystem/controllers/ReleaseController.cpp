#include "controllers/ReleaseController.h"
#include "utils/DateTimeHelper.h"
#include <algorithm>

ReleaseController::ReleaseController(IRepository<Order>* orderRepo)
    : m_orderRepo(orderRepo) {}

std::vector<Order> ReleaseController::getConfirmedOrders() const {
    auto all = m_orderRepo->findAll();
    std::vector<Order> result;
    for (const auto& o : all)
        if (o.m_status == OrderStatus::CONFIRMED) result.push_back(o);
    return result;
}

bool ReleaseController::release(const std::string& orderId) {
    auto optOrder = m_orderRepo->findById(orderId);
    if (!optOrder || optOrder->m_status != OrderStatus::CONFIRMED) return false;

    Order order      = *optOrder;
    order.m_status    = OrderStatus::RELEASE;
    order.m_updatedAt = DateTimeHelper::nowIso();
    return m_orderRepo->save(order);
}
