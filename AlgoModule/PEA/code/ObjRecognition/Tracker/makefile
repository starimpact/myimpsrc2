# target source
include ../../../../../Makefile.param
INCLUDES := $(INCLUDES) -I./include -I$(ALGODIR)/ALGO_PARA/Para/include/ -I$(ALGODIR)/GA/CommonAlgo/Common/include -I$(ALGODIR)/GA/CommonAlgo/ImageAlgo/include -I$(ALGODIR)/PEA/code/PEACommon/include/ -I$(ALGODIR)/GA/EventManager/include/ -I$(ALGODIR)/PEA/code/System/include/ -I$(ALGODIR)/PEA/code/ObjRecognition -I$(ALGODIR)/PEA/code/ObjRecognition/Detector/include/ -I$(ALGODIR)/AVD/include -I$(ALGODIR)/GA/WaterMark/include/ -I$(ALGODIR)/PEA/code/ObjRecognition/Tracker/include/ -I$(ALGODIR)/PEA/code/BVA/include 
    
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

