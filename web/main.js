import "./styles.css";

const GENERATED_BASE = "/generated";
const statusText = document.querySelector("#status-text");
const canvas = document.querySelector("#game-canvas");

// Right click opens the app's own menu for adding shapes, so keep the browser's menu away
canvas.addEventListener("contextmenu", (event) => event.preventDefault());

let moduleInstance = null;

function setStatus(message) {
  if (statusText) {
    statusText.textContent = message;
  }
}

function appendRuntimeLog(message) {
  if (typeof message !== "string" || message.length === 0) {
    return;
  }

  console.log(message);
}

async function loadGeneratedModule() {
  return import(
    /* @vite-ignore */
    `${GENERATED_BASE}/softbodyphysics.js?cache=${Date.now()}`
  );
}

async function boot() {
  setStatus("Loading generated Emscripten module...");
  appendRuntimeLog(`Loading ${GENERATED_BASE}/softbodyphysics.js`);

  const { default: createSoftBodyPhysicsModule } = await loadGeneratedModule();
  const moduleConfig = {
    canvas,
    locateFile: (path) => `${GENERATED_BASE}/${path}`,
    print: appendRuntimeLog,
    printErr: (message) => {
      if (typeof message === "string" && message.length > 0) {
        console.error(message);
      }
    },
    setStatus,
  };

  // SDL's Emscripten backend assigns Module.requestFullscreen at runtime.
  Object.defineProperty(moduleConfig, "requestFullscreen", {
    value: undefined,
    writable: true,
    configurable: true,
    enumerable: true,
  });

  moduleInstance = await createSoftBodyPhysicsModule(moduleConfig);

  window.addEventListener("beforeunload", () => {
    moduleInstance?.requestPersistSync?.();
  });

  window.setInterval(() => {
    moduleInstance?.requestPersistSync?.();
  }, 1000);

  canvas.focus();
  setStatus("Running. Rebuilds trigger a full page reload.");
}

boot().catch((error) => {
  console.error(error);
  appendRuntimeLog(String(error));
  setStatus("Failed to load the generated WebAssembly build. Run `npm run dev:web` after installing Emscripten.");
});
