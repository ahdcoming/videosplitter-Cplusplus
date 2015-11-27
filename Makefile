#
# This is an example Makefile for a countwords program.  This
# program uses both the scanner module and a counter module.
# Typing 'make' or 'make count' will create the executable file.
#

# define some Makefile variables for the compiler and compiler flags
# to use Makefile variables later in the Makefile: $()
#
#  -g    adds debugging information to the executable file
#  -Wall turns on most, but not all, compiler warnings
#
# for C++ define  CC = g++
CC = c++
LIBS =   -lavcodec -lswscale -lswresample -lavutil -lavformat
INCLUDES = -I include
INCDIR = include
OBJDIR = obj
SRCDIR = src
BUILDDIR = build
LDFLAGS = -g -Wall
CFLAGS = -g -c -Wall  ${INCLUDES} -D__STDC_CONSTANT_MACROS


OBJS      = ${OBJDIR}/main.o ${OBJDIR}/in_context.o ${OBJDIR}/out_context.o ${OBJDIR}/in_audio.o ${OBJDIR}/in_video.o ${OBJDIR}/in_stream.o ${OBJDIR}/out_stream.o  ${OBJDIR}/out_stream_wmv2.o ${OBJDIR}/out_stream_wmav2.o


# typing 'make' will invoke the first target entry in the file 
# (in this case the default target entry)
# you can name this target entry anything, but "default" or "all"
# are the most commonly used names by convention
#
default: videosplitter

# To create the executable file count we need the object files
# countwords.o, counter.o, and scanner.o:
#

${OBJDIR}/in_stream.o: ${SRCDIR}/in_stream.cpp ${INCDIR}/in_stream.h
		${CC} ${CFLAGS} -o ${OBJDIR}/in_stream.o ${SRCDIR}/in_stream.cpp 

${OBJDIR}/in_video.o:  ${SRCDIR}/in_video.cpp ${INCDIR}/in_video.h
		${CC} ${CFLAGS} -o ${OBJDIR}/in_video.o ${SRCDIR}/in_video.cpp 

${OBJDIR}/in_audio.o:  ${SRCDIR}/in_audio.cpp ${INCDIR}/in_audio.h
		${CC} ${CFLAGS} -o ${OBJDIR}/in_audio.o ${SRCDIR}/in_audio.cpp 

${OBJDIR}/in_context.o: ${SRCDIR}/in_context.cpp ${INCDIR}/in_context.h
		${CC} ${CFLAGS} -o ${OBJDIR}/in_context.o ${SRCDIR}/in_context.cpp 

${OBJDIR}/out_stream.o: ${SRCDIR}/out_stream.cpp ${INCDIR}/out_stream.h
		${CC} ${CFLAGS} -o ${OBJDIR}/out_stream.o ${SRCDIR}/out_stream.cpp 

${OBJDIR}/out_stream_wmv2.o: ${INCDIR}/out_stream_wmv2.h
		${CC} ${CFLAGS} -o ${OBJDIR}/out_stream_wmv2.o ${SRCDIR}/out_stream_wmv2.cpp

${OBJDIR}/out_stream_wmav2.o: ${INCDIR}/out_stream_wmav2.h
		${CC} ${CFLAGS} -o ${OBJDIR}/out_stream_wmav2.o ${SRCDIR}/out_stream_wmav2.cpp


${OBJDIR}/out_context.o: ${SRCDIR}/out_context.cpp ${INCDIR}/out_context.h
		${CC} ${CFLAGS} -o ${OBJDIR}/out_context.o ${SRCDIR}/out_context.cpp 


${OBJDIR}/main.o: ${SRCDIR}/main.cpp ${INCDIR}/main.h
		${CC} ${CFLAGS} -o ${OBJDIR}/main.o ${SRCDIR}/main.cpp 

videosplitter:  ${OBJS}
		${CC} ${LDFLAGS} -o ${BUILDDIR}/videosplitter  ${OBJS} ${LIBS}



clean: 
		${RM} ${OBJDIR}/*.o  ${BUILDDIR}/videosplitter