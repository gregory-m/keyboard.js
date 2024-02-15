PRJ = keyboard_js
MCU = atmega32u4
CLK = 16000000

PRG = flip1
PRG_ISP = usbasp


# see https://eleccelerator.com/fusecalc/fusecalc.php?chip=atmega32u4
# LFU = 0x5E # Default vaule
LFU = 0xDE
HFU = 0xD9
EFU = 0xF3


SRC = $(wildcard *.c)
# where to look for external libraries (consisting of .c/.cpp files and .h files)
# e.g. EXT = ../../EyeToSee ../../YouSART
EXT =


INCLUDE := $(foreach dir, $(EXT), -I$(dir))

CFLAGS    = -Wall -Os -DF_CPU=$(CLK) -mmcu=$(MCU) $(INCLUDE)
CPPFLAGS =

BOOTLOADER = ATMega32U4-usbdevice_dfu-1_0_0.hex


AVRDUDE = avrdude -c $(PRG) -p $(MCU)
AVRDUDE_ISP = avrdude -c $(PRG_ISP) -p $(MCU)
OBJCOPY = avr-objcopy
OBJDUMP = avr-objdump
SIZE    = avr-size --format=avr --mcu=$(MCU)
CC      = avr-gcc

# generate list of objects
CFILES    = $(filter %.c, $(SRC))
EXTC     := $(foreach dir, $(EXT), $(wildcard $(dir)/*.c))
CPPFILES  = $(filter %.cpp, $(SRC))
EXTCPP   := $(foreach dir, $(EXT), $(wildcard $(dir)/*.cpp))
OBJ       = $(CFILES:.c=.o) $(EXTC:.c=.o) $(CPPFILES:.cpp=.o) $(EXTCPP:.cpp=.o)

# compile all files
all: $(PRJ).hex

prepare: flash_bootloader fuse

test:
	$(AVRDUDE) -v

flash: all
	$(AVRDUDE) -U flash:w:$(PRJ).hex:i

fuse:
	$(AVRDUDE_ISP) -U lfuse:w:$(LFU):m -U hfuse:w:$(HFU):m -U efuse:w:$(EFU):m

read_fuse:
	$(AVRDUDE_ISP) -U lfuse:r:-:h -U hfuse:r:-:h -U efuse:r:-:h

flash_bootloader:
	$(AVRDUDE_ISP) -U flash:w:$(BOOTLOADER):i

disasm: $(PRJ).elf
	$(OBJDUMP) -d $(PRJ).elf

clean:
	rm -f $(PRJ).hex *.elf *.o
	$(foreach dir, $(EXT), rm -f $(dir)/*.o;)

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

.cpp.o:
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

$(PRJ).elf: $(OBJ)
	$(CC) $(CFLAGS) -o $(PRJ).elf $(OBJ)

$(PRJ).hex: $(PRJ).elf
	rm -f $(PRJ).hex
	$(OBJCOPY) -j .text -j .data -O ihex $(PRJ).elf $(PRJ).hex
	$(SIZE) $(PRJ).elf
