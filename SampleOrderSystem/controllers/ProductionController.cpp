#include "controllers/ProductionController.h"
#include "utils/DateTimeHelper.h"
#include <algorithm>
#include <iostream>

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
    return m_productionRepo->findBy(
        [](const ProductionJob& j) { return j.m_status == JobStatus::RUNNING; });
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

    if (!m_productionRepo->save(job)) {
        std::cerr << "[ERROR] completeJob: ProductionJob 저장 실패\n";
        return false;
    }
    if (!m_sampleRepo->save(sample)) {
        std::cerr << "[ERROR] completeJob: Sample 저장 실패 (job 은 이미 저장됨)\n";
        return false;
    }
    if (!m_orderRepo->save(order)) {
        std::cerr << "[ERROR] completeJob: Order 저장 실패 (job+sample 은 이미 저장됨)\n";
        return false;
    }
    return true;
}
