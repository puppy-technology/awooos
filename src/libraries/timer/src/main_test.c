#include <tinker.h>
#include <stddef.h>
#include <timer.h>
#include <string.h>

// void timer_copy(Timer *original, Timer *copy);
//
// void gtimer_increment_us(uint32_t microseconds);
// void gtimer_increment_ms(uint32_t milliseconds);
// TimerDuration gtimer_in_microseconds(Timer *timer);
// TimerDuration gtimer_in_milliseconds(Timer *timer);
// void gtimer_copy(Timer *copy);

char *tinker_uint_to_str(unsigned long n);
void tinker_print(const char *string);

void test_timer_instance(void)
{
    Timer a = {0,0};
    Timer b = {0,0};
    Timer c = {0,0};

    // a = {.seconds=0, .microseconds=0}
    // b = {.seconds=0, .microseconds=0}
    tinker_assert(timer_in_microseconds(&a) == 0);
    tinker_assert(timer_in_milliseconds(&a) == 0);
    tinker_assert(timer_in_microseconds(&b) == 0);
    tinker_assert(timer_in_milliseconds(&b) == 0);

    // a = {.seconds=0, .microseconds=5}
    // b = {.seconds=0, .microseconds=0}
    timer_increment_us(&a, 5);
    tinker_assert(a.seconds == 0);
    tinker_assert(a.microseconds == 5);
    tinker_assert(timer_in_microseconds(&a) == 5);
    tinker_assert(timer_in_microseconds(&b) == 0);

    // a = {.seconds=0, .microseconds=5}
    // b = {.seconds=0, .microseconds=5000}
    timer_increment_ms(&b, 5);
    tinker_assert(timer_in_microseconds(&a) == 5);
    tinker_assert(timer_in_microseconds(&b) == 5000);
    tinker_assert(timer_duration_us(&a, &b) == 4995);

    // a = {.seconds=0, .microseconds=5}
    // b = {.seconds=0, .microseconds=5005}
    timer_increment_us(&b, 5);
    tinker_assert(b.seconds == 0);
    tinker_assert(b.microseconds == 5005);
    tinker_assert(timer_duration_us(&a, &b) == 5000);
    tinker_assert(timer_duration(&a, &b) == 5);

    // a = {.seconds=0, .microseconds=5}
    // b = {.seconds=0, .microseconds=5005}
    // c = {.seconds=0, .microseconds=5005}
    timer_copy(&b, &c);
    tinker_assert(b.seconds == c.seconds);
    tinker_assert(b.microseconds == c.microseconds);
    tinker_assert(timer_in_microseconds(&b) == timer_in_microseconds(&c));

    // a = {.seconds=0, .microseconds=5}
    // b = {.seconds=0, .microseconds=5005}
    // c = {.seconds=0, .microseconds=5005}
    timer_increment_ms(&c, 1000);
    timer_increment_us(&c, 5);
    tinker_assert(b.seconds == 0);
    tinker_assert(b.microseconds == 5005);
    tinker_assert(c.seconds == 1);
    tinker_assert(c.microseconds == 5010);
}


void add_timer_tests()
{
    tinker_add_test(timer_instance);
}
