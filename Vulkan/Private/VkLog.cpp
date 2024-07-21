//
// Created by Yun on 2024/7/19.
//


#include "VkLog.hpp"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/async.h"

namespace toy
{
    VkLog VkLog::sLoggerInstance{};

    static std::shared_ptr<spdlog::logger> sSpdLogger{};

    void VkLog::init()
    {
        sSpdLogger = spdlog::stdout_color_mt<spdlog::async_factory>("async_logger");
        sSpdLogger->set_level(spdlog::level::trace);
        sSpdLogger->set_pattern("%^%H:%M:%S:%e [%P-%t] [%1!L] [%20s:%-4#] - %v%$");
    }

    void VkLog::Log(spdlog::source_loc loc, spdlog::level::level_enum lvl, const spdlog::memory_buf_t& buffer)
    {
        sSpdLogger->log(loc, lvl, spdlog::string_view_t(buffer.data(), buffer.size()));
    }
}
