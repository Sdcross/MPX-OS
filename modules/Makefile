#
# Makefile for MPX Modules

AS  = nasm
CC      = i386-elf-gcc
CFLAGS  = -Wall -Wextra -Werror -nostartfiles -nostdlib -nodefaultlibs -ffreestanding -g -c
LD      = i386-elf-ld
LDFLAGS = 
ASFLAGS = -f elf -g

# add new files as you create them.
# I'll start you off with just mpx_supt.o
OBJFILES =\
mpx_supt.o\
R2/R2.o\
R3/R3.o\
R5/R5.o
.c.s:
	$(CC) $(CFLAGS) -S -o $@ $<
.s.o:
	$(AS) $(ASFLAGS) -o $@ $<
.c.o:
	$(CC) $(CFLAGS) -I../include -o $@ $<

all: modules.o

.PHONY : R2/R2.o
R2/R2.o:
	(cd R2 ; make)

.PHONY : R3/R3.o
R3/R3.o:
	(cd R3 ; make)

.PHONY : R5/R5.o
R5/R5.o:
	(cd R5 ; make)

modules.o: $(OBJFILES)
	$(LD) -r -o modules.o $(OBJFILES)

clean:
	(cd R2 ; make clean)
	rm $(OBJFILES)
