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
commands/r3commands.o\
procsr3.o

.c.s:
	$(CC) $(CFLAGS) -S -o $@ $<
.s.o:
	$(AS) $(ASFLAGS) -o $@ $<
.c.o:
	$(CC) $(CFLAGS) -I../../include -o $@ $<

all: R3.o

R3.o: $(OBJFILES)
	$(LD) -r -o R3.o $(OBJFILES)

clean:
	rm $(OBJFILES)
