CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

SET (CMAKE_VERBOSE_MAKEFILE on)

ENABLE_LANGUAGE(C)

SET (CFLAGS "-Wall -Werror")
SET (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${CFLAGS}")

ADD_SUBDIRECTORY (src)
