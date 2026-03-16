# Soft-body dev #

This is a test project where I develop a soft-body 2D physics game in C++. It is heavily inspired by this video from the author of JellyCar: https://www.youtube.com/watch?v=3OmkehAJoyo

I usually do full stack web development these days, but I have a background in writing fairly low-level C++ code.

## Goals ##

* Lightning fast compile times. Good header file organization, forward declares and very restrictive use of templates.
* No STL. This might seem odd, but I am challenging myself and will write my own containers. There are always different opionions on STL, my biggest gripes are the snowball effect on compilation times and over-engineering. For more serious projects, I have always used STL, but this is a hobby project so why not challenge your perspective. I've also never been a fan of too clever template code that takes ages to compile (Boost et al).
* Simple, data-oriented structures inspired by Mike Actons talks.

## Approach ##

I will start using SDL2 for rendering. Further on I will integrate some more advanced rendering engine. Why not completely torture myself and write a Vulkan backend? :)

## WebAssembly ##

The repo now includes a browser target that is intended to be built with Emscripten and served through Vite.

1. Install and activate the Emscripten SDK. The repo scripts will use `emcmake` from your `PATH`, or auto-detect a sibling `../emsdk` checkout if present.
2. Install the frontend tooling with `npm install`.
3. Start the browser workflow with `npm run dev:web`.

The web scripts use a repo-local Emscripten cache at `.cache/emscripten` by default. If you override `EM_CACHE`, the scripts normalize it to the real filesystem path before invoking `emcc`. On macOS this avoids the `/tmp` -> `/private/tmp` symlink issue that breaks Emscripten 5.0.3 system library builds.

That command will:

* Configure and build the `SoftBodyPhysicsWeb` Emscripten target into `web/public/generated`.
* Watch the C++ sources and bundled asset folders for changes.
* Start a Vite dev server and trigger a full browser reload whenever the generated wasm bundle changes.

For a production-style browser bundle, run `npm run build:web`.
