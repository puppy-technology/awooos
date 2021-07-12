#include <ali.h>
#include <ali/event.h>
#include <stdio.h>
#include <dmm.h>
#include <flail.h>
#include <hal.h>
#include <timer.h>
#include <tinker.h>

#define ADD_TESTS(TEST_CATEGORY) { \
        fputs("Adding " #TEST_CATEGORY " tests... ", stdout);  \
        add_##TEST_CATEGORY##_tests(); \
        fputs("Done!\n", stdout);      \
    };

void test_shutdown(bool all_tests_passed)
{
    // If a test failed, do a test-fail shutdown.
    if (!all_tests_passed) {
        puts("\n\n!!! Encountered failing tests; not booting. !!!");
        event_trigger("HAL shutdown test fail", NULL);
    }

    // If we get this far, we assume all of the tests passed.
    event_trigger("HAL shutdown", NULL);
}

void run_tests(void *data)
{
    bool all_tests_passed;
    bool test_build = *((bool*)data);

    ADD_TESTS(hal)
    ADD_TESTS(ali)
    ADD_TESTS(dmm)
    ADD_TESTS(timer)

    all_tests_passed = tinker_run_tests(&putchar);

    // Handle things we _only_ do in test builds.
    if (test_build) {
        if (!all_tests_passed) {
            test_shutdown(false);
        }

        // This doesn't catch all failures, but it'll catch
        // unintentional infinite loops by causing the CI
        // build to time out.
        //
        // We currently only run this during test builds,
        // to avoid concerning people
        test_flail_intentional_panic();

        // Test builds always shut down.
        // The `all_tests_passed` argument dictates
        // if it tries to tell QEMU to return a nonzero exit code.
        test_shutdown(all_tests_passed);
    }
}

__attribute__((constructor))
void tests_register_events(void)
{
    event_watch("tests run", run_tests);
}
