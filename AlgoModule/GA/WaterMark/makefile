# target source
include ../../../Makefile.param
INCLUDES := $(INCLUDES) -I./include -I../../PEA/code/ObjRecognition/Detector/include -I../../AVD/include -I../CommonAlgo/Common/include -I../CommonAlgo/ImageAlgo/include -I../../PEA/code/PEACommon/include/ -I../../ALGO_PARA/Para/include/ -I../EventManager/include/
VPATH+= src include
ORG := $(wildcard src/*.c)
SRC := $(subst src/,,$(ORG)) 
OBJ := $(SRC:%.c=%.o)
.PHONY : all clean

all: $(OBJ)


%.o: %.c
	 $(CC) $(CFLAGS) -c $< $(INCLUDES) 

clean:
	@rm -rf *.o
