#pragma once
#include "repositories/IRepository.h"
#include "models/Sample.h"
#include <string>
#include <vector>
#include <optional>

class SampleController {
public:
    explicit SampleController(IRepository<Sample>* repo);

    std::vector<Sample>       getAllSamples() const;
    std::optional<Sample>     findSample(const std::string& id) const;
    std::vector<Sample>       searchByName(const std::string& keyword) const;
    bool addSample(const std::string& name, double avgProdTime,
                   double yieldRate, int initialStock);

private:
    IRepository<Sample>* m_repo;
};
