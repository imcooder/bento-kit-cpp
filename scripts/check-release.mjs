import { readFileSync } from "node:fs";

const pkgVersion = JSON.parse(readFileSync("package.json", "utf8")).version;
const parts = pkgVersion.split(".");
if (parts.length < 2) {
  console.error(`package.json version must be major.minor[.patch], got "${pkgVersion}".`);
  process.exit(1);
}

const majorMinor = `${parts[0]}.${parts[1]}`;
const branch = process.env.RELEASE_BRANCH;

if (branch) {
  if (!/^release_[0-9]+\.[0-9]+$/.test(branch)) {
    console.error(`Release branch must look like release_Major.Minor, got "${branch}".`);
    process.exit(1);
  }
  const expected = branch.slice("release_".length);
  if (majorMinor !== expected) {
    console.error(
      `Release branch ${branch} does not match package.json major.minor (${majorMinor}).`,
    );
    process.exit(1);
  }
}

console.log(`Release check passed: ${majorMinor} (patch from GITHUB_RUN_NUMBER in CI).`);
