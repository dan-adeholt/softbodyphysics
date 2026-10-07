# The soft body physics engine on its own, without the sandbox's editor, scenes and rendering, for this project and
# for games built on it. The sandbox's CMakeLists.txt compiles these files into its own targets; another project
# includes this file and links SoftBodyPhysicsEngine:
#
#     include(../softbodyphysics/engine.cmake)
#     target_link_libraries(MyGame PRIVATE SoftBodyPhysicsEngine)
#
# Its headers are included from the engine's root, as "physics/PhysicsSpace.h", "game/Shapes.h" and so on.
#
# The engine logs through Console::log (utils/Console.h) and doesn't define it: the sandbox's utils/Console.cpp does,
# with its whole debug console, and a game defines its own.

set(SOFTBODY_ENGINE_DIR "${CMAKE_CURRENT_LIST_DIR}")

set(SOFTBODY_ENGINE_SOURCE_FILES
    timer.cpp
    containers/ContainerPlatform.cpp
    game/Shapes.cpp
    physics/Physics.cpp
    physics/Physics.h
    physics/CollisionSet.h
    physics/CollisionSet.cpp
    physics/PhysicsSpace.h
    physics/PhysicsSpace.cpp
    physics/Springs.h
    physics/Springs.cpp
    physics/CollisionSolver.h
    physics/CollisionSolver.cpp
    physics/CollisionSolverInternal.h
    physics/CollisionSolverPerf.cpp
    physics/ShapeAxisSeparator.h
    physics/ShapeAxisSeparator.cpp
    physics/Integrator.h
    physics/Integrator.cpp
    physics/ShapeUtils.h
    physics/ShapeUtils.cpp
    physics/PhysicsSpaceStorage.h
    physics/PhysicsSpaceStorage.cpp
    math/Vector2.h
    math/Vector2.cpp
    tasks/Scheduler.h
    tasks/Scheduler.cpp
    stb_sprintf/stb_sprintf.cpp
    utils/MinMax.h
    utils/Console.h
    containers/Array.h
    containers/ContainerPlatform.h
    containers/Range.h
    containers/Span.h
)

if(NOT TARGET SoftBodyPhysicsEngine)
    list(TRANSFORM SOFTBODY_ENGINE_SOURCE_FILES PREPEND "${SOFTBODY_ENGINE_DIR}/" OUTPUT_VARIABLE SOFTBODY_ENGINE_SOURCE_PATHS)
    add_library(SoftBodyPhysicsEngine STATIC ${SOFTBODY_ENGINE_SOURCE_PATHS})
    target_include_directories(SoftBodyPhysicsEngine PUBLIC "${SOFTBODY_ENGINE_DIR}")

    # SDL3 for timing and reading files: Emscripten's port on the web, the installed framework natively. Emscripten
    # calls the port experimental, with a warning that -Werror would stop the build on.
    if(EMSCRIPTEN)
        target_compile_options(SoftBodyPhysicsEngine PUBLIC "SHELL:-sUSE_SDL=3" -Wno-experimental)
        target_link_options(SoftBodyPhysicsEngine PUBLIC "SHELL:-sUSE_SDL=3" -Wno-experimental)
    else()
        target_compile_options(SoftBodyPhysicsEngine PUBLIC "-F/Library/Frameworks")
        target_link_options(SoftBodyPhysicsEngine PUBLIC "-F/Library/Frameworks")
        target_link_libraries(SoftBodyPhysicsEngine PUBLIC "-framework SDL3")
    endif()

    # The sandbox compiles these files into its own targets, so it only builds the library when asked to
    if(CMAKE_CURRENT_LIST_DIR STREQUAL CMAKE_SOURCE_DIR)
        set_target_properties(SoftBodyPhysicsEngine PROPERTIES EXCLUDE_FROM_ALL TRUE)
    endif()
endif()
