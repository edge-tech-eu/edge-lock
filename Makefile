# 20191108 version 1.2 : integrate git into makefile
#

# platform stuff 
# support for atmega328pb
# see http://distribute.atmel.no/tools/opensource/Atmel-AVR-GNU-Toolchain/3.6.1/
# download avr8-gnu-toolchain-osx-3.6.1.xxx-darwin.any.x86_64.tar.gz from http://distribute.atmel.no/tools/opensource/Atmel-AVR-GNU-Toolchain/3.6.1/
# new url: https://www.microchip.com/mplab/avr-support/avr-and-arm-toolchains-c-compilers
# download packs from http://packs.download.atmel.com
# mv xxx.apack to xxx.zip and unzip

ARCH := $(shell uname)
USER := $(shell whoami)

ifeq ($(USER),walter)

		PATH_TOOLCHAIN = /home/walter/Downloads/avr8-gnu-toolchain-linux_x86_64/bin
		PATH_PACK      = /home/walter/Downloads/frankatmel
		AVRDUDE_EXEC   = /usr/bin/avrdude

		# adafruit programmer
		# PROGRAMMER = usbtiny
		# open evse programmer
		PROGRAMMER = USBasp

else ifeq ($(USER),sven)
         
		PATH_TOOLCHAIN = /home/sven/AVR/avr8-gnu-toolchain-linux_x86_64/bin
		PATH_PACK      = /home/sven/AVR/Atmel
		AVRDUDE_EXEC   = /usr/bin/avrdude

		# adafruit programmer
		# PROGRAMMER = usbtiny
		# open evse programmer
		PROGRAMMER = USBasp
				
else ifeq ($(USER),frank)

		PATH_TOOLCHAIN = /Users/frank/Projects/atmega328pb/avr8-gnu-toolchain-darwin_x86_64/bin
		PATH_PACK      = /Users/frank/Projects/atmega328pb/Atmel.ATmega_DFP.1.2.209
		AVRDUDE_EXEC   = /Applications/Arduino.app/Contents/Java/hardware/tools/avr/bin/avrdude
		
		# adafruit programmer, sparkfun
		# PROGRAMMER = usbtiny
		
		# open evse programmer, baite
		PROGRAMMER = USBasp
		
		# STK500 programmer needs port specified, not sure how that works
		# pololu
		# PROGRAMMER  = STK500

endif

#
# attiny84a @ 8 mhz internal rc
#

# avrdude: safemode: Fuses OK (E:FF, H:DF, L:62)

# http://www.engbedded.com/fusecalc/

# efuse: 11111111 0xff
# hfuse: 11011111 0xdf
# lfuse: 01100010 0x62 = 8 mhz/8
# lfuse: 11100010 0xe2 = 8 mhz/1

# default:
# efuse: 0xff
# hfuse: 0xdf
# lfuse: 0x62 = 1 mhz

DEVICE    = attiny84
OBJECTS   = main.o board.o uart.o
FUSES     = -U lfuse:w:0xe2:m -U hfuse:w:0xdf:m -U efuse:w:0xff:m
CLOCK     = 8000000
READFUSES = -U efuse:r:efuse.hex:h -U lfuse:r:lfuse.hex:h -U hfuse:r:hfuse.hex:h

# for extra verbose: -v -v -v
AVRDUDE   = $(AVRDUDE_EXEC) -c $(PROGRAMMER) -C avrdude.conf -p $(DEVICE)

# compiler options:
# -Wall   warnings, all
# -Os     optimization for size
COMPILE = $(PATH_TOOLCHAIN)/avr-gcc -Wall -DF_CPU=$(CLOCK) -mmcu=$(DEVICE) -B $(PATH_PACK)/gcc/dev/attiny84/ -I $(PATH_PACK)/include/
OBJDUMP = $(PATH_TOOLCHAIN)/avr-objdump
OBJCOPY = $(PATH_TOOLCHAIN)/avr-objcopy

BUILD_NUMBER_FILE=build-number.txt
MYDATE=$(shell date +'%Y%m%d')


all:	main.hex main.lss

#NB build is made from flash, in the workflow dont call "make build"
build:
	@echo $$(date +'%Y%m%d-%T')   > BUILD_DATE_FILE
	@if ! test -f $(BUILD_NUMBER_FILE); then echo 0 > $(BUILD_NUMBER_FILE); fi
	@printf $$(($$(cat $(BUILD_NUMBER_FILE)) + 1)) > $(BUILD_NUMBER_FILE)
	@printf  "#define FW_BUILD  $$(cat $(BUILD_NUMBER_FILE))"  > build.h

commit: clean
	@echo "committing to local git"
	sleep 2
	git add *.c *.h Makefile
	git commit


push: 
	@echo "committing to remote git"
	sleep 2
	git push

release:
	@echo "did you do git tag vMAJOR.MINOR.0 -m 'yyyymmdd_description' ?"
	@echo "did you edit version.h by adding datum_description in same format as the to be added gittag"
	@echo "did you edit version.h by increasing FW_MINOR?, else hit cancel"
	@sleep 10
	echo "Reset Buildnumber"
	@echo 0 > $(BUILD_NUMBER_FILE); 
	sleep 5
	@make clean flash
	cp main.hex releases/main_$(MYDATE).hex
	git add releases/main_$(MYDATE).hex
	$(OBJDUMP) -t    $< > releases/SymbolTable_$(MYDATE)
	@echo "OK"


gitdifflocalmaster:
	git diff master origin/master                                          

.c.o:
	$(COMPILE) -c $< -o $@

.S.o:
	$(COMPILE) -x assembler-with-cpp -c $< -o $@

.c.s:
	$(COMPILE) -S $< -o $@

flash:	build all
	$(AVRDUDE) -U flash:w:main.hex:i

fuse:
	$(AVRDUDE) $(FUSES)

program:	
	$(AVRDUDE) -U flash:w:main.hex:i

readfuse:
	$(AVRDUDE) $(READFUSES)
	cat efuse.hex hfuse.hex lfuse.hex

clean:
	rm -f main.hex main.lss main.elf $(OBJECTS)
	
main.elf: $(OBJECTS)
	$(COMPILE) -o main.elf $(OBJECTS)

main.hex: main.elf
	rm -f main.hex
	$(OBJCOPY) -j .text -j .data -O ihex main.elf main.hex

%.lss: %.elf
	@echo
	$(OBJDUMP) -h -S $< > $@

cpp:
	$(COMPILE) -E main.c
