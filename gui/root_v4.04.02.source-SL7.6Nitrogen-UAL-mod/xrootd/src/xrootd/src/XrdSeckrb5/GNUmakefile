#              $Id: GNUmakefile,v 1.9 2005/03/11 12:01:32 ganis Exp $

#------------------------------------------------------------------------------#
#                       C o m m o n   V a r i a b l e s                        #
#------------------------------------------------------------------------------#
  
include ../GNUmake.env

MORELIBS = -L$(LIBDIR) -lXrdOuc $(LIBKRB5)

LIBDEPS = $(LIBDIR)/libXrdOuc.a

#------------------------------------------------------------------------------#
#                             C o m p o n e n t s                              #
#------------------------------------------------------------------------------#
  
SOURCES = \
        XrdSecProtocolkrb5.cc

OBJECTS = \
        $(OBJDIR)/XrdSecProtocolkrb5.o

TARGETS = $(LIBDIR)/libXrdSeckrb5.$(TYPESHLIB)

#------------------------------------------------------------------------------#
#                           S e a r c h   P a t h s                            #
#------------------------------------------------------------------------------#

vpath XrdOuc% ../XrdOuc
vpath XrdSec% ../XrdSec

#------------------------------------------------------------------------------#
#                          I n i t i a l   R u l e s                           #
#------------------------------------------------------------------------------#
 
include ../GNUmake.options

anything: $(TARGETS)
	@echo Make XrdSeckrb5 done.

#------------------------------------------------------------------------------#
#                           D e p e n d e n c i e s                            #
#------------------------------------------------------------------------------#

$(TARGETS): $(OBJECTS) $(LIBDEPS)
	@echo Creating shared library $(TARGETS) 
	$(ECHO)if [ "x$(TYPE)" = "xcygwin" ]; then \
	   $(CC) $(OBJECTS) $(LDSO) $(MORELIBS) $(LIBS) \
		-Wl,--out-implib=$(TARGETS).a -o $(TARGETS); \
	else \
	   $(CC) $(OBJECTS) $(LDSO) $(MORELIBS) $(LIBS) -o $(TARGETS); \
	fi

$(OBJDIR)/XrdSecProtocolkrb5.o: XrdSecProtocolkrb5.cc XrdSecInterface.hh \
                                XrdOucPthread.hh      XrdOucTokenizer.hh \
                                XrdOucErrInfo.hh
	@echo Compiling XrdSecProtocolkrb5.cc 
	$(ECHO)$(CC) -c $(CFLAGS) $(INCLUDE) $(INCKRB5) -o $(OBJDIR)/XrdSecProtocolkrb5.o XrdSecProtocolkrb5.cc
