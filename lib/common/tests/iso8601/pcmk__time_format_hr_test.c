/*
 * Copyright 2024-2025 the Pacemaker project contributors
 *
 * The version control history for this file may have further details.
 *
 * This source code is licensed under the GNU General Public License version 2
 * or later (GPLv2+) WITHOUT ANY WARRANTY.
 */

#include <crm_internal.h>

#include <stdio.h>  // NULL

#include <crm/common/iso8601_internal.h>
#include <crm/common/unittest_internal.h>

#define DATE_S "2024-06-02"

#define HOUR_S "03"
#define MINUTE_S "04"
#define SECOND_S "05"
#define TIME_S HOUR_S ":" MINUTE_S ":" SECOND_S

#define OFFSET_S "+00:00"

#define TEST_TIME pcmk__time_hr_new(DATE_S " " TIME_S " " OFFSET_S)

/*!
 * \internal
 * \brief Assert that pcmk__time_format_hr() produced expected result
 *
 * \param[in] format     Time format string
 * \param[in] expected   Assertion succeeds if result matches this
 * \param[in] alternate  If this is not NULL, assertion may also succeed if
 *                       result matches this
 * \param[in] usec       Microseconds component of the reference time
 *
 * \note This allows two possible results because different strftime()
 *       implementations handle certain format syntax differently.
 */
static void
assert_hr_format(const char *format, const char *expected,
                 const char *alternate, int usec)
{
    pcmk__time_hr_t *hr = TEST_TIME;
    char *result = NULL;

    hr->useconds = usec;
    result = pcmk__time_format_hr(format, hr);
    pcmk__time_hr_free(hr);

    if (expected == NULL) {
        assert_null(result);
        return;
    }

    assert_non_null(result);

    if (alternate == NULL) {
        assert_string_equal(result, expected);
    } else {
        assert_true((strcmp(result, expected) == 0)
                    || (strcmp(result, alternate) == 0));
    }

    free(result);
}

static void
null_format(void **state)
{
    assert_null(pcmk__time_format_hr(NULL, NULL));
    assert_hr_format(NULL, NULL, NULL, 0); // for pcmk__time_format_hr(NULL, hr)
}

static void
no_specifiers(void **state)
{
    assert_hr_format("no specifiers", "no specifiers", NULL, 0);

    /* @COMPAT The following four tests will fail when we remove the fallback
     * to strftime(). g_date_time_format() requires a literal '%' to be escaped
     * as "%%".
     */
    assert_hr_format("this has a literal % in it",
                     "this has a literal % in it",
                     // *BSD strftime() strips single %
                     "this has a literal  in it", 0);
    assert_hr_format("this has a literal %01 in it",
                     "this has a literal %01 in it",
                     // *BSD strftime() strips %0
                     "this has a literal 1 in it", 0);
    assert_hr_format("%2 this starts and ends with %",
                     "%2 this starts and ends with %",
                     // *BSD strftime() strips % in front of nonzero number
                     "2 this starts and ends with %", 0);

    /* strftime() treats % with a number (and no specifier) as a literal string
     * to be formatted with a field width (undocumented and probably a bug ...)
     */
    assert_hr_format("this ends with %10", "this ends with        %10",
                     // *BSD strftime() strips % in front of nonzero number
                     "this ends with 10", 0);
}

static void
without_frac(void **state)
{
    assert_hr_format("%Y-%m-%d %H:%M:%S", DATE_S " " TIME_S, NULL, 0);

    assert_hr_format("%H:%M %a %b %d", HOUR_S ":" MINUTE_S " Sun Jun 02", NULL,
                     0);
    assert_hr_format("%H:%M:%S", TIME_S, NULL, 0);
    assert_hr_format("The time is %H:%M right now",
                     "The time is " HOUR_S ":" MINUTE_S " right now", NULL, 0);

    /* @COMPAT This test will fail when we remove the fallback to strftime().
     * g_date_time_format() doesn't support field widths.
     */
    assert_hr_format("%3S seconds", "0" SECOND_S " seconds",
                     // *BSD strftime() doesn't support field widths
                     "3S seconds", 0);

    // strftime() treats %% as a literal %
    assert_hr_format("%%H %%N", "%H %N", NULL, 0);
}

static void
with_frac(void **state)
{
    int usec = 123456;

    // Display time with no fractional seconds component
    assert_hr_format("%Y-%m-%d %H:%M:%S.%6N", DATE_S " " TIME_S ".000000", NULL,
                     0);
    assert_hr_format("%H:%M:%S.%6N", TIME_S ".000000", NULL, 0);
    assert_hr_format("%H:%M:%S.%6N %H", TIME_S ".000000 " HOUR_S, NULL, 0);
    assert_hr_format("%H:%M:%S.%3N", TIME_S ".000", NULL, 0);
    assert_hr_format("%H:%M:%S.%0N", TIME_S ".", NULL, 0);
    assert_hr_format("%H:%M:%S.%N", TIME_S ".", NULL, 0);
    assert_hr_format("The time is %H:%M:%S.%06N right NOW",
                     "The time is " TIME_S ".000000 right NOW", NULL, 0);

    // Display at appropriate resolution by truncating toward zero
    assert_hr_format("%Y-%m-%d %H:%M:%S.%6N", DATE_S " " TIME_S ".123456", NULL,
                     usec);
    assert_hr_format("%H:%M:%S.%6N", TIME_S ".123456", NULL, usec);
    assert_hr_format("%H:%M:%S.%6N %M", TIME_S ".123456 " MINUTE_S, NULL, usec);
    assert_hr_format("%H:%M:%S.%5N", TIME_S ".12345", NULL, usec);
    assert_hr_format("%H:%M:%S.%4N", TIME_S ".1234", NULL, usec);
    assert_hr_format("%H:%M:%S.%3N", TIME_S ".123", NULL, usec);
    assert_hr_format("%H:%M:%S.%2N", TIME_S ".12", NULL, usec);
    assert_hr_format("%H:%M:%S.%1N", TIME_S ".1", NULL, usec);
    assert_hr_format("%H:%M:%S.%0N", TIME_S ".", NULL, usec);
    assert_hr_format("%H:%M:%S.%N", TIME_S ".", NULL, usec);

    // Leading zero is ignored, not treated as a request for zero-padding
    assert_hr_format("%H:%M:%S.%03N", TIME_S ".123", NULL, usec);

    // Same as above, but with zero-padding for smaller fractional components
    usec = 789;
    assert_hr_format("%Y-%m-%d %H:%M:%S.%6N", DATE_S " " TIME_S ".000789", NULL,
                     usec);
    assert_hr_format("%H:%M:%S.%6N", TIME_S ".000789", NULL, usec);
    assert_hr_format("%H:%M:%S.%6N %S", TIME_S ".000789 " SECOND_S, NULL, usec);
    assert_hr_format("%H:%M:%S.%5N", TIME_S ".00078", NULL, usec);
    assert_hr_format("%H:%M:%S.%4N", TIME_S ".0007", NULL, usec);
    assert_hr_format("%H:%M:%S.%3N", TIME_S ".000", NULL, usec);
    assert_hr_format("%H:%M:%S.%2N", TIME_S ".00", NULL, usec);
    assert_hr_format("%H:%M:%S.%1N", TIME_S ".0", NULL, usec);
    assert_hr_format("%H:%M:%S.%0N", TIME_S ".", NULL, usec);
    assert_hr_format("%H:%M:%S.%N", TIME_S ".", NULL, usec);
}

PCMK__UNIT_TEST(NULL, NULL,
                cmocka_unit_test(null_format),
                cmocka_unit_test(no_specifiers),
                cmocka_unit_test(without_frac),
                cmocka_unit_test(with_frac))
