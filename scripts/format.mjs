import { spawnSync } from "node:child_process";
import { readdirSync } from "node:fs";
import { join } from "node:path";

function collectSources(dir, out = []) {
  for (const entry of readdirSync(dir, { withFileTypes: true })) {
    const path = join(dir, entry.name);
    if (entry.isDirectory()) {
      collectSources(path, out);
      continue;
    }
    if (entry.name.endsWith(".hpp") || entry.name.endsWith(".cpp")) {
      out.push(path);
    }
  }
  return out;
}

const mode = process.argv[2] ?? "check";
const files = collectSources("src").sort();

if (files.length === 0) {
  console.error("No source files found under src/.");
  process.exit(1);
}

const clangFormat = process.env.CLANG_FORMAT ?? "clang-format";
const args =
  mode === "fix"
    ? ["-i", ...files]
    : ["--dry-run", "--Werror", ...files];

const result = spawnSync(clangFormat, args, { stdio: "inherit" });

if (result.error) {
  console.error(`Failed to run ${clangFormat}: ${result.error.message}`);
  process.exit(1);
}

if (result.status !== 0) {
  process.exit(result.status ?? 1);
}

if (mode === "fix") {
  console.log(`Formatted ${files.length} file(s).`);
} else {
  console.log(`Format check passed (${files.length} file(s)).`);
}
