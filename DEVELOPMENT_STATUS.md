# DEVELOPMENT_STATUS.md — 개발 진행 현황

> 이 문서는 세션 간 작업 연속성을 위한 핵심 핸드오프 문서다.
> 새 세션 시작 시 **이 문서를 가장 먼저 읽어라.**
> 작업 완료 시마다 이 문서를 갱신한다.

---

## 현재 진행 상황 (2026-06-12 기준)

| 단계 | 상태 | 커밋 |
|---|---|---|
| 1단계: 솔루션 세팅 | ✅ 완료 | `[feat] 1단계: 솔루션 세팅` |
| 2단계: Models + IRepository | 🔶 파일 작성 완료, 커밋 대기 | 다음 세션에서 빌드 검증 후 커밋 |
| 3단계: Repositories | ✅ 완료 | `[feat] 3단계: Repositories` |
| 4단계: Controllers | ✅ 완료 | `[feat] 4단계: Controllers + Utils` |
| 5단계: Views | ✅ 완료 | `[feat] 5단계: Views` |
| 6단계: 통합 (main.cpp) | ✅ 완료 | `[feat] 6단계: 통합` |
| 7단계: 단위 테스트 | ✅ 완료 | `[feat] 7단계: 단위 테스트 (16/16 PASS)` |

---

## 완료된 파일 목록

### 솔루션/빌드 인프라
```
SampleOrderSystem.sln
SampleOrderSystem/SampleOrderSystem.vcxproj
SampleOrderSystemTests/SampleOrderSystemTests.vcxproj
DummyDataGenerator/DummyDataGenerator.vcxproj
third_party/nlohmann/json.hpp
third_party/googletest/googletest/include/gtest/*.h
third_party/googletest/googletest/src/gtest-all.cc
third_party/googletest/googlemock/include/gmock/*.h
third_party/googletest/googlemock/src/gmock-all.cc
```

### Models (2단계 완료)
```
SampleOrderSystem/models/Sample.h
SampleOrderSystem/models/Sample.cpp
SampleOrderSystem/models/Order.h
SampleOrderSystem/models/Order.cpp
SampleOrderSystem/models/ProductionJob.h
SampleOrderSystem/models/ProductionJob.cpp
```

### Repositories (2단계 일부 — 미커밋)
```
SampleOrderSystem/repositories/IRepository.h   ← 작성 완료 (미커밋)
```

### 2단계 미커밋 파일 (다음 세션에서 빌드 검증 후 커밋)
```
SampleOrderSystem/models/Sample.h/.cpp     ← stock_updated_at 필드 포함
SampleOrderSystem/models/Order.h/.cpp      ← OrderStatus enum (RESERVED/CONFIRMED/PRODUCING/REJECTED/RELEASE)
SampleOrderSystem/models/ProductionJob.h/.cpp  ← completed_at 필드 포함
SampleOrderSystem/repositories/IRepository.h
```

---

## 다음 세션에서 해야 할 일

### 즉시 착수: 3단계 — Repositories

아래 6개 파일을 작성한다.

**SampleRepository.h / SampleRepository.cpp**
- `IRepository<Sample>` 구현
- ID 자동 생성: `S-001`, `S-002`, ...
- `load()`: `data/samples.json` 읽기 (없으면 빈 배열로 초기화)
- `flush()`: atomic write (`.tmp` → rename)

**OrderRepository.h / OrderRepository.cpp**
- `IRepository<Order>` 구현
- ID 자동 생성: `ORD-YYYYMMDD-NNNN`
- `load()`: `data/orders.json` 읽기

**ProductionRepository.h / ProductionRepository.cpp**
- `IRepository<ProductionJob>` 구현
- ID 자동 생성: `JOB-YYYYMMDD-NNNN`
- `load()`: `data/production_jobs.json` 읽기

**3단계 완료 후 → 빌드 검증 → 커밋**

---

### 이후: 4단계 — Controllers

**Utils 먼저 작성** (Controllers가 의존):

`SampleOrderSystem/utils/DateTimeHelper.h/.cpp`
```cpp
class DateTimeHelper {
public:
    static std::string nowIso();       // "2026-06-12T09:00:00"
    static std::string todayCompact(); // "20260612"
};
```

`SampleOrderSystem/utils/ConsoleHelper.h/.cpp`
```cpp
class ConsoleHelper {
public:
    static int displayWidth(const std::string& s);  // CJK=2, ASCII=1
    static std::string padRight(const std::string& s, int targetWidth);
    static std::string padLeft(const std::string& s, int targetWidth);
};
```

**그 다음 Controller 4개 작성:**

`SampleController.h/.cpp` — 생성자: `IRepository<Sample>*`
- `getAllSamples()` → `vector<Sample>`
- `findSample(id)` → `optional<Sample>`
- `searchByName(keyword)` → `vector<Sample>`
- `addSample(name, avgProdTime, yieldRate, initialStock)` → `bool`

`OrderController.h/.cpp` — 생성자: `IRepository<Order>*, IRepository<Sample>*, IRepository<ProductionJob>*`
- `getReservedOrders()` → `vector<Order>`
- `getAllOrders()` → `vector<Order>`
- `createOrder(sampleId, customer, quantity)` → `bool`
- `approve(orderId)` → `bool` (재고 확인 후 CONFIRMED 또는 PRODUCING)
- `reject(orderId)` → `bool`

승인 로직:
```cpp
if (stock >= quantity) {
    order.status = CONFIRMED; sample.stock -= quantity;
    sample.stockUpdatedAt = DateTimeHelper::nowIso();
} else {
    int shortage = quantity - stock;
    int actualProd = (int)ceil(shortage / (yieldRate * 0.9));
    double totalMins = avgProdTime * actualProd;
    // ProductionJob 생성, status = PRODUCING
}
```

`ProductionController.h/.cpp` — 생성자: `IRepository<ProductionJob>*, IRepository<Order>*, IRepository<Sample>*`
- `getAllJobs()` → `vector<ProductionJob>`
- `getRunningJobs()` → `vector<ProductionJob>`
- `completeJob(jobId)` → `bool`
  - job.status → COMPLETED, job.completedAt = now
  - sample.stock += actualProduction − orderQuantity
  - sample.stockUpdatedAt = now
  - order.status → CONFIRMED

`ReleaseController.h/.cpp` — 생성자: `IRepository<Order>*`
- `getConfirmedOrders()` → `vector<Order>`
- `release(orderId)` → `bool` (status → RELEASE, updatedAt = now)

**4단계 완료 후 → 빌드 검증 → 커밋**

---

### 이후: 5단계 — Views

**6개 View 파일** (각 h/cpp):

| View | 의존 Controller |
|---|---|
| `SampleView` | `SampleController` |
| `OrderView` | `OrderController` |
| `ProductionView` | `ProductionController` |
| `ReleaseView` | `ReleaseController` |
| `MonitoringView` | `SampleController`, `OrderController` |
| `MainView` | 모든 View + Controller (summary 표시용) |

각 View는 Controller 포인터를 생성자에서 받는다.

모니터링 뷰에서 재고 상태 판정:
- **여유**: `sample.stock >= CONFIRMED 주문 합산`
- **부족**: `sample.stock < CONFIRMED 주문 합산`
- **고갈**: `sample.stock == 0`

ConsoleHelper 활용하여 한글 컬럼 정렬 처리 (CJK 2칸 보정).

**5단계 완료 후 → 빌드 검증 → 커밋**

---

### 이후: 6단계 — 통합 (main.cpp + DummyDataGenerator)

`SampleOrderSystem/main.cpp`:
```cpp
#ifdef _WIN32
#include <windows.h>
#endif
// SetConsoleOutputCP(CP_UTF8); SetConsoleCP(CP_UTF8);
// 데이터 디렉토리: "data/"
// SampleRepository, OrderRepository, ProductionRepository 생성
// Controller 생성 (repo 주입)
// View 생성 (ctrl 주입)
// MainView::run() 루프
```

`DummyDataGenerator/main.cpp`:
- Sample 목록(랜덤 10종), Order 목록(랜덤 30건), ProductionJob 0건 생성
- models의 to_json/from_json ADL 활용
- 출력 경로: `../SampleOrderSystem/data/` (기본값)
- CLI 인수: `--samples N --orders N --out PATH --seed N`

**6단계 완료 후 → 빌드 검증(app-build 스모크 포함) → 커밋**

---

### 최종: 7단계 — 단위 테스트

**테스트 파일 3종 + Mock 3종:**

`SampleOrderSystemTests/mocks/MockSampleRepository.h`
```cpp
class MockSampleRepository : public IRepository<Sample> {
    MOCK_METHOD(bool, save, (Sample&), (override));
    MOCK_METHOD((std::optional<Sample>), findById, (const std::string&), (const, override));
    MOCK_METHOD((std::vector<Sample>), findAll, (), (const, override));
    MOCK_METHOD(bool, remove, (const std::string&), (override));
    MOCK_METHOD(bool, load, (), (override));
    MOCK_METHOD(bool, flush, (), (override));
};
```
`MockOrderRepository.h`, `MockProductionRepository.h` 동일 패턴.

`Test_SampleController.cpp` — `SampleControllerTest` 클래스:
- getAllSamples 반환값 검증
- searchByName 필터링 검증
- addSample 저장 호출 검증

`Test_OrderController.cpp` — `OrderControllerTest` 클래스:
- approve: 재고 충분 → CONFIRMED + 재고 차감
- approve: 재고 부족 → PRODUCING + ProductionJob 생성
- reject: REJECTED 전환

`Test_ProductionController.cpp` — `ProductionControllerTest` 클래스:
- completeJob: COMPLETED + 재고 반영 + 주문 CONFIRMED

`SampleOrderSystemTests/test_main.cpp`:
```cpp
#include <gtest/gtest.h>
#include <gmock/gmock.h>
int main(int argc, char** argv) {
    ::testing::InitGoogleMock(&argc, argv);
    return RUN_ALL_TESTS();
}
```

**7단계 완료 후 → 빌드 검증(unit-test 전체 PASS) → 커밋**

---

## PRD 대비 설계 변경 사항 (중요!)

| 항목 | PRD 원안 | 실제 구현 | 이유 |
|---|---|---|---|
| `Sample.stock_updated_at` | 없음 | **추가** | 모니터링에서 재고 갱신 시각 표시 필요 |
| `ProductionJob.completed_at` | 없음 | **추가** | 생산 완료 시각 추적 필요 |
| DummyDataGenerator 소스 | 독립 구현 | 본 프로젝트 models 공유 | JSON 포맷 일관성 보장 |
| `.gitignore` | `*.pdf`만 제외 | **x64/, .vs/, 빌드 아티팩트 패턴 추가** | 빌드 산출물이 git에 포함되는 문제 발견 후 보완 |
| gtest/gmock `custom/` 헤더 | third_party 배포 시 포함 가정 | **5개 파일 수동 추가** (`gtest-port.h`, `gtest-printers.h`, `gmock-port.h`, `gmock-matchers.h`, `gmock-generated-actions.h`) | PoC에서는 NuGet 방식 사용 — 본 프로젝트 amalgamation 방식에서는 해당 파일이 누락됨을 런타임에 발견 |
| Repository의 날짜 기반 ID 생성 | `DateTimeHelper` 의존 예정 | **Repository .cpp 내부 로컬 헬퍼로 처리** | `DateTimeHelper`는 4단계(utils) 산출물이므로 3단계 Repository가 의존하면 계층 역전 발생. 로컬 `static std::string todayCompact()` 인라인으로 의존성 제거 |

---

## 핵심 구현 패턴 (다음 세션 참조용)

### include 경로 규칙
- `SampleOrderSystem.vcxproj` AdditionalIncludeDirectories에 `$(SolutionDir)third_party;$(ProjectDir)` 포함
- 소스 파일에서 `#include "models/Sample.h"` (ProjectDir 기준)
- nlohmann: `#include <nlohmann/json.hpp>`

### 파일 저장 경로
- 실행 파일 기준 상대 경로: `data/samples.json`, `data/orders.json`, `data/production_jobs.json`
- 실행 파일 위치: `x64/Debug/SampleOrderSystem.exe` (솔루션 루트 기준)
- 따라서 데이터 파일: `x64/Debug/data/*.json`

### Atomic write 패턴
```cpp
const std::string tmpPath = m_filePath + ".tmp";
{ std::ofstream ofs(tmpPath); ofs << j.dump(2); }
std::error_code ec;
std::filesystem::rename(tmpPath, m_filePath, ec);
return !ec;
```

### ID 자동 생성 패턴
```cpp
// SampleRepository
char buf[16];
std::snprintf(buf, sizeof(buf), "S-%03d", m_nextId++);

// OrderRepository
char buf[32];
std::snprintf(buf, sizeof(buf), "ORD-%s-%04d",
    DateTimeHelper::todayCompact().c_str(), m_nextSeq++);

// ProductionRepository
char buf[32];
std::snprintf(buf, sizeof(buf), "JOB-%s-%04d",
    DateTimeHelper::todayCompact().c_str(), m_nextSeq++);
```

### save() insert vs update 판별
```cpp
bool save(T& entity) override {
    if (entity.m_XxxId.empty()) {
        entity.m_XxxId = generateId();  // insert
    } else {
        auto it = std::find_if(...);
        if (it != m_data.end()) { *it = entity; return flush(); }  // update
        // ID 있지만 없으면 insert
    }
    m_data.push_back(entity);
    return flush();
}
```

### 생산량 계산 공식 (OrderController::approve 내부)
```cpp
int shortage     = order.m_quantity - sample.m_stock;
int actualProd   = (int)std::ceil((double)shortage / (sample.m_yieldRate * 0.9));
double totalMins = sample.m_avgProductionTime * actualProd;
```

---

## 빌드 검증 명령

```powershell
# 전체 빌드
& "C:\Program Files\Microsoft Visual Studio\18\Community\MSBuild\Current\Bin\amd64\MSBuild.exe" `
    .\SampleOrderSystem.sln /p:Configuration=Debug /p:Platform=x64 /v:minimal

# 앱 스모크 테스트
echo "0" | ./x64/Debug/SampleOrderSystem.exe

# 유닛 테스트 실행
./x64/Debug/SampleOrderSystemTests.exe --gtest_color=yes
```
