#*************************************************
# *
# *  Copyright (c) 2015 sphantix 
# *  All Rights Reserved
# *
# * ***********************************************
# #
#
#set your own environment option  
LDFLAGS += -L.
CFLAGS +=

#make target lib and relevant obj
PRG = osl.a
OBJS = osl_logging.o osl_prctl.o osl_time.o osl_memory.o \
	   osl_strconv.o

#all target  
all: $(PRG)
$(PRG): $(OBJS)
	$(AR) -rcs $@ $(OBJS)

.SUFFIXES: .c .o
.c.o:
	$(CC) $(CFLAGS) -c $*.c -o $*.o

.PRONY:clean
clean:
	@echo "Removing linked and compiled files......"
	rm -f $(OBJS) $(PRG)

#
# Include the rule for making dependency files.
# The '-' in front of the second include suppresses
# error messages when make cannot find the .d files.
# It will just regenerate them.
# See Section 4.14 of Gnu Make.
#

-include $(OBJS:.o=.d)
