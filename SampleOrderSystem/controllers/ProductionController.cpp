#include "controllers/ProductionController.h"
#include "utils/DateTimeHelper.h"
#include <algorithm>

ProductionController::ProductionController(IRepository<ProductionJob>* productionRepo,
                                            IRepository<Order>*         orderRepo,
                                            IRepository<Sample>*        sampleRepo)
    : m_productionRepo(productionRepo)
    , m_orderRepo(orderRepo)
    , m_sampleRepo(sampleRepo) {}

std::vector<ProductionJob> ProductionController::getAllJobs() const {
    return m_productionRepo->findAll();
}

std::vector<ProductionJob> ProductionController::getRunningJobs() const {
    auto all = m_productionRepo->findAll();
    std::vector<ProductionJob> result;
    for (const auto& j : all)
        if (j.m_status == JobStatus::RUNNING) result.push_back(j);
    return result;
}

bool ProductionController::completeJob(const std::string& jobId) {
    auto optJob = m_productionRepo->findById(jobId);
    if (!optJob || optJob->m_status != JobStatus::RUNNING) return false;

    auto optOrder = m_orderRepo->findById(optJob->m_orderId);
    if (!optOrder) return false;

    auto optSample = m_sampleRepo->findById(optJob->m_sampleId);
    if (!optSample) return false;

    ProductionJob job    = *optJob;
    Order         order  = *optOrder;
    Sample        sample = *optSample;
    const std::string now = DateTimeHelper::nowIso();

    job.m_status      = JobStatus::COMPLETED;
    job.m_completedAt = now;

    // 생산량에서 주문 수량 차감 후 잔여분을 재고에 반영
    sample.m_stock         += job.m_actualProduction - order.m_quantity;
    sample.m_stockUpdatedAt = now;

    order.m_status    = OrderStatus::CONFIRMED;
    order.m_updatedAt = now;

    m_productionRepo->save(job);
    m_sampleRepo->save(sample);
    m_orderRepo->save(order);
    return true;
}
