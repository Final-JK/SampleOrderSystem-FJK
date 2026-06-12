# SampleOrderSystem — 반도체 시료 생산주문관리 시스템

반도체 시료(Sample)의 주문 접수부터 생산·출고까지 전 과정을 관리하는 콘솔 기반 시스템.  
C++17 / MVC 아키텍처 / nlohmann·json / Google Test로 구성된 Visual Studio 솔루션이다.

---

## 목차

1. [솔루션 구성](#1-솔루션-구성)
2. [공통 구성 요소](#2-공통-구성-요소)
3. [아키텍처](#3-아키텍처)
4. [주문 상태 흐름](#4-주문-상태-흐름)
5. [개발 환경 및 사전 조건](#5-개발-환경-및-사전-조건)
6. [빌드 방법](#6-빌드-방법)
7. [실행 방법](#7-실행-방법)
8. [테스트](#8-테스트)
9. [사용 시나리오](#9-사용-시나리오)

---

## 1. 솔루션 구성

`SampleOrderSystem.sln` 하나의 솔루션 파일 아래 세 개의 독립 프로젝트가 포함된다.

| 프로젝트 | 출력 파일 | 역할 |
|---|---|---|
| `SampleOrderSystem` | `SampleOrderSystem.exe` | 메인 관리 시스템 (MVC 콘솔 앱) |
| `SampleOrderSystemTests` | `SampleOrderSystemTests.exe` | Google Test 단위 테스트 실행 파일 |
| `DummyDataGenerator` | `DummyDataGenerator.exe` | 테스트용 더미 데이터 생성 도구 |

세 프로젝트 모두 `x64/Debug/` 또는 `x64/Release/` 디렉터리에 바이너리를 출력한다.

---

<details id="2-공통-구성-요소">
<summary><strong>2. 공통 구성 요소</strong></summary>

### 2-1. 서드파티 라이브러리 (`third_party/`)

| 라이브러리 | 방식 | 용도 |
|---|---|---|
| [nlohmann/json](https://github.com/nlohmann/json) v3.11.3 | 헤더 전용 (`json.hpp` 1파일) | JSON 직렬화 / 역직렬화 |
| [Google Test](https://github.com/google/googletest) v1.14.0 | amalgamation (`gtest-all.cc`, `gmock-all.cc`) | 단위 테스트 / 목 객체 |

라이브러리를 솔루션 내부에 vendoring한 이유는 별도 패키지 관리자 없이 `git clone` 한 번으로 즉시 빌드 가능하게 하기 위해서다.

### 2-2. 데이터 모델 공유 (`SampleOrderSystem/models/`)

`SampleOrderSystem` 프로젝트의 모델 파일(`Sample`, `Order`, `ProductionJob`)을 `DummyDataGenerator`가 직접 참조한다.

```
SampleOrderSystem/models/   ←── SampleOrderSystem 빌드에 포함
                            ←── DummyDataGenerator 빌드에도 포함 (추가 include 경로)
```

같은 모델 코드에서 `to_json` / `from_json`이 정의되므로, 생성된 더미 JSON과 메인 앱이 읽는 JSON의 스키마가 항상 일치한다.

### 2-3. 데이터 파일 (`SampleOrderSystem/data/`)

세 개의 JSON 파일이 영속 계층 역할을 한다. 파일이 없으면 빈 배열(`[]`)로 자동 초기화된다.

| 파일 | 내용 |
|---|---|
| `data/samples.json` | 시료 목록 (ID, 이름, 생산시간, 수율, 재고) |
| `data/orders.json` | 주문 목록 (주문 ID, 시료 ID, 고객사, 수량, 상태) |
| `data/production_jobs.json` | 생산 작업 목록 (JOB ID, 부족량, 실생산량, 예상 시간, 상태) |

> 실행 파일 기준 상대 경로 `data/`를 사용하므로, `x64/Debug/` 또는 `x64/Release/` 하위에 `data/` 폴더가 위치해야 한다.

</details>

---

<details id="3-아키텍처">
<summary><strong>3. 아키텍처</strong></summary>

```
View  →  Controller  →  Repository  →  Model
```

계층 간 의존성은 단방향이다. View는 Repository를 직접 호출하지 않으며, Model은 어떤 계층도 의존하지 않는다.

| 계층 | 책임 |
|---|---|
| **Model** | 데이터 구조 정의 + JSON 직렬화 규칙 |
| **Repository** | JSON 파일 CRUD. `IRepository<T>` 인터페이스로 추상화 |
| **Controller** | 비즈니스 로직 (재고 확인, 상태 전환, 생산량 계산) |
| **View** | 콘솔 출력 형식 + 사용자 입력 수집 |

Controller는 `IRepository<T>*`(인터페이스 포인터)를 생성자에서 주입받으므로, 단위 테스트 시 실제 Repository 대신 Mock을 주입할 수 있다.

</details>

---

## 4. 주문 상태 흐름

```
주문 접수 ──────────────────────── RESERVED
                                       │
                    ┌──────────────────┤──────────────────┐
                승인 │ (재고 충분)      │ (재고 부족)        │ 거절
                    ▼                  ▼                   ▼
               CONFIRMED          PRODUCING           REJECTED
               (출고 대기)         (생산 진행)
                    ▲                  │ 생산 완료
                    └──────────────────┘
                    │ 출고 처리
                    ▼
                RELEASE
```

**핵심 계산 공식 (재고 부족 시 승인):**

```
부족분     = 주문 수량 - 현재 재고
실생산량   = ceil(부족분 / (수율 × 0.9))
총 시간(분) = 평균 생산시간 × 실생산량
```

---

<details id="5-개발-환경-및-사전-조건">
<summary><strong>5. 개발 환경 및 사전 조건</strong></summary>

| 항목 | 값 |
|---|---|
| OS | Windows 10 / 11 |
| IDE | Visual Studio Community 2026 |
| PlatformToolset | `v145` |
| C++ 표준 | C++17 |
| 추가 설치 | 없음 (라이브러리 내장) |

> **중요**: PlatformToolset을 `v143`으로 변경하면 빌드 오류가 발생한다. 솔루션을 열었을 때 툴셋 업그레이드 팝업이 나오면 **취소**를 선택한다.

</details>

---

<details id="6-빌드-방법">
<summary><strong>6. 빌드 방법</strong></summary>

### Visual Studio IDE

`SampleOrderSystem.sln`을 열고 **빌드 > 솔루션 빌드** (`Ctrl+Shift+B`).

### MSBuild (명령줄)

```powershell
$msbuild = "C:\Program Files\Microsoft Visual Studio\18\Community\MSBuild\Current\Bin\amd64\MSBuild.exe"

# 전체 솔루션 빌드
& $msbuild .\SampleOrderSystem.sln /p:Configuration=Debug /p:Platform=x64

# 메인 앱만 빌드
& $msbuild .\SampleOrderSystem.sln /t:SampleOrderSystem /p:Configuration=Debug /p:Platform=x64

# 테스트 프로젝트만 빌드
& $msbuild .\SampleOrderSystem.sln /t:SampleOrderSystemTests /p:Configuration=Debug /p:Platform=x64

# 더미 데이터 생성기만 빌드
& $msbuild .\SampleOrderSystem.sln /t:DummyDataGenerator /p:Configuration=Debug /p:Platform=x64
```

빌드 성공 시 출력 위치:

```
x64/Debug/
├── SampleOrderSystem.exe
├── SampleOrderSystemTests.exe
└── DummyDataGenerator.exe
```

</details>

---

<details id="7-실행-방법">
<summary><strong>7. 실행 방법</strong></summary>

### 메인 앱

```powershell
.\x64\Debug\SampleOrderSystem.exe
```

콘솔이 UTF-8(Code Page 65001)로 설정되어 있지 않으면 시작 시 아래와 같은 경고가 출력된다.

```
[WARNING] Console encoding is not UTF-8 (Current Code Page: 949)
...
How to fix:
  Option 1: Run before launching:  chcp 65001
  Option 2: Use Windows Terminal (defaults to UTF-8).
```

경고 이후 자동으로 UTF-8 전환을 시도한다. 한글이 깨지는 경우 `chcp 65001` 또는 Windows Terminal 사용을 권장한다.

### 더미 데이터 생성기

```powershell
# 기본값: 시료 10종, 주문 30건, 시드 42 → ../SampleOrderSystem/data/ 출력
.\x64\Debug\DummyDataGenerator.exe

# 옵션 지정
.\x64\Debug\DummyDataGenerator.exe --samples 5 --orders 20 --seed 1234 --out .\x64\Debug\data
```

| 옵션 | 기본값 | 설명 |
|---|---|---|
| `--samples N` | 10 | 생성할 시료 수 |
| `--orders N` | 30 | 생성할 주문 수 |
| `--seed N` | 42 | 난수 시드 (재현성 보장) |
| `--out PATH` | `../SampleOrderSystem/data` | JSON 출력 디렉터리 |

</details>

---

## 8. 테스트

### 단위 테스트 실행

```powershell
.\x64\Debug\SampleOrderSystemTests.exe
.\x64\Debug\SampleOrderSystemTests.exe --gtest_color=yes   # 컬러 출력
.\x64\Debug\SampleOrderSystemTests.exe --gtest_verbose     # 상세 출력
```

### 테스트 구성

| 테스트 파일 | 대상 | 케이스 수 |
|---|---|---|
| `Test_SampleController.cpp` | SampleController | 5 |
| `Test_OrderController.cpp` | OrderController | 7 |
| `Test_ProductionController.cpp` | ProductionController | 4 |
| **합계** | | **16** |

**테스트 설계 원칙**: Controller 생성자가 `IRepository<T>*` 인터페이스를 받으므로, 실제 파일 I/O 없이 `MockXxxRepository`(Google Mock)를 주입하여 순수 비즈니스 로직만 검증한다.

주요 검증 항목:
- 재고 충분 시 승인 → `CONFIRMED` 전환 + 재고 차감
- 재고 부족 시 승인 → `PRODUCING` 전환 + `ProductionJob` 생성
- 생산 완료 → `CONFIRMED` 전환 + 재고 반영
- 존재하지 않는 ID / 잘못된 상태 전환 요청 시 `false` 반환

---

## 9. 사용 시나리오

아래는 시스템의 전체 흐름을 처음부터 끝까지 거치는 예시 시나리오다.

### Step 0. 더미 데이터로 시작하기 (선택)

```powershell
.\x64\Debug\DummyDataGenerator.exe --out .\x64\Debug\data
.\x64\Debug\SampleOrderSystem.exe
```

데이터 파일이 없어도 빈 상태로 시작할 수 있다.

---

### Step 1. 시료 등록 (`메뉴 1 → 시료 관리`)

1. `3. 시료 등록` 선택
2. 시료명, 평균 생산시간(h), 수율(0.0~1.0), 초기 재고 입력
3. 등록 완료 → `1. 전체 목록`으로 확인

```
[시료 관리]
  1. 전체 목록
  2. 이름 검색
  3. 시료 등록
  선택: 3

  시료명: 실리콘 웨이퍼-8인치
  평균 생산시간(h): 0.5
  수율(0.0~1.0): 0.92
  초기 재고: 100
  등록 완료.
```

---

### Step 2. 주문 접수 (`메뉴 2 → 주문 관리`)

1. `3. 주문 등록` 선택
2. 시료 ID, 고객사명, 주문 수량 입력
3. 주문 상태 `RESERVED`로 저장됨

```
  시료 ID: S-001
  고객사: 삼성전자
  주문 수량: 80
  주문 등록 완료.
```

---

### Step 3-A. 승인 — 재고 충분 (`메뉴 2 → 4. 승인/거절`)

1. 대기 주문 목록 확인
2. 주문 ID 입력 후 `1. 승인` 선택
3. 재고(100) ≥ 주문량(80) → `CONFIRMED`로 전환, 재고 20으로 차감

```
  주문 ID: ORD-20260612-0001
  1.승인  2.거절: 1
  처리 완료.
```

---

### Step 3-B. 승인 — 재고 부족 (생산라인 등록)

1. 동일 시나리오에서 주문량이 재고를 초과하는 경우
2. 재고(20) < 주문량(150) → `PRODUCING`으로 전환
3. 부족분 130, 실생산량 `ceil(130 / (0.92 × 0.9))` = 157ea, 예상 시간 78.5분으로 생산 JOB 등록

---

### Step 4. 생산 완료 처리 (`메뉴 3 → 생산 관리`)

1. `3. 생산 완료 처리` 선택
2. 진행 중 JOB 목록에서 JOB ID 입력
3. JOB `COMPLETED` 전환, 재고에 실생산량 반영, 해당 주문 `CONFIRMED`로 전환

```
  완료 처리할 JOB ID: JOB-20260612-0001
  생산 완료 처리됨.
```

---

### Step 5. 출고 처리 (`메뉴 4 → 출고 관리`)

1. `2. 출고 처리` 선택
2. `CONFIRMED` 주문 목록에서 주문 ID 입력
3. 상태 `RELEASE`로 전환, 처리 일시 기록

---

### Step 6. 재고 모니터링 (`메뉴 5`)

주문 현황(상태별 건수)과 시료별 재고를 한눈에 확인한다.

```
[주문 현황]
  대기(RESERVED): 2   확정(CONFIRMED): 5   생산중(PRODUCING): 1   출고(RELEASE): 3

ID      이름                      재고    확정량  상태    바
--------------------------------------------------------------------
S-001   실리콘 웨이퍼-8인치        20      80      부족    [####................]
S-002   갈륨비소 기판              350     100     여유    [####################]
```

