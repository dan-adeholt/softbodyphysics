import { spawn } from "node:child_process";

import chokidar from "chokidar";

import {
  buildWebTarget,
  ensureEmscriptenToolchain,
  repoRoot,
  resolveLocalBin,
  webRoot,
} from "./web-common.mjs";

const watchTargets = [
  "CMakeLists.txt",
  "main.cpp",
  "main.h",
  "shell.cpp",
  "web_main.cpp",
  "containers",
  "data",
  "external/include",
  "fontawesome",
  "game",
  "imgui",
  "levels",
  "math",
  "physics",
  "scenedefs",
  "stb_image",
  "stb_sprintf",
  "tasks",
  "utils",
  "web/pre.js",
];

let buildRunning = false;
let buildQueued = false;
let shuttingDown = false;
const usePolling = process.env.CHOKIDAR_USEPOLLING === "1" || process.platform === "darwin";

async function rebuild(reason) {
  if (buildRunning) {
    buildQueued = true;
    return;
  }

  buildRunning = true;
  console.log(`[web] rebuilding (${reason})`);

  try {
    await buildWebTarget("debug");
    console.log("[web] rebuild completed");
  } catch (error) {
    console.error("[web] rebuild failed");
    console.error(error);
  } finally {
    buildRunning = false;
    if (buildQueued && !shuttingDown) {
      buildQueued = false;
      void rebuild("queued changes");
    }
  }
}

await ensureEmscriptenToolchain();
await buildWebTarget("debug");

const viteProcess = spawn(resolveLocalBin("vite"), ["--config", `${webRoot}/vite.config.js`], {
  cwd: repoRoot,
  stdio: "inherit",
});

viteProcess.on("error", (error) => {
  console.error("[vite] failed to start");
  console.error(error);
  process.exit(1);
});

const watcher = chokidar.watch(watchTargets, {
  cwd: repoRoot,
  ignoreInitial: true,
  usePolling,
  interval: usePolling ? 200 : undefined,
  ignored: [
    "**/.git/**",
    "**/build/**",
    "**/build-ai/**",
    "**/build-osx/**",
    "**/build-release/**",
    "**/build-web-debug/**",
    "**/build-web-release/**",
    "**/node_modules/**",
    "**/web/dist/**",
    "**/web/public/generated/**",
  ],
});

watcher.on("all", (eventName, filePath) => {
  void rebuild(`${eventName}: ${filePath}`);
});

watcher.on("error", (error) => {
  console.error("[web] file watcher failed");
  console.error(error);
});

async function shutdown(signal) {
  if (shuttingDown) {
    return;
  }

  shuttingDown = true;
  await watcher.close();
  viteProcess.kill(signal);
}

process.on("SIGINT", async () => {
  await shutdown("SIGINT");
  process.exit(0);
});

process.on("SIGTERM", async () => {
  await shutdown("SIGTERM");
  process.exit(0);
});

viteProcess.on("exit", async (code) => {
  await watcher.close();
  if (code !== 0 && code !== null) {
    process.exit(code);
  }
});
