add_executable(Sandbox
        Sandbox/main.cpp
        Sandbox/Private/Application.cpp
)

target_link_libraries(Sandbox PUBLIC Vulkan)