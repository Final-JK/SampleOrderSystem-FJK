#pragma once
#include "views/SampleView.h"
#include "views/OrderView.h"
#include "views/ProductionView.h"
#include "views/ReleaseView.h"
#include "views/MonitoringView.h"

class MainView {
public:
    MainView(SampleView* sampleView, OrderView* orderView,
             ProductionView* productionView, ReleaseView* releaseView,
             MonitoringView* monitoringView);
    void run();
private:
    SampleView*     m_sampleView;
    OrderView*      m_orderView;
    ProductionView* m_productionView;
    ReleaseView*    m_releaseView;
    MonitoringView* m_monitoringView;
};
