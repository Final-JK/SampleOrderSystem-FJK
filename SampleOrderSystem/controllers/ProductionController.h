#pragma once
#include "repositories/IRepository.h"
#include "models/ProductionJob.h"
#include "models/Order.h"
#include "models/Sample.h"
#include <string>
#include <vector>

class ProductionController {
public:
    ProductionController(IRepository<ProductionJob>* productionRepo,
                         IRepository<Order>*         orderRepo,
                         IRepository<Sample>*        sampleRepo);

    std::vector<ProductionJob> getAllJobs() const;
    std::vector<ProductionJob> getRunningJobs() const;
    bool completeJob(const std::string& jobId);

private:
    IRepository<ProductionJob>* m_productionRepo;
    IRepository<Order>*         m_orderRepo;
    IRepository<Sample>*        m_sampleRepo;
};
