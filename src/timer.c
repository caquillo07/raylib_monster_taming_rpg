//
// Created by Hector Mejia on 8/20/24.
//

#include "timer.h"
#include <raylib.h>

void timer_start(Timer *timer, const double lifetime) {
    panicIfNil(timer, "provided a nil timer");
    timer->startTime = GetTime();
    timer->lifeTime = lifetime;
}

void timer_stop(Timer *timer) {
    panicIfNil(timer, "provided a nil timer");
    timer->lifeTime = 0;
    timer->startTime = 0;
}

bool timer_is_valid(const Timer timer) {
    return timer.lifeTime != 0 && timer.startTime != 0;
}

bool timer_done(const Timer timer) {
    panicIf(!timer_is_valid(timer), "received invalid timer in timer_done");
    return GetTime() - timer.startTime >= timer.lifeTime;
}

f64 timer_get_elapsed(const Timer timer) {
    return GetTime() - timer.startTime;
}

void timer_reset(Timer *timer) {
    panicIfNil(timer, "provided a nil timer");
    timer->startTime = GetTime();
}
