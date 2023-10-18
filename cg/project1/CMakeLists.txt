project(asteroids)
add_executable(${PROJECT_NAME} main.cpp window.cpp bullets.cpp
                               bubbles.cpp)
enable_abcg(${PROJECT_NAME})
