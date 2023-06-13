#include "ros_e2e.h"

result_t ros_e2e_run_all_tests(size_t heap_size) {
    log_info("Starting ros_e2e_run_all_tests().");

    result_t res;

    /* rcutils: allocator.c */

    res = ros_e2e_rcutils_allocate(heap_size);
    if (result_is_err(res)) return res;

    res = ros_e2e_rcutils_zero_allocate(heap_size);
    if (result_is_err(res)) return res;

    res = ros_e2e_rcutils_reallocate(heap_size);
    if (result_is_err(res)) return res;

    /* rcutils: filesystem.c */

    res = ros_e2e_rcutils_is_directory();
    if (result_is_err(res)) return res;

    res = ros_e2e_rcutils_is_file();
    if (result_is_err(res)) return res;

    res = ros_e2e_rcutils_exists();
    if (result_is_err(res)) return res;

    /* rmw: init.c */

    res = ros_e2e_rmw_get_zero_initialized_context();
    if (result_is_err(res)) return res;

    log_info("Finished ros_e2e_run_all_tests().");
    return result_ok();
}

/* ================================
 * rcutils
 * ================================ */

/* allocator.c */

result_t ros_e2e_rcutils_allocate(size_t heap_size) {
    log_info("Starting ros_e2e_rcutils_allocate().");

    /* Simple malloc/free test.*/

    size_t size_of_element = 10;
    void *memory = rcutils_get_default_allocator().allocate(size_of_element, NULL);
    assert(NULL != memory);
    /* Deallocate memory. */
    rcutils_get_default_allocator().deallocate(memory, NULL);

    log_info("Stress testing malloc to ensure memory is not leaked.");
    size_t const iterations = 100;
    size_t const alloc_size = heap_size - (4 * sizeof(size_t));
    for (size_t i = 0; i < iterations; ++i) {
        memory = rcutils_get_default_allocator().allocate(alloc_size, NULL);
        assert(NULL != memory);
        rcutils_get_default_allocator().deallocate(memory, NULL);
    }

    log_info("Finished ros_e2e_rcutils_allocate().");
    return result_ok();
}

result_t ros_e2e_rcutils_zero_allocate(size_t heap_size) {
    log_info("Starting ros_e2e_rcutils_zero_allocate().");

    /* Simple test. */
    size_t const size_of_element = 1;
    size_t const num_elements = 10;
    char *memory = (char *) rcutils_get_default_allocator().zero_allocate(
            num_elements,
            size_of_element,
            NULL
    );
    log_info("Asserting memory is not NULL for simple test.");
    assert(NULL != memory);
    log_info("Asserting memory is zeroed for simple test.");
    char const expected[10] = {0};
    assert(0 == memcmp(memory, expected, size_of_element * num_elements));
    rcutils_get_default_allocator().deallocate(memory, NULL);

    /* Stress testing calloc and free by malloc-ing the size of almost the entire heap. */

    log_info("Stress testing zero allocate to ensure memory is not leaked.");
    size_t const iterations = 100;
    size_t const alloc_size = heap_size - (4 * sizeof(size_t));
    for (size_t i = 0; i < iterations; ++i) {
        memory = rcutils_get_default_allocator().zero_allocate(alloc_size, 1, NULL);
        assert(NULL != memory);
        for (size_t j = 0; j < alloc_size; ++j) {
            assert(0 == memory[j]);
        }
        rcutils_get_default_allocator().deallocate(memory, NULL);
    }

    log_info("Finished ros_e2e_rcutils_zero_allocate().");
    return result_ok();
}

result_t ros_e2e_rcutils_reallocate(size_t heap_size) {
    log_info("Starting ros_e2e_rcutils_reallocate().");

    /* Allocate memory. */
    size_t size_of_element = 10;
    void *allocated_memory = rcutils_get_default_allocator().allocate(size_of_element, NULL);
    assert(NULL != allocated_memory);

    /* Reallocate memory to same size. */
    void *memory = rcutils_get_default_allocator().reallocate(
            allocated_memory,
            size_of_element,
            NULL
    );
    assert(NULL != memory);

    /* Reallocate memory larger. */
    memory = rcutils_get_default_allocator().reallocate(
            memory,
            size_of_element * 2,
            NULL
    );
    assert(NULL != memory);

    /* Reallocate memory smaller. */
    memory = rcutils_get_default_allocator().reallocate(
            memory,
            size_of_element / 2,
            NULL
    );
    assert(NULL != memory);

    /* Reallocate memory zero. */
    memory = rcutils_get_default_allocator().reallocate(
            memory,
            0,
            NULL
    );
    assert(NULL != memory);

    log_info("Stress testing reallocate to ensure memory is not leaked.");
    size_t const iterations = 50;
    size_t const alloc_size = (heap_size - (4 * sizeof(size_t))) / 3;
    for (size_t i = 0; i < iterations; ++i) {
        memory = rcutils_get_default_allocator().reallocate(
                memory,
                alloc_size + (i % 2),
                NULL
        );
        assert(NULL != memory);
    }

    rcutils_get_default_allocator().deallocate(memory, NULL);

    log_info("Finished ros_e2e_rcutils_reallocate().");
    return result_ok();
}

/* filesystem.c */

result_t ros_e2e_rcutils_is_directory(void) {
    log_info("Starting ros_e2e_rcutils_is_directory().");

    FRESULT res;
    /* Mount FS. */
    FATFS fs;
    res = f_mount(&fs, "", 0);
    if (res != FR_OK) {
        log_info("Error mounting FS with res of %d.", res);
    }
    assert(FR_OK == res);

    /* Delete the directory first. */
    char const *dir_path = "ros_e2e_rcutils_is_directory_dir";
    /* Intentionally not checking return code because this might fail if file
     * doesn't exist. */
    f_unlink(dir_path);

    /* Create a new directory. */
    bool res_mkdir = rcutils_mkdir(dir_path);
    assert(true == res_mkdir);

    /* Checking `rcutils` returns true for directory we just created. */
    bool is_directory = rcutils_is_directory(dir_path);
    assert(true == is_directory);

    /* Delete file. */
    char const *file_path = "ros_e2e_rcutils_is_directory.txt";
    /* Intentionally not checking return code because this might fail if file
     * doesn't exist. */
    f_unlink(file_path);

    /* Creating file. */
    FIL fp = {0};
    res = f_open(&fp, file_path, FA_WRITE | FA_CREATE_ALWAYS);
    if (res != FR_OK) {
        log_info("Error opening file with res of %d.", res);
    }
    assert(FR_OK == res);

    /* Close file. */
    res = f_close(&fp);
    if (res != FR_OK) {
        log_info("Error closing file with res of %d.", res);
    }
    assert(FR_OK == res);

    /* File should return `false` for `rcutils_is_directory()` */
    is_directory = rcutils_is_directory(file_path);
    assert(false == is_directory);

    log_info("Finished ros_e2e_rcutils_is_directory().");
    return result_ok();
}

result_t ros_e2e_rcutils_is_file(void) {
    log_info("Starting ros_e2e_rcutils_is_file().");

    /* Checking that is_file returns false if file does not exist. */
    bool is_file = rcutils_is_file("i_dont_exist.txt");
    assert(false == is_file);

    FRESULT res;

    /* Mount FS. */
    FATFS fs;
    res = f_mount(&fs, "", 0);
    if (res != FR_OK) {
        log_info("Error mounting FS with res of %d.", res);
    }
    assert(FR_OK == res);

    /* Delete file. */
    char const *file_path = "ros_e2e_rcutils_is_file.txt";
    /* Intentionally not checking return code because this might fail if file
     * doesn't exist. */
    f_unlink(file_path);

    /* Creating file. */
    FIL fp = {0};
    res = f_open(&fp, file_path, FA_WRITE | FA_CREATE_ALWAYS);
    if (res != FR_OK) {
        log_info("Error opening file with res of %d.", res);
    }
    assert(FR_OK == res);

    /* Close file. */
    res = f_close(&fp);
    if (res != FR_OK) {
        log_info("Error closing file with res of %d.", res);
    }
    assert(FR_OK == res);

    /* Checking `rcutils` returns true for file that does exist. */
    log_info("Checking `rcutils` returns true for file that does exist.");
    is_file = rcutils_is_file(file_path);
    assert(true == is_file);

    /* Delete the directory first. */
    char const *dir_path = "ros_e2e_rcutils_is_file_dir";
    /* Intentionally not checking return code because this might fail if file
     * doesn't exist. */
    f_unlink(dir_path);

    /* Create a new directory. */
    bool res_mkdir = rcutils_mkdir(dir_path);
    assert(true == res_mkdir);

    /* Checking `rcutils` returns false for directory we just created. */
    is_file = rcutils_is_file(dir_path);
    assert(false == is_file);

    log_info("Finished ros_e2e_rcutils_is_file().");
    return result_ok();
}

result_t ros_e2e_rcutils_exists(void) {
    log_info("Starting ros_e2e_rcutils_exists().");

    /* Checking that rcutils_exists returns false if file does not exist. */
    bool exists = rcutils_exists("i_dont_exist.txt");
    assert(false == exists);

    /* Creating a new file. */
    FRESULT res;
    FATFS fs;
    res = f_mount(&fs, "", 0);
    if (res != FR_OK) {
        log_info("Error mounting FS with res of %d.", res);
    }
    assert(FR_OK == res);

    FIL fp = {0};
    char const *path = "ros_e2e_rcutils_exists.txt";
    res = f_open(&fp, path, FA_WRITE | FA_CREATE_ALWAYS);
    if (res != FR_OK) {
        log_info("Error opening file with res of %d.", res);
    }
    assert(FR_OK == res);

    res = f_close(&fp);
    if (res != FR_OK) {
        log_info("Error closing file with res of %d.", res);
    }
    assert(FR_OK == res);

    /* Checking `rcutils` returns true for file that does exist. */
    log_info("Checking `rcutils` returns true for file that does exist.");
    exists = rcutils_exists(path);
    assert(true == exists);

    log_info("Finished ros_e2e_rcutils_exists().");
    return result_ok();
}

/* ================================
 * rmw
 * ================================ */

/* init.c */

result_t ros_e2e_rmw_get_zero_initialized_context(void) {
    log_info("Starting ros_e2e_rmw_get_zero_initialized_context().");

    rmw_context_t context = rmw_get_zero_initialized_context();
    assert(0 == context.instance_id);
    assert(NULL == context.implementation_identifier);
    assert(0 == context.actual_domain_id);
    assert(NULL == context.impl);

    log_info("Finished ros_e2e_rmw_get_zero_initialized_context().");
    return result_ok();
}

