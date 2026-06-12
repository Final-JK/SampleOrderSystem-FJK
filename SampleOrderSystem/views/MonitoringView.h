#pragma once
#include "controllers/SampleController.h"
#include "controllers/OrderController.h"

class MonitoringView {
public:
    MonitoringView(SampleController* sampleCtrl, OrderController* orderCtrl);
    void run() const;
private:
    SampleController* m_sampleCtrl;
    OrderController*  m_orderCtrl;
};
