#pragma once

#include <stdbool.h>
#include "result.h"
#include "assert.h"
#include "log.h"
#include "ff.h"

#include "rcutils/filesystem.h"
#include "rcl/macros.h"
//#include "rcl/publisher.h"
//#include "rcl/logging.h"

/**
 * Runs all tests for `rcutils` functions.
 * @return
 */
result_t ros_e2e_run_all_tests(void);

/* ================================
 * Tests for rcutils `filesystem.c` functions.
 * ================================ */

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

