import { readFileSync, writeFileSync } from "node:fs";
import { resolve } from "node:path";

const portDir = resolve("ports/bento-kit-cpp");
const vcpkgJsonPath = resolve(portDir, "vcpkg.json");
const portfilePath = resolve(portDir, "portfile.cmake");

function usage() {
  console.error("Usage: node scripts/update-vcpkg-port.mjs <version> <sha512>");
  process.exit(1);
}

const version = process.argv[2];
const sha512 = process.argv[3];
if (!version || !sha512) {
  usage();
}

const vcpkgJson = JSON.parse(readFileSync(vcpkgJsonPath, "utf8"));
vcpkgJson.version = version;
writeFileSync(vcpkgJsonPath, `${JSON.stringify(vcpkgJson, null, 2)}\n`);

let portfile = readFileSync(portfilePath, "utf8");
if (!/SHA512\s+\S+/.test(portfile)) {
  throw new Error("portfile.cmake must contain a SHA512 field.");
}
portfile = portfile.replace(/SHA512\s+\S+/, `SHA512 ${sha512}`);
writeFileSync(portfilePath, portfile);

console.log(`Updated vcpkg port to ${version} (SHA512 ${sha512.slice(0, 16)}…)`);
