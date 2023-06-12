#include "ros_e2e.h"

result_t ros_e2e_run_all_tests(void) {
    log_info("Starting ros_e2e_run_all_tests().");

    result_t res;

    /* rcutils */

    res = ros_e2e_rcutils_is_directory();
    if (result_is_err(res)) return res;

    res = ros_e2e_rcutils_is_file();
    if (result_is_err(res)) return res;

    res = ros_e2e_rcutils_exists();
    if (result_is_err(res)) return res;

    /* rmw */

    res = ros_e2e_rmw_get_zero_initialized_context();
    if (result_is_err(res)) return res;

    log_info("Finished ros_e2e_run_all_tests().");
    return result_ok();
}

/* ================================
 * rcutils
 * ================================ */

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

