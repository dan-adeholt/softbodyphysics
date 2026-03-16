(function () {
  function pathExists(path) {
    try {
      FS.lookupPath(path);
      return true;
    } catch (error) {
      return false;
    }
  }

  function ensureDir(path) {
    if (pathExists(path)) {
      return;
    }

    var parts = path.split("/");
    var current = "";
    for (var i = 0; i < parts.length; i++) {
      var part = parts[i];
      if (part.length === 0) {
        continue;
      }

      current += "/" + part;
      if (!pathExists(current)) {
        FS.mkdir(current);
      }
    }
  }

  function copyMissingTree(sourceDir, targetDir) {
    ensureDir(targetDir);

    var entries = FS.readdir(sourceDir);
    for (var i = 0; i < entries.length; i++) {
      var entry = entries[i];
      if (entry === "." || entry === "..") {
        continue;
      }

      var sourcePath = sourceDir + "/" + entry;
      var targetPath = targetDir + "/" + entry;
      var stat = FS.stat(sourcePath);

      if (FS.isDir(stat.mode)) {
        copyMissingTree(sourcePath, targetPath);
        continue;
      }

      if (!pathExists(targetPath)) {
        FS.writeFile(targetPath, FS.readFile(sourcePath, { encoding: "binary" }));
      }
    }
  }

  var syncInFlight = false;
  var syncQueued = false;

  function requestPersistSync() {
    if (syncInFlight) {
      syncQueued = true;
      return;
    }

    syncInFlight = true;
    FS.syncfs(false, function (error) {
      if (error) {
        console.error("FS.syncfs(false) failed", error);
      }

      syncInFlight = false;
      if (syncQueued) {
        syncQueued = false;
        requestPersistSync();
      }
    });
  }

  Module.requestPersistSync = requestPersistSync;
  Module.preRun = Module.preRun || [];
  Module.preRun.push(function () {
    addRunDependency("persistent-fs");

    ensureDir("/persist");
    FS.mount(IDBFS, {}, "/persist");

    FS.syncfs(true, function (error) {
      if (error) {
        console.error("FS.syncfs(true) failed", error);
      } else {
        copyMissingTree("/data", "/persist/data");
        copyMissingTree("/levels", "/persist/levels");
        copyMissingTree("/scenedefs", "/persist/scenedefs");
      }

      FS.chdir("/persist");
      removeRunDependency("persistent-fs");
    });
  });
})();
