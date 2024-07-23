cmake_minimum_required(VERSION 3.10)
project(SoftBodyPhysics LANGUAGES CXX ISPC)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
# Set the compiler flags for the debug build type
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Og -g -fno-omit-frame-pointer -finline-functions")


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
add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/PhysicsSIMD.o
    COMMAND ispc ${CMAKE_CURRENT_SOURCE_DIR}/game/PhysicsSIMD.ispc
            -o ${CMAKE_CURRENT_BINARY_DIR}/PhysicsSIMD.o
            -h ${CMAKE_CURRENT_BINARY_DIR}/PhysicsSIMD_ispc.h
            ${CMAKE_ISPC_FLAGS}
            --target=${CMAKE_ISPC_INSTRUCTION_SETS}
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/game/PhysicsSIMD.ispc
    COMMENT "Compiling ISPC file PhysicsSIMD.ispc"
)


SET(SOURCE_FILES timer.cpp
game/Game.cpp
game/Scenes.cpp
utils/Console.cpp
containers/ContainerPlatform.cpp
game/Shapes.cpp
game/Physics.cpp
game/PhysicsTests.cpp
imgui/imgui.cpp
imgui/imgui_demo.cpp
imgui/imgui_draw.cpp
imgui/imgui_widgets.cpp
imgui/imgui_tables.cpp
imgui/imgui_impl_sdlrenderer2.cpp
imgui/imgui_impl_sdl2.cpp
game/GameRenderer.cpp
game/GameRenderer.h
game/Physics.test.h
game/Physics.h
game/PhysicsTests.h
game/Scenes.h
stb_image/stb_image.cpp
utils/Console.h
utils/UnitTestUtil.h
utils/MinMax.h
containers/Array.h
containers/ContainerPlatform.h
containers/Range.h
containers/Span.h
${CMAKE_CURRENT_BINARY_DIR}/PhysicsSIMD.o)

add_executable(SoftBodyPhysics
 main.cpp
 ${SOURCE_FILES}
 )

 add_executable(PerfTest
 perftest.cpp
 ${SOURCE_FILES}
 ) 

target_include_directories(SoftBodyPhysics PRIVATE ${CMAKE_SOURCE_DIR}/imgui)
target_include_directories(PerfTest PRIVATE ${CMAKE_SOURCE_DIR}/imgui)

# add_executable(SoftBodyPhysicsTest test.main.cpp containers/ContainerPlatform.cpp game/Shapes.cpp game/Physics.cpp)
# target_include_directories(SoftBodyPhysicsTest PRIVATE ${CMAKE_SOURCE_DIR}/imgui)
# Link against C standard library
if(CMAKE_COMPILER_IS_GNUCXX OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    target_link_libraries(SoftBodyPhysics PRIVATE c)
    target_link_libraries(PerfTest PRIVATE c)

endif()

target_include_directories(SoftBodyPhysics PRIVATE ${SDL2_INCLUDE_DIRS})
target_link_libraries(SoftBodyPhysics PRIVATE ${SDL2_LIBRARIES})

target_include_directories(PerfTest PRIVATE ${SDL2_INCLUDE_DIRS})
target_link_libraries(PerfTest PRIVATE ${SDL2_LIBRARIES})