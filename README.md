# bento-kit-cpp

[![CI](https://github.com/imcooder/bento-kit-cpp/actions/workflows/ci.yml/badge.svg)](https://github.com/imcooder/bento-kit-cpp/actions/workflows/ci.yml)

A **header-only** C++ utility library. Modules are added incrementally so new projects can **cold-start** faster — less boilerplate, fewer ad-hoc dependencies.

Currently available: **`bento::id`** (IDs & random helpers) and **`bento::mask`** (log-safe redaction). More modules (timing, strings, paths, …) will follow as needed.

## Principles

- **One library, many modules** — `#include <bento/...>`; pull in only what you use
- **Header-only** — easy to integrate via vcpkg, `add_subdirectory`, or `find_package`
- **Minimal third-party deps** — core modules rely on C++17 only
- **Thread safety** — concurrency behavior is documented per API
- **Pragmatic APIs** — built for real project tasks, not heavy abstraction layers

## Layout

```text
src/           # all source (headers, tests, examples)
  bento/       # public headers  →  #include <bento/...>
  tests/
  examples/
resource/      # static assets
output/        # build artifacts (gitignored)
  build/       # CMake cache  →  rm -rf output to clean
  bin/
  lib/
cmake/         # CMake package config templates
ports/         # vcpkg port
```

## Naming

| Kind | Rule | Example |
|------|------|---------|
| Namespace | `bento::<module>` | `bento::id`, `bento::mask` |
| Functions | lowerCamelCase | `generateSessionId`, `maskPhone` |
| Types | PascalCase | `TimeUse` (planned) |
| Members | `m_` + lowerCamelCase | `m_prefix` |
| Constants | `k` + PascalCase | `kDefaultNanoidLen` |

**Product vs package names** (see `scripts/release-names.mjs`):

| Context | Name | Example |
|---------|------|---------|
| Product / GitHub Release / tarball / vcpkg / CMake | `bento-kit` | `bento-kit v0.1.42`, `vcpkg install bento-kit`, `find_package(bento-kit)` |
| CMake target | `bento::kit` | `target_link_libraries(my_app PRIVATE bento::kit)` |
| GitHub repo | `bento-kit-cpp` | unchanged |

### Platform support

| Platform | vcpkg / API | Notes |
|----------|-------------|-------|
| Windows desktop | `x64-windows`, `arm64-windows`, static variants | Full API; UTF-8 + `wstring` mask overloads |
| UWP | not supported | `supports` excludes `uwp` |
| Linux | `x64-linux`, `arm64-linux` | Full UTF-8 API |
| macOS | `arm64-osx`, `x64-osx` | Full UTF-8 API |
| iOS / Android | `ios`, `*-android` | UTF-8 API; RNG falls back if `random_device` is weak |
| FreeBSD / Emscripten | declared in `supports` | UTF-8 API only |

`BENTO_HAS_WSTRING_MASK` is enabled only on desktop Windows (not UWP). See `src/bento/platform.hpp`.

## Versioning

**Source of truth:** `package.json` → `"version": "MAJOR.MINOR.PATCH"`.

| Context | Resolved version | Example |
|---------|------------------|---------|
| Local dev | `package.json` as-is | `0.1.0` |
| CI / Release | `MAJOR.MINOR` from `package.json` + `GITHUB_RUN_NUMBER` as patch | `0.1.42` (run #42) |

You bump **major** and **minor** manually in `package.json`. The **patch** segment is the GitHub Actions run number in CI and release (not edited by hand).

```bash
npm run version          # print resolved version for current environment
```

`npm run configure` / `npm run configure:ci` pass the resolved version to CMake as `BENTO_KIT_VERSION` (used by `project(... VERSION ...)` and install metadata).

## Modules

### `bento::id` — IDs & random

| Header | API |
|--------|-----|
| `bento/id/session.hpp` | `setSessionPrefix`, `generateSessionId` |
| `bento/id/random.hpp` | `randomInt`, `randomString` |
| `bento/id/uuid.hpp` | `uuidV4`, `uuidV4Simple`, `uuidV7`, `uuidV7Simple` |
| `bento/id/nanoid.hpp` | `nanoid`, `nanoidWithLen`, `nanoidWithAlphabet` |
| `bento/bento.hpp` | umbrella header (id + mask) |

Session ID format: `<prefix><unix_ms><5-char-base36-suffix>` — e.g. `myapp1735088400123x9y8z`.

`random*` / `uuidV4` / `nanoid` use a thread-local PRNG — **not cryptographically secure**. Use a CSPRNG for secrets and tokens.

### `bento::mask` — log redaction

| Header | API |
|--------|-----|
| `bento/mask/mask.hpp` | `maskPhone`, `maskEmail`, `maskIdCard`, `maskBankCard`, `maskToken`, `maskName`, `maskMiddle`, `maskSecret` |

Designed for **log output**: malformed input is best-effort masked, never throws. Counts Unicode scalars (not UTF-8 bytes).

```cpp
#include <bento/bento.hpp>

bento::id::setSessionPrefix("myapp");
auto sessionId = bento::id::generateSessionId();
auto token = bento::id::nanoid();

bento::mask::maskPhone("13812345678");
bento::mask::maskSecret("vault:AES256:abcXYZ", 3);
```

## Integration

```cmake
add_subdirectory(path/to/bento-kit-cpp)   # or find_package(bento-kit)
target_link_libraries(my_app PRIVATE bento::kit)
```

## Build & test

All tasks are exposed as npm scripts (local dev and CI use the same commands):

```bash
npm run dev          # configure + build + test (default preset)
npm run ci           # configure + build + test (ci / Release preset)
npm run format       # clang-format check
npm run format:fix   # apply clang-format
npm run clean        # remove output/
```

Manual CMake presets (equivalent to the npm scripts above):

```bash
cmake --preset default
cmake --build --preset default
ctest --preset default
```

Demos:

```bash
./output/bin/session_demo
./output/bin/id_demo
./output/bin/mask_demo
```

Clean: `rm -rf output`

## CI

GitHub Actions (`.github/workflows/ci.yml`) on every push/PR to `main`:

| Job | Command |
|-----|---------|
| format | `npm run format` (Ubuntu; requires `clang-format`) |
| build & test | `npm run ci` (Ubuntu, macOS, Windows) |
| **ci success** | gates merge; fails if any job above fails |

Local format check requires `clang-format` on `PATH`, or set `CLANG_FORMAT=clang-format-18`.

### Merge policy

`main` requires the **`ci success`** check to pass before merge (branch protection).

One-time setup (repo admin):

```bash
gh api --method PUT repos/imcooder/bento-kit-cpp/branches/main/protection \
  --input - <<'EOF'
{
  "required_status_checks": {
    "strict": true,
    "checks": [{ "context": "ci success" }]
  },
  "enforce_admins": false,
  "required_pull_request_reviews": null,
  "restrictions": null,
  "allow_force_pushes": false,
  "allow_deletions": false
}
EOF
```

Or in GitHub: **Settings -> Branches -> Add rule** for `main`, enable **Require status checks to pass**, select **ci success**.

## vcpkg

Port files live under `ports/bento-kit/`.

### Custom registry (current, self-hosted in this repo)

Point vcpkg at this repo as a **git registry** (not the official catalog):

```json
{
  "registries": [
    {
      "kind": "git",
      "repository": "https://github.com/imcooder/bento-kit-cpp",
      "baseline": "<commit with chore(vcpkg): release …>",
      "packages": ["bento-kit"]
    }
  ]
}
```

Project `vcpkg.json`: `{ "dependencies": ["bento-kit"] }`.

This works after each release, but **`vcpkg search bento-kit` will not find it** until you also publish to the official registry (below).

### Official vcpkg catalog (anyone can search)

To appear in [https://vcpkg.io](https://vcpkg.io) and `vcpkg search` without a custom registry:

1. Fork [microsoft/vcpkg](https://github.com/microsoft/vcpkg).
2. Copy `ports/bento-kit/` from this repo into your fork’s `ports/bento-kit/` (first time) or bump `version` + `SHA512` in `portfile.cmake` (each release).
3. From your vcpkg clone root, run:
   ```bash
   ./vcpkg install bento-kit --triplet x64-linux
   ```
   Fix any port issues until it installs cleanly on Linux (CI requirement for new ports).
4. Open a PR to `microsoft/vcpkg` with title like `[bento-kit] add port` or `[bento-kit] update to 0.1.x`.
5. After merge (usually a few days), anyone can:
   ```bash
   vcpkg search bento-kit
   vcpkg install bento-kit
   ```

The vcpkg port and CMake package are both **`bento-kit`**; link with target **`bento::kit`**.

Each new GitHub Release updates `ports/` on `main` here automatically. To also sync your **microsoft/vcpkg** fork and open/update the upstream PR, configure **`VCPKG_FORK_TOKEN`** (see below).

#### Automate the microsoft/vcpkg PR (optional)

After you [fork microsoft/vcpkg](https://github.com/microsoft/vcpkg), add a repository secret on **bento-kit-cpp**:

| Secret | Value |
|--------|--------|
| `VCPKG_FORK_TOKEN` | GitHub PAT (classic) with **`repo`** scope, able to push to `imcooder/vcpkg` and open PRs |

Each successful **Build & Release** run will then:

1. Copy `ports/bento-kit/` to your fork branch `bento-kit-port` (rebased on `microsoft/vcpkg` `master`)
2. Run `./vcpkg install bento-kit:x64-linux` and `x-add-version`
3. Push to **`imcooder/vcpkg`**
4. **Create** a PR to `microsoft/vcpkg`, or **update** the existing open PR

If the secret is not set, this step is skipped (custom registry in this repo still works).

Manual one-off sync from your machine:

```bash
export VCPKG_FORK_TOKEN=ghp_...
export RELEASE_VERSION=0.1.42
bash scripts/sync-microsoft-vcpkg.sh
```

### Release (automated)

The **Build & Release** workflow (`.github/workflows/release.yml`) runs only on branches named `release_Major.Minor` (e.g. `release_0.1`). The **release version** uses the same rule as CI: `MAJOR.MINOR` from `package.json` + **`GITHUB_RUN_NUMBER`** as patch (e.g. run #42 → `0.1.42`, tag `v0.1.42`).

1. Bump **major.minor** in `package.json` when needed (patch in `package.json` is ignored in CI/release).
2. Create and push a release branch:

```bash
RELEASE_BRANCH=release_0.1 npm run release:check
git push origin main
git checkout release_0.1 || git checkout -b release_0.1
git cherry-pick <commit-on-main>   # pick the commit(s) you want to ship
git push -u origin release_0.1
```

The workflow creates a GitHub Release titled **`bento-kit v0.1.<run>`**, builds on ubuntu / macOS / Windows, uploads `bento-kit-0.1.<run>.tar.gz`, updates `ports/bento-kit/` on `main` (SHA512 from the **release branch** tarball), then finalizes the Release page. A tag `v0.1.<run>` is still created for the Release page only.

Re-run manually via **Actions -> Build & Release -> Run workflow** — select a `release_*` branch (not `main`).

Consumers pin the vcpkg registry `baseline` to the `chore(vcpkg): release …` commit on `main`.

## Roadmap

| Module | Scope | Status |
|--------|-------|--------|
| `id` | session / random / uuid / nanoid | done |
| `mask` | sensitive-field redaction | done |
| `time` | staged timer, timestamp formatting | planned |
| … | more utilities as projects need them | — |

## License

MIT — see [LICENSE](LICENSE).
