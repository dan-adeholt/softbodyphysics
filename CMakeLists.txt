cmake_minimum_required(VERSION 3.10)
#project(SoftBodyPhysics LANGUAGES CXX ISPC)
project(SoftBodyPhysics LANGUAGES CXX)

include(CTest)   # or enable_testing()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
# Set the compiler flags for the debug build type
#set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Og -g -fno-omit-frame-pointer -finline-functions")

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

# ISPC settings for ARM (M1)
set(CMAKE_ISPC_INSTRUCTION_SETS "neon")

# Disable exceptions
if(CMAKE_COMPILER_IS_GNUCXX OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    add_compile_options(-fno-exceptions -fno-rtti -nostdlib++)
elseif(MSVC)
    add_compile_options(/EHsc- /GR- /Zl)
endif()

set(CMAKE_INSTALL_RPATH "/Library/Frameworks")
set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)

find_package(SDL2 REQUIRED)

# ISPC compilation
# add_custom_command(
#     OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/PhysicsSIMD.o
#     COMMAND ispc ${CMAKE_CURRENT_SOURCE_DIR}/physics/PhysicsSIMD.ispc
#             -o ${CMAKE_CURRENT_BINARY_DIR}/PhysicsSIMD.o
#             -h ${CMAKE_CURRENT_BINARY_DIR}/PhysicsSIMD_ispc.h
#             ${CMAKE_ISPC_FLAGS}
#             --target=${CMAKE_ISPC_INSTRUCTION_SETS}
#     DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/physics/PhysicsSIMD.ispc
#     COMMENT "Compiling ISPC file PhysicsSIMD.ispc"
# )



# Path to the Python script
set(FIND_TESTS_SCRIPT "${CMAKE_SOURCE_DIR}/find_tests.py")

# Run the Python script to generate a temporary CMake file with add_test calls
execute_process(
    COMMAND python3 "${FIND_TESTS_SCRIPT}"
    WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
    OUTPUT_FILE "${CMAKE_BINARY_DIR}/generated_tests.cmake"
    RESULT_VARIABLE result
)


if(result)
    message(FATAL_ERROR "Failed to run ${FIND_TESTS_SCRIPT}")
endif()

SET(SOURCE_FILES timer.cpp
    game/Game.cpp
    game/Editor.cpp
    game/Scenes.cpp
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
    imgui/imgui_impl_sdlrenderer2.cpp
    imgui/imgui_impl_sdl2.cpp
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
    physics/Integrator.h
    physics/Integrator.cpp
    physics/PhysicsSpaceStorage.h
    physics/PhysicsSpaceStorage.cpp
    physics/ShapeMatching.h
    physics/ShapeMatching.cpp
    physics/PhysicsTests.h
    math/Vector2.h
    math/Vector2.cpp
    game/Scenes.h
    game/Editor.h
    tasks/Scheduler.h
    stb_image/stb_image.cpp
    utils/Console.h
    utils/UnitTestUtil.h
    utils/MinMax.h
    containers/Array.h
    containers/ContainerPlatform.h
    containers/Range.h
    containers/Span.h
    # ${CMAKE_CURRENT_BINARY_DIR}/PhysicsSIMD.o
)

add_executable(SoftBodyPhysics
    main.cpp
    ${SOURCE_FILES}
)

 add_executable(PerfTest
    perftest.cpp
    ${SOURCE_FILES}
 ) 

 add_executable(SoftBodyPhysicsTests
    tests.cpp
    ${SOURCE_FILES}
 )  

target_include_directories(SoftBodyPhysics PRIVATE ${CMAKE_SOURCE_DIR}/imgui)
target_include_directories(PerfTest PRIVATE ${CMAKE_SOURCE_DIR}/imgui)
target_include_directories(SoftBodyPhysicsTests PRIVATE ${CMAKE_SOURCE_DIR}/imgui)


# Link against C standard library
if(CMAKE_COMPILER_IS_GNUCXX OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    target_link_libraries(SoftBodyPhysics PRIVATE c)
    target_link_libraries(PerfTest PRIVATE c)
    target_link_libraries(SoftBodyPhysicsTests PRIVATE c)

endif()

target_include_directories(SoftBodyPhysics PRIVATE ${SDL2_INCLUDE_DIRS})
target_link_libraries(SoftBodyPhysics PRIVATE ${SDL2_LIBRARIES})

target_include_directories(PerfTest PRIVATE ${SDL2_INCLUDE_DIRS})
target_link_libraries(PerfTest PRIVATE ${SDL2_LIBRARIES})

target_include_directories(SoftBodyPhysicsTests PRIVATE ${SDL2_INCLUDE_DIRS})
target_link_libraries(SoftBodyPhysicsTests PRIVATE ${SDL2_LIBRARIES})



# Include the generated add_test calls
include("${CMAKE_BINARY_DIR}/generated_tests.cmake")
