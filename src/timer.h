//
// Created by Hector Mejia on 8/20/24.
//

#ifndef TIMER_H
#define TIMER_H
#include "common.h"

typedef struct Timer {
    double startTime;   // Start time (seconds)
    double lifeTime;    // Lifetime (seconds)
} Timer;

/**
 * Initializes a given timer with the given amount of seconds.
 * @param timer the timer to init
 * @param lifetime how long is the timer, in seconds
 */
void timer_start(Timer *timer, double lifetime);

/**
 * Stops the timer, this sets all of its interval values to 0. A timer must be
 * re-initialized with timer_start for it to work again.
 * @param timer the timer to stop
 */
void timer_stop(Timer *timer);

/**
 * checks wether or not a given timer is running
 * @param timer the timer to check
 * @return true if valid, false otherwise
 */
bool timer_is_valid(Timer timer);

/**
 * Returns a boolean to indicate if the given timer has reached its elapsed time,
 * must be reset with timer_reset for it to start counting again.
 * @param timer the timer to check
 * @return true if done, false otherwise
 */
bool timer_done(Timer timer);

/**
 * Returns the elapsed time since the timer was started.
 * @param timer the timer to check
 * @return the timer that has passed since the timer was started
 */
f64 timer_get_elapsed(Timer timer);

/**
 * resets the timer for another tick, the timer will start counting from 0 again
 * up to the desired elapsed time.
 * @param timer the timer to reset
 */
void timer_reset(Timer *timer);

#endif //TIMER_H
