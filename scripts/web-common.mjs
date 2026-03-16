import { spawn } from "node:child_process";
import { access, constants, mkdir, realpath } from "node:fs/promises";
import path from "node:path";
import { fileURLToPath } from "node:url";

const scriptDir = path.dirname(fileURLToPath(import.meta.url));

export const repoRoot = path.resolve(scriptDir, "..");
export const webRoot = path.join(repoRoot, "web");
export const generatedDir = path.join(webRoot, "public", "generated");
export const buildDirs = {
  debug: path.join(repoRoot, "build-web-debug"),
  release: path.join(repoRoot, "build-web-release"),
};
export const webTarget = "SoftBodyPhysicsWeb";
export const defaultEmscriptenCacheDir = path.join(repoRoot, ".cache", "emscripten");

const fallbackEmsdkRoot = path.join(path.dirname(repoRoot), "emsdk");

function normalizeMode(mode) {
  return mode === "release" ? "release" : "debug";
}

export function resolveLocalBin(binaryName) {
  return path.join(
    repoRoot,
    "node_modules",
    ".bin",
    process.platform === "win32" ? `${binaryName}.cmd` : binaryName,
  );
}

async function resolveCanonicalPath(targetPath) {
  await mkdir(targetPath, { recursive: true });
  return realpath(targetPath);
}

async function pathExists(targetPath) {
  try {
    await access(targetPath, constants.F_OK);
    return true;
  } catch {
    return false;
  }
}

async function resolveEmsdkRoot() {
  for (const candidate of [process.env.EMSDK, fallbackEmsdkRoot]) {
    if (!candidate) {
      continue;
    }

    const resolvedRoot = path.resolve(candidate);
    const emccPath = path.join(resolvedRoot, "upstream", "emscripten", "emcc");
    const emConfigPath = path.join(resolvedRoot, ".emscripten");
    if (await pathExists(emccPath) && await pathExists(emConfigPath)) {
      return realpath(resolvedRoot);
    }
  }

  return null;
}

async function resolveEmscriptenTool(toolName) {
  const emsdkRoot = await resolveEmsdkRoot();
  if (!emsdkRoot) {
    return toolName;
  }

  return path.join(emsdkRoot, "upstream", "emscripten", toolName);
}

export async function getWebBuildEnv() {
  const emsdkRoot = await resolveEmsdkRoot();
  const requestedCacheDir = process.env.EM_CACHE
    ? path.resolve(process.env.EM_CACHE)
    : defaultEmscriptenCacheDir;
  const emCache = await resolveCanonicalPath(requestedCacheDir);
  const emscriptenBinDir = emsdkRoot ? path.join(emsdkRoot, "upstream", "emscripten") : null;
  const prefixedPathParts = [emsdkRoot, emscriptenBinDir, process.env.PATH].filter(Boolean);

  return {
    ...process.env,
    EMSDK_QUIET: process.env.EMSDK_QUIET ?? "1",
    EM_CACHE: emCache,
    ...(emsdkRoot
      ? {
          EMSDK: emsdkRoot,
          EM_CONFIG: path.join(emsdkRoot, ".emscripten"),
          PATH: prefixedPathParts.join(path.delimiter),
        }
      : {}),
  };
}

export async function runCommand(command, args, options = {}) {
  const defaultEnv = await getWebBuildEnv();
  await new Promise((resolve, reject) => {
    const child = spawn(command, args, {
      cwd: repoRoot,
      stdio: "inherit",
      env: defaultEnv,
      ...options,
      env: {
        ...defaultEnv,
        ...options.env,
      },
    });

    child.on("error", reject);
    child.on("exit", (code) => {
      if (code === 0) {
        resolve();
        return;
      }

      reject(new Error(`Command failed: ${command} ${args.join(" ")}`));
    });
  });
}

export async function ensureGeneratedDir() {
  await mkdir(generatedDir, { recursive: true });
}

export async function ensureEmscriptenToolchain() {
  const env = await getWebBuildEnv();
  const emcc = await resolveEmscriptenTool("emcc");
  try {
    await runCommand(emcc, ["--version"], {
      stdio: "ignore",
      env,
    });
  } catch (error) {
    throw new Error("Emscripten was not found on PATH. Install and activate the emsdk before running the web scripts.");
  }

  if (env.EMSDK) {
    console.log(`[web] using EMSDK=${env.EMSDK}`);
  }
  if (process.env.EM_CACHE && env.EM_CACHE !== process.env.EM_CACHE) {
    console.warn(`[web] normalized EM_CACHE from ${process.env.EM_CACHE} to ${env.EM_CACHE}`);
  } else {
    console.log(`[web] using EM_CACHE=${env.EM_CACHE}`);
  }
}

export async function configureWebBuild(mode) {
  const normalizedMode = normalizeMode(mode);
  const buildType = normalizedMode === "release" ? "Release" : "Debug";
  const buildDir = buildDirs[normalizedMode];
  const env = await getWebBuildEnv();
  const emcmake = await resolveEmscriptenTool("emcmake");

  await ensureGeneratedDir();
  await runCommand(emcmake, [
    "cmake",
    "-S",
    repoRoot,
    "-B",
    buildDir,
    `-DCMAKE_BUILD_TYPE=${buildType}`,
  ], {
    env,
  });

  return buildDir;
}

export async function buildWebTarget(mode) {
  const normalizedMode = normalizeMode(mode);
  const buildDir = await configureWebBuild(normalizedMode);
  const env = await getWebBuildEnv();
  await runCommand("cmake", ["--build", buildDir, "--target", webTarget], {
    env,
  });
  return buildDir;
}
