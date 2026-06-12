#ifdef _WIN32
#include <windows.h>
#endif

#include "models/Sample.h"
#include "models/Order.h"
#include "models/ProductionJob.h"
#include "utils/DateTimeHelper.h"
#include <nlohmann/json.hpp>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <vector>
#include <string>
#include <random>
#include <cstdio>

static void writeJson(const std::string& path, const nlohmann::json& j) {
    std::filesystem::create_directories(std::filesystem::path(path).parent_path());
    std::ofstream ofs(path);
    ofs << j.dump(2);
}

int main(int argc, char* argv[]) {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif

    // CLI 인수 파싱
    int         nSamples = 10, nOrders = 30, seed = 42;
    std::string outDir   = "../SampleOrderSystem/data";

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--samples" && i + 1 < argc) nSamples = std::stoi(argv[++i]);
        else if (arg == "--orders"  && i + 1 < argc) nOrders  = std::stoi(argv[++i]);
        else if (arg == "--seed"    && i + 1 < argc) seed     = std::stoi(argv[++i]);
        else if (arg == "--out"     && i + 1 < argc) outDir   = argv[++i];
    }

    std::mt19937 rng(static_cast<unsigned>(seed));

    // Sample 생성
    static const char* sampleNames[] = {
        "실리콘 웨이퍼-8인치",  "실리콘 웨이퍼-12인치", "갈륨비소 기판",
        "사파이어 기판",         "탄화규소 기판",         "게르마늄 웨이퍼",
        "인화인듐 기판",         "산화아연 박막",         "질화갈륨 기판",
        "다이아몬드 기판",       "실리콘 카바이드",       "알루미늄 기판"
    };
    int nameCount = static_cast<int>(sizeof(sampleNames) / sizeof(sampleNames[0]));

    std::uniform_real_distribution<double> timeDist(0.3, 3.0);
    std::uniform_real_distribution<double> yieldDist(0.75, 0.98);
    std::uniform_int_distribution<int>     stockDist(50, 800);

    std::vector<Sample> samples;
    const std::string   now = DateTimeHelper::nowIso();
    for (int i = 0; i < nSamples; ++i) {
        Sample s;
        char buf[16]; std::snprintf(buf, sizeof(buf), "S-%03d", i + 1);
        s.m_sampleId          = buf;
        s.m_name              = sampleNames[i % nameCount];
        s.m_avgProductionTime = timeDist(rng);
        s.m_yieldRate         = yieldDist(rng);
        s.m_stock             = stockDist(rng);
        s.m_stockUpdatedAt    = now;
        samples.push_back(s);
    }

    // Order 생성
    static const char* customers[] = {
        "삼성전자", "SK하이닉스", "LG전자", "인텔코리아",
        "TSMC", "마이크론", "퀄컴", "엔비디아"
    };
    int custCount = static_cast<int>(sizeof(customers) / sizeof(customers[0]));

    static const char* statuses[] = {
        "RESERVED", "CONFIRMED", "PRODUCING", "REJECTED", "RELEASE"
    };
    std::uniform_int_distribution<int> custDist(0, custCount - 1);
    std::uniform_int_distribution<int> sampleDist(0, nSamples - 1);
    std::uniform_int_distribution<int> qtyDist(50, 400);
    std::uniform_int_distribution<int> statusDist(0, 4);

    std::vector<Order> orders;
    const std::string  today = DateTimeHelper::todayCompact();
    for (int i = 0; i < nOrders; ++i) {
        Order o;
        char buf[32]; std::snprintf(buf, sizeof(buf), "ORD-%s-%04d", today.c_str(), i + 1);
        o.m_orderId  = buf;
        o.m_sampleId = samples[sampleDist(rng)].m_sampleId;
        o.m_customer = customers[custDist(rng)];
        o.m_quantity = qtyDist(rng);
        o.m_status   = orderStatusFromString(statuses[statusDist(rng)]);
        o.m_createdAt = now;
        o.m_updatedAt = now;
        orders.push_back(o);
    }

    // JSON 출력
    writeJson(outDir + "/samples.json",          nlohmann::json(samples));
    writeJson(outDir + "/orders.json",           nlohmann::json(orders));
    writeJson(outDir + "/production_jobs.json",  nlohmann::json::array());

    std::cout << "생성 완료: samples=" << nSamples
              << ", orders=" << nOrders
              << " -> " << outDir << '\n';
    return 0;
}
