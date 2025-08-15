/*
 * Copyright 2024-2025 the Pacemaker project contributors
 *
 * The version control history for this file may have further details.
 *
 * This source code is licensed under the GNU General Public License version 2
 * or later (GPLv2+) WITHOUT ANY WARRANTY.
 */

#ifndef PCMK__CRM_COMMON_LOGGING_COMPAT__H
#define PCMK__CRM_COMMON_LOGGING_COMPAT__H

#include <qb/qblog.h>   // qb_log_ctl(), QB_LOG_CONF_EXTENDED

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * \file
 * \brief Deprecated Pacemaker logging API
 * \ingroup core
 * \deprecated Do not include this header directly. The logging APIs in this
 *             header, and the header itself, will be removed in a future
 *             release.
 */

//! \deprecated use QB_XS instead
#define CRM_XS QB_XS

//! \deprecated Use qb_log_ctl() directly instead
#define crm_extended_logging(t, e) qb_log_ctl((t), QB_LOG_CONF_EXTENDED, (e))

// NOTE: sbd (as of at least 1.5.2) uses this
//! \deprecated Do not use Pacemaker for general-purpose logging
#define crm_perror(level, fmt, args...) do {                                \
        uint8_t _level = pcmk__clip_log_level(level);                       \
                                                                            \
        switch (_level) {                                                   \
            case LOG_NEVER:                                                 \
                break;                                                      \
            default: {                                                      \
                const char *err = strerror(errno);                          \
                if (_level <= crm_log_level) {                              \
                    fprintf(stderr, fmt ": %s (%d)\n" , ##args, err,        \
                            errno);                                         \
                }                                                           \
                /* Pass original level arg since do_crm_log() also declares \
                 * _level                                                   \
                 */                                                         \
                do_crm_log((level), fmt ": %s (%d)" , ##args, err, errno);  \
            }                                                               \
            break;                                                          \
        }                                                                   \
    } while (0)

#ifdef __cplusplus
}
#endif

#endif // PCMK__CRM_COMMON_LOGGING_COMPAT__H
