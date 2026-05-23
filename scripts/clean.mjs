import { rmSync } from "node:fs";

rmSync("output", { recursive: true, force: true });
console.log("Removed output/");
