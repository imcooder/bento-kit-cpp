/**
 * Render GitHub Release notes for bento-kit-cpp.
 *
 * Usage:
 *   node scripts/render-release-notes.mjs initial > body.md
 *   node scripts/render-release-notes.mjs final   > body.md
 */

const mode = process.argv[2];
if (mode !== "initial" && mode !== "final") {
  console.error("Usage: node scripts/render-release-notes.mjs <initial|final>");
  process.exit(1);
}

function req(name) {
  const value = process.env[name];
  if (!value) {
    throw new Error(`Missing env: ${name}`);
  }
  return value;
}

function opt(name, fallback = "—") {
  return process.env[name] || fallback;
}

function workflowUrl() {
  const repo = req("GITHUB_REPOSITORY");
  const runId = req("GITHUB_RUN_ID");
  return `https://github.com/${repo}/actions/runs/${runId}`;
}

function releasePageUrl() {
  const repo = req("GITHUB_REPOSITORY");
  const tag = req("RELEASE_TAG");
  return `https://github.com/${repo}/releases/tag/${tag}`;
}

function shortSha() {
  return req("GITHUB_SHA").slice(0, 7);
}

function buildInfoSection() {
  return `## Release Information

- **Version:** ${req("RELEASE_VERSION")}
- **Release Tag:** ${req("RELEASE_TAG")}
- **Released by:** ${req("GITHUB_ACTOR")}
- **Release Date:** ${req("BUILD_TIME")}
- **Branch:** \`${req("RELEASE_BRANCH")}\`
- **Commit:** \`${req("GITHUB_SHA")}\` (${shortSha()})
- **Build ID:** ${req("GITHUB_RUN_ID")}
- **Workflow:** [View Run](${workflowUrl()})
- **Release Page:** [${req("RELEASE_TAG")}](${releasePageUrl()})`;
}

function pendingBuildTable() {
  return `## Build Status

| Platform | Status |
|----------|--------|
| Ubuntu | pending |
| macOS | pending |
| Windows | pending |

> Building in progress… Release assets and the vcpkg port update will appear when all jobs finish.`;
}

function finalBuildTable() {
  const rows = [
    ["Ubuntu", opt("BUILD_UBUNTU")],
    ["macOS", opt("BUILD_MACOS")],
    ["Windows", opt("BUILD_WINDOWS")],
  ];
  const lines = rows.map(([platform, result]) => {
    const icon = result === "success" ? "pass" : result === "failure" ? "fail" : "warn";
    const label = icon === "pass" ? "success" : icon === "fail" ? "failure" : result;
    return `| ${platform} | ${label} |`;
  });
  return `## Build Results

| Platform | Result |
|----------|--------|
${lines.join("\n")}`;
}

function downloadsInitial() {
  return `## Downloads

Release assets will be listed here after packaging completes. Check the **Assets** section below.`;
}

function downloadsFinal() {
  const pkgName = req("PKG_NAME");
  const assetUrl = req("PKG_ASSET_URL");
  const size = opt("PKG_SIZE");
  return `## Downloads

| File | Size | Description |
|------|------|-------------|
| [${pkgName}.tar.gz](${assetUrl}) | ${size} | Installed headers + CMake \`find_package\` config + LICENSE |

### Source code

GitHub also attaches auto-generated **Source code (zip/tar.gz)** archives for tag \`${req("RELEASE_TAG")}\`.`;
}

function integrationSection() {
  const version = req("RELEASE_VERSION");
  const pkgName = mode === "final" ? req("PKG_NAME") : `bento-kit-cpp-${version}`;
  return `## Integration

### CMake (\`find_package\`)

\`\`\`bash
tar -xzf ${pkgName}.tar.gz
\`\`\`

\`\`\`cmake
list(APPEND CMAKE_PREFIX_PATH "\${CMAKE_SOURCE_DIR}/path/to/${pkgName}")
find_package(bento-kit-cpp CONFIG REQUIRED)
target_link_libraries(my_app PRIVATE bento::kit)
\`\`\`

### vcpkg (custom registry)

Add this repo as a registry and depend on \`bento-kit-cpp\`:

\`\`\`json
{ "dependencies": ["bento-kit-cpp"] }
\`\`\`

Pin the registry \`baseline\` to the port update commit on \`main\` (see below).`;
}

function vcpkgSectionInitial() {
  return `## vcpkg Port

The vcpkg port under \`ports/bento-kit-cpp/\` will be updated on \`main\` after this run completes.`;
}

function vcpkgSectionFinal() {
  const commit = opt("VCPKG_PORT_COMMIT");
  const sha512 = opt("VCPKG_SHA512");
  const version = req("RELEASE_VERSION");
  const shortHash = commit !== "—" ? commit.slice(0, 7) : "—";
  const shaShort = sha512 !== "—" && sha512.length > 32 ? `${sha512.slice(0, 32)}…` : sha512;
  return `## vcpkg Port

| Field | Value |
|-------|-------|
| Port version | \`${version}\` |
| Baseline commit | \`${commit}\` (${shortHash}) |
| SHA512 | \`${shaShort}\` |

Commit message on \`main\`: \`chore(vcpkg): release ${version}\``;
}

const parts = [buildInfoSection(), "---"];

if (mode === "initial") {
  parts.push(pendingBuildTable(), downloadsInitial(), integrationSection(), vcpkgSectionInitial());
} else {
  parts.push(finalBuildTable(), downloadsFinal(), integrationSection(), vcpkgSectionFinal());
}

process.stdout.write(`${parts.join("\n\n")}\n`);
