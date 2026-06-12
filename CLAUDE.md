# CLAUDE.md — SampleOrderSystem (반도체 시료 생산주문관리 시스템)

## 이 공간의 역할

이 Repository(`SampleOrderSystem-FJK`)는 **S-Semi 반도체 시료 생산주문관리 시스템의 본 과제 구현 공간**이다.
전체 과제 개요와 요구사항은 `PRD.md` 참조.

---

## [MANDATORY] 빌드 검증 강제 규칙

> **이 규칙은 예외 없이 적용된다. 어떤 상황에서도 우회하지 않는다.**
> Claude는 이 규칙을 지시로 인식하며, 아래 조건이 충족될 때마다 반드시 수행한다.

### 트리거 조건

`.cpp` 또는 `.h` 파일 변경이 완료된 직후 (feature 단위 구현 완료 시점).

### 필수 수행 절차

**[자동]** Stop 훅이 Claude가 작업을 마칠 때마다 `SampleOrderSystemTests`와 `SampleOrderSystem`을 자동으로 병렬 빌드한다.
빌드가 실패하면 훅이 Claude를 재활성화(rewake)하고 FAIL 내용을 전달한다.

**[수동]** 단계 완료 전 명시적 검증이 필요한 경우: `unit-test`, `app-build` 에이전트를 병렬로 직접 호출한다.

두 경우 모두 FAIL이 확인되는 즉시 아래 강제 중단 규칙을 적용한다.

### FAIL 시 강제 중단 규칙

**둘 중 하나라도 FAIL이면 즉시 아래를 수행하고 멈춘다. 예외 없음.**

1. 진행 중이던 모든 작업을 **즉시 중단**한다 — 다음 단계 착수 절대 금지
2. 독단적 수정 시도 금지 — 사용자 허가 없이 오류를 자의적으로 수정하지 않는다
3. 사용자에게 다음을 보고한다:
   - 어떤 빌드가 실패했는지 (`unit-test` / `app-build` / 둘 다)
   - 핵심 오류 메시지 요약 (컴파일 에러, 테스트 실패 항목)
4. 사용자의 명시적 지시가 올 때까지 **대기**한다

### 금지 사항 (절대 금지)

| 금지 행동 | 이유 |
|---|---|
| 빌드 실패 무시하고 다음 단계 진행 | 깨진 상태로 쌓이는 코드는 복구 비용이 기하급수적 |
| 빌드 확인 없이 완료 선언 | 실제 동작을 검증하지 않은 완료는 완료가 아님 |
| 사용자 허가 없이 오류 자의적 수정 후 재시도 | 수정 방향은 사용자가 결정해야 함 |

---

## 과제 배경 및 현재 상태

### 진행 경위
- 총괄 관리 공간(`Project_S-Semi`)에서 전체 과제를 기획하고 PoC 4개를 먼저 완료했다
- PoC-1/2/3 코드는 본 시스템에 **직접 포함**한다. PoC-4(DummyDataGenerator)는 솔루션 내 독립 프로젝트로 분리한다 (PRD.md 8.2, 8.3 참조)
- 이제 PoC에서 검증한 패턴을 바탕으로 본 시스템을 구현한다

### PoC 완료 현황 (GitHub 참조)

| # | PoC | GitHub | 참고할 패턴 |
|---|---|---|---|
| 1 | ConsoleMVC | https://github.com/Final-JK/ConsoleMVC-FJK | MVC 3계층 구조, 단방향 의존성 |
| 2 | DataPersistence | https://github.com/Final-JK/DataPersistence-FJK | nlohmann/json CRUD, IRepository 패턴 |
| 3 | DataMonitor | https://github.com/Final-JK/DataMonitor-FJK | 상태별 집계, 재고 판정, ASCII 바, 한글 정렬 보정 |
| 4 | DummyDataGenerator | https://github.com/Final-JK/DummyDataGenerator-FJK | SampleGenerator, OrderGenerator, JsonWriter |

---

## 개발 환경

> 상세 내용은 `DEV_ENV.md` 참조.

| 항목 | 값 |
|---|---|
| IDE | Visual Studio Community 2026 |
| 설치 경로 | `C:\Program Files\Microsoft Visual Studio\18\Community` |
| MSBuild | `...\18\Community\MSBuild\Current\Bin\amd64\MSBuild.exe` |
| PlatformToolset | `v145` (`v143` 사용 시 빌드 오류 발생) |
| 셸 | `pwsh` (PowerShell 7) |

---

## 아키텍처

### MVC 의존성 방향
```
View → Controller → Repository → Model
```
역방향 의존성 금지. View가 Repository를 직접 호출하지 않는다.

### 솔루션 구조
```
SampleOrderSystem-FJK/
├── SampleOrderSystem.sln
├── SampleOrderSystem/          ← 메인 프로젝트
│   ├── main.cpp
│   ├── models/
│   ├── repositories/
│   ├── controllers/
│   ├── views/
│   ├── utils/
│   └── data/                   ← JSON 파일 저장 위치
│       ├── samples.json
│       ├── orders.json
│       └── production_jobs.json
├── SampleOrderSystemTests/     ← Google Test 프로젝트 (gtest/gmock)
└── DummyDataGenerator/         ← 독립 실행 파일 (더미 데이터 생성)
```

### 계층별 역할
| 계층 | 역할 |
|---|---|
| `models/` | 데이터 구조 정의만 (비즈니스 로직 없음) |
| `repositories/` | JSON 파일 CRUD만 (비즈니스 로직 없음) |
| `controllers/` | 비즈니스 로직, 상태 전환, 계산 |
| `views/` | 콘솔 출력 + 사용자 입력 수집만 (로직 없음) |
| `utils/` | 날짜/시간 포맷, JSON 헬퍼 등 공통 유틸 |

---

## 주문 상태 전환 규칙

```
RESERVED → CONFIRMED  (재고 충분 시 승인, 재고 차감)
RESERVED → PRODUCING  (재고 부족 시 승인 → 생산라인 큐 등록)
RESERVED → REJECTED   (거절)
PRODUCING → CONFIRMED (생산 완료)
CONFIRMED → RELEASE   (출고 처리)
```
위 전환 외의 상태 변경은 허용하지 않는다.

---

## 핵심 비즈니스 공식

```cpp
int shortage        = order.quantity - currentStock;
int actualProd      = (int)std::ceil((double)shortage / (sample.yieldRate * 0.9));
double totalTimeMins = sample.avgProductionTime * actualProd;
```

---

## 코드 규칙

### 네이밍
| 대상 | 규칙 | 예시 |
|---|---|---|
| 클래스/구조체 | PascalCase | `SampleController` |
| 멤버 변수 | camelCase + `m_` | `m_sampleId` |
| 함수 | camelCase | `findById` |
| 상수 | UPPER_SNAKE_CASE | `MAX_STOCK` |
| 파일 | 클래스명과 동일 | `SampleController.h` |

### 일반 규칙
- 주석은 WHY가 비명확할 때만 작성. WHAT 설명 주석 금지
- 헤더에 구현 포함 금지 (인라인/템플릿 제외)
- `using namespace std;` 헤더 파일에서 사용 금지
- Raw pointer 대신 `std::unique_ptr` / `std::shared_ptr` 사용

---

## 빌드 명령

```powershell
# Debug 빌드
& "C:\Program Files\Microsoft Visual Studio\18\Community\MSBuild\Current\Bin\amd64\MSBuild.exe" `
    .\SampleOrderSystem.sln /p:Configuration=Debug /p:Platform=x64

# 클린 빌드
& "C:\Program Files\Microsoft Visual Studio\18\Community\MSBuild\Current\Bin\amd64\MSBuild.exe" `
    .\SampleOrderSystem.sln /t:Clean,Build /p:Configuration=Debug /p:Platform=x64

# DummyDataGenerator 단독 빌드 (models/ 변경 후 수동 검증 시)
& "C:\Program Files\Microsoft Visual Studio\18\Community\MSBuild\Current\Bin\amd64\MSBuild.exe" `
    .\SampleOrderSystem.sln /t:DummyDataGenerator /p:Configuration=Debug /p:Platform=x64
```

---

## Git 커밋 규칙

> 상세 내용은 `GIT_CONVENTION.md` 참조.

```bash
git commit -s -m "[타입] 제목"
```

| 타입 | 의미 |
|---|---|
| `feat` | 새 기능 추가 |
| `fix` | 버그 수정 |
| `refactor` | 코드 구조 개선 |
| `test` | 테스트 추가/수정 |
| `docs` | 문서 수정 |
| `chore` | 빌드 설정, 패키지 등 |

---

## 개발 순서 (미션2 체크리스트)

- [ ] **1단계**: 솔루션 초기 세팅 (SampleOrderSystem / SampleOrderSystemTests / DummyDataGenerator 프로젝트, nlohmann/json, gtest/gmock)
- [ ] **2단계**: Models — `Sample`, `Order`(OrderStatus enum), `ProductionJob`
- [ ] **3단계**: Repositories — `IRepository<T>`, `SampleRepo`, `OrderRepo`, `ProductionRepo`
- [ ] **4단계**: Controllers — `SampleCtrl`, `OrderCtrl`, `ProductionCtrl`, `ReleaseCtrl`
- [ ] **5단계**: Views — `MainView`, `SampleView`, `OrderView`, `MonitoringView`, `ProductionView`, `ReleaseView`
- [ ] **6단계**: 통합 — `main.cpp` 루프, 전체 메뉴 연결
- [ ] **7단계**: 단위 테스트 — `Test_SampleController`, `Test_OrderController`, `Test_ProductionController` 전체 통과

---

## 데이터 파일

파일이 없으면 빈 배열 `[]`로 자동 초기화 (크래시 없음).

### samples.json
```json
[{ "sample_id": "S-001", "name": "실리콘 웨이퍼-8인치", "avg_production_time": 0.5, "yield_rate": 0.92, "stock": 480 }]
```

### orders.json
```json
[{ "order_id": "ORD-20260612-0001", "sample_id": "S-001", "customer": "삼성전자", "quantity": 200, "status": "RESERVED", "created_at": "2026-06-12T09:00:00", "updated_at": "2026-06-12T09:00:00" }]
```

### production_jobs.json
```json
[{ "job_id": "JOB-20260612-0001", "order_id": "ORD-20260612-0001", "sample_id": "S-001", "shortage": 170, "actual_production": 206, "total_time_min": 164.8, "status": "RUNNING", "created_at": "2026-06-12T09:05:00" }]
```
