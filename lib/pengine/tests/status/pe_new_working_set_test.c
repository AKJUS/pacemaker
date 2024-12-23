/*
 * Copyright 2022-2023 the Pacemaker project contributors
 *
 * The version control history for this file may have further details.
 *
 * This source code is licensed under the GNU General Public License version 2
 * or later (GPLv2+) WITHOUT ANY WARRANTY.
 */

#include <crm_internal.h>

#include <crm/common/unittest_internal.h>
#include <crm/pengine/internal.h>

#include "mock_private.h"

static void
calloc_fails(void **state) {
    pcmk__mock_calloc = true;   // calloc() will return NULL

    expect_value(__wrap_calloc, nmemb, 1);
    expect_value(__wrap_calloc, size, sizeof(pcmk_scheduler_t));
    assert_null(pe_new_working_set());

    pcmk__mock_calloc = false;  // Use real calloc()
}

static void
calloc_succeeds(void **state)
{
    pcmk_scheduler_t *scheduler = pe_new_working_set();

    /* We only need to test that the return value is non-NULL, as all the
     * function does is call pcmk__set_scheduler_defaults(), which should have
     * its own unit test.
     */
    assert_non_null(scheduler);

    /* Avoid calling pe_free_working_set here so we don't artificially
     * inflate the coverage numbers.
     */
    free(scheduler);
}

PCMK__UNIT_TEST(NULL, NULL,
                cmocka_unit_test(calloc_fails),
                cmocka_unit_test(calloc_succeeds))
