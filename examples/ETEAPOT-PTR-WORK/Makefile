CC       = g++                       # Compiler
CCFLAGS  = -g -fpic -ansi -Wall -Wextra -fpermissive     # Compilation flags

DLD      = g++                       # Dynamic linker
DLDFLAGS = -shared                   # Dynamic linker flags
LD       = g++                       # Linker

INC += -I. -I./src
INC += -I$(UAL)/ext/UI/include
INC += -I$(UAL_SXF)/src
INC += -I$(UAL_ACCSIM)/src
INC += -I$(UAL_TEAPOT)/src
INC += -I$(UAL_ETEAPOT)/src
INC += -I$(UAL_PAC)/src
INC += -I$(UAL)/codes/UAL/include
INC += -I$(UAL_ZLIB)/src
INC += -I$(UAL)/tools/include
INC += -I$(SXF)/src
INC += -I/home/ual/tools/gsl/include
INC += -I$(UAL)/codes/ETEAPOT/src/ETEAPOT/Integrator

LIBS += -L$(UAL)/ext/UI/lib/$(UAL_ARCH)  	 -lUalUI
LIBS += -L$(UAL_TIBETAN)/lib/$(UAL_ARCH)  	 -lTibetan
LIBS += -L$(UAL_SXF)/lib/$(UAL_ARCH) 		 -lUalSXF2
LIBS += -L$(UAL_ACCSIM)/lib/$(UAL_ARCH) 	 -lAccsim
LIBS += -L$(UAL_TEAPOT)/lib/$(UAL_ARCH) 	 -lTeapot
LIBS += -L$(UAL_ETEAPOT)/lib/$(UAL_ARCH)         -lETeapot
LIBS += -L$(UAL_PAC)/lib/$(UAL_ARCH)  		 -lPacSMF -lPacOptics -lPac
LIBS += -L$(UAL_ZLIB)/lib/$(UAL_ARCH) 		 -lZTps
LIBS += -L$(UAL)/codes/UAL/lib/$(UAL_ARCH) 	 -lUal
LIBS += -L$(SXF)/lib/$(UAL_ARCH) 		 -lSXF
LIBS += -L$(UAL)/tools/lib/$(UAL_ARCH) 		 -lpcre -lxml2 -lrfftw -lfftw
LIBS += 					 -lm

OBJ_DIR  = ./lib/$(UAL_ARCH)/obj

compile : $(OBJ_DIR)/DipoleTracker.o ./tracker

clean :
	rm -f P* $(OBJ_DIR)/* ./tracker out/cpp/* OUT E* echo.sxf m_m Nu*

$(OBJ_DIR)/DipoleTracker.o :
	$(CC) $(CCFLAGS) -c DipoleTracker.cc $(INC) -o $(OBJ_DIR)/DipoleTracker.o

$(OBJ_DIR)/tracker.o : 
	$(CC) $(CCFLAGS) -c tracker.cc $(INC) -o $(OBJ_DIR)/tracker.o

./tracker  : $(OBJ_DIR)/DipoleTracker.o $(OBJ_DIR)/tracker.o
	$(CC) -o $@  $(INC) $(OBJ_DIR)/tracker.o $(LIBS) $(OBJ_DIR)/DipoleTracker.o
