#include "controllers/OrderController.h"
#include "utils/DateTimeHelper.h"
#include <algorithm>
#include <cmath>

OrderController::OrderController(IRepository<Order>*         orderRepo,
                                  IRepository<Sample>*        sampleRepo,
                                  IRepository<ProductionJob>* productionRepo)
    : m_orderRepo(orderRepo)
    , m_sampleRepo(sampleRepo)
    , m_productionRepo(productionRepo) {}

std::vector<Order> OrderController::getReservedOrders() const {
    auto all = m_orderRepo->findAll();
    std::vector<Order> result;
    for (const auto& o : all)
        if (o.m_status == OrderStatus::RESERVED) result.push_back(o);
    return result;
}

std::vector<Order> OrderController::getAllOrders() const {
    return m_orderRepo->findAll();
}

bool OrderController::createOrder(const std::string& sampleId,
                                   const std::string& customer, int quantity) {
    if (!m_sampleRepo->findById(sampleId)) return false;

    Order o;
    o.m_sampleId   = sampleId;
    o.m_customer   = customer;
    o.m_quantity   = quantity;
    o.m_status     = OrderStatus::RESERVED;
    o.m_createdAt  = DateTimeHelper::nowIso();
    o.m_updatedAt  = o.m_createdAt;
    return m_orderRepo->save(o);
}

bool OrderController::approve(const std::string& orderId) {
    auto optOrder = m_orderRepo->findById(orderId);
    if (!optOrder || optOrder->m_status != OrderStatus::RESERVED) return false;

    auto optSample = m_sampleRepo->findById(optOrder->m_sampleId);
    if (!optSample) return false;

    Order  order  = *optOrder;
    Sample sample = *optSample;
    const std::string now = DateTimeHelper::nowIso();

    if (sample.m_stock >= order.m_quantity) {
        sample.m_stock         -= order.m_quantity;
        sample.m_stockUpdatedAt = now;
        order.m_status          = OrderStatus::CONFIRMED;
        order.m_updatedAt       = now;
        m_sampleRepo->save(sample);
        m_orderRepo->save(order);
    } else {
        int    shortage   = order.m_quantity - sample.m_stock;
        int    actualProd = static_cast<int>(
                                std::ceil(static_cast<double>(shortage)
                                          / (sample.m_yieldRate * 0.9)));
        double totalMins  = sample.m_avgProductionTime * actualProd;

        ProductionJob job;
        job.m_orderId          = order.m_orderId;
        job.m_sampleId         = sample.m_sampleId;
        job.m_shortage         = shortage;
        job.m_actualProduction = actualProd;
        job.m_totalTimeMins    = totalMins;
        job.m_status           = JobStatus::RUNNING;
        job.m_createdAt        = now;

        order.m_status    = OrderStatus::PRODUCING;
        order.m_updatedAt = now;
        m_productionRepo->save(job);
        m_orderRepo->save(order);
    }
    return true;
}

bool OrderController::reject(const std::string& orderId) {
    auto optOrder = m_orderRepo->findById(orderId);
    if (!optOrder || optOrder->m_status != OrderStatus::RESERVED) return false;

    Order order      = *optOrder;
    order.m_status    = OrderStatus::REJECTED;
    order.m_updatedAt = DateTimeHelper::nowIso();
    return m_orderRepo->save(order);
}
