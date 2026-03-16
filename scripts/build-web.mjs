import {
  buildWebTarget,
  ensureEmscriptenToolchain,
  resolveLocalBin,
  runCommand,
  webRoot,
} from "./web-common.mjs";

await ensureEmscriptenToolchain();
await buildWebTarget("release");
await runCommand(resolveLocalBin("vite"), ["build", "--config", `${webRoot}/vite.config.js`]);
