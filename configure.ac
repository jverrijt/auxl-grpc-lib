#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.71])
AC_INIT([auxl-grpc-lib], [0.1], [joost@metamotifs.nl])
AM_INIT_AUTOMAKE([foreign])
AM_PROG_CC_C_O
AC_CONFIG_SRCDIR([src/cli/auxl_grpc_cli.cpp])
AC_CONFIG_HEADERS([config.h])

AC_CANONICAL_HOST

build_linux=no
build_windows=no
build_mac=no

# Detect the target system
case "${host_os}" in
    linux*)
        build_linux=yes
        ;;
    cygwin*|mingw*)
        build_windows=yes
        ;;
    darwin*)
        build_mac=yes
        ;;
    *)
        AC_MSG_ERROR(["OS $host_os is not supported"])
        ;;
esac

# Pass the conditionals to automake
AM_CONDITIONAL([LINUX], [test "$build_linux" = "yes"])
AM_CONDITIONAL([WINDOWS], [test "$build_windows" = "yes"])
AM_CONDITIONAL([OSX], [test "$build_mac" = "yes"])

# Checks for programs.
AC_PROG_CXX
AC_PROG_AWK
AC_PROG_CC
AC_PROG_CPP
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
AC_PROG_OBJC
AC_PROG_RANLIB

# Checks for libraries.
AC_CHECK_LIB([absl_bad_any_cast_impl], [main])
AC_CHECK_LIB([absl_bad_optional_access], [main])
AC_CHECK_LIB([absl_bad_variant_access], [main])
AC_CHECK_LIB([absl_base], [main])
AC_CHECK_LIB([absl_city], [main])
AC_CHECK_LIB([absl_civil_time], [main])
AC_CHECK_LIB([absl_cord], [main])
AC_CHECK_LIB([absl_cord_internal], [main])
AC_CHECK_LIB([absl_cordz_functions], [main])
AC_CHECK_LIB([absl_cordz_handle], [main])
AC_CHECK_LIB([absl_cordz_info], [main])
AC_CHECK_LIB([absl_cordz_sample_token], [main])
AC_CHECK_LIB([absl_debugging_internal], [main])
AC_CHECK_LIB([absl_demangle_internal], [main])
AC_CHECK_LIB([absl_examine_stack], [main])
AC_CHECK_LIB([absl_exponential_biased], [main])
AC_CHECK_LIB([absl_failure_signal_handler], [main])
AC_CHECK_LIB([absl_flags], [main])
AC_CHECK_LIB([absl_flags_commandlineflag], [main])
AC_CHECK_LIB([absl_flags_commandlineflag_internal], [main])
AC_CHECK_LIB([absl_flags_config], [main])
AC_CHECK_LIB([absl_flags_internal], [main])
AC_CHECK_LIB([absl_flags_marshalling], [main])
AC_CHECK_LIB([absl_flags_parse], [main])
AC_CHECK_LIB([absl_flags_private_handle_accessor], [main])
AC_CHECK_LIB([absl_flags_program_name], [main])
AC_CHECK_LIB([absl_flags_reflection], [main])
AC_CHECK_LIB([absl_flags_usage], [main])
AC_CHECK_LIB([absl_flags_usage_internal], [main])
AC_CHECK_LIB([absl_graphcycles_internal], [main])
AC_CHECK_LIB([absl_hash], [main])
AC_CHECK_LIB([absl_hashtablez_sampler], [main])
AC_CHECK_LIB([absl_int128], [main])
AC_CHECK_LIB([absl_leak_check], [main])
AC_CHECK_LIB([absl_leak_check_disable], [main])
AC_CHECK_LIB([absl_log_severity], [main])
AC_CHECK_LIB([absl_low_level_hash], [main])
AC_CHECK_LIB([absl_malloc_internal], [main])
AC_CHECK_LIB([absl_periodic_sampler], [main])
AC_CHECK_LIB([absl_random_distributions], [main])
AC_CHECK_LIB([absl_random_internal_distribution_test_util], [main])
AC_CHECK_LIB([absl_random_internal_platform], [main])
AC_CHECK_LIB([absl_random_internal_pool_urbg], [main])
AC_CHECK_LIB([absl_random_internal_randen], [main])
AC_CHECK_LIB([absl_random_internal_randen_hwaes], [main])
AC_CHECK_LIB([absl_random_internal_randen_hwaes_impl], [main])
AC_CHECK_LIB([absl_random_internal_randen_slow], [main])
AC_CHECK_LIB([absl_random_internal_seed_material], [main])
AC_CHECK_LIB([absl_random_seed_gen_exception], [main])
AC_CHECK_LIB([absl_random_seed_sequences], [main])
AC_CHECK_LIB([absl_raw_hash_set], [main])
AC_CHECK_LIB([absl_raw_logging_internal], [main])
AC_CHECK_LIB([absl_scoped_set_env], [main])
AC_CHECK_LIB([absl_spinlock_wait], [main])
AC_CHECK_LIB([absl_stacktrace], [main])
AC_CHECK_LIB([absl_status], [main])
AC_CHECK_LIB([absl_statusor], [main])
AC_CHECK_LIB([absl_str_format_internal], [main])
AC_CHECK_LIB([absl_strerror], [main])
AC_CHECK_LIB([absl_strings], [main])
AC_CHECK_LIB([absl_strings_internal], [main])
AC_CHECK_LIB([absl_symbolize], [main])
AC_CHECK_LIB([absl_synchronization], [main])
AC_CHECK_LIB([absl_throw_delegate], [main])
AC_CHECK_LIB([absl_time], [main])
AC_CHECK_LIB([absl_time_zone], [main])
AC_CHECK_LIB([address_sorting], [main])
AC_CHECK_LIB([cares], [main])
AC_CHECK_LIB([dl], [main])
AC_CHECK_LIB([gpr], [main])
AC_CHECK_LIB([grpc], [main])
AC_CHECK_LIB([grpc++], [main])
AC_CHECK_LIB([grpcpp_channelz], [main])
AC_CHECK_LIB([protobuf], [main])
AC_CHECK_LIB([pthread], [main])
AC_CHECK_LIB([re2], [main])
AC_CHECK_LIB([upb], [main])
AC_CHECK_LIB([z], [main])
AC_CHECK_LIB([ssl], [main])
AC_CHECK_LIB([crypto], [main])

# Checks for header files.
AC_CHECK_HEADERS([arpa/inet.h fcntl.h inttypes.h mach/mach.h malloc.h netdb.h netinet/in.h stdint.h strings.h sys/ioctl.h sys/param.h sys/socket.h sys/time.h sys/timeb.h unistd.h])

AC_CHECK_FILES([gen/gen/reflection.pb.h gen/gen/reflection.pb.cc], [], AC_MSG_ERROR([Could not find the required generated proto files. Run generate-reflection-support.sh]))

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_INT16_T
AC_TYPE_INT32_T
AC_TYPE_INT64_T
AC_TYPE_INT8_T
AC_TYPE_OFF_T
AC_TYPE_PID_T
AC_C_RESTRICT
AC_TYPE_SIZE_T
AC_TYPE_SSIZE_T
AC_STRUCT_TIMEZONE
AC_TYPE_UID_T
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T
AC_TYPE_UINT64_T
AC_TYPE_UINT8_T
AC_CHECK_TYPES([ptrdiff_t])

# Checks for library functions.
AC_FUNC_ALLOCA
AC_FUNC_ERROR_AT_LINE
AC_FUNC_FORK
AC_FUNC_MALLOC
AC_FUNC_MKTIME
AC_FUNC_MMAP
AC_FUNC_REALLOC
AC_FUNC_STRERROR_R
AC_FUNC_STRNLEN
AC_FUNC_STRTOD
AC_CHECK_FUNCS([alarm atexit clock_gettime dup2 fchdir floor getcwd gethostname getpagesize gettimeofday isascii localeconv localtime_r memchr memmove memset mkdir modf munmap pow putenv regcomp rint rmdir select setenv setlocale socket sqrt strcasecmp strchr strdup strerror strncasecmp strrchr strstr strtol strtoul strtoull tzset uname])

AC_CONFIG_FILES([Makefile])
AC_CONFIG_SUBDIRS([])
AC_OUTPUT
