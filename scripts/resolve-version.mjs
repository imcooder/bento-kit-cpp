import { readFileSync } from "node:fs";
import { resolve } from "node:path";
import { fileURLToPath } from "node:url";

function readPackageVersion() {
  const pkg = JSON.parse(readFileSync("package.json", "utf8"));
  if (!pkg.version || typeof pkg.version !== "string") {
    throw new Error("package.json must contain a string \"version\" field.");
  }
  return pkg.version;
}

/**
 * Local: use package.json version as-is (e.g. 0.1.0).
 * CI: major.minor from package.json + GITHUB_RUN_NUMBER as patch (e.g. 0.1.42).
 */
export function resolveVersion() {
  const raw = readPackageVersion();
  const parts = raw.split(".");
  if (parts.length < 2) {
    throw new Error(
      `package.json version must be major.minor[.patch], got "${raw}".`,
    );
  }

  const major = parts[0];
  const minor = parts[1];
  const inCi = process.env.CI === "true" || process.env.CI === "1";
  const runNumber = process.env.GITHUB_RUN_NUMBER;

  if (inCi && runNumber) {
    return `${major}.${minor}.${runNumber}`;
  }

  return raw;
}

if (process.argv[1] && resolve(process.argv[1]) === fileURLToPath(import.meta.url)) {
  console.log(resolveVersion());
}
