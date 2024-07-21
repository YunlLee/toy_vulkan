//
// Created by Yun on 2024/7/19.
//

#include <VkLog.hpp>

#include "iostream"
#include "Application.hpp"

int main()
{
    toy::VkLog::init();
    Application app;
    try
    {
        app.run();
    }catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
