cmake_minimum_required(VERSION 3.10)
project(SoftBodyPhysics LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Disable exceptions
if(CMAKE_COMPILER_IS_GNUCXX OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    add_compile_options(-fno-exceptions -fno-rtti -nostdlib++)
elseif(MSVC)
    add_compile_options(/EHsc- /GR- /Zl)
endif()

set(CMAKE_INSTALL_RPATH "/Library/Frameworks")
set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)

find_package(SDL2 REQUIRED)

add_executable(SoftBodyPhysics
 main.cpp
 SDLGameRenderer.cpp
 timer.cpp
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
 game/Physics.test.h
 game/Physics.h
 game/PhysicsTests.h
 game/Scenes.h
 utils/Console.h
 utils/UnitTestUtil.h
 utils/MinMax.h
 containers/Array.h
 containers/ContainerPlatform.h
 containers/Range.h
 containers/Span.h
 )
target_include_directories(SoftBodyPhysics PRIVATE ${CMAKE_SOURCE_DIR}/imgui)

# add_executable(SoftBodyPhysicsTest test.main.cpp containers/ContainerPlatform.cpp game/Shapes.cpp game/Physics.cpp)
# target_include_directories(SoftBodyPhysicsTest PRIVATE ${CMAKE_SOURCE_DIR}/imgui)
# Link against C standard library
if(CMAKE_COMPILER_IS_GNUCXX OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    target_link_libraries(SoftBodyPhysics PRIVATE c)
endif()

target_include_directories(SoftBodyPhysics PRIVATE ${SDL2_INCLUDE_DIRS})
target_link_libraries(SoftBodyPhysics PRIVATE ${SDL2_LIBRARIES})
