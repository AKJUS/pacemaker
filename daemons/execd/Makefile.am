#
# Copyright 2012-2025 the Pacemaker project contributors
#
# The version control history for this file may have further details.
#
# This source code is licensed under the GNU Lesser General Public License
# version 2.1 or later (LGPLv2.1+) WITHOUT ANY WARRANTY.
#

include $(top_srcdir)/mk/common.mk
include $(top_srcdir)/mk/man.mk

halibdir		= $(CRM_DAEMON_DIR)

halib_PROGRAMS		= pacemaker-execd \
			  cts-exec-helper

EXTRA_DIST	= pacemaker-remoted.8.inc

pacemaker_execd_CFLAGS		= $(CFLAGS_HARDENED_EXE)
pacemaker_execd_LDFLAGS		= $(LDFLAGS_HARDENED_EXE)

pacemaker_execd_LDADD = $(top_builddir)/lib/fencing/libstonithd.la
pacemaker_execd_LDADD += $(top_builddir)/lib/services/libcrmservice.la
pacemaker_execd_LDADD += $(top_builddir)/lib/common/libcrmcommon.la
pacemaker_execd_SOURCES	= pacemaker-execd.c 	\
			  execd_commands.c 	\
			  execd_alerts.c 	\
			  execd_messages.c

sbin_PROGRAMS		= pacemaker-remoted
if BUILD_SYSTEMD
systemdsystemunit_DATA	= pacemaker_remote.service
else
if BUILD_LSB
initdir			= $(INITDIR)
init_SCRIPTS		= pacemaker_remote
endif
endif

pacemaker_remoted_CPPFLAGS	= -DPCMK__COMPILE_REMOTE \
				  $(AM_CPPFLAGS)

pacemaker_remoted_CFLAGS	= $(CFLAGS_HARDENED_EXE)
pacemaker_remoted_LDFLAGS	= $(LDFLAGS_HARDENED_EXE)

pacemaker_remoted_LDADD = $(top_builddir)/lib/fencing/libstonithd.la
pacemaker_remoted_LDADD	+= $(top_builddir)/lib/services/libcrmservice.la
pacemaker_remoted_LDADD += $(top_builddir)/lib/cib/libcib.la
pacemaker_remoted_LDADD += $(top_builddir)/lib/lrmd/liblrmd.la
pacemaker_remoted_LDADD += $(top_builddir)/lib/common/libcrmcommon.la
pacemaker_remoted_SOURCES	= $(pacemaker_execd_SOURCES) 	\
				  remoted_tls.c 		\
				  remoted_pidone.c 		\
				  remoted_proxy.c 		\
				  remoted_schemas.c

cts_exec_helper_LDADD = $(top_builddir)/lib/pengine/libpe_status.la
cts_exec_helper_LDADD += $(top_builddir)/lib/cib/libcib.la
cts_exec_helper_LDADD += $(top_builddir)/lib/lrmd/liblrmd.la
cts_exec_helper_LDADD += $(top_builddir)/lib/services/libcrmservice.la
cts_exec_helper_LDADD += $(top_builddir)/lib/common/libcrmcommon.la
cts_exec_helper_SOURCES	= cts-exec-helper.c

noinst_HEADERS  = pacemaker-execd.h

CLEANFILES = $(man8_MANS)
