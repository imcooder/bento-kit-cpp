#!/usr/bin/env bash
# Sync ports/bento-kit to your vcpkg fork and open/update a PR to microsoft/vcpkg.
#
# Required env:
#   VCPKG_FORK_TOKEN  - PAT with repo scope on the fork (imcooder/vcpkg)
#   RELEASE_VERSION   - port version string (e.g. 0.1.42)
#
# Optional env:
#   VCPKG_FORK_REPO   - default imcooder/vcpkg
#   BENTO_KIT_CPP_DIR - path to repo root with ports/ (default: cwd)

set -euo pipefail

write_github_output() {
  if [[ -n "${GITHUB_OUTPUT:-}" ]]; then
    echo "$1" >> "$GITHUB_OUTPUT"
  fi
}

resolve_open_pr_url() {
  gh pr list \
    --repo "${VCPKG_UPSTREAM_REPO}" \
    --head "${HEAD_REF}" \
    --state open \
    --json url \
    --jq '.[0].url // empty' 2>/dev/null || true
}

if [[ -z "${VCPKG_FORK_TOKEN:-}" ]]; then
  echo "VCPKG_FORK_TOKEN not configured; skipping microsoft/vcpkg PR sync."
  exit 0
fi

RELEASE_VERSION="${RELEASE_VERSION:?RELEASE_VERSION is required}"
BENTO_KIT_CPP_DIR="${BENTO_KIT_CPP_DIR:-$PWD}"
VCPKG_FORK_REPO="${VCPKG_FORK_REPO:-imcooder/vcpkg}"
VCPKG_UPSTREAM_REPO="${VCPKG_UPSTREAM_REPO:-microsoft/vcpkg}"
VCPKG_PR_BRANCH="${VCPKG_PR_BRANCH:-bento-kit-port}"
VCPKG_PORT="${VCPKG_PORT:-bento-kit}"

PORT_SRC="${BENTO_KIT_CPP_DIR}/ports/${VCPKG_PORT}"
if [[ ! -d "$PORT_SRC" ]]; then
  echo "Port directory not found: ${PORT_SRC}" >&2
  exit 1
fi

WORK="${RUNNER_TEMP:-/tmp}/vcpkg-sync-$$"
rm -rf "$WORK"
mkdir -p "$WORK"

clone_url="https://x-access-token:${VCPKG_FORK_TOKEN}@github.com/${VCPKG_FORK_REPO}.git"
git clone --depth 1 "$clone_url" "${WORK}/vcpkg"
cd "${WORK}/vcpkg"

git remote add upstream "https://github.com/${VCPKG_UPSTREAM_REPO}.git"
git fetch upstream master --depth 1
git checkout -B "${VCPKG_PR_BRANCH}" FETCH_HEAD

rm -rf "ports/${VCPKG_PORT}"
cp -R "${PORT_SRC}" "ports/${VCPKG_PORT}"

./bootstrap-vcpkg.sh -disableMetrics
./vcpkg install "${VCPKG_PORT}:x64-linux"

./vcpkg x-add-version "${VCPKG_PORT}" --overwrite-version

git config user.name "imcooder"
git config user.email "imcooder@gmail.com"
git add "ports/${VCPKG_PORT}" "versions"

export GH_TOKEN="$VCPKG_FORK_TOKEN"
FORK_OWNER="${VCPKG_FORK_REPO%%/*}"
HEAD_REF="${FORK_OWNER}:${VCPKG_PR_BRANCH}"

if git diff --cached --quiet; then
  echo "No changes for microsoft/vcpkg; fork already up to date."
  PR_URL="$(resolve_open_pr_url)"
  if [[ -n "$PR_URL" ]]; then
    write_github_output "pr_url=${PR_URL}"
    echo "Existing upstream PR: ${PR_URL}"
  fi
  exit 0
fi

git commit -m "[${VCPKG_PORT}] release ${RELEASE_VERSION}"
git push -f origin "${VCPKG_PR_BRANCH}"

existing="$(gh pr list \
  --repo "${VCPKG_UPSTREAM_REPO}" \
  --head "${HEAD_REF}" \
  --state open \
  --json number \
  --jq '.[0].number' 2>/dev/null || true)"

PR_URL=""
if [[ -n "$existing" && "$existing" != "null" ]]; then
  PR_URL="$(gh pr view "$existing" --repo "${VCPKG_UPSTREAM_REPO}" --json url --jq .url)"
  echo "Updated open PR #${existing} on ${VCPKG_UPSTREAM_REPO} (branch ${VCPKG_PR_BRANCH})."
else
  PR_URL="$(gh pr create \
    --repo "${VCPKG_UPSTREAM_REPO}" \
    --base master \
    --head "${HEAD_REF}" \
    --title "[${VCPKG_PORT}] add port" \
    --body "$(cat <<EOF
Automated sync from [${GITHUB_REPOSITORY:-imcooder/bento-kit-cpp}](https://github.com/imcooder/bento-kit-cpp) release **${RELEASE_VERSION}**.

- **Upstream:** https://github.com/imcooder/bento-kit-cpp
- **Port version:** ${RELEASE_VERSION}
- **Local test:** \`./vcpkg install ${VCPKG_PORT}:x64-linux\` (run in CI before push)

\`\`\`cmake
find_package(bento-kit CONFIG REQUIRED)
target_link_libraries(my_app PRIVATE bento::kit)
\`\`\`
EOF
)")"
  echo "Created new PR on ${VCPKG_UPSTREAM_REPO}."
fi

if [[ -n "$PR_URL" ]]; then
  write_github_output "pr_url=${PR_URL}"
  echo "Upstream PR: ${PR_URL}"
fi
