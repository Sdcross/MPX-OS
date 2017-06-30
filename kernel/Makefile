#
# Makefile for MPX/Linux

AS	  = nasm
CC	  = i386-elf-gcc
CFLAGS  = -Wall -Wextra -Werror -nostartfiles -nostdlib -nodefaultlibs -ffreestanding -g -c
LD	  = i386-elf-ld
LDFLAGS = 
ASFLAGS = -f elf -g

OBJFILES =\
core/comHandler.o\
core/commands.o\
core/gdt.o\
core/idt.o\
core/interrupts.o\
core/io.o\
core/irq.o\
core/kmain.o\
core/pcb.o\
core/serial.o\
core/system.o\
core/tables.o\
core/queue.o\
mem/heap.o\
mem/memoryControl.o\
mem/paging.o


.s.o:
	$(AS) $(ASFLAGS) -o $@ $<
.c.o:
	$(CC) $(CFLAGS) -I../include -o $@ $<

all: kernel.o

kernel.o: $(OBJFILES)
	$(LD) $(LDFLAGS) -r -o kernel.o $(OBJFILES)

clean:
	rm $(OBJFILES)
