# target source
include ../../../../Makefile.param
INCLUDES := $(INCLUDES) -I./include -I../../../GA/CommonAlgo/Common/include -I../../../GA/CommonAlgo/ImageAlgo/include -I../PEACommon/include/ -I../../../ALGO_PARA/Para/include/ -I../../../GA/EventManager/include/ -I../ObjRecognition/Detector/include/ -I../System/include/ -I../ObjRecognition/ -I../../../AVD/include -I../../../GA/WaterMark/include/ -I../ObjRecognition/Tracker/include/ -I../ObjRecognition/Classify/include/ -I../ObjRecognition/Classify/svmlight601/
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



