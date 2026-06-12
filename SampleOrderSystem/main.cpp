#ifdef _WIN32
#include <windows.h>
#endif

#include "repositories/SampleRepository.h"
#include "repositories/OrderRepository.h"
#include "repositories/ProductionRepository.h"
#include "controllers/SampleController.h"
#include "controllers/OrderController.h"
#include "controllers/ProductionController.h"
#include "controllers/ReleaseController.h"
#include "views/SampleView.h"
#include "views/OrderView.h"
#include "views/ProductionView.h"
#include "views/ReleaseView.h"
#include "views/MonitoringView.h"
#include "views/MainView.h"

int main() {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif

    // Repositories
    SampleRepository     sampleRepo("data/samples.json");
    OrderRepository      orderRepo("data/orders.json");
    ProductionRepository productionRepo("data/production_jobs.json");

    sampleRepo.load();
    orderRepo.load();
    productionRepo.load();

    // Controllers
    SampleController     sampleCtrl(&sampleRepo);
    OrderController      orderCtrl(&orderRepo, &sampleRepo, &productionRepo);
    ProductionController productionCtrl(&productionRepo, &orderRepo, &sampleRepo);
    ReleaseController    releaseCtrl(&orderRepo);

    // Views
    SampleView     sampleView(&sampleCtrl);
    OrderView      orderView(&orderCtrl);
    ProductionView productionView(&productionCtrl);
    ReleaseView    releaseView(&releaseCtrl);
    MonitoringView monitoringView(&sampleCtrl, &orderCtrl);
    MainView       mainView(&sampleView, &orderView, &productionView,
                            &releaseView, &monitoringView);

    mainView.run();
    return 0;
}
