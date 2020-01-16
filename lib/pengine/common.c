/*
 * Copyright 2004-2020 the Pacemaker project contributors
 *
 * The version control history for this file may have further details.
 *
 * This source code is licensed under the GNU Lesser General Public License
 * version 2.1 or later (LGPLv2.1+) WITHOUT ANY WARRANTY.
 */

#include <crm_internal.h>
#include <crm/crm.h>
#include <crm/msg_xml.h>
#include <crm/common/xml.h>
#include <crm/common/util.h>

#include <glib.h>

#include <crm/pengine/internal.h>

gboolean was_processing_error = FALSE;
gboolean was_processing_warning = FALSE;

static gboolean
check_health(const char *value)
{
    if (safe_str_eq(value, "none")) {
        return TRUE;

    } else if (safe_str_eq(value, "custom")) {
        return TRUE;

    } else if (safe_str_eq(value, "only-green")) {
        return TRUE;

    } else if (safe_str_eq(value, "progressive")) {
        return TRUE;

    } else if (safe_str_eq(value, "migrate-on-red")) {
        return TRUE;
    }
    return FALSE;
}

static gboolean
check_stonith_action(const char *value)
{
    if (safe_str_eq(value, "reboot")) {
        return TRUE;

    } else if (safe_str_eq(value, "poweroff")) {
        return TRUE;

    } else if (safe_str_eq(value, "off")) {
        return TRUE;
    }
    return FALSE;
}

static gboolean
check_placement_strategy(const char *value)
{
    if (safe_str_eq(value, "default")) {
        return TRUE;

    } else if (safe_str_eq(value, "utilization")) {
        return TRUE;

    } else if (safe_str_eq(value, "minimal")) {
        return TRUE;

    } else if (safe_str_eq(value, "balanced")) {
        return TRUE;
    }
    return FALSE;
}

/* *INDENT-OFF* */
static pe_cluster_option pe_opts[] = {
	/* name, old-name, validate, default, description */
	{ "no-quorum-policy", NULL, "enum", "stop, freeze, ignore, suicide", "stop", &check_quorum,
	  "What to do when the cluster does not have quorum", NULL },
	{ "symmetric-cluster", NULL, "boolean", NULL, "true", &check_boolean,
	  "All resources can run anywhere by default", NULL },
	{ "maintenance-mode", NULL, "boolean", NULL, "false", &check_boolean,
	  "Should the cluster monitor resources and start/stop them as required", NULL },
	{ "start-failure-is-fatal", NULL, "boolean", NULL, "true", &check_boolean, "Always treat start failures as fatal",
	  "When set to TRUE, the cluster will immediately ban a resource from a node if it fails to start there. When FALSE, the cluster will instead check the resource's fail count against its migration-threshold." },
	{ "enable-startup-probes", NULL, "boolean", NULL, "true", &check_boolean,
	  "Should the cluster check for active resources during startup", NULL },
    {
        XML_CONFIG_ATTR_SHUTDOWN_LOCK,
        NULL, "boolean", NULL, "false", &check_boolean,
        "Whether to lock resources to a cleanly shut down node",
        "When true, resources active on a node when it is cleanly shut down "
            "are kept \"locked\" to that node (not allowed to run elsewhere) "
            "until they start again on that node after it rejoins (or for at "
            "most shutdown-lock-limit, if set). Stonith resources and "
            "Pacemaker Remote connections are never locked. Clone and bundle "
            "instances and the master role of promotable clones are currently "
            "never locked, though support could be added in a future release."
    },
    {
        XML_CONFIG_ATTR_SHUTDOWN_LOCK_LIMIT,
        NULL, "time", NULL, "0", &check_timer,
        "Do not lock resources to a cleanly shut down node longer than this",
        "If shutdown-lock is true and this is set to a nonzero time duration, "
            "shutdown locks will expire after this much time has passed since "
            "the shutdown was initiated, even if the node has not rejoined."
    },

	/* Stonith Options */
	{ "stonith-enabled", NULL, "boolean", NULL, "true", &check_boolean,
	  "Failed nodes are STONITH'd", NULL },
	{ "stonith-action", NULL, "enum", "reboot, off, poweroff", "reboot", &check_stonith_action,
	  "Action to send to STONITH device ('poweroff' is a deprecated alias for 'off')", NULL },
	{ "stonith-timeout", NULL, "time", NULL, "60s", &check_timer,
	  "How long to wait for the STONITH action (reboot,on,off) to complete", NULL },
	{ XML_ATTR_HAVE_WATCHDOG, NULL, "boolean", NULL, "false", &check_boolean,
	  "Enable watchdog integration", "Set automatically by the cluster if SBD is detected.  User configured values are ignored." },
	{ "concurrent-fencing", NULL, "boolean", NULL,
#ifdef DEFAULT_CONCURRENT_FENCING_TRUE
      "true",
#else
      "false",
#endif
      &check_boolean,
	  "Allow performing fencing operations in parallel", NULL },
	{ "startup-fencing", NULL, "boolean", NULL, "true", &check_boolean,
	  "STONITH unseen nodes", "Advanced Use Only!  Not using the default is very unsafe!" },

	/* Timeouts etc */
	{ "cluster-delay", NULL, "time", NULL, "60s", &check_time,
	  "Round trip delay over the network (excluding action execution)",
	  "The \"correct\" value will depend on the speed and load of your network and cluster nodes." },
	{ "batch-limit", NULL, "integer", NULL, "0", &check_number,
	  "The number of jobs that the TE is allowed to execute in parallel",
	  "The \"correct\" value will depend on the speed and load of your network and cluster nodes." },
	{ "migration-limit", NULL, "integer", NULL, "-1", &check_number,
	  "The number of migration jobs that the TE is allowed to execute in parallel on a node"},

	/* Orphans and stopping */
	{ "stop-all-resources", NULL, "boolean", NULL, "false", &check_boolean,
	  "Should the cluster stop all active resources", NULL },
	{ "stop-orphan-resources", NULL, "boolean", NULL, "true", &check_boolean,
	  "Should deleted resources be stopped", NULL },
	{ "stop-orphan-actions", NULL, "boolean", NULL, "true", &check_boolean,
	  "Should deleted actions be cancelled", NULL },
 	{ "remove-after-stop", NULL, "boolean", NULL, "false", &check_boolean,
	  "Remove resources from the executor after they are stopped",
	  "Always set this to false.  Other values are, at best, poorly tested and potentially dangerous." },
/* 	{ "", "", , "0", "", NULL }, */

	/* Storing inputs */
	{
        "pe-error-series-max", NULL, "integer", NULL, "-1", &check_number,
	    "The number of scheduler inputs resulting in ERRORs to save",
        "Zero to disable, -1 to store unlimited"
    },
	{
        "pe-warn-series-max",  NULL, "integer", NULL, "5000", &check_number,
	    "The number of scheduler inputs resulting in WARNINGs to save",
        "Zero to disable, -1 to store unlimited"
    },
	{
        "pe-input-series-max", NULL, "integer", NULL, "4000", &check_number,
	    "The number of other scheduler inputs to save",
        "Zero to disable, -1 to store unlimited"
    },

	/* Node health */
	{ "node-health-strategy", NULL, "enum", "none, migrate-on-red, only-green, progressive, custom", "none", &check_health,
	  "The strategy combining node attributes to determine overall node health.",
	  "Requires external entities to create node attributes (named with the prefix '#health') with values: 'red', 'yellow' or 'green'."},
	{ "node-health-base", NULL, "integer", NULL, "0", &check_number,
	  "The base score assigned to a node",
	  "Only used when node-health-strategy is set to progressive." },
	{ "node-health-green", NULL, "integer", NULL, "0", &check_number,
	  "The score 'green' translates to in rsc_location constraints",
	  "Only used when node-health-strategy is set to custom or progressive." },
	{ "node-health-yellow", NULL, "integer", NULL, "0", &check_number,
	  "The score 'yellow' translates to in rsc_location constraints",
	  "Only used when node-health-strategy is set to custom or progressive." },
	{ "node-health-red", NULL, "integer", NULL, "-INFINITY", &check_number,
	  "The score 'red' translates to in rsc_location constraints",
	  "Only used when node-health-strategy is set to custom or progressive." },

	/*Placement Strategy*/
	{ "placement-strategy", NULL, "enum", "default, utilization, minimal, balanced", "default", &check_placement_strategy,
	  "The strategy to determine resource placement", NULL},
};
/* *INDENT-ON* */

void
pe_metadata(void)
{
    config_metadata("pacemaker-schedulerd", "1.0", "scheduler properties",
                    "Cluster properties used by Pacemaker's scheduler,"
                    " formerly known as pengine",
                    pe_opts, DIMOF(pe_opts));
}

void
verify_pe_options(GHashTable * options)
{
    verify_all_options(options, pe_opts, DIMOF(pe_opts));
}

const char *
pe_pref(GHashTable * options, const char *name)
{
    return get_cluster_pref(options, pe_opts, DIMOF(pe_opts), name);
}

const char *
fail2text(enum action_fail_response fail)
{
    const char *result = "<unknown>";

    switch (fail) {
        case action_fail_ignore:
            result = "ignore";
            break;
        case action_fail_block:
            result = "block";
            break;
        case action_fail_recover:
            result = "recover";
            break;
        case action_fail_migrate:
            result = "migrate";
            break;
        case action_fail_stop:
            result = "stop";
            break;
        case action_fail_fence:
            result = "fence";
            break;
        case action_fail_standby:
            result = "standby";
            break;
        case action_fail_restart_container:
            result = "restart-container";
            break;
        case action_fail_reset_remote:
            result = "reset-remote";
            break;
    }
    return result;
}

enum action_tasks
text2task(const char *task)
{
    if (safe_str_eq(task, CRMD_ACTION_STOP)) {
        return stop_rsc;
    } else if (safe_str_eq(task, CRMD_ACTION_STOPPED)) {
        return stopped_rsc;
    } else if (safe_str_eq(task, CRMD_ACTION_START)) {
        return start_rsc;
    } else if (safe_str_eq(task, CRMD_ACTION_STARTED)) {
        return started_rsc;
    } else if (safe_str_eq(task, CRM_OP_SHUTDOWN)) {
        return shutdown_crm;
    } else if (safe_str_eq(task, CRM_OP_FENCE)) {
        return stonith_node;
    } else if (safe_str_eq(task, CRMD_ACTION_STATUS)) {
        return monitor_rsc;
    } else if (safe_str_eq(task, CRMD_ACTION_NOTIFY)) {
        return action_notify;
    } else if (safe_str_eq(task, CRMD_ACTION_NOTIFIED)) {
        return action_notified;
    } else if (safe_str_eq(task, CRMD_ACTION_PROMOTE)) {
        return action_promote;
    } else if (safe_str_eq(task, CRMD_ACTION_DEMOTE)) {
        return action_demote;
    } else if (safe_str_eq(task, CRMD_ACTION_PROMOTED)) {
        return action_promoted;
    } else if (safe_str_eq(task, CRMD_ACTION_DEMOTED)) {
        return action_demoted;
    }
#if SUPPORT_TRACING
    if (safe_str_eq(task, CRMD_ACTION_CANCEL)) {
        return no_action;
    } else if (safe_str_eq(task, CRMD_ACTION_DELETE)) {
        return no_action;
    } else if (safe_str_eq(task, CRMD_ACTION_STATUS)) {
        return no_action;
    } else if (safe_str_eq(task, CRM_OP_PROBED)) {
        return no_action;
    } else if (safe_str_eq(task, CRM_OP_LRM_REFRESH)) {
        return no_action;
    } else if (safe_str_eq(task, CRMD_ACTION_MIGRATE)) {
        return no_action;
    } else if (safe_str_eq(task, CRMD_ACTION_MIGRATED)) {
        return no_action;
    }
    crm_trace("Unsupported action: %s", task);
#endif

    return no_action;
}

const char *
task2text(enum action_tasks task)
{
    const char *result = "<unknown>";

    switch (task) {
        case no_action:
            result = "no_action";
            break;
        case stop_rsc:
            result = CRMD_ACTION_STOP;
            break;
        case stopped_rsc:
            result = CRMD_ACTION_STOPPED;
            break;
        case start_rsc:
            result = CRMD_ACTION_START;
            break;
        case started_rsc:
            result = CRMD_ACTION_STARTED;
            break;
        case shutdown_crm:
            result = CRM_OP_SHUTDOWN;
            break;
        case stonith_node:
            result = CRM_OP_FENCE;
            break;
        case monitor_rsc:
            result = CRMD_ACTION_STATUS;
            break;
        case action_notify:
            result = CRMD_ACTION_NOTIFY;
            break;
        case action_notified:
            result = CRMD_ACTION_NOTIFIED;
            break;
        case action_promote:
            result = CRMD_ACTION_PROMOTE;
            break;
        case action_promoted:
            result = CRMD_ACTION_PROMOTED;
            break;
        case action_demote:
            result = CRMD_ACTION_DEMOTE;
            break;
        case action_demoted:
            result = CRMD_ACTION_DEMOTED;
            break;
    }

    return result;
}

const char *
role2text(enum rsc_role_e role)
{
    switch (role) {
        case RSC_ROLE_UNKNOWN:
            return RSC_ROLE_UNKNOWN_S;
        case RSC_ROLE_STOPPED:
            return RSC_ROLE_STOPPED_S;
        case RSC_ROLE_STARTED:
            return RSC_ROLE_STARTED_S;
        case RSC_ROLE_SLAVE:
            return RSC_ROLE_SLAVE_S;
        case RSC_ROLE_MASTER:
            return RSC_ROLE_MASTER_S;
    }
    CRM_CHECK(role >= RSC_ROLE_UNKNOWN, return RSC_ROLE_UNKNOWN_S);
    CRM_CHECK(role < RSC_ROLE_MAX, return RSC_ROLE_UNKNOWN_S);
    // coverity[dead_error_line]
    return RSC_ROLE_UNKNOWN_S;
}

enum rsc_role_e
text2role(const char *role)
{
    CRM_ASSERT(role != NULL);
    if (safe_str_eq(role, RSC_ROLE_STOPPED_S)) {
        return RSC_ROLE_STOPPED;
    } else if (safe_str_eq(role, RSC_ROLE_STARTED_S)) {
        return RSC_ROLE_STARTED;
    } else if (safe_str_eq(role, RSC_ROLE_SLAVE_S)) {
        return RSC_ROLE_SLAVE;
    } else if (safe_str_eq(role, RSC_ROLE_MASTER_S)) {
        return RSC_ROLE_MASTER;
    } else if (safe_str_eq(role, RSC_ROLE_UNKNOWN_S)) {
        return RSC_ROLE_UNKNOWN;
    }
    crm_err("Unknown role: %s", role);
    return RSC_ROLE_UNKNOWN;
}

int
merge_weights(int w1, int w2)
{
    int result = w1 + w2;

    if (w1 <= -INFINITY || w2 <= -INFINITY) {
        if (w1 >= INFINITY || w2 >= INFINITY) {
            crm_trace("-INFINITY + INFINITY == -INFINITY");
        }
        return -INFINITY;

    } else if (w1 >= INFINITY || w2 >= INFINITY) {
        return INFINITY;
    }

    /* detect wrap-around */
    if (result > 0) {
        if (w1 <= 0 && w2 < 0) {
            result = -INFINITY;
        }

    } else if (w1 > 0 && w2 > 0) {
        result = INFINITY;
    }

    /* detect +/- INFINITY */
    if (result >= INFINITY) {
        result = INFINITY;

    } else if (result <= -INFINITY) {
        result = -INFINITY;
    }

    crm_trace("%d + %d = %d", w1, w2, result);
    return result;
}

void
add_hash_param(GHashTable * hash, const char *name, const char *value)
{
    CRM_CHECK(hash != NULL, return);

    crm_trace("adding: name=%s value=%s", crm_str(name), crm_str(value));
    if (name == NULL || value == NULL) {
        return;

    } else if (safe_str_eq(value, "#default")) {
        return;

    } else if (g_hash_table_lookup(hash, name) == NULL) {
        g_hash_table_insert(hash, strdup(name), strdup(value));
    }
}

const char *
pe_node_attribute_calculated(const pe_node_t *node, const char *name,
                             const resource_t *rsc)
{
    const char *source;

    if(node == NULL) {
        return NULL;

    } else if(rsc == NULL) {
        return g_hash_table_lookup(node->details->attrs, name);
    }

    source = g_hash_table_lookup(rsc->meta, XML_RSC_ATTR_TARGET);
    if(source == NULL || safe_str_eq("host", source) == FALSE) {
        return g_hash_table_lookup(node->details->attrs, name);
    }

    /* Use attributes set for the containers location
     * instead of for the container itself
     *
     * Useful when the container is using the host's local
     * storage
     */

    CRM_ASSERT(node->details->remote_rsc);
    CRM_ASSERT(node->details->remote_rsc->container);

    if(node->details->remote_rsc->container->running_on) {
        pe_node_t *host = node->details->remote_rsc->container->running_on->data;
        pe_rsc_trace(rsc, "%s: Looking for %s on the container host %s", rsc->id, name, host->details->uname);
        return g_hash_table_lookup(host->details->attrs, name);
    }

    pe_rsc_trace(rsc, "%s: Not looking for %s on the container host: %s is inactive",
                 rsc->id, name, node->details->remote_rsc->container->id);
    return NULL;
}

const char *
pe_node_attribute_raw(pe_node_t *node, const char *name)
{
    if(node == NULL) {
        return NULL;
    }
    return g_hash_table_lookup(node->details->attrs, name);
}
