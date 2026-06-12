# PRD: 반도체 시료 생산주문관리 시스템 (S-Semi SampleOrderSystem)

## 1. 개요

### 1.1 배경
가상의 반도체 회사 S-Semi는 다양한 반도체 시료(Sample)를 생산하여 연구소, 팹리스 업체, 대학 연구실 등에 납품한다. 주문량 급증으로 인해 엑셀·메모장 기반 관리의 한계가 드러났고, 체계적인 주문·재고·생산 관리 시스템이 필요해졌다.

### 1.2 목적
- 시료 주문 접수부터 출고까지 전 과정을 콘솔 기반으로 통합 관리
- 주문 상태를 실시간으로 추적하여 담당자 간 혼선을 제거
- 재고·생산라인 현황을 한눈에 파악할 수 있는 모니터링 제공

### 1.3 기술 스택
| 항목 | 선택 |
|---|---|
| 언어 | C++17 |
| IDE | Visual Studio 2022 Community |
| 빌드 시스템 | MSBuild (.sln / .vcxproj) |
| 데이터 영속성 | JSON 파일 (nlohmann/json 헤더 라이브러리) |
| 테스트 | Google Test (gtest) |
| 아키텍처 | MVC (콘솔 기반) |

### 1.4 범위
- 콘솔(CLI) 기반 단일 실행 파일
- MVC 아키텍처 적용
- 단일 생산 라인, FIFO 스케줄링
- 애플리케이션 재실행 후에도 데이터 유지 (JSON 파일 영속성)

---

## 2. 역할 및 흐름

```
고객 ──시료 요청──▶ 주문 담당자 ──주문서 전달──▶ 생산 담당자
                         ▲                           │
                         └──────── 승인 / 거절 ◀─────┘
```

| 역할 | 책임 |
|---|---|
| 고객 | 필요한 시료를 이메일로 요청 |
| 주문 담당자 | 요청에 맞게 주문서 작성 (시스템에서 주문 접수) |
| 생산 담당자 | 시료 등록, 주문 수신 후 승인 또는 거절 |

---

## 3. 주문 상태 흐름

```
RESERVED ──승인──▶ [재고 확인]
                       ├─ 충분 ──▶ CONFIRMED ──▶ RELEASE
                       └─ 부족 ──▶ PRODUCING ──▶ CONFIRMED ──▶ RELEASE
RESERVED ──거절──▶ REJECTED
```

| 상태 | 의미 |
|---|---|
| RESERVED | 주문 접수 (초기 상태) |
| REJECTED | 주문 거절 (모니터링 제외) |
| PRODUCING | 승인 완료, 재고 부족으로 생산 중 |
| CONFIRMED | 승인 완료, 출고 대기 |
| RELEASE | 출고 완료 |

---

## 4. 기능 명세

### 4.1 메인 메뉴
시작 시 전체 시료 요약(등록 수, 총 재고, 전체 주문 수, 생산라인 대기 수)을 표시하고 기능을 선택한다.

| 번호 | 메뉴 | 설명 |
|---|---|---|
| 1 | 시료 관리 | 시료 등록, 목록 조회, 이름 검색 |
| 2 | 시료 주문 | 고객 주문 접수 (RESERVED 생성) |
| 3 | 주문 승인/거절 | RESERVED 주문 승인 또는 거절 처리 |
| 4 | 모니터링 | 상태별 주문 수 + 시료별 재고 현황 |
| 5 | 생산라인 조회 | 현재 생산 중 및 대기 큐 확인 |
| 6 | 출고 처리 | CONFIRMED 주문 출고 실행 |
| 0 | 종료 | 시스템 종료 |

### 4.2 시료 관리

**시료 속성**
| 필드 | 타입 | 설명 |
|---|---|---|
| sample_id | string | 고유 ID (예: S-001) |
| name | string | 시료명 |
| avg_production_time | double | 평균 생산시간 (분/ea) |
| yield_rate | double | 수율 (0.0 ~ 1.0) |
| stock | int | 현재 재고 수량 (ea) |

**기능**
- 시료 등록: 위 속성 입력 후 JSON 저장
- 시료 목록: 전체 시료 + 현재 재고 표시
- 시료 검색: 이름으로 검색

### 4.3 시료 주문 (주문 접수)

**입력값**
- 시료 ID
- 고객명
- 주문 수량

**처리**
- 주문번호 자동 생성: `ORD-YYYYMMDD-NNNN`
- 주문 상태: `RESERVED`로 저장

### 4.4 주문 승인/거절

**승인 처리 로직**
```
if 현재 재고 >= 주문 수량:
    상태 → CONFIRMED, 재고 차감
else:
    부족분 = 주문 수량 - 현재 재고
    실생산량 = ceil(부족분 / (수율 * 0.9))
    생산라인 큐에 등록
    상태 → PRODUCING
```

**거절 처리**
- 즉시 `REJECTED` 상태로 전환

### 4.5 모니터링

**주문량 확인**
- 상태별(RESERVED / CONFIRMED / PRODUCING / RELEASE) 주문 건수 표시
- REJECTED 제외

**재고량 확인**
- 시료별 현재 재고 수량
- 재고 상태 표기:
  - **여유**: 현재 재고 >= CONFIRMED 주문 수량 합산
  - **부족**: 현재 재고 < CONFIRMED 주문 수량 합산
  - **고갈**: 재고 = 0

### 4.6 생산라인

**생산량 계산**
```
부족분 = 주문 수량 - 현재 재고
실생산량 = ceil(부족분 / (수율 * 0.9))
총 생산 시간(분) = 평균 생산시간 * 실생산량
```

**표시 정보**
- 현재 생산 중인 주문 정보 (주문번호, 시료명, 주문량, 부족분, 실생산량, 예상 완료 시간)
- 대기 중인 주문 목록 (FIFO 순)

**생산 완료 처리**
- PRODUCING → CONFIRMED 상태 전환
- 재고에 실생산량 반영 후 재고에서 주문 수량 차감

### 4.7 출고 처리

- CONFIRMED 상태 주문 목록 표시
- 선택한 주문을 RELEASE로 전환
- 처리일시 기록

---

## 5. 데이터 모델 (JSON 스키마)

### samples.json
```json
[
  {
    "sample_id": "S-001",
    "name": "실리콘 웨이퍼-8인치",
    "avg_production_time": 0.5,
    "yield_rate": 0.92,
    "stock": 480
  }
]
```

### orders.json
```json
[
  {
    "order_id": "ORD-20260612-0001",
    "sample_id": "S-001",
    "customer": "삼성전자 파운드리",
    "quantity": 200,
    "status": "RESERVED",
    "created_at": "2026-06-12T09:00:00",
    "updated_at": "2026-06-12T09:00:00"
  }
]
```

### production_jobs.json
```json
[
  {
    "job_id": "JOB-20260612-0001",
    "order_id": "ORD-20260612-0001",
    "sample_id": "S-001",
    "shortage": 170,
    "actual_production": 206,
    "total_time_min": 103.0,
    "status": "RUNNING",
    "created_at": "2026-06-12T09:05:00"
  }
]
```

---

## 6. 프로젝트 아키텍처

### Visual Studio 솔루션 구조
```
SampleOrderSystem.sln
├── SampleOrderSystem/          # 메인 프로젝트
│   ├── main.cpp
│   ├── models/
│   │   ├── Sample.h / Sample.cpp
│   │   ├── Order.h / Order.cpp
│   │   └── ProductionJob.h / ProductionJob.cpp
│   ├── repositories/
│   │   ├── IRepository.h
│   │   ├── SampleRepository.h / SampleRepository.cpp
│   │   ├── OrderRepository.h / OrderRepository.cpp
│   │   └── ProductionRepository.h / ProductionRepository.cpp
│   ├── controllers/
│   │   ├── SampleController.h / SampleController.cpp
│   │   ├── OrderController.h / OrderController.cpp
│   │   ├── ProductionController.h / ProductionController.cpp
│   │   └── ReleaseController.h / ReleaseController.cpp
│   ├── views/
│   │   ├── MainView.h / MainView.cpp
│   │   ├── SampleView.h / SampleView.cpp
│   │   ├── OrderView.h / OrderView.cpp
│   │   ├── MonitoringView.h / MonitoringView.cpp
│   │   ├── ProductionView.h / ProductionView.cpp
│   │   └── ReleaseView.h / ReleaseView.cpp
│   ├── utils/
│   │   ├── JsonHelper.h / JsonHelper.cpp
│   │   └── DateTimeHelper.h / DateTimeHelper.cpp
│   └── data/                   # 런타임 JSON 저장 위치
│       ├── samples.json
│       ├── orders.json
│       └── production_jobs.json
├── SampleOrderSystemTests/     # Google Test 프로젝트
│   ├── test_sample.cpp
│   ├── test_order.cpp
│   └── test_production.cpp
└── third_party/
    └── nlohmann/
        └── json.hpp            # 헤더 전용 JSON 라이브러리
```

### MVC 역할 분리
| 계층 | 역할 |
|---|---|
| Model | 데이터 구조(struct/class) 정의 |
| Repository | JSON 파일 CRUD (영속성 계층) |
| Controller | 비즈니스 로직, 상태 전환, 계산 |
| View | 콘솔 출력 형식, 사용자 입력 수집 |

---

## 7. 비기능 요구사항

- **데이터 영속성**: JSON 파일 저장, 재실행 시 데이터 유지
- **언어**: C++17
- **빌드**: MSBuild (Visual Studio 2022 Community)
- **외부 라이브러리**: nlohmann/json (헤더 전용), Google Test
- **커밋**: 기능 단위로 의미 있는 커밋 이력 유지
- **코드**: CleanCode 원칙, 불필요한 주석 최소화
- **모델**: Claude Sonnet / Effort Medium 사용
