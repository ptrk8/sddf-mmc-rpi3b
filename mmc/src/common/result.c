#include "result.h"

result_data_t global_result_data = {0};

result_t result_ok() {
//    result_t res = {};
//    memset(&res, 0, sizeof(res));
//    return res;
    return true;
}

result_t result_err(const char *err_msg) {
//    result_t result = {
//            .is_err = true,
//            .err_msgs = {0},
//            .num_err_msgs = 1,
//            .total_num_err = 1,
//    };
//    memset(result.err_msgs, 0, sizeof(result.err_msgs));
//    result.err_msgs[0] = err_msg;
//    return result;
    global_result_data.is_err = true;
    global_result_data.err_msgs[0] = err_msg;
    global_result_data.num_err_msgs = 1;
    global_result_data.total_num_err = 1;
    return false;
}

result_t result_err_chain(result_t result, const char *err_msg) {
    if (err_msg == NULL) {
        return result;
    }
    /* If the result is not an error, return an error. */
    if (result_is_ok(result)) {
        return result_err("User tried chaining error message to Ok result!");
    }
    /* Increment `total_num_err`. */
    global_result_data.total_num_err++;
    /* Do nothing if we're already at our limit. */
    if (global_result_data.num_err_msgs == MAX_NUM_ERR_MSGS) {
        return result;
    }
    /* Set the next value in `err_msgs` to our new error message string. */
    global_result_data.err_msgs[global_result_data.num_err_msgs] = err_msg;
    /* Increment the `num_err_msgs`. */
    global_result_data.num_err_msgs++;
    return result;
}

bool result_is_ok(result_t result) {
    return global_result_data.is_err != true;
}

bool result_is_err(result_t result) {
    return !result_is_ok(result);
}

const char *result_get_err_msg_at(result_t result, size_t index) {
    (void) result;
    if (index >= global_result_data.num_err_msgs) {
        return "";
    }
    return global_result_data.err_msgs[index];
}

/* TODO: Figure out why I can't run this function on CP. */
void result_get_err_msg(result_t result, char *ret_buf, size_t buf_len) {
    if (ret_buf == NULL || buf_len == 0) {
        return;
    }
    /* Zero out the buffer. */
    memset(ret_buf, 0, buf_len);
    if (global_result_data.num_err_msgs == 0) {
        return;
    }
    /* Iterate through the buffer. */
    size_t offset = 0;
    for (size_t i = 0; i < global_result_data.num_err_msgs; i++) {
        const char *err_msg = global_result_data.err_msgs[i];
        /* The following length does not include the NULL terminator. */
        size_t err_msg_len = strlen(err_msg);
        /* Only if we have space in `ret_buf` will we copy the next string into the buffer. */
        if (offset + err_msg_len < buf_len) {
            memcpy(
                    ret_buf + offset,
                    err_msg,
                    err_msg_len
            );
            offset += err_msg_len;
            /* If there's space, we'll add a newline character. */
            if (offset + 1 < buf_len) {
                ret_buf[offset] = '\n';
                offset++;
            } else {
                break; /* Minor performance optimisation to break early if we're out of space. */
            }
        } else {
            break; /* Minor performance optimisation to break early if we're out of space. */
        }
    }
}

result_t result_ok_or(result_t result, const char *err_msg) {
    if (result_is_err(result)) {
        return result_err_chain(result, err_msg);
    }
    return result;
}

size_t result_get_num_err_msgs(result_t result) {
    (void) result;
    return global_result_data.num_err_msgs;
}

size_t result_get_total_num_err(result_t result) {
    (void) result;
    return global_result_data.total_num_err;
}


