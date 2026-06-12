#pragma once
#include "controllers/ProductionController.h"

class ProductionView {
public:
    explicit ProductionView(ProductionController* ctrl);
    void run();
private:
    void showAll() const;
    void showRunning() const;
    void showComplete();
    ProductionController* m_ctrl;
};
