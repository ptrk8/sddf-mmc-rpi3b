#pragma once

#include "result.h"
#include "diskio_test.h"
#include "ff.h"
#include "assert.h"
#include "log.h"
#include "memory_facade.h"

/**
 * Tests the low-level diskio.c glue functions that call into our MMC driver.
 * @return
 */
result_t fatfs_e2e_diskio_test(void);

/**
 * Tests the `f_stat` function.
 * @return
 */
result_t fatfs_e2e_stat(void);

/**
 * Simple read/write test.
 * @return
 */
result_t fatfs_e2e_write_close_read_simple(void);

/**
 * Tests fsync.
 * @return
 */
result_t fatfs_e2e_write_fsync_read_close_simple(void);

/**
 * Tests writing/reading a user-specified amount to/from file.
 * @param buf_size_in_bytes
 * @return
 */
result_t fatfs_e2e_write_read_custom(size_t buf_size_in_bytes);
