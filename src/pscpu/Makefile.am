ACLOCAL_AMFLAGS = -I m4
lib_LTLIBRARIES=libpscpu.la

AM_CFLAGS = $(TEST_CFLAGS) -I../ieeeformat -I../../include/superlu -I../../include/coin -I../../include/ipopt -g
AM_CXXFLAGS = $(AM_CFLAGS)

MAIN_HEADERFILES = libpscpu.hpp

pscpu_HEADERFILES=\
AbstractOPF.h    AbstractPF.h    AbstractPFCal.h    AbstractSE.h    AbstractSM.h \
BasicCPF.h    BusNumberOpt.h    CPFModel.h    Hessian.h    IpoptOPF.h    IpoptPF.h \
IpoptSE.h    Jacobian.h    MapSparseMatrix.h    MathUtil.h    MeasureConstants.h \
MeasureInfo.h    MeasureVector.h    NewtonAlg.h    NewtonModel.h    NewtonPF.h \
ObjectiveFunction.h    OPFModel.h    OPFObjFunc.h    PFModel.h    PFModel_CI.h \
PFUtil.h    SEModel.h    SparseMatrix.h    StateCalculator.h    YMatrix.h

SRC=\
AbstractOPF.cpp    AbstractPF.cpp    AbstractSE.cpp    AbstractSM.cpp    BasicCPF.cpp \
BusNumberOpt.cpp    Hessian.cpp    IpoptOPF.cpp    IpoptPF.cpp    IpoptSE.cpp    Jacobian.cpp \
MapSparseMatrix.cpp    MeasureInfo.cpp    MeasureVector.cpp    NewtonAlg.cpp    NewtonPF.cpp \
OPFObjFunc.cpp     PFModel.cpp    PFModel_CI.cpp    PFUtil.cpp    SEModel.cpp    SigmoidObjFuction.cpp \
SparseMatrix.cpp    StateCalculator.cpp    WLSObjFunction.cpp    YMatrix.cpp

libpscpu_la_CFLAGS=$(MAIN_CFLAGS)
libpscpu_la_SOURCES=$(SRC) $(MAIN_HEADERFILES) $(pscpu_HEADERFILES)

headerfilesdir=$(includedir)/libcalg-1.0
headerfiles_HEADERS=$(MAIN_HEADERFILES)

pscpuheaderfilesdir=$(headerfilesdir)/libcalg
pscpuheaderfiles_HEADERS=$(pscpu_HEADERFILES)

EXTRA_DIST=gencov

