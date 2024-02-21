cmake_minimum_required(VERSION 3.22.2)

project(TinTin++
    LANGUAGES C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find 3rdparty libraries
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

find_package(GnuTLS REQUIRED)
find_package(PCRE REQUIRED)
find_package(ZLIB REQUIRED)

set(MY_INCLUDE_DIRS)
list(APPEND MY_INCLUDE_DIRS "${GNUTLS_INCLUDE_DIRS}")
list(APPEND MY_INCLUDE_DIRS "${PCRE_INCLUDE_DIRS}")
list(APPEND MY_INCLUDE_DIRS "${ZLIB_INCLUDE_DIRS}")

set(MY_LIBRARIES)
list(APPEND MY_LIBRARIES "${GNUTLS_LIBRARIES}")
list(APPEND MY_LIBRARIES "${PCRE_LIBRARIES}")
list(APPEND MY_LIBRARIES "${ZLIB_LIBRARIES}")

# Configure check
include(ConfigureCheck)
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/cmake/config.h.in" "${CMAKE_CURRENT_SOURCE_DIR}/config.h")

# Library tinlib
add_library(tinlib
    advertise.c
    banner.c
    base.c
    buffer.c
    chat.c
    class.c
    command.c
    config.c
    cursor.c
    daemon.c
    data.c
    debug.c
    dict.c
    draw.c
    edit.c
    event.c
    files.c
    forkpty.c
    help.c
    history.c
    input.c
    line.c
    list.c
    log.c
    mapper.c
    math.c
    mccp.c
    memory.c
    misc.c
    msdp.c
    nest.c
    net.c
    parse.c
    path.c
    port.c
    regex.c
    scan.c
    screen.c
    session.c
    show.c
    sort.c
    split.c
    ssl.c
    string.c
    substitute.c
    system.c
    tables.c
    telopt_client.c
    telopt_server.c
    terminal.c
    text.c
    tokenize.c
    trigger.c
    update.c
    utf8.c
    utils.c
    variable.c
    vt102.c
    config.h
    dict.h
    gui.h
    telnet.h
    tintin.h
)
target_include_directories(tinlib PUBLIC "${MY_INCLUDE_DIRS}")
target_link_libraries(tinlib PUBLIC "${MY_LIBRARIES}")

# Executable tinlib
add_executable(tt++
    main.c
)
target_include_directories(tt++ PUBLIC "${MY_INCLUDE_DIRS}")
target_link_libraries(tt++ PUBLIC tinlib "${MY_LIBRARIES}")