PROG_ROOT = ../..

AM_CFLAGS = -g -DTEST_FUNCTION -I${PROG_ROOT}/ -I${PROG_ROOT}/${PBS_MACH} --coverage

lib_LTLIBRARIES = libenc_ReqHdr.la

AM_LDFLAGS = @CHECK_LIBS@ ${lib_LTLIBRARIES}

check_PROGRAMS = test_enc_ReqHdr

libenc_ReqHdr_la_SOURCES = scaffolding.c ${PROG_ROOT}/enc_ReqHdr.c
libenc_ReqHdr_la_LDFLAGS = @CHECK_LIBS@ -shared -lgcov

test_enc_ReqHdr_SOURCES = test_enc_ReqHdr.c

check_SCRIPTS = ${PROG_ROOT}/../../test/coverage_run.sh

TESTS = ${check_PROGRAMS} ${check_SCRIPTS} 

CLEANFILES = *.gcno *.gcda *.gcov core *.lo
