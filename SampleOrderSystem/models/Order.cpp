#include "models/Order.h"

std::string orderStatusToString(OrderStatus s) {
    switch (s) {
        case OrderStatus::RESERVED:  return "RESERVED";
        case OrderStatus::CONFIRMED: return "CONFIRMED";
        case OrderStatus::PRODUCING: return "PRODUCING";
        case OrderStatus::REJECTED:  return "REJECTED";
        case OrderStatus::RELEASE:   return "RELEASE";
    }
    return "RESERVED";
}

OrderStatus orderStatusFromString(const std::string& s) {
    if (s == "CONFIRMED") return OrderStatus::CONFIRMED;
    if (s == "PRODUCING") return OrderStatus::PRODUCING;
    if (s == "REJECTED")  return OrderStatus::REJECTED;
    if (s == "RELEASE")   return OrderStatus::RELEASE;
    return OrderStatus::RESERVED;
}

void to_json(nlohmann::json& j, const Order& o) {
    j = {
        {"order_id",   o.m_orderId},
        {"sample_id",  o.m_sampleId},
        {"customer",   o.m_customer},
        {"quantity",   o.m_quantity},
        {"status",     orderStatusToString(o.m_status)},
        {"created_at", o.m_createdAt},
        {"updated_at", o.m_updatedAt}
    };
}

void from_json(const nlohmann::json& j, Order& o) {
    o.m_orderId   = j.value("order_id",   "");
    o.m_sampleId  = j.value("sample_id",  "");
    o.m_customer  = j.value("customer",   "");
    o.m_quantity  = j.value("quantity",   0);
    o.m_status    = orderStatusFromString(j.value("status", "RESERVED"));
    o.m_createdAt = j.value("created_at", "");
    o.m_updatedAt = j.value("updated_at", "");
}
