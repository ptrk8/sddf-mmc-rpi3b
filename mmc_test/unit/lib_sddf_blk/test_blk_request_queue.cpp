#include <gtest/gtest.h>

extern "C" {
#include "blk_request_queue.h"
}

/* init() */

TEST(test_blk_request_queue, init_should_reject_null_queue) {
    ASSERT_EQ(
            ERR_NULL_BLK_REQUEST_QUEUE,
            blk_request_queue_init(
                    NULL,
                    0
            )
    );
}

TEST(test_blk_request_queue, init_should_reject_shared_memory_regions_that_are_too_small) {
    blk_request_queue_t queue = {0};
    ASSERT_EQ(
            ERR_BLK_REQUEST_QUEUE_REGION_TOO_SMALL,
            blk_request_queue_init(
                    &queue,
                    sizeof(blk_request_queue_t) - 1
            )
    );
}

TEST(test_blk_request_queue, init_should_clear_the_queue) {
    blk_request_queue_t queue = {1};
    ASSERT_EQ(
            OK_BLK_REQUEST_QUEUE,
            blk_request_queue_init(
                    &queue,
                    sizeof(blk_request_queue_t)
            )
    );
    /* Entire ring buffer should be zeroed. */
    blk_request_queue_t expected = {0};
    ASSERT_EQ(0, memcmp(&expected, &queue, sizeof(queue)));
}

TEST(test_blk_request_queue, init_should_accept_shared_memory_regions_that_are_slightly_oversized) {
    blk_request_queue_t queue = {1};
    ASSERT_EQ(
            OK_BLK_REQUEST_QUEUE,
            blk_request_queue_init(
                    &queue,
                    sizeof(blk_request_queue_t) + sizeof(blk_request_t) - 1
            )
    );
    /* Entire ring buffer should be zeroed. */
    blk_request_queue_t expected = {0};
    ASSERT_EQ(0, memcmp(&expected, &queue, sizeof(queue)));
}

TEST(test_blk_request_queue, init_should_reject_shared_memory_regions_that_are_too_oversized) {
    blk_request_queue_t queue = {0};
    ASSERT_EQ(
            ERR_BLK_REQUEST_QUEUE_REGION_TOO_LARGE,
            blk_request_queue_init(
                    &queue,
                    sizeof(blk_request_queue_t) + sizeof(blk_request_t)
            )
    );
}

TEST(test_blk_request_queue, init_should_accept_shared_memory_regions_set_to_sel4s_maximum_page_size) {
    blk_request_queue_t queue = {1};
    ASSERT_EQ(
            OK_BLK_REQUEST_QUEUE,
            blk_request_queue_init(
                    &queue,
                    0x200000
            )
    );
    /* Entire ring buffer should be zeroed. */
    blk_request_queue_t expected = {0};
    ASSERT_EQ(0, memcmp(&expected, &queue, sizeof(queue)));
}

/* blk_request_queue_capacity */

TEST(test_blk_request_queue, blk_request_queue_capacity_should_reject_null_queue) {
    size_t capacity = 0;
    ASSERT_EQ(
            ERR_NULL_BLK_REQUEST_QUEUE,
            blk_request_queue_capacity(
                    NULL,
                    &capacity
            )
    );
}

TEST(test_blk_request_queue, blk_request_queue_capacity_should_reject_null_ret_val) {
    blk_request_queue_t queue = {0};
    ASSERT_EQ(
            ERR_NULL_RET_VAL_PTR_PASSED_TO_BLK_REQUEST_QUEUE_FN,
            blk_request_queue_capacity(
                    &queue,
                    NULL
            )
    );
}

TEST(test_blk_request_queue, blk_request_queue_capacity_should_return_capacity_of_queue) {
    blk_request_queue_t queue = {0};
    size_t capacity = 0;
    ASSERT_EQ(
            OK_BLK_REQUEST_QUEUE,
            blk_request_queue_capacity(
                    &queue,
                    &capacity
            )
    );
    ASSERT_EQ(MAX_NUM_BLK_REQUESTS - 1, capacity);
}

/* enqueue() */

TEST(test_blk_request_queue, enqueue_should_only_allow_user_to_enqueue_capacity_number_of_bufs) {
    blk_request_queue_t queue = {0};
    ASSERT_EQ(
            OK_BLK_REQUEST_QUEUE,
            blk_request_queue_init(
                    &queue,
                    sizeof(blk_request_queue_t)
            )
    );
    /* Get the capacity of the ring buffer. */
    size_t capacity = 0;
    ASSERT_EQ(
            OK_BLK_REQUEST_QUEUE,
            blk_request_queue_capacity(
                    &queue,
                    &capacity
            )
    );
    /* Enqueue ring buffers. */
    blk_request_t req = {};
    for (size_t i = 0; i < capacity; i++) {
        ASSERT_EQ(
                OK_BLK_REQUEST_QUEUE,
                blk_request_queue_enqueue(
                        &queue,
                        &req
                )
        );
        /* Checking the state of the ring buffer is correct. */
        size_t size = 0;
        ASSERT_EQ(
                OK_BLK_REQUEST_QUEUE,
                blk_request_queue_size(
                        &queue,
                        &size
                )
        );
        ASSERT_EQ(i + 1, size);
    }
    /* Checking the state of the ring buffer after we've filled it is correct. */
    bool is_full = false;
    ASSERT_EQ(
            OK_BLK_REQUEST_QUEUE,
            blk_request_queue_is_full(
                    &queue,
                    &is_full
            )
    );
    ASSERT_EQ(true, is_full);
    /* The ring buffer should prevent the user from enqueuing more buffers. */
    ASSERT_EQ(
            ERR_BLK_REQUEST_QUEUE_FULL,
            blk_request_queue_enqueue(
                    &queue,
                    &req
            )
    );
}

/* dequeue() */

TEST(test_blk_request_queue, dequeue_should_prevent_user_from_dequeueing_from_empty_queue) {
    blk_request_queue_t queue = {0};
    ASSERT_EQ(
            OK_BLK_REQUEST_QUEUE,
            blk_request_queue_init(
                    &queue,
                    sizeof(blk_request_queue_t)
            )
    );
    blk_request_t req = {};
    ASSERT_EQ(
            ERR_BLK_REQUEST_QUEUE_EMPTY,
            blk_request_queue_dequeue(
                    &queue,
                    &req
            )
    );
}

TEST(test_blk_request_queue, dequeue_should_be_able_to_dequeue_capacity_number_of_times_from_a_full_queuefer) {
    blk_request_queue_t queue = {0};
    ASSERT_EQ(
            OK_BLK_REQUEST_QUEUE,
            blk_request_queue_init(
                    &queue,
                    sizeof(blk_request_queue_t)
            )
    );
    /* Get the capacity of the ring buffer. */
    size_t capacity = 0;
    ASSERT_EQ(
            OK_BLK_REQUEST_QUEUE,
            blk_request_queue_capacity(
                    &queue,
                    &capacity
            )
    );
    /* Enqueuing ring buffers until the buffer is full. */
    blk_request_t req = {};
    for (size_t i = 0; i < capacity; i++) {
        ASSERT_EQ(
                OK_BLK_REQUEST_QUEUE,
                blk_request_queue_enqueue(
                        &queue,
                        &req
                )
        );
    }
    /* Now we dequeue the `capacity` number of times. */
    for (size_t i = 0; i < capacity; i++) {
        ASSERT_EQ(
                OK_BLK_REQUEST_QUEUE,
                blk_request_queue_dequeue(
                        &queue,
                        &req
                )
        );
        /* Checking the state of the ring buffer is correct. */
        size_t size = 0;
        ASSERT_EQ(
                OK_BLK_REQUEST_QUEUE,
                blk_request_queue_size(
                        &queue,
                        &size
                )
        );
        ASSERT_EQ(capacity - i - 1, size);
    }
    /* Checking the state of the ring buffer after we've filled it is correct. */
    bool is_empty = false;
    ASSERT_EQ(
            OK_BLK_REQUEST_QUEUE,
            blk_request_queue_is_empty(
                    &queue,
                    &is_empty
            )
    );
    ASSERT_EQ(true, is_empty);
}


