# Automatically generated by qmake (1.07a) Sun Jun 26 02:03:47 2005
# Adjusted by hand to include $ROOTSYS/include/rootcint.pri file

TEMPLATE = app thread
CONFIG -= moc
INCLUDEPATH += .
INCLUDEPATH += $(ROOTSYS)/include/

# include "by hand" the qmake include file from
# ROOT distribution to define
#  1. include path to the ROOT system header files
#  2. the list of the ROOT shared libraries to link
#     Qt application against of
#  3. qmake rules to generate ROOT/Cint dictionaries

#include("$(ROOTSYS)/include/rootcint.pri")

# Input
SOURCES += HelloCanvas.cxx