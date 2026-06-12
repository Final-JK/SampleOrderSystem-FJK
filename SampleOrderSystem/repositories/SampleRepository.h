#pragma once
#include "repositories/IRepository.h"
#include "models/Sample.h"
#include <string>
#include <vector>

class SampleRepository : public IRepository<Sample> {
public:
    explicit SampleRepository(const std::string& filePath);

    bool save(Sample& entity) override;
    std::optional<Sample> findById(const std::string& id) const override;
    std::vector<Sample> findAll() const override;
    bool remove(const std::string& id) override;
    bool load() override;
    bool flush() override;

private:
    std::string generateId();

    std::string        m_filePath;
    std::vector<Sample> m_data;
    int                m_nextId{1};
};
