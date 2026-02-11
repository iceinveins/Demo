#include "logger.hpp"
#include <spdlog/async.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <iostream>
#include <ctime>
#include <string>

std::shared_ptr<spdlog::logger> g_logger;
// 获取格式化时间戳字符串：YYYYMMDDHHMMSS 格式（如20260115095805）
std::string getCurrentTimeStampStr()
{
    // 1. 获取当前系统时间戳（秒级，从1970到现在的秒数）
    std::time_t nowTime = std::time(nullptr);
    
    // 2. 转换为「本地时区」的年月日时分秒结构体
    std::tm localTime = *std::localtime(&nowTime);

    // 3. 格式化输出到字符数组，固定长度15足够存 14位时间+结束符
    char timeBuf[15] = {0};
    // 核心格式化指令：%Y%m%d%H%M%S 严格对应 年 月 日 时 分 秒
    std::strftime(timeBuf, sizeof(timeBuf), "%Y%m%d%H%M%S", &localTime);

    // 4. 转为string返回
    return std::string(timeBuf);
}

void initLogger() {
    try {
        spdlog::init_thread_pool(102400, 1);
        auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(
            "log_file/" + getCurrentTimeStampStr() + ".log", false);
        auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        
        std::vector<spdlog::sink_ptr> sinks{file_sink, console_sink};
        g_logger = std::make_shared<spdlog::async_logger>(
            "async_logger", sinks.begin(), sinks.end(),
            spdlog::thread_pool(),
            spdlog::async_overflow_policy::block);
            
        g_logger->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%l] [%t] %v");
        g_logger->set_level(spdlog::level::info);
        
        spdlog::register_logger(g_logger);
    }
    catch (const spdlog::spdlog_ex& ex) {
        std::cerr << "Logger initialization failed: " << ex.what() << std::endl;
    }
}