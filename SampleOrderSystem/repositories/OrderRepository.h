#pragma once
#include "repositories/IRepository.h"
#include "models/Order.h"
#include <string>
#include <vector>

class OrderRepository : public IRepository<Order> {
public:
    explicit OrderRepository(const std::string& filePath);

    bool save(Order& entity) override;
    std::optional<Order> findById(const std::string& id) const override;
    std::vector<Order> findAll() const override;
    bool remove(const std::string& id) override;
    bool load() override;
    bool flush() override;

private:
    std::string generateId();

    std::string       m_filePath;
    std::vector<Order> m_data;
    int               m_nextSeq{1};
};
