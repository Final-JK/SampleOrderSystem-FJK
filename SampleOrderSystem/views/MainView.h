#pragma once
#include "views/SampleView.h"
#include "views/OrderView.h"
#include "views/ProductionView.h"
#include "views/ReleaseView.h"
#include "views/MonitoringView.h"
#include "controllers/SampleController.h"
#include "controllers/OrderController.h"
#include "controllers/ProductionController.h"

class MainView {
public:
    MainView(SampleView* sampleView, OrderView* orderView,
             ProductionView* productionView, ReleaseView* releaseView,
             MonitoringView* monitoringView,
             SampleController* sampleCtrl, OrderController* orderCtrl,
             ProductionController* productionCtrl);
    void run();
private:
    void showSummary() const;

    SampleView*          m_sampleView;
    OrderView*           m_orderView;
    ProductionView*      m_productionView;
    ReleaseView*         m_releaseView;
    MonitoringView*      m_monitoringView;
    SampleController*    m_sampleCtrl;
    OrderController*     m_orderCtrl;
    ProductionController* m_productionCtrl;
};
