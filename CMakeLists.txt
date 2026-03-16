cmake_minimum_required(VERSION 3.10)
project(SoftBodyPhysics LANGUAGES CXX)

include(CTest)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Og -g -fno-omit-frame-pointer -finline-functions")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -UNDEBUG")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -UNDEBUG")

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(
        -Werror
        -Wall
        -Wextra
        -Wpedantic
        -Werror=conversion
        -Werror=sign-conversion
        -Werror=null-dereference
        -Werror=return-type
        -Werror=uninitialized
        -Wno-unused-variable
        -Wno-unused-parameter
        -Wno-unused-but-set-variable
    )
endif()

if(MSVC)
    add_compile_options(
        /W4
        /WX
        /we4244
        /we4267
        /we4389
        /we4700
        /we4701
        /wd4100
    )
endif()

if(NOT EMSCRIPTEN)
    if(CMAKE_COMPILER_IS_GNUCXX OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
        add_compile_options(-fno-exceptions -fno-rtti -nostdlib++)
    elseif(MSVC)
        add_compile_options(/EHsc- /GR- /Zl)
    endif()

    set(CMAKE_INSTALL_RPATH "/Library/Frameworks")
    set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
    find_package(SDL3 REQUIRED CONFIG REQUIRED COMPONENTS SDL3-shared)
endif()

set(SOURCE_FILES
    timer.cpp
    game/Game.cpp
    game/Editor.cpp
    game/Scenes.cpp
    game/Textures.cpp
    utils/Console.cpp
    utils/UnitTestUtil.cpp
    containers/ContainerPlatform.cpp
    game/Shapes.cpp
    physics/Physics.cpp
    physics/PhysicsTests.cpp
    imgui/imgui.cpp
    imgui/imgui_demo.cpp
    imgui/imgui_draw.cpp
    imgui/imgui_widgets.cpp
    imgui/imgui_tables.cpp
    imgui/imgui_impl_sdlrenderer3.cpp
    imgui/imgui_impl_sdl3.cpp
    tasks/Scheduler.cpp
    game/GameRenderer.cpp
    game/GameRenderer.h
    physics/Physics.test.h
    physics/Physics.h
    physics/CollisionSet.h
    physics/CollisionSet.cpp
    physics/PhysicsSpace.h
    physics/PhysicsSpace.cpp
    physics/Springs.h
    physics/Springs.cpp
    physics/CollisionSolver.h
    physics/CollisionSolver.cpp
    physics/ShapeAxisSeparator.h
    physics/ShapeAxisSeparator.cpp
    physics/Integrator.h
    physics/Integrator.cpp
    physics/ShapeUtils.h
    physics/ShapeUtils.cpp
    physics/PhysicsSpaceStorage.h
    physics/PhysicsSpaceStorage.cpp
    utils/CustomFont.cpp
    physics/PhysicsTests.h
    math/Vector2.h
    math/Vector2.cpp
    game/Scenes.h
    game/Editor.h
    game/Textures.h
    tasks/Scheduler.h
    stb_image/stb_image.cpp
    stb_sprintf/stb_sprintf.cpp
    utils/Console.h
    utils/UnitTestUtil.h
    utils/MinMax.h
    utils/DynamicLibrary.h
    utils/CustomFont.h
    containers/Array.h
    containers/ContainerPlatform.h
    containers/Range.h
    containers/Span.h
)

set(NATIVE_ONLY_SOURCE_FILES
    utils/DynamicLibrary.cpp
)

function(configure_common_target target_name)
    target_include_directories(${target_name} PRIVATE "${CMAKE_SOURCE_DIR}/imgui")
    target_include_directories(${target_name} PRIVATE "${CMAKE_SOURCE_DIR}/external/include")

    if(CMAKE_COMPILER_IS_GNUCXX OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
        target_link_libraries(${target_name} PRIVATE c)
    endif()
endfunction()

if(EMSCRIPTEN)
    file(MAKE_DIRECTORY "${CMAKE_SOURCE_DIR}/web/public/generated")

    add_executable(SoftBodyPhysicsWeb
        web_main.cpp
        main.cpp
        ${SOURCE_FILES}
    )

    configure_common_target(SoftBodyPhysicsWeb)
    target_compile_options(SoftBodyPhysicsWeb PRIVATE
        -Wno-error
        -Wno-error=c++20-designator
        -Wno-error=deprecated-pragma
        -Wno-error=implicit-int-float-conversion
        -Wno-error=missing-designated-field-initializers
        -Wno-error=reorder-init-list
        -Wno-error=uninitialized
        -Wno-error=uninitialized-const-pointer
    )

    set_target_properties(SoftBodyPhysicsWeb PROPERTIES
        OUTPUT_NAME "softbodyphysics"
        SUFFIX ".js"
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/web/public/generated"
    )

    target_link_options(SoftBodyPhysicsWeb PRIVATE
        "SHELL:-sUSE_SDL=3"
        "SHELL:-sALLOW_MEMORY_GROWTH=1"
        "SHELL:-sFORCE_FILESYSTEM=1"
        "SHELL:-lidbfs.js"
        "SHELL:-sMODULARIZE=1"
        "SHELL:-sEXPORT_ES6=1"
        "SHELL:-sEXPORT_NAME=createSoftBodyPhysicsModule"
        "SHELL:-sEXIT_RUNTIME=0"
        "SHELL:--pre-js ${CMAKE_SOURCE_DIR}/web/pre.js"
        "SHELL:--preload-file ${CMAKE_SOURCE_DIR}/data@/data"
        "SHELL:--preload-file ${CMAKE_SOURCE_DIR}/levels@/levels"
        "SHELL:--preload-file ${CMAKE_SOURCE_DIR}/scenedefs@/scenedefs"
    )

    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        target_link_options(SoftBodyPhysicsWeb PRIVATE
            "SHELL:-sASSERTIONS=2"
        )
    else()
        target_link_options(SoftBodyPhysicsWeb PRIVATE "SHELL:-O3")
    endif()
else()
    set(MYGAME_ICON "${CMAKE_CURRENT_SOURCE_DIR}/icon.icns")
    set_source_files_properties(${MYGAME_ICON} PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")

    add_library(SoftBodyPhysicsShared SHARED
        main.cpp
        ${SOURCE_FILES}
        ${NATIVE_ONLY_SOURCE_FILES}
    )

    add_executable(SoftBodyPhysics MACOSX_BUNDLE
        shell.cpp
        ${SOURCE_FILES}
        ${NATIVE_ONLY_SOURCE_FILES}
        ${MYGAME_ICON}
    )

    add_executable(PerfTest
        perftest.cpp
        ${SOURCE_FILES}
        ${NATIVE_ONLY_SOURCE_FILES}
    )

    add_executable(SoftBodyPhysicsTests
        tests.cpp
        ${SOURCE_FILES}
        ${NATIVE_ONLY_SOURCE_FILES}
    )

    foreach(target_name IN ITEMS SoftBodyPhysicsShared SoftBodyPhysics PerfTest SoftBodyPhysicsTests)
        configure_common_target(${target_name})
        target_link_options(${target_name} PRIVATE "-F/Library/Frameworks")
        target_link_libraries(${target_name} PRIVATE "-framework SDL3")
    endforeach()

    set(FIND_TESTS_SCRIPT "${CMAKE_SOURCE_DIR}/find_tests.py")
    execute_process(
        COMMAND python3 "${FIND_TESTS_SCRIPT}"
        WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
        OUTPUT_FILE "${CMAKE_BINARY_DIR}/generated_tests.cmake"
        RESULT_VARIABLE result
    )

    if(result)
        message(FATAL_ERROR "Failed to run ${FIND_TESTS_SCRIPT}")
    endif()

    include("${CMAKE_BINARY_DIR}/generated_tests.cmake")
endif()
