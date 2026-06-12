# DEVELOPMENT_STATUS.md — 개발 진행 현황

> 이 문서는 세션 간 작업 연속성을 위한 핵심 핸드오프 문서다.
> 새 세션 시작 시 **이 문서를 가장 먼저 읽어라.**
> 작업 완료 시마다 이 문서를 갱신한다.

---

## 현재 진행 상황 (2026-06-12 기준)

> **전체 구현 완료.** 7단계 모두 커밋 완료, 빌드 검증 PASS.

| 단계 | 상태 | 커밋 |
|---|---|---|
| 1단계: 솔루션 세팅 | ✅ 완료 | `[feat] 1단계: 솔루션 세팅` |
| 2단계: Models + IRepository | ✅ 완료 | `[feat] 2단계: Models + IRepository` |
| 3단계: Repositories | ✅ 완료 | `[feat] 3단계: Repositories` |
| 4단계: Controllers + Utils | ✅ 완료 | `[feat] 4단계: Controllers + Utils` |
| 5단계: Views | ✅ 완료 | `[feat] 5단계: Views` |
| 6단계: 통합 (main.cpp) | ✅ 완료 | `[feat] 6단계: 통합` |
| 7단계: 단위 테스트 | ✅ 완료 | `[feat] 7단계: 단위 테스트 (16/16 PASS)` |
| PRD 갭 A: MonitoringView 상태 카운트 | ✅ 완료 | `[feat] 단계A: MonitoringView 상태별 주문 건수 추가` |
| PRD 갭 B: MainView 진입 요약 | ✅ 완료 | `[feat] 단계B: MainView 진입 요약 표시 추가` |
| 콘솔 출력 한글 정렬 | ✅ 완료 | `[style] 콘솔 출력 한글 정렬 보정` |

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

### Models (2단계)
```
SampleOrderSystem/models/Sample.h/.cpp
SampleOrderSystem/models/Order.h/.cpp         ← OrderStatus enum 포함
SampleOrderSystem/models/ProductionJob.h/.cpp ← JobStatus enum 포함
SampleOrderSystem/repositories/IRepository.h  ← 템플릿 인터페이스
```

### Repositories (3단계)
```
SampleOrderSystem/repositories/SampleRepository.h/.cpp
SampleOrderSystem/repositories/OrderRepository.h/.cpp
SampleOrderSystem/repositories/ProductionRepository.h/.cpp
```

### Controllers + Utils (4단계)
```
SampleOrderSystem/utils/DateTimeHelper.h/.cpp
SampleOrderSystem/utils/ConsoleHelper.h/.cpp  ← CJK displayWidth 지원
SampleOrderSystem/controllers/SampleController.h/.cpp
SampleOrderSystem/controllers/OrderController.h/.cpp
SampleOrderSystem/controllers/ProductionController.h/.cpp
SampleOrderSystem/controllers/ReleaseController.h/.cpp
```

### Views (5단계)
```
SampleOrderSystem/views/MainView.h/.cpp
SampleOrderSystem/views/SampleView.h/.cpp
SampleOrderSystem/views/OrderView.h/.cpp
SampleOrderSystem/views/ProductionView.h/.cpp
SampleOrderSystem/views/ReleaseView.h/.cpp
SampleOrderSystem/views/MonitoringView.h/.cpp
```

### 통합 (6단계)
```
SampleOrderSystem/main.cpp
DummyDataGenerator/main.cpp
```

### 단위 테스트 (7단계)
```
SampleOrderSystemTests/test_main.cpp
SampleOrderSystemTests/mocks/MockSampleRepository.h
SampleOrderSystemTests/mocks/MockOrderRepository.h
SampleOrderSystemTests/mocks/MockProductionRepository.h
SampleOrderSystemTests/Test_SampleController.cpp
SampleOrderSystemTests/Test_OrderController.cpp
SampleOrderSystemTests/Test_ProductionController.cpp
```

---

## PRD 대비 설계 변경 사항

| 항목 | PRD 원안 | 실제 구현 | 이유 |
|---|---|---|---|
| `Sample.stock_updated_at` | 없음 | **추가** | 모니터링에서 재고 갱신 시각 표시 필요 |
| `ProductionJob.completed_at` | 없음 | **추가** | 생산 완료 시각 추적 필요 |
| DummyDataGenerator 소스 | 독립 구현 | 본 프로젝트 models 공유 | JSON 포맷 일관성 보장 |
| `.gitignore` | `*.pdf`만 제외 | x64/, .vs/, 빌드 아티팩트 패턴 추가 | 빌드 산출물이 git에 포함되는 문제 발견 후 보완 |
| gtest/gmock `custom/` 헤더 | third_party 배포 시 포함 가정 | 5개 파일 수동 추가 | PoC는 NuGet 방식 — 본 프로젝트 amalgamation 방식에서 파일 누락 확인 |
| Repository의 날짜 기반 ID 생성 | `DateTimeHelper` 의존 예정 | Repository .cpp 내부 로컬 헬퍼로 처리 | DateTimeHelper는 4단계 산출물이므로 3단계에서 의존하면 계층 역전 발생 |
| MonitoringView 주문 현황 | 미명세 | 상태별(RESERVED/CONFIRMED/PRODUCING/RELEASE) 건수 헤더 추가 | PRD 4.5 요구사항 구현 |
| MainView 메인 메뉴 요약 | 미명세 | 시료 종수·총 재고·주문 건수·생산 진행 건수 한 줄 표시 | PRD 4.1 요구사항 구현 |
| 콘솔 출력 컬럼 너비 | 미명세 | CJK displayWidth 기준으로 모든 View padRight 재조정 | 한글 2-width 미반영 시 헤더/데이터 정렬 불일치 |

---

## 핵심 구현 패턴 (참조용)

### include 경로 규칙
- `SampleOrderSystem.vcxproj` AdditionalIncludeDirectories에 `$(SolutionDir)third_party;$(ProjectDir)` 포함
- 소스 파일에서 `#include "models/Sample.h"` (ProjectDir 기준)
- nlohmann: `#include <nlohmann/json.hpp>`

### 파일 저장 경로
- 실행 파일 기준 상대 경로: `data/samples.json`, `data/orders.json`, `data/production_jobs.json`
- 실행 파일 위치: `x64/Debug/SampleOrderSystem.exe`
- 따라서 데이터 파일: `x64/Debug/data/*.json`

### Atomic write 패턴
```cpp
const std::string tmpPath = m_filePath + ".tmp";
{ std::ofstream ofs(tmpPath); ofs << j.dump(2); }
std::error_code ec;
std::filesystem::rename(tmpPath, m_filePath, ec);
return !ec;
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
