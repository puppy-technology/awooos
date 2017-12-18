#ifndef KERNEL_H
#define KERNEL_H

#include <stddef.h>
#include <awoo/modifiers.h>

void hal_init();
void hal_shutdown();
void hal_hard_shutdown();
void hal_test_fail_shutdown();
size_t *hal_badmalloc_start_address();
char *hal_compiler_information();
void kprint(const char *string);

noreturn _panic(const char *message, const char *function,
                const char* filename, size_t line);

#define panic(message) _panic(message, __FUNCTION__, __FILE__, __LINE__)

#endif
