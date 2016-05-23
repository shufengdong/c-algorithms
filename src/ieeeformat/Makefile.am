ACLOCAL_AMFLAGS = -I m4
check_LIBRARIES=libieeeformattest.a
lib_LTLIBRARIES=libieeeformat.la

MAIN_HEADERFILES = libieeeformat.hpp

IeeeFormat_HEADERFILES=\
ieee-data.h

SRC=\
ieee-data.c

libieeeformattest_a_CFLAGS=$(TEST_CFLAGS) -DALLOC_TESTING -I../../test -g
libieeeformattest_a_SOURCES=$(SRC) $(MAIN_HEADERFILES)

libieeeformat_la_CFLAGS=$(MAIN_CFLAGS)
libieeeformat_la_SOURCES=$(SRC) $(MAIN_HEADERFILES) $(IeeeFormat_HEADERFILES)

headerfilesdir=$(includedir)/libcalg-1.0
headerfiles_HEADERS=$(MAIN_HEADERFILES)

ieeeformatheaderfilesdir=$(headerfilesdir)/libcalg
ieeeformatheaderfiles_HEADERS=$(IeeeFormat_HEADERFILES)

EXTRA_DIST=gencov

