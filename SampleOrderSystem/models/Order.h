#pragma once
#include <string>
#include <nlohmann/json.hpp>

enum class OrderStatus {
    RESERVED,
    CONFIRMED,
    PRODUCING,
    REJECTED,
    RELEASE
};

std::string orderStatusToString(OrderStatus s);
OrderStatus orderStatusFromString(const std::string& s);

struct Order {
    std::string m_orderId;
    std::string m_sampleId;
    std::string m_customer;
    int m_quantity{0};
    OrderStatus m_status{OrderStatus::RESERVED};
    std::string m_createdAt;
    std::string m_updatedAt;
};

void to_json(nlohmann::json& j, const Order& o);
void from_json(const nlohmann::json& j, Order& o);
