#pragma once

#include <stdbool.h>
#include "result.h"
#include "assert.h"
#include "log.h"
#include "ff.h"

#include "rcutils/filesystem.h"
#include "rcutils/allocator.h"
#include "rmw/init.h"
#include "rcl/macros.h"
//#include "rcl/publisher.h"
//#include "rcl/logging.h"
#include "ucdr/microcdr.h"
#include "rosidl_typesupport_microxrcedds_c/message_type_support.h"
#include "rosidl_runtime_c/u16string_functions.h"
#include "rosidl_runtime_c/primitives_sequence_functions.h"

/**
 * Runs all tests for `rcutils` functions.
 * @param heap_size Heap size in bytes.
 * @return
 */
result_t ros_e2e_run_all_tests(size_t heap_size);

/* ================================
 * rcutils
 * ================================ */

/* allocator.c */

/**
 * Tests the `__default_allocate` function.
 * @param heap_size Heap size in bytes.
 * @return
 */
result_t ros_e2e_rcutils_allocate(size_t heap_size);

/**
 * Tests the `__default_zero_allocate` function.
 * @param heap_size Heap size in bytes.
 * @return
 */
result_t ros_e2e_rcutils_zero_allocate(size_t heap_size);

/**
 * Tests the `__default_reallocate` function.
 * @param heap_size Heap size in bytes.
 * @return
 */
result_t ros_e2e_rcutils_reallocate(size_t heap_size);

/* filesystem.c */

/**
 * Tests the `rcutils_is_directory` function.
 * @return
 */
result_t ros_e2e_rcutils_is_directory(void);

/**
 * Tests the `rcutils_is_file` function.
 * @return
 */
result_t ros_e2e_rcutils_is_file(void);

/**
 * Tests the `rcutils_exists` function.
 * @return
 */
result_t ros_e2e_rcutils_exists(void);

/* ================================
 * rmw
 * ================================ */

/* init.c */

result_t ros_e2e_rmw_get_zero_initialized_context(void);

result_t ros_e2e_rmw_basic_init_shutdown(void);

/* ================================
 * Micro CDR
 * ================================ */

result_t ros_e2e_micro_cdr_usage_example(void);

/* ================================
 * rosidl_runtime_c
 * ================================ */

/* u16string_functions.c */

result_t ros_e2e_rosidl_runtime_c__U16String__Sequence__fini(void);

/* ================================
 * rosidl_typesupport_microxrcedds_c
 * ================================ */

result_t ros_e2e_rosidl_typesupport_microxrcedds_c_sanity(void);
