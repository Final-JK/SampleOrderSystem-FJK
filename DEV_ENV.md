# DEV_ENV.md — 개발 환경 설정

> 이 파일은 모든 Repository에 복사하여 사용한다.
> Claude Code는 빌드 및 실행 명령 수행 시 이 문서의 경로와 도구를 사용한다.

---

## IDE

| 항목 | 값 |
|---|---|
| IDE | **Visual Studio Community 2026** |
| 표시 이름 | `Visual Studio Community 2026` |
| 버전 | `18.6.11828.311` |
| 설치 경로 | `C:\Program Files\Microsoft Visual Studio\18\Community` |
| 솔루션 파일 | `*.sln` |
| 프로젝트 파일 | `*.vcxproj` |

> 설치 경로가 `\2026\` 이 아닌 **`\18\`** 임에 주의한다. (VS 버전 번호 기준)

---

## 버전별 경로 및 툴셋 비교

| Visual Studio | 설치 경로 | PlatformToolset | MSVC 버전 |
|---|---|---|---|
| VS 2019 | `\2019\` | `v142` | 14.2x |
| VS 2022 | `\2022\` | `v143` | 14.3x |
| **VS 2026 (현재)** | **`\18\`** | **`v145`** | **14.51** |

> `.vcxproj` 작성 시 `<PlatformToolset>v145</PlatformToolset>` 을 사용한다.
> `v143` 등 이전 버전 툴셋은 이 환경에서 동작하지 않는다.

---

## 빌드 도구

### MSBuild

| 항목 | 값 |
|---|---|
| 실행 파일 | `C:\Program Files\Microsoft Visual Studio\18\Community\MSBuild\Current\Bin\amd64\MSBuild.exe` |
| MSBuild 버전 | `18.6.3` |
| 기본 플랫폼 | `x64` |
| 기본 구성 | `Debug` |
| 플랫폼 툴셋 | `v145` (MSVC 14.51.36231) |

**기본 빌드 명령**
```powershell
& "C:\Program Files\Microsoft Visual Studio\18\Community\MSBuild\Current\Bin\amd64\MSBuild.exe" `
    .\SolutionName.sln `
    /p:Configuration=Debug `
    /p:Platform=x64
```

**Release 빌드**
```powershell
& "C:\Program Files\Microsoft Visual Studio\18\Community\MSBuild\Current\Bin\amd64\MSBuild.exe" `
    .\SolutionName.sln `
    /p:Configuration=Release `
    /p:Platform=x64
```

**클린 빌드**
```powershell
& "C:\Program Files\Microsoft Visual Studio\18\Community\MSBuild\Current\Bin\amd64\MSBuild.exe" `
    .\SolutionName.sln `
    /t:Clean,Build `
    /p:Configuration=Debug `
    /p:Platform=x64
```

---

## 셸 환경

| 항목 | 값 |
|---|---|
| 셸 | PowerShell 7 (`pwsh`) |
| 실행 파일 | `pwsh.exe` |

> `powershell.exe` (Windows PowerShell 5.x) 가 아닌 `pwsh.exe` (PowerShell 7+) 를 사용한다.

**PowerShell 7 버전 확인**
```powershell
pwsh --version
```

---

## Claude Code 사용 시 주의

- 빌드 명령 실행 시 위의 MSBuild 전체 경로를 사용한다
- `.vcxproj` 생성 시 반드시 `<PlatformToolset>v145</PlatformToolset>` 을 사용한다
- 셸 명령은 `pwsh` 기준으로 작성한다 (`powershell` 사용 금지)
- 솔루션 파일명은 각 프로젝트의 실제 `.sln` 파일명으로 대체한다
