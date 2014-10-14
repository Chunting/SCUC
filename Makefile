#############################################################################
# Makefile for building: Thermal_31BUS
# Project:  ../Project/Thermal_31BUS
# Author: Chunting
# Date: 7/26/2014

#############################################################################

MAKEFILE      = Makefile
SYSTEM     		= x86-64_linux
LIBFORMAT  		= static_pic
CPLEXDIR     	= /home/azureuser/Chunting/CPLEX/cplex
CONCERTDIR   	= /home/azureuser/Chunting/CPLEX/concert


# ---------------------------------------------------------------------
# Link options and libraries
# ---------------------------------------------------------------------

CPLEXBINDIR   = $(CPLEXDIR)/bin/$(SYSTEM)
CPLEXLIBDIR   = $(CPLEXDIR)/lib/$(SYSTEM)/$(LIBFORMAT)
CONCERTLIBDIR = $(CONCERTDIR)/lib/$(SYSTEM)/$(LIBFORMAT)

CPLEXFLAGS  = -I$(CPLEXDIR)/include -I$(CONCERTDIR)/include -L$(CPLEXLIBDIR) -lilocplex -lcplex -L$(CONCERTLIBDIR) -lconcert -lm -m64 -lpthread

####### Compiler, tools and options

CC            = g++
CFLAGS        = -O2 -Wall -ansi -pedantic -DIL_STD
DEBUG         = -pg -g -Wall -ansi -pedantic -DIL_STD
OBJECTS       = SCUC.o

exec :	$(OBJECTS)
	$(CC) $(CFLAGS) -o exe $(OBJECTS) $(CPLEXFLAGS)
.cpp.o :
	$(CC) $(CFLAGS) $(CPLEXFLAGS) -c $< -o $@
clean :
	rm -f $(OBJECTS) exe *.lp *.o  check.dat resultpp.dat
	
