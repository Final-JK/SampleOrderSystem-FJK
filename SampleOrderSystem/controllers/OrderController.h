#pragma once
#include "repositories/IRepository.h"
#include "models/Order.h"
#include "models/Sample.h"
#include "models/ProductionJob.h"
#include <string>
#include <vector>
#include <optional>

class OrderController {
public:
    OrderController(IRepository<Order>* orderRepo,
                    IRepository<Sample>* sampleRepo,
                    IRepository<ProductionJob>* productionRepo);

    std::vector<Order> getReservedOrders() const;
    std::vector<Order> getAllOrders() const;
    bool createOrder(const std::string& sampleId,
                     const std::string& customer, int quantity);
    bool approve(const std::string& orderId);
    bool reject(const std::string& orderId);

private:
    IRepository<Order>*         m_orderRepo;
    IRepository<Sample>*        m_sampleRepo;
    IRepository<ProductionJob>* m_productionRepo;
};
