ACLOCAL_AMFLAGS = -I m4
lib_LTLIBRARIES=libieeeformat.la

MAIN_HEADERFILES = libieeeformat.hpp

IeeeFormat_HEADERFILES=\
DefaultIEEEFormatRW.hpp  BranchData.hpp      BusData.hpp      IEEEDataIsland.hpp     \
InterchangeData.hpp    LossZoneData.hpp    TieLineData.hpp    TitleData.hpp

SRC=\
DefaultIEEEFormatRW.cpp    IEEEDataImpl.cpp

libieeeformat_la_CFLAGS=$(MAIN_CFLAGS)
libieeeformat_la_SOURCES=$(SRC) $(MAIN_HEADERFILES) $(IeeeFormat_HEADERFILES)

headerfilesdir=$(includedir)/libcalg-1.0
headerfiles_HEADERS=$(MAIN_HEADERFILES)

ieeeformatheaderfilesdir=$(headerfilesdir)/libcalg
ieeeformatheaderfiles_HEADERS=$(IeeeFormat_HEADERFILES)

EXTRA_DIST=gencov

