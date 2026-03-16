import path from "node:path";
import { fileURLToPath } from "node:url";
import { defineConfig } from "vite";

const rootDir = path.dirname(fileURLToPath(import.meta.url));
const generatedDir = path.resolve(rootDir, "public/generated");

function generatedArtifactsReloadPlugin() {
  return {
    name: "generated-artifacts-reload",
    configureServer(server) {
      server.watcher.add(generatedDir);

      const triggerReload = (file) => {
        if (!file.startsWith(generatedDir)) {
          return;
        }

        server.ws.send({ type: "full-reload" });
      };

      server.watcher.on("add", triggerReload);
      server.watcher.on("change", triggerReload);
      server.watcher.on("unlink", triggerReload);
    },
  };
}

export default defineConfig({
  root: rootDir,
  publicDir: path.resolve(rootDir, "public"),
  plugins: [generatedArtifactsReloadPlugin()],
  server: {
    host: "127.0.0.1",
  },
});
