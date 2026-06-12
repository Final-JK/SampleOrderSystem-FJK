#pragma once
#include <string>
#include <vector>
#include <optional>

template<typename T>
class IRepository {
public:
    virtual ~IRepository() = default;
    virtual bool save(T& entity) = 0;
    virtual std::optional<T> findById(const std::string& id) const = 0;
    virtual std::vector<T> findAll() const = 0;
    virtual bool remove(const std::string& id) = 0;
    virtual bool load() = 0;
    virtual bool flush() = 0;
};
