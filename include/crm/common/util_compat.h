/*
 * Copyright 2004-2021 the Pacemaker project contributors
 *
 * The version control history for this file may have further details.
 *
 * This source code is licensed under the GNU Lesser General Public License
 * version 2.1 or later (LGPLv2.1+) WITHOUT ANY WARRANTY.
 */

#ifndef PCMK__COMMON_UTIL_COMPAT__H
#  define PCMK__COMMON_UTIL_COMPAT__H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \file
 * \brief Deprecated Pacemaker utilities
 * \ingroup core
 * \deprecated Do not include this header directly. The utilities in this
 *             header, and the header itself, will be removed in a future
 *             release.
 */

//! \deprecated Use crm_parse_interval_spec() instead
#define crm_get_interval crm_parse_interval_spec

//! \deprecated Use !pcmk_is_set() or !pcmk_all_flags_set() instead
static inline gboolean
is_not_set(long long word, long long bit)
{
    return ((word & bit) == 0);
}

//! \deprecated Use pcmk_is_set() or pcmk_all_flags_set() instead
static inline gboolean
is_set(long long word, long long bit)
{
    return ((word & bit) == bit);
}

//! \deprecated Use pcmk_any_flags_set() instead
static inline gboolean
is_set_any(long long word, long long bit)
{
    return ((word & bit) != 0);
}

//! \deprecated Use strcmp() or strcasecmp() instead
gboolean crm_str_eq(const char *a, const char *b, gboolean use_case);

//! \deprecated Use strcmp() instead
gboolean safe_str_neq(const char *a, const char *b);

//! \deprecated Use strcasecmp() instead
#define safe_str_eq(a, b) crm_str_eq(a, b, FALSE)

//! \deprecated Use snprintf() instead
char *crm_itoa_stack(int an_int, char *buf, size_t len);

//! \deprecated Use sscanf() instead
int pcmk_scan_nvpair(const char *input, char **name, char **value);

//! \deprecated Use a standard printf()-style function instead
char *pcmk_format_nvpair(const char *name, const char *value,
                         const char *units);

//! \deprecated Use a standard printf()-style function instead
char *pcmk_format_named_time(const char *name, time_t epoch_time);

//! \deprecated Use g_str_hash() instead
guint g_str_hash_traditional(gconstpointer v);

//! \deprecated Use g_str_hash() instead
#define crm_str_hash g_str_hash_traditional

//! \deprecated Do not use Pacemaker for generic string comparison
gboolean crm_strcase_equal(gconstpointer a, gconstpointer b);

//! \deprecated Use g_hash_table_new_full() instead
static inline GHashTable *
crm_str_table_new(void)
{
    return g_hash_table_new_full(crm_str_hash, g_str_equal, free, free);
}

//! \deprecated Use g_hash_table_new_full() instead
static inline GHashTable *
crm_strcase_table_new(void)
{
    return g_hash_table_new_full(crm_strcase_hash, crm_strcase_equal,
                                 free, free);
}

#ifdef __cplusplus
}
#endif

#endif // PCMK__COMMON_UTIL_COMPAT__H
