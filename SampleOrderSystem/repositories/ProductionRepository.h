#pragma once
#include "repositories/IRepository.h"
#include "models/ProductionJob.h"
#include <string>
#include <vector>

class ProductionRepository : public IRepository<ProductionJob> {
public:
    explicit ProductionRepository(const std::string& filePath);

    bool save(ProductionJob& entity) override;
    std::optional<ProductionJob> findById(const std::string& id) const override;
    std::vector<ProductionJob> findAll() const override;
    bool remove(const std::string& id) override;
    bool load() override;
    bool flush() override;

private:
    std::string generateId();

    std::string              m_filePath;
    std::vector<ProductionJob> m_data;
    int                      m_nextSeq{1};
};
