# Arduino Makefile modified and cleaned up by Jordan Perr
# Original from Arduino.cc
#
#
# To Use this Makefile:
#   -Copy this makefile into your project's directory
#	  -set BUILD_TARGETS all of your project's files to BUILD_TARGETS
#	  -set TARGET to your main target file's name. (eg. main for main.cpp)
#	  -set INSTALL_DIR to the location of your arduino installation
#	  -set LOAD_PORT to the serial port you wish to download to. Use * as a wildcard
#   -set READ_PORT to the serial port you wish to read from (cat port)
#
#
#
# make
#   -builds into current directory, creating a TARGET.hex binary
#
# make upload
#   -uses avrdude to download TARGET.hex to PORT
#
# make clean
#   -removes all temp and binary files created during build
#
#

# project specific settings



BUILD_DIR=build
TARGET=$(BUILD_DIR)/ipodshield
OBJDIR=$(BUILD_DIR)
#MODULES=src/x.cpp
BUILD_TARGETS=src/main.cpp src/xm.cpp src/iap.cpp src/avr.cpp src/pc.cpp src/ArduinoSerial.cpp \
	src/PCSerial.cpp src/serports.cpp \
	${MODULES}
	
BUILD_INCLUDES=src/podshield.h src/podshieldconfig.h src/podshieldresources.h  
ARDUINO_DIR=./arduino-015/
ARDUINO_CUSTOM_DIR=./arduino-custom/
MEMCHECK_FILE=build/memcheck-in.txt
LOAD_PORT = /dev/ttyUSB0
READ_PORT = /dev/cu.usb*

# arduino model specific settings

UPLOAD_RATE = 57600 
AVRDUDE_PROGRAMMER = stk500v1
#MCU = atmega1280
CLOCK=16000000
###############################
##  don't go below this line ##
###############################

# Arduino Distrobution Settings
ARDUINO = $(CORE_DIR)/hardware/cores/$(CORE)
ARDUINO_LIBS = $(ARDUINO_DIR)/hardware/libraries
ARDUINO_CUSTOM_LIBS=$(ARDUINO_CUSTOM_DIR)/hardware/libraries
ARDUINO_LIB_TARGETS=$(ARDUINO_LIBS)/EEPROM/EEPROM.cpp
#AVR_TOOLS_PATH = $(INSTALL_DIR)/hardware/tools/avr/bin
AVR_TOOLS_PATH=/usr/bin
FORMAT = ihex

# DEFAULT
all: build avrsize memcheck

$(BUILD_DIR)/makdefines: src/util/makdefine.cpp src/podshieldconfig.h src/podshieldresources.h
	rm -rf $(BUILD_DIR)
	@mkdir -p $(BUILD_DIR)
	gcc -lstdc++ src/util/makdefine.cpp -o $(BUILD_DIR)/makdefines

-include $(BUILD_DIR)/makdefines.mak

CORE=arduino
CORE_DIR=$(ARDUINO_DIR)

ifdef STANDALONE_PC
	CORE=pc
	CORE_DIR=$(ARDUINO_CUSTOM_DIR)
endif
ifdef SHIELD_STANDALONE
	CORE=arduino-644p
	CORE_DIR=$(ARDUINO_CUSTOM_DIR)
endif

# Environment settings
ifdef STANDALONE_PC
CC = gcc
CXX = gcc -DWiring_h=1
EXTRA_LIB=-lrt
MCUARG=
SRC=
CXXSRC = $(BUILD_TARGETS) $(ARDUINO)/Print.cpp
else
CC = $(AVR_TOOLS_PATH)/avr-gcc
CXX = $(AVR_TOOLS_PATH)/avr-gcc
MCUARG="-mmcu=$(MCU)"
SRC =  $(ARDUINO)/pins_arduino.c $(ARDUINO)/wiring.c \
$(ARDUINO)/wiring_analog.c $(ARDUINO)/wiring_digital.c \
$(ARDUINO)/wiring_pulse.c \
$(ARDUINO)/wiring_shift.c $(ARDUINO)/WInterrupts.c
# removed wiring_serial.c
CXXSRC = $(BUILD_TARGETS) $(ARDUINO)/HardwareSerial.cpp $(ARDUINO)/WMath.cpp $(ARDUINO)/Print.cpp $(ARDUINO_LIB_TARGETS)
endif


OBJCOPY = $(AVR_TOOLS_PATH)/avr-objcopy
OBJDUMP = $(AVR_TOOLS_PATH)/avr-objdump
SIZE = $(AVR_TOOLS_PATH)/avr-size
NM = $(AVR_TOOLS_PATH)/avr-nm
#AVRDUDE = $(AVR_TOOLS_PATH)/avrdude
AVRDUDE=./tools/avrdude
REMOVE = rm -f
MV = mv -f


###### HELPFUL DEFINITIONS =====

# Compiler Flags
ALL_CFLAGS = -MD -c -g -Os -w -ffunction-sections -fdata-sections $(MCUARG) -DF_CPU=$(CLOCK)L -I$(ARDUINO) -I$(ARDUINO_LIBS)
ALL_CXXFLAGS = -MD -c -g -Os -w -fno-exceptions -ffunction-sections -fdata-sections $(MCUARG) -DF_CPU=$(CLOCK)L -I$(ARDUINO) -I$(ARDUINO_LIBS)

# Assembler Flags
ALL_ASFLAGS = -mmcu=$(MCU) -I. -x assembler-with-cpp

# Loader flags
AVRDUDE_FLAGS = -C ./tools/avrdude.conf -V -p $(MCU) -P $(LOAD_PORT) -c $(AVRDUDE_PROGRAMMER) -b $(UPLOAD_RATE) -D -U flash:w:$(TARGET).hex:i

# Define all object files.
OBJ = $(SRC:%.c=$(OBJDIR)/%.o) $(CXXSRC:%.cpp=$(OBJDIR)/%.o) $(ASRC:%.S=$(OBJDIR)/%.o)

###### ROUTINES #######




# READ
read:
	cat $(READ_PORT)

# UPLAOD  
load: upload
upload: $(TARGET).hex
	$(AVRDUDE) $(AVRDUDE_FLAGS)

# CLEAN
clean:
#	$(REMOVE) $(TARGET).hex $(TARGET).eep $(TARGET).cof $(TARGET).elf \
#	$(TARGET).map $(TARGET).sym $(TARGET).lss $(MEMCHECK_OUT_FILE)\
#	$(OBJ) $(SRC:%.c=$(OBJDIR)/%.s) $(SRC:%.c=$(OBJDIR)/%.d) $(CXXSRC:%.cpp=$(OBJDIR)/%.s) $(CXXSRC:%.cpp=$(OBJDIR)/%.d)
	rm -rf $(BUILD_DIR)

# BUILD
build: incmakdefines hex

incmakdefines: $(BUILD_DIR)/makdefines

$(BUILD_DIR)/makdefines.mak: $(BUILD_DIR)/makdefines
	@$(BUILD_DIR)/makdefines > $(BUILD_DIR)/makdefines.mak


# Transcode: create HEX file from ELF file.
hex: $(TARGET).elf
	$(OBJCOPY) -O $(FORMAT) -R .eeprom $(TARGET).elf $(TARGET).hex


# Link: create ELF output file from object files.
$(TARGET).elf: $(OBJ)
	$(CC) -Os -Wl,--gc-sections -mmcu=${MCU} -o $(TARGET).elf $(OBJ) -lm $(EXTRA_LIB)

#elf: $(TARGET).elf

avrsize: 
	$(AVR_TOOLS_PATH)/avr-size --format=avr --mcu=$(MCU) $(TARGET).elf
	

memcheck:
#	@gcc -DSHIELD_MEMCHECK -lstdc++ src/util/memcheck.cpp -o build/memcheck 
#	@$(AVR_TOOLS_PATH)/avr-size --format=avr --mcu=$(MCU) $(TARGET).elf > $(MEMCHECK_OUT_FILE)
#	@cat $(MEMCHECK_OUT_FILE)  | sed -n "s/.*:[^0-9]*\([0-9]*\).*%.*/\1/p" | build/memcheck

 
# Compile: create object files from C++ source files.
#.cpp.o: 
$(OBJDIR)/%.o: %.cpp
	@mkdir -p `dirname $@`
	$(CXX) -c $(ALL_CXXFLAGS) $< -o $@
	@sed -i 's,\($(OBJDIR)/$*\.o\)[ :]*\(.*\),$@ : $$\(wildcard \2\)\n\1 : \2,g' $(OBJDIR)/$*.d

# Compile: create object files from C source files.
#.c.o: 
$(OBJDIR)/%.o: %.c
	@mkdir -p `dirname $@`
	$(CC) -c $(ALL_CFLAGS) $< -o $@
	@sed -i 's,\($(OBJDIR)/$*\.o\)[ :]*\(.*\),$@ : $$\(wildcard \2\)\n\1 : \2,g' $(OBJDIR)/$*.d

-include $(SRC:%.c=$(BUILD_DIR)/%.d)
-include $(CXXSRC:%.cpp=$(BUILD_DIR)/%.d)

# Compile: create assembler files from C source files.
#.c.s:
#$(OBJDIR)/%.s: %.c
#	$(CC) -S $(ALL_CFLAGS) $< -o $@


# Assemble: create object files from assembler source files.
#.S.o:
$(OBJDIR)/%.o: %.S
	$(CC) -c $(ALL_ASFLAGS) $< -o $@
