PROG_ROOT = ../..

AM_CFLAGS = -g -DTEST_FUNCTION -I${PROG_ROOT}/ -I${PROG_ROOT}/${PBS_MACH} --coverage

lib_LTLIBRARIES = libpbsD_asyrun.la

AM_LDFLAGS = @CHECK_LIBS@ ${lib_LTLIBRARIES}

check_PROGRAMS = test_pbsD_asyrun

libpbsD_asyrun_la_SOURCES = scaffolding.c ${PROG_ROOT}/pbsD_asyrun.c
libpbsD_asyrun_la_LDFLAGS = @CHECK_LIBS@ -shared -lgcov

test_pbsD_asyrun_SOURCES = test_pbsD_asyrun.c

check_SCRIPTS = ${PROG_ROOT}/../../test/coverage_run.sh

TESTS = ${check_PROGRAMS} ${check_SCRIPTS} 

CLEANFILES = *.gcno *.gcda *.gcov core *.lo
