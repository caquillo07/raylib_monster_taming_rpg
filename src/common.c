//
// Created by Hector Mejia on 3/15/24.
//

#include "common.h"

bool isDebug = false;

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

void panic(const char *message) {
    slogf("%s", message);
    exit(EXIT_FAILURE);
}

// todo(hector) - add line number through a macro?
void panicIf(bool condition, const char *message) {
    if (condition) {
        panic(message);
    }
}

void panicIfNil(void *ptr, const char *message) {
    if (ptr == nil) {
        panic(message);
    }
}

Size size_from_rectangle(Rectangle rect) {
    Size result = {.height = rect.height, .width = rect.width};
    return result;
}
