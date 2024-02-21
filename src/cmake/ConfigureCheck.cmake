include(AutoconfHelper)
include(AutoconfHelper2)
include(CMakePushCheckState)


# Checks for programs.
# AC_PROG_CC
# AC_PROG_MAKE_SET


# Checks for header files.
ac_header_stdc()
ac_check_headers(arpa/inet.h ctype.h fcntl.h net/errno.h netdb.h netinet/in.h param.h pthread.h socks.h stdlib.h string.h strings.h sys/ioctl.h sys/param.h sys/ptem.h sys/socket.h sys/termio.h sys/time.h time.h unistd.h util.h pty.h stropts.h)

# not listed in configure.ac
ac_check_headers(inttypes.h memory.h sys/select.h sys/stat.h)

cmake_push_check_state()
list(APPEND CMAKE_REQUIRED_INCLUDES "${ZLIB_INCLUDE_DIR}")
ac_check_headers(zlib.h)
if(NOT HAVE_ZLIB_H)
    message(FATAL_ERROR "zlib header file not found, is the development part present")
endif()
cmake_pop_check_state()

cmake_push_check_state()
list(APPEND CMAKE_REQUIRED_INCLUDES "${PCRE_INCLUDE_DIR}")
ac_check_headers(pcre.h)
if(NOT HAVE_PCRE_H)
    message(FATAL_ERROR "pcre header file not found, is the development part present")
endif()
cmake_pop_check_state()

cmake_push_check_state()
list(APPEND CMAKE_REQUIRED_INCLUDES "${GNUTLS_INCLUDE_DIR}")
ac_check_headers(gnutls/gnutls.h)
if(HAVE_GNUTLS_GNUTLS_H)
    set(HAVE_GNUTLS_H yes)
else()
    message(FATAL_ERROR "gnutls header file not found, is the development part present")
endif()
cmake_pop_check_state()


# Checks for typedefs, structures, and compiler characteristics.
ac_c_const()
ac_header_stdbool()
ac_type_size_t()
ac_header_time()
ac_struct_tm()


# Checks for library functions.
ac_func_memcmp()
ac_func_realloc()
ac_func_select_argtypes()
ac_type_signal()
ac_func_strftime()
ac_func_utime_null()
ac_func_vprintf()
ac_check_funcs(gethostbyname gethostname gettimeofday inet_ntoa memset select socket strcasecmp strchr strdup strerror strftime strncasecmp strstr utime getaddrinfo forkpty popen)


# Checks libraries
ac_check_lib(z inflate)
ac_check_lib(pthread pthread_create)
ac_check_lib(nsl gethostbyname)
ac_check_lib(socket rresvport)
ac_check_lib(util forkpty)
ac_check_lib(pcre pcre_compile)
ac_check_lib(gnutls gnutls_init)
ac_check_lib(m sqrt)

ac_check_files(/dev/ptmx)

ac_func_getmntent()
ac_header_dirent()
