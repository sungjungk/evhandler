set(TARGET evhandler)

set(SRCS
	evhandler.c
   )

add_library(${TARGET} SHARED ${SRCS})
