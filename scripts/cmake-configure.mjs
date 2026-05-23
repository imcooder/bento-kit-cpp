import { spawnSync } from "node:child_process";
import { resolveVersion } from "./resolve-version.mjs";

const preset = process.argv[2];
if (!preset) {
  console.error("Usage: node scripts/cmake-configure.mjs <preset>");
  process.exit(1);
}

const version = resolveVersion();
console.log(`Configuring preset "${preset}" with version ${version}`);

const result = spawnSync(
  "cmake",
  ["--preset", preset, `-DBENTO_KIT_VERSION=${version}`],
  { stdio: "inherit" },
);

if (result.error) {
  console.error(result.error.message);
  process.exit(1);
}

process.exit(result.status ?? 1);
