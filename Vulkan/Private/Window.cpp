//
// Created by Yun on 2024/7/19.
//

#include "Window.hpp"
#include "Application.hpp"

namespace toy
{
    static void framebufferResizeCallback(GLFWwindow* window, int w, int h)
    {
        LOG_T("width: {0}, height: {1}", w, h);
    }

    Window::Window(int width, int height, const std::string& title)
    {
        glfwInit();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
        mHandle = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
        glfwSetWindowUserPointer(mHandle, this);
        // glfwSetFramebufferSizeCallback(mHandle, framebufferResizeCallback);

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

