import "./styles.css";

// The Emscripten build's files, next to the page, so the demo works hosted under a sub-path such as GitHub Pages'
// /softbodyphysics/ as well as at the dev server's root. A plain relative path would resolve against this
// module's own URL, which is in assets/ once built.
const GENERATED_BASE = new URL("generated", document.baseURI).href;
const statusText = document.querySelector("#status-text");
const canvas = document.querySelector("#game-canvas");

// Right click opens the app's own menu for adding shapes, so keep the browser's menu away
canvas.addEventListener("contextmenu", (event) => event.preventDefault());

// SDL cancels the browser's default action for every key it passes to the app, so F12 and the other developer
// tools and reload shortcuts did nothing on the page. Those keys are kept from SDL here, before its listener on
// the canvas sees them, so the browser handles them. The app's own function keys are F1 to F10.
function isBrowserShortcut(event) {
  // By physical key, as with Option held on macOS the key types a different character
  const code = event.code;
  const command = event.metaKey || event.ctrlKey;

  return (
    code === "F12" ||
    code === "F11" ||
    // Developer tools: Cmd+Option+I/J/C on macOS, Ctrl+Shift+I/J/C elsewhere
    (command && (event.altKey || event.shiftKey) && ["KeyI", "KeyJ", "KeyC"].includes(code)) ||
    // Reload, and reload without the cache
    (command && code === "KeyR")
  );
}

for (const type of ["keydown", "keyup"]) {
  window.addEventListener(
    type,
    (event) => {
      if (isBrowserShortcut(event)) {
        event.stopImmediatePropagation();
      }
    },
    { capture: true },
  );
}

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

// SDL measures the canvas and the device pixel ratio only on the window's resize event. The canvas can change
// size without one, as when a device preview resizes its frame or the notch padding settles, and zooming
// changes the pixel ratio. Then SDL kept drawing at the old size, into part of the canvas or blurred, so these
// changes are passed on to it as resize events too.
function forwardCanvasChangesToSdl() {
  const notify = () => window.dispatchEvent(new Event("resize"));

  new ResizeObserver(notify).observe(canvas);

  // A media query for the current ratio stops matching when it changes; then watch for the new one
  const watchPixelRatio = () => {
    const query = window.matchMedia(`(resolution: ${window.devicePixelRatio}dppx)`);
    query.addEventListener(
      "change",
      () => {
        notify();
        watchPixelRatio();
      },
      { once: true },
    );
  };

  watchPixelRatio();
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
  forwardCanvasChangesToSdl();

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
