
AM_CFLAGS = $(TEST_CFLAGS) -I../src/pscpu -I../src/ieeeformat -I../src/base -I../include/superlu -I../include/coin -I../include/ipopt -L../src/pscpu/.libs -L../src/ieeeformat/.libs -L../lib/cygwin -g
AM_CXXFLAGS = $(AM_CFLAGS)
LDADD =-lpscpu ../src/ieeeformat/libieeeformattest.a -lipopt -lmumps -lsuperlu -lmetis -llapack -lblas -lgfortran ../src/base/libcalgtest.a  libtestframework.a

#if USE_VALGRIND
#TESTS_ENVIRONMENT=./valgrind-wrapper
#endif

TESTS =                          \
        test-alloc-testing       \
        test-arraylist           \
        test-avl-tree            \
        test-bi-tree             \
        test-binary-heap         \
        test-binomial-heap       \
        test-bloom-filter        \
        test-bs-tree             \
        test-cpp                 \
        test-graph               \
        test-list                \
        test-slist               \
        test-queue               \
        test-compare-functions   \
        test-hash-functions      \
        test-hash-table          \
        test-rb-tree             \
        test-set                 \
        test-trie                \
        test-ieee                \
        test-pf

check_PROGRAMS = $(TESTS)
check_LIBRARIES = libtestframework.a

libtestframework_a_SOURCES=\
alloc-testing.c      alloc-testing.h        \
framework.c          framework.h

test_cpp_SOURCES = test-cpp.cpp
test_pf_SOURCES = test-pf.cpp

EXTRA_DIST=valgrind-wrapper

