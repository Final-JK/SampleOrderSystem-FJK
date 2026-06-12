#pragma once
#include "repositories/IRepository.h"
#include "models/Order.h"
#include <string>
#include <vector>

class ReleaseController {
public:
    explicit ReleaseController(IRepository<Order>* orderRepo);

    std::vector<Order> getConfirmedOrders() const;
    bool release(const std::string& orderId);

private:
    IRepository<Order>* m_orderRepo;
};
