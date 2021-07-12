#include <tinker.h>
#include <stdint.h>

#define MULTIBOOT_MAGIC 0x2BADB002

uint32_t hal_get_magic(void);

void test_hal_assertions(void)
{
    tinker_assert(hal_get_magic() == MULTIBOOT_MAGIC);
}

void add_hal_tests(void)
{
    tinker_add_test(hal_assertions);
}
