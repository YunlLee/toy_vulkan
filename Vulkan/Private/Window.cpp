//
// Created by Yun on 2024/7/19.
//

#include "Window.hpp"

namespace toy
{
    Window::Window(int width, int height, const std::string& title)
    {
        glfwInit();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        mHandle = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);

        LOG_T("----------------------");
        LOG_T("{0} : window: {1}", __FUNCTION__, (void*)mHandle);
    }

    Window::~Window()
    {
        glfwDestroyWindow(mHandle);
        glfwTerminate();
        LOG_T("----------------------");
        LOG_T("The application running end.");
    }
}

