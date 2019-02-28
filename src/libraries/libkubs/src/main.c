#include <libkubs.h>
#include <stddef.h>

static Kubs_PanicFn *panicfn = NULL;

// NOTE: When kubs_panic() is called, it will always be given THIS filename
//       and a function from HANDLER_NORECOVER().
void kubs_panic(const char *message, const char *function,
        const char *filename, size_t line)
{
    if (panicfn != NULL) {
        panicfn(message, function, filename, line);
    }
}


void kubs_init(Kubs_PanicFn *panicfn_)
{
    panicfn = panicfn_;
}

// FIXME: add caller pc to the error message (possibly as "ubsan: error-type
// @1234ABCD").
#define HANDLER_NORECOVER(name, msg)                             \
  void __ubsan_handle_##name##_abort() {                         \
    kubs_panic("ubsan: " msg, "unknown function", "(unknown file)", 0); \
  }

#define XHANDLER_NORECOVER(name, msg)                             \
  void __ubsan_handle_##name##_abort() { /* no-op. */ }

HANDLER_NORECOVER(type_mismatch, "type-mismatch")
HANDLER_NORECOVER(add_overflow, "add-overflow")
// FIXME: Use XHANDLER_NORECOVER->HANDLER_NORECOVER for _everything_.
XHANDLER_NORECOVER(sub_overflow, "sub-overflow")
XHANDLER_NORECOVER(mul_overflow, "mul-overflow")
HANDLER_NORECOVER(negate_overflow, "negate-overflow")
HANDLER_NORECOVER(divrem_overflow, "divrem-overflow")
HANDLER_NORECOVER(shift_out_of_bounds, "shift-out-of-bounds")
HANDLER_NORECOVER(out_of_bounds, "out-of-bounds")
HANDLER_NORECOVER(builtin_unreachable, "builtin-unreachable")
HANDLER_NORECOVER(missing_return, "missing-return")
HANDLER_NORECOVER(vla_bound_not_positive, "vla-bound-not-positive")
HANDLER_NORECOVER(float_cast_overflow, "float-cast-overflow")
HANDLER_NORECOVER(load_invalid_value, "load-invalid-value")
HANDLER_NORECOVER(invalid_builtin, "invalid-builtin")
HANDLER_NORECOVER(function_type_mismatch, "function-type-mismatch")
HANDLER_NORECOVER(implicit_conversion, "implicit-conversion")
HANDLER_NORECOVER(nonnull_arg, "nonnull-arg")
HANDLER_NORECOVER(nonnull_return, "nonnull-return")
HANDLER_NORECOVER(nullability_arg, "nullability-arg")
HANDLER_NORECOVER(nullability_return, "nullability-return")
HANDLER_NORECOVER(pointer_overflow, "pointer-overflow")
HANDLER_NORECOVER(cfi_check_fail, "cfi-check-fail")
XHANDLER_NORECOVER(type_mismatch_v1, "type-mismatch-v1")
