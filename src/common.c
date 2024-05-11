//
// Created by Hector Mejia on 3/15/24.
//

#include "common.h"

void initLogger() {
    slog_init("logs", SLOG_FLAGS_ALL, false);

    slog_config_t cfg;
    slog_config_get(&cfg);
    cfg.eDateControl = SLOG_TIME_ONLY;
    cfg.nIndent = true;
//    cfg.nTraceTid = true;
    cfg.eColorFormat = SLOG_COLORING_FULL;
    cfg.nToFile = false;
    cfg.nKeepOpen = false;

    slog_config_set(&cfg);
}

void panic(const char* message) {
    slogf("%s", message);
    exit(EXIT_FAILURE);
}

void panicIf(bool condition, const char* message) {
    if (condition) {
        panic(message);
    }
}
