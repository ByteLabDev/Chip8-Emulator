cmake_minimum_required(VERSION 3.24)
project(Chip8_Emulator)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

# 1. Fetch SDL3
FetchContent_Declare(
  SDL3
  GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
  GIT_TAG        main 
)
FetchContent_MakeAvailable(SDL3)

# 2. Fetch ImGui
FetchContent_Declare(
  imgui
  GIT_REPOSITORY https://github.com/ocornut/imgui.git
  GIT_TAG        master 
)
FetchContent_MakeAvailable(imgui)
FetchContent_GetProperties(imgui SOURCE_DIR IMGUI_SOURCE_DIR)

# 3. Define Source Files (Including ImGui core and backends)
set(SOURCE_FILES 
    src/main.cpp 
    src/Menu.cpp 
    src/Screen.cpp 
    src/Sound.cpp 
    src/Chip.cpp
    src/Keypad.cpp
    # ImGui Core
    ${IMGUI_SOURCE_DIR}/imgui.cpp
    ${IMGUI_SOURCE_DIR}/imgui_draw.cpp
    ${IMGUI_SOURCE_DIR}/imgui_widgets.cpp
    ${IMGUI_SOURCE_DIR}/imgui_tables.cpp
    ${IMGUI_SOURCE_DIR}/imgui_demo.cpp
    ${IMGUI_SOURCE_DIR}/backends/imgui_impl_sdl3.cpp
    ${IMGUI_SOURCE_DIR}/backends/imgui_impl_sdlrenderer3.cpp)

# 4. Create the executable
add_executable(chip8 WIN32 ${SOURCE_FILES})

# 5. Setup Include Directories for ImGui
target_include_directories(chip8 PRIVATE 
    ${IMGUI_SOURCE_DIR} 
    ${IMGUI_SOURCE_DIR}/backends
)

# 6. Link SDL3 and necessary system libraries
target_link_libraries(chip8 PRIVATE SDL3::SDL3 opengl32)

# 7. Automated DLL Copy (Windows only)
if(WIN32)
    add_custom_command(TARGET chip8 POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:SDL3::SDL3>
        $<TARGET_FILE_DIR:chip8>
    )
endif()

install(TARGETS chip8 RUNTIME DESTINATION bin)
if(TARGET SDL3::SDL3-shared)
    install(FILES $<TARGET_FILE:SDL3::SDL3-shared> DESTINATION bin)
elseif(TARGET SDL3::SDL3)
    install(FILES $<TARGET_FILE:SDL3::SDL3> DESTINATION bin)
endif()