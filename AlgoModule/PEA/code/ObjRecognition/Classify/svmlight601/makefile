 # target source
include ../../../../../../Makefile.param
    
VPATH+= src include 
SRC := $(wildcard *.c) 
OBJ := $(SRC:%.c=%.o)
.PHONY : all clean

all: $(OBJ)


%.o: %.c
	 $(CC) $(CFLAGS) -c $< $(INCLUDES) 

clean:
	@rm -rf *.o


