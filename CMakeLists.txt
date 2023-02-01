cmake_minimum_required(VERSION 3.10)
project(epanet-gui)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
# Include headers
include_directories(includes/imgui includes/implot includes/imgui/backends)

# Set linker flags
# set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /subsystem:console")

# Add source files
# file(GLOB IMGUI_SOURCES "includes/imgui/*.cpp")
# file(GLOB IMPLOT_SOURCES "includes/implot/*.cpp")
# file(GLOB IMGUI_BACKEND_SOURCES "includes/imgui/backends/imgui_impl_win32.cpp" "includes/imgui/backends/imgui_impl_dx12.cpp")
# file(GLOB PROJECT_SOURCES "drawing.cpp" "parseinp.cpp" "views.cpp" "saveinp.cpp" "main.cpp")
file(GLOB PROJECT_SOURCES_GA "parseinp.cpp" "saveinp.cpp" "epanet-ga.cpp" "main-ga.cpp")

# Create executable
# add_executable(epanet ${IMGUI_SOURCES} ${IMPLOT_SOURCES} ${IMGUI_BACKEND_SOURCES} ${PROJECT_SOURCES})
add_executable(epanet-ga ${PROJECT_SOURCES_GA})

# Link libraries
# target_link_libraries(epanet d3d12.lib d3dcompiler.lib idxgi.lib)
target_link_libraries(epanet-ga Threads::Threads )

# Compiler definitions
add_compile_definitions(ImTextureID=ImU64)
add_compile_definitions(UNICODE)
add_compile_definitions(_UNICODE)

# Copy resources
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/resources/ DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/resources/)

