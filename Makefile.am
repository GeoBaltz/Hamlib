## Process this file with automake to produce Makefile.in

aclocaldir = $(datadir)/aclocal
aclocal_DATA = hamlib.m4

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = hamlib.pc

EXTRA_DIST = PLAN LICENSE hamlib.m4 hamlib.pc.in README.md README.developer \
	README.betatester README.freqranges README.multicast README.osx \
	Android.mk

doc_DATA = ChangeLog COPYING COPYING.LIB LICENSE \
	README.md README.betatester README.developer

SUBDIRS = macros include lib security \
	$(BACKEND_LIST) \
	$(RIG_BACKEND_LIST) \
	$(ROT_BACKEND_LIST) \
	$(AMP_BACKEND_LIST) \
	src \
	$(BINDINGS) \
	tests doc

## Static list of distributed directories.
DIST_SUBDIRS = \
	$(RIG_BACKEND_OPTIONAL_LIST) \
	$(ROT_BACKEND_OPTIONAL_LIST) \
	macros include lib src c++ bindings tests doc android scripts simulators\
	security $(BACKEND_LIST) $(RIG_BACKEND_LIST) $(ROT_BACKEND_LIST) $(AMP_BACKEND_LIST)

# Install any third party macros into our tree for distribution
ACLOCAL_AMFLAGS = -I macros --install 

AM_CFLAGS=-D_TIME_BITS=64 -Winitializer-overrides
