#*************************************************
# *
# *  Copyright (c) 2015 Sphantix 
# *  All Rights Reserved
# *
# * ***********************************************
# #
# In most cases, you only need to modify this first section.
#
LIB = main.a

OBJS = event.o plugin_info.o plugin.o firmware.o ws_thread.o handler_thread.o sys_interface.o client.o main.o

all: $(LIB)
$(LIB): $(OBJS)
	$(AR) -rcs $@ $(OBJS)

.SUFFIXES: .c .o .cpp
.cpp.o:
	$(CXX) $(CFLAGS) -c $*.cpp -o $*.o

.PRONY:clean
clean:
	@echo "Removing linked and compiled files......"
	rm -f $(OBJS) $(LIB)

#
# Include the rule for making dependency files.
# The '-' in front of the second include suppresses
# error messages when make cannot find the .d files.
# It will just regenerate them.
# See Section 4.14 of Gnu Make.
#

-include $(OBJS:.o=.d)
