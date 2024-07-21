//
// Created by Yun on 2024/7/19.
//

#ifndef VKLOG_HPP
#define VKLOG_HPP

#include "spdlog/common.h"

namespace toy
{
    class VkLog
    {
    public:
        VkLog(const VkLog&) = delete;
        VkLog& operator=(const VkLog&) = delete;

        static void init();

        static VkLog* GetLoggerInstance(){ return &sLoggerInstance; }

        template<typename... Args>
        void Log(spdlog::source_loc loc, spdlog::level::level_enum lvl, spdlog::format_string_t<Args...> fmt, Args &&...args){
            spdlog::memory_buf_t buf;
            fmt::vformat_to(fmt::appender(buf), fmt, fmt::make_format_args(args...));
            Log(loc, lvl, buf);
        }

        ~VkLog() = default;
    private:
        VkLog() = default;

        void Log(spdlog::source_loc loc, spdlog::level::level_enum lvl, const spdlog::memory_buf_t& buffer);
        static VkLog sLoggerInstance;
    };

#define AD_LOG_LOGGER_CALL(adLog, level, ...)\
    (adLog)->Log(spdlog::source_loc{__FILE__, __LINE__, SPDLOG_FUNCTION}, level, __VA_ARGS__)

#define LOG_T(...) AD_LOG_LOGGER_CALL(toy::VkLog::GetLoggerInstance(), spdlog::level::trace, __VA_ARGS__)
#define LOG_D(...) AD_LOG_LOGGER_CALL(toy::VkLog::GetLoggerInstance(), spdlog::level::debug,  __VA_ARGS__)
#define LOG_I(...) AD_LOG_LOGGER_CALL(toy::VkLog::GetLoggerInstance(), spdlog::level::info, __VA_ARGS__)
#define LOG_W(...) AD_LOG_LOGGER_CALL(toy::VkLog::GetLoggerInstance(), spdlog::level::warn, __VA_ARGS__)
#define LOG_E(...) AD_LOG_LOGGER_CALL(toy::VkLog::GetLoggerInstance(), spdlog::level::err, __VA_ARGS__)

}

#endif //VKLOG_HPP
