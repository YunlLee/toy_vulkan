//
// Created by Yun on 2024/7/19.
//

#ifndef WINDOW_HPP
#define WINDOW_HPP

#include "Common.hpp"

namespace toy
{
    class Window
    {
    public:
        Window(int width, int height, const std::string& title);
        ~Window();

        [[nodiscard]] GLFWwindow* GetHandle() const { return mHandle; }

    private:
        GLFWwindow* mHandle;
    };
}

#endif //WINDOW_HPP
