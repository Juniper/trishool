#
# Copyright(C) 2018, Juniper Networks, Inc.
# All rights reserved
#
# This SOFTWARE is licensed under the license provided in the LICENSE.txt
# file.  By downloading, installing, copying, or otherwise using the
# SOFTWARE, you agree to be bound by the terms of that license.  This
# SOFTWARE is not an official Juniper product.
#
# Third-Party Code: This SOFTWARE may depend on other components under
# separate copyright notice and license terms.  Your use of the source
# code for those components is subject to the term and conditions of
# the respective license as noted in the Third-Party source code.
#

#
# 1-27-2011: the $(word 1, $(subst....))) stuff takes the CURDIR
# and strips off everything after "/cyphort-endpoint". If the path
# doesn't have '/cyphort-endpoint', DEPOT_ROOT will be the curr. dir.
#
ROOT_PATTERN	= /trishool
DEPOT_ROOT		= $(word 1,$(subst $(ROOT_PATTERN), ,$(CURDIR)))
ROOT_PATH		= $(DEPOT_ROOT)$(ROOT_PATTERN)

include $(ROOT_PATH)/Makefile.globals

SUB_DIRS		= trishool-bt

default:
	for dir in $(SUB_DIRS) ; do  \
		make -C $$dir default || exit 1; \
	done

clean :
	for dir in $(SUB_DIRS) ; do  \
		make -C $$dir clean || exit 1; \
	done

all : default

allclean: clean
	rm -rf $(TRISHOOL_BUILD_ROOT)
