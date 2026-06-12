#include "controllers/SampleController.h"
#include "utils/DateTimeHelper.h"
#include <algorithm>

SampleController::SampleController(IRepository<Sample>* repo)
    : m_repo(repo) {}

std::vector<Sample> SampleController::getAllSamples() const {
    return m_repo->findAll();
}

std::optional<Sample> SampleController::findSample(const std::string& id) const {
    return m_repo->findById(id);
}

std::vector<Sample> SampleController::searchByName(const std::string& keyword) const {
    auto all = m_repo->findAll();
    std::vector<Sample> result;
    for (const auto& s : all) {
        if (s.m_name.find(keyword) != std::string::npos)
            result.push_back(s);
    }
    return result;
}

bool SampleController::addSample(const std::string& name, double avgProdTime,
                                  double yieldRate, int initialStock) {
    Sample s;
    s.m_name = name;
    s.m_avgProductionTime = avgProdTime;
    s.m_yieldRate = yieldRate;
    s.m_stock = initialStock;
    s.m_stockUpdatedAt = DateTimeHelper::nowIso();
    return m_repo->save(s);
}
