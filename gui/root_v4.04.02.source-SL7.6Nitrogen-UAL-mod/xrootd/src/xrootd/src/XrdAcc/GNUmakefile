#              $Id: GNUmakefile,v 1.12 2005/03/01 05:57:21 abh Exp $

#-----------------------------------------------------------------------------#
#                            E n v i r o n m e n t                            #
#-----------------------------------------------------------------------------#
 
include ../GNUmake.env

BINLIBS = -L$(LIBDIR) -lXrdAcc -lXrdOuc

LIBDEPS = $(LIBDIR)/libXrdAcc.a $(LIBDIR)/libXrdOuc.a

#------------------------------------------------------------------------------#
#                             C o m p o n e n t s                              #
#------------------------------------------------------------------------------#
  
SOURCES = \
        XrdAccAccess.cc      \
        XrdAccAudit.cc       \
        XrdAccAuthFile.cc    \
        XrdAccCapability.cc  \
        XrdAccConfig.cc      \
        XrdAccGroups.cc      \
        XrdAccTest.cc
  
OBJECTA = \
        $(OBJDIR)/XrdAccAccess.o      \
        $(OBJDIR)/XrdAccAudit.o       \
        $(OBJDIR)/XrdAccAuthFile.o    \
        $(OBJDIR)/XrdAccCapability.o  \
        $(OBJDIR)/XrdAccConfig.o      \
        $(OBJDIR)/XrdAccGroups.o

OBJECTB = \
        $(OBJDIR)/XrdAccTest.o

OBJECTS = $(OBJECTA) $(OBJECTB)

LIBRARY = $(LIBDIR)/libXrdAcc.a

TESTBIN = $(BINDIR)/testaccess

TARGETS = $(LIBRARY) $(TESTBIN)

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
	@echo Make XrdAcc done.

#------------------------------------------------------------------------------#
#                           D e p e n d e n c i e s                            #
#------------------------------------------------------------------------------#

$(LIBRARY): $(OBJECTA)
	@echo Creating archive $(LIBRARY) 
	$(ECHO)rm -f $(LIBRARY)
	$(ECHO)ar -rc $(LIBRARY) $(OBJECTA)
	$(ECHO)ranlib $(LIBRARY)
	$(ECHO)if [ "x$(SUNCACHE)" != "x" ]; then \
	   if [ "$(TYPE)" = "SunOS" -a "$(CC)" = "CC" ]; then \
	      ar -rc $(LIBRARY) $(OBJDIR)$(SUNCACHE)/*/*.o; \
	   fi;\
	fi

$(TESTBIN): $(OBJECTB) $(LIBDEPS)
	@echo Creating executable $(TESTBIN) 
	$(ECHO)$(CC) $(CFLAGS) $(LDOP) $(OBJECTB) $(BINLIBS) $(LIBS) -o $(TESTBIN)

$(OBJDIR)/XrdAccAccess.o: XrdAccAccess.cc     XrdAccAccess.hh \
                          XrdAccCapability.hh XrdAccConfig.hh    XrdAccGroups.hh \
                          XrdAccAudit.hh     XrdAccAuthorize.hh  XrdAccPrivs.hh \
                          XrdOucError.hh     XrdOucHash.hh       XrdOucLogger.hh \
                          XrdOucXSLock.hh    XrdSecEntity.hh
	@echo Compiling XrdAccAccess.cc 
	$(ECHO)$(CC) -c $(CFLAGS) $(INCLUDE) -o $(OBJDIR)/XrdAccAccess.o XrdAccAccess.cc

$(OBJDIR)/XrdAccAudit.o: XrdAccAudit.cc XrdAccAudit.hh XrdOucError.hh
	@echo Compiling XrdAccAudit.cc 
	$(ECHO)$(CC) -c $(CFLAGS) $(INCLUDE) -o $(OBJDIR)/XrdAccAudit.o XrdAccAudit.cc

$(OBJDIR)/XrdAccAuthFile.o: XrdAccAuthFile.hh XrdAccAuthFile.cc \
                            XrdOucError.hh  XrdOucPthread.hh  XrdOucStream.hh \
                            XrdAccAuthDB.hh
	@echo Compiling XrdAccAuthFile.cc 
	$(ECHO)$(CC) -c $(CFLAGS) $(INCLUDE) -o $(OBJDIR)/XrdAccAuthFile.o XrdAccAuthFile.cc

$(OBJDIR)/XrdAccCapability.o: XrdAccCapability.hh XrdAccCapability.cc \
                              XrdAccPrivs.hh
	@echo Compiling XrdAccCapability.cc 
	$(ECHO)$(CC) -c $(CFLAGS) $(INCLUDE) -o $(OBJDIR)/XrdAccCapability.o XrdAccCapability.cc

$(OBJDIR)/XrdAccConfig.o: XrdAccConfig.hh     XrdAccConfig.cc \
                          XrdAccAccess.hh     XrdAccAudit.hh  XrdAccAuthDB.hh \
                          XrdAccCapability.hh XrdAccGroups.hh XrdOucLock.hh \
                          XrdOuca2x.hh        XrdOucError.hh  XrdOucHash.hh \
                          XrdOucPthread.hh    XrdOucStream.hh XrdOucStream.hh
	@echo Compiling XrdAccConfig.cc 
	$(ECHO)$(CC) -c $(CFLAGS) $(INCLUDE) -o $(OBJDIR)/XrdAccConfig.o XrdAccConfig.cc

$(OBJDIR)/XrdAccGroups.o: XrdAccGroups.hh     XrdAccGroups.cc \
                          XrdAccCapability.hh XrdAccPrivs.hh \
                          XrdOucHash.hh       XrdOucPthread.hh
	@echo Compiling XrdAccGroups.cc 
	$(ECHO)$(CC) -c $(CFLAGS) $(INCLUDE) -o $(OBJDIR)/XrdAccGroups.o XrdAccGroups.cc

$(OBJDIR)/XrdAccTest.o: XrdAccAuthorize.hh XrdAccConfig.hh XrdAccGroups.hh \
                        XrdAccPrivs.hh     XrdOucStream.hh XrdOucError.hh \
                        XrdOucLogger.hh    XrdAccTest.cc   XrdSecEntity.hh
	@echo Compiling XrdAccTest.cc 
	$(ECHO)$(CC) -c $(CFLAGS) $(INCLUDE) -o $(OBJDIR)/XrdAccTest.o XrdAccTest.cc
