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

ROOT_PATTERN		= /trishool/
DEPOT_ROOT			= $(word 1,$(subst $(ROOT_PATTERN), ,$(CURDIR)))
ROOT_PATH			= $(DEPOT_ROOT)$(ROOT_PATTERN)

include $(ROOT_PATH)/Makefile.globals

YARA_VERSION		= 3.7.1
DIST_PKG_SUFFIX     = .zip

YARA_BUILD_ROOT		= $(VENDOR_BUILD_ROOT)/yara
YARA_TAR_FILE		= $(CURDIR)/yara-$(YARA_VERSION)$(DIST_PKG_SUFFIX)

default : unzip setup $(LIB_TARGETS) $(INCLUDE_TARGETS)

unzip:
	unzip -d $(VENDOR_BUILD_ROOT)  -q $(YARA_TAR_FILE)
	mv $(VENDOR_BUILD_ROOT)/yara-$(YARA_VERSION) $(VENDOR_BUILD_ROOT)/yara

setup : $(VENDOR_LIB_DIR) $(VENDOR_INC_DIR) $(VENDOR_BIN_DIR)
	cp -r $(YARA_BUILD_ROOT)/include/* $(VENDOR_INC_DIR)/
	cp $(YARA_BUILD_ROOT)/lib/* $(VENDOR_LIB_DIR)/

$(INCLUDE_TARGETS) :

$(LIB_TARGETS) :

$(VENDOR_LIB_DIR) :
	mkdir -p $@

$(VENDOR_INC_DIR) :
	mkdir -p $@

$(VENDOR_BIN_DIR) :
	mkdir -p $@

clean :
	rm -rf $(YARA_BUILD_ROOT)
	rm -rf $(VENDOR_INC_DIR)/yara*
	rm -rf $(VENDOR_LIB_DIR)/*yara*
