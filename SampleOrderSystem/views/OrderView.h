#pragma once
#include "controllers/OrderController.h"

class OrderView {
public:
    explicit OrderView(OrderController* ctrl);
    void run();
private:
    void showAll() const;
    void showReserved() const;
    void showCreate();
    void showApprove();
    OrderController* m_ctrl;
};
