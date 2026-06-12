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
#include <iostream>

namespace {
    void initConsole() {
#ifdef _WIN32
        UINT originalCp = GetConsoleOutputCP();
        if (originalCp != CP_UTF8) {
            std::cout
                << "========================================\n"
                << "[WARNING] Console encoding is not UTF-8\n"
                << "          Current Code Page: " << originalCp << "\n"
                << "========================================\n"
                << "\n"
                << "Why Korean characters may appear garbled:\n"
                << "  This application encodes all text in UTF-8 (Code Page 65001).\n"
                << "  Your console is using Code Page " << originalCp << ", which\n"
                << "  maps Korean code points differently and cannot render\n"
                << "  UTF-8 multi-byte sequences correctly.\n"
                << "\n"
                << "How to fix:\n"
                << "  Option 1 (recommended): Run the following command before\n"
                << "            launching this application:\n"
                << "              chcp 65001\n"
                << "  Option 2: Use Windows Terminal -- it defaults to UTF-8.\n"
                << "  Option 3: Enable system-wide UTF-8:\n"
                << "              Settings > Time & Language > Language\n"
                << "              > Administrative Language Settings\n"
                << "              > Change system locale\n"
                << "              > Check 'Beta: Use Unicode UTF-8 for worldwide\n"
                << "                language support'\n"
                << "\n"
                << "  Switching to UTF-8 now (Code Page 65001)...\n"
                << "  Korean text should display correctly from this point.\n"
                << "  If it still appears garbled, apply one of the options above.\n"
                << "========================================\n\n";
        }
        SetConsoleOutputCP(CP_UTF8);
        SetConsoleCP(CP_UTF8);
#endif
    }
}

int main() {
    initConsole();

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
                            &releaseView, &monitoringView,
                            &sampleCtrl, &orderCtrl, &productionCtrl);

    mainView.run();
    return 0;
}
