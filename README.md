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

## Versioning

**Source of truth:** `package.json` → `"version": "MAJOR.MINOR.PATCH"`.

| Context | Resolved version | Example |
|---------|------------------|---------|
| Local dev | `package.json` as-is | `0.1.0` |
| CI build | `MAJOR.MINOR` from `package.json` + `GITHUB_RUN_NUMBER` as patch | `0.1.42` (run #42) |

You bump **major** and **minor** manually in `package.json`. The **patch** segment is the GitHub Actions run number in CI (not edited by hand).

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
add_subdirectory(path/to/bento-kit-cpp)   # or find_package(bento-kit-cpp)
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

Port files live under `ports/bento-kit-cpp/`. Point a custom registry at this repo:

```json
{ "dependencies": ["bento-kit-cpp"] }
```

Update `REPO` and `SHA512` in `portfile.cmake` before publishing.

## Roadmap

| Module | Scope | Status |
|--------|-------|--------|
| `id` | session / random / uuid / nanoid | done |
| `mask` | sensitive-field redaction | done |
| `time` | staged timer, timestamp formatting | planned |
| … | more utilities as projects need them | — |

## License

MIT — see [LICENSE](LICENSE).
