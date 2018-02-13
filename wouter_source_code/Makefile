SHELL = /bin/csh

# invocation:
#   make
#   make Build


# Capitals   = output files, to be produced by this Makefile
# Lower case = input files, i.e. source code
SML   = Simul
sml   = simul.o
BLD   = Build
bld   = build.o
hdrs  = headers.H

# CPP   = g++
# OPT1  = -g -Wall -Wextra -D Trace
CPP   = icpc
OPT1  = -g -C -Wall -wd383,981,1572 -U Trace
#OPT1  = -O3 -U Trace

# remark  #383: value copied to temporary, reference to temporary used: i = max( imax, abs(i))
# remark  #981: operands are evaluated in unspecified order: cout << setw(10) << getal << etc
# remark #1572: floating-point equality and inequality comparisons are unreliable: if ( dble == 0. )

obj_sml =	read_field.o read_config.o read_control.o \
	write_config.o write_dcd.o \
	force.o
obj_bld =                write_config.o rnd.o \
                         read_field.o

inc =	defs.h

all:	$(SML) $(BLD)

$(SML):	$(hdrs) $(inc) $(obj_sml) $(sml)
	$(CPP) -o $(SML) $(sml) $(OPT1) $(obj_sml)

$(BLD):	$(hdrs) $(inc) $(obj_bld) $(bld)
	$(CPP) -o $(BLD) $(bld) $(OPT1) $(obj_bld)

$(hdrs):
	./Headers.make

.cpp.o:	$(inc)
	$(CPP) -c $(OPT1) $*.cpp
clean:
	\rm -f ${SML} ${BLD} $(sml) $(bld) $(obj_sml) $(obj_bld) *.H
