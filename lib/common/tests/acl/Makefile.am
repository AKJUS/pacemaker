#
# Copyright 2021-2025 the Pacemaker project contributors
#
# The version control history for this file may have further details.
#
# This source code is licensed under the GNU General Public License version 2
# or later (GPLv2+) WITHOUT ANY WARRANTY.
#

include $(top_srcdir)/mk/common.mk
include $(top_srcdir)/mk/tap.mk
include $(top_srcdir)/mk/unittest.mk

# Add "_test" to the end of all test program names to simplify .gitignore.

check_PROGRAMS = pcmk__is_user_in_group_test 	\
		 pcmk_acl_required_test 	\
		 xml_acl_denied_test

TESTS = $(check_PROGRAMS)
