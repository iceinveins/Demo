#pragma once
#include <spdlog/spdlog.h>

extern std::shared_ptr<spdlog::logger> g_logger;

void initLogger();

#define logd(tag, fmt, ...) \
    g_logger->debug(spdlog::fmt_lib::format(FMT_STRING("{} " fmt), tag, ##__VA_ARGS__))
#define loge(tag, fmt, ...) \
    g_logger->error(spdlog::fmt_lib::format(FMT_STRING("{} " fmt), tag, ##__VA_ARGS__))
#define logi(tag, fmt, ...) \
    g_logger->info(spdlog::fmt_lib::format(FMT_STRING("{} " fmt), tag, ##__VA_ARGS__))
