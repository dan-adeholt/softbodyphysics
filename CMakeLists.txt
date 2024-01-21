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


find_package(SDL2 REQUIRED)

add_executable(SoftBodyPhysics main.cpp SDLGameRenderer.cpp timer.cpp game/Game.cpp containers/ContainerPlatform.cpp game/Shapes.cpp game/Physics.cpp)
add_executable(SoftBodyPhysicsTest test.main.cpp containers/ContainerPlatform.cpp game/Shapes.cpp game/Physics.cpp)

# Link against C standard library
if(CMAKE_COMPILER_IS_GNUCXX OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    target_link_libraries(SoftBodyPhysics PRIVATE c)
endif()

target_include_directories(SoftBodyPhysics PRIVATE ${SDL2_INCLUDE_DIRS})
target_link_libraries(SoftBodyPhysics PRIVATE ${SDL2_LIBRARIES})
