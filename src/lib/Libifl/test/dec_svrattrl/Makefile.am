PROG_ROOT = ../..

AM_CFLAGS = -g -DTEST_FUNCTION -I${PROG_ROOT}/ -I${PROG_ROOT}/${PBS_MACH} --coverage

lib_LTLIBRARIES = libdec_svrattrl.la

AM_LDFLAGS = @CHECK_LIBS@ ${lib_LTLIBRARIES}

check_PROGRAMS = test_dec_svrattrl

libdec_svrattrl_la_SOURCES = scaffolding.c ${PROG_ROOT}/dec_svrattrl.c
libdec_svrattrl_la_LDFLAGS = @CHECK_LIBS@ -shared -lgcov

test_dec_svrattrl_SOURCES = test_dec_svrattrl.c

check_SCRIPTS = ${PROG_ROOT}/../../test/coverage_run.sh

TESTS = ${check_PROGRAMS} ${check_SCRIPTS} 

CLEANFILES = *.gcno *.gcda *.gcov core *.lo
