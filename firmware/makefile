LDSCRIPT = linker.ld
BOOTUP = startup.o
MCUTYPE=__SAMD51G19A__

OBJS=$(BOOTUP) main.o

# Tools
CC=arm-none-eabi-gcc
LD=arm-none-eabi-gcc
AR=arm-none-eabi-ar
AS=arm-none-eabi-as

ELF=main.elf

LDFLAGS+= -T$(LDSCRIPT) -mthumb -mcpu=cortex-m0 -Wl,--gc-sections
CFLAGS+= -mcpu=cortex-m4 -mthumb -g
CFLAGS+= -I xdk-asf -Iinclude -Iinclude/cmsis -I .
CFLAGS+= -D$(MCUTYPE) -DDONT_USE_CMSIS_INIT

$(ELF): $(OBJS)
	$(LD) $(LDFLAGS) -o $@ $(OBJS) $(LDLIBS)

# compile and generate dependency info
%.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@

%.o: %.s
	$(AS) $< -o $@

clean:
	rm -f $(OBJS) $(OBJS:.o=.d) $(ELF) $(CLEANOTHER)

load:  $(ELF)
	gdb-multiarch --batch -iex "target extended-remote localhost:3333" -ex "load" -ex "monitor reset" $(ELF) 

# pull in dependencies
-include        $(OBJS:.o=.d)
