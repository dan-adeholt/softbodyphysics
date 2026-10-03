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

  // Bundled files are copied into persistent storage so the app can save next to them. A small
  // manifest records the hash of each file as copied, which tells apart files the visitor edited
  // from ones that are just an older copy of the bundle.
  var bundledManifestPath = "/persist/.bundled-files.json";

  // FNV-1a, enough to tell whether a file still matches what was copied
  function hashBytes(bytes) {
    var hash = 0x811c9dc5;
    for (var i = 0; i < bytes.length; i++) {
      hash ^= bytes[i];
      hash = Math.imul(hash, 0x01000193) >>> 0;
    }
    return hash.toString(16);
  }

  function readBundledManifest() {
    if (!pathExists(bundledManifestPath)) {
      return {};
    }

    try {
      return JSON.parse(FS.readFile(bundledManifestPath, { encoding: "utf8" }));
    } catch (error) {
      return {};
    }
  }

  function listFiles(dir, out) {
    var entries = FS.readdir(dir);
    for (var i = 0; i < entries.length; i++) {
      var entry = entries[i];
      if (entry === "." || entry === "..") {
        continue;
      }

      var path = dir + "/" + entry;
      if (FS.isDir(FS.stat(path).mode)) {
        listFiles(path, out);
      } else {
        out.push(path);
      }
    }
  }

  // Brings the bundled files into persistent storage. A stored file the visitor never changed is
  // replaced with the bundled version, so updated scenes and assets reach returning visitors. A file
  // they edited and saved is kept. Files dropped from the bundle are removed unless edited, and files
  // the visitor created are never touched.
  function syncBundledFiles(dirs) {
    var previous = readBundledManifest();
    var current = {};

    var bundledPaths = [];
    for (var i = 0; i < dirs.length; i++) {
      listFiles(dirs[i], bundledPaths);
    }

    for (var j = 0; j < bundledPaths.length; j++) {
      var bundledPath = bundledPaths[j];
      var targetPath = "/persist" + bundledPath;
      var bundled = FS.readFile(bundledPath);
      var bundledHash = hashBytes(bundled);

      if (pathExists(targetPath)) {
        var storedHash = hashBytes(FS.readFile(targetPath));

        if (storedHash === bundledHash) {
          current[bundledPath] = bundledHash;
          continue;
        }

        // Without a manifest entry the file was copied before the manifest existed, so it counts as unedited
        var editedByVisitor = previous[bundledPath] !== undefined && storedHash !== previous[bundledPath];

        if (editedByVisitor) {
          current[bundledPath] = previous[bundledPath];
          continue;
        }
      }

      ensureDir(targetPath.substring(0, targetPath.lastIndexOf("/")));
      FS.writeFile(targetPath, bundled);
      current[bundledPath] = bundledHash;
    }

    for (var path in previous) {
      var stalePath = "/persist" + path;
      if (current[path] === undefined && pathExists(stalePath) && hashBytes(FS.readFile(stalePath)) === previous[path]) {
        FS.unlink(stalePath);
      }
    }

    FS.writeFile(bundledManifestPath, JSON.stringify(current));
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
        syncBundledFiles(["/data", "/levels", "/scenedefs"]);
      }

      FS.chdir("/persist");
      removeRunDependency("persistent-fs");

      // Store what the sync changed right away, rather than on the next save
      requestPersistSync();
    });
  });
})();
