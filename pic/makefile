DEVICE	= 18F2550
CC	= /opt/microchip/xc8/v1.38/bin/xc8 -Q --chip=$(DEVICE) -I.
DATE	= $(shell date '+_%y%m%d_%H%M%S')
TARGET	= target
HEADER  = app_device_keyboard.h system.h usb_config.h
SRC	= main.c app_device_keyboard.c system.c usb_descriptors.c usb/src/usb_device.c usb/src/usb_device_hid.c
OBJ	= $(SRC:.c=.p1)

help:
	@awk '/#[#]/{sub(":[^#]*", "\t\t");print $0;}' makefile

build:	$(TARGET).hex makefile.done		## build .hex

check:	poff.mdb	## check PICkit
	echo y |mdb poff.mdb

burn:	program
program:	build $(TARGET).mdb poff.mdb		## program a device.
	echo y |mdb $(TARGET).mdb

burnandoff:	program poff.mdb			## program and power-off
	echo y |mdb poff.mdb

$(TARGET).mdb:	makefile
	echo '# # # # this file is auto built by makefile.' >$@
	echo 'device pic$(DEVICE)' >>$@
	echo 'set AutoSelectMemRanges auto' >>$@
	echo 'set poweroptions.powerenable false' >>$@
	echo 'set voltagevalue 4.9' >>$@
	echo 'hwtool pickit3 -p' >>$@
	echo 'program $(TARGET).hex' >>$@
	echo 'sleep 1000' >>$@
	echo 'quit' >>$@
	@mkdir -p hist
	cp $@ hist/$@$(DATE)

poff.mdb:	makefile
	echo '# # # # this file is auto built by makefile.' >$@
	echo 'device pic$(DEVICE)' >>$@
	echo 'set AutoSelectMemRanges auto' >>$@
	echo 'set poweroptions.powerenable false' >>$@
	echo 'set voltagevalue 4.9' >>$@
	echo 'hwtool pickit3 -p' >>$@
	echo 'quit' >>$@
	@mkdir -p hist
	cp $@ hist/$@$(DATE)


$(TARGET).hex:	$(OBJ)
	$(CC) -M$(TARGET).map -O$(TARGET) $^
	@mkdir -p hist
	cp $@ hist/$@$(DATE)


%.p1: %.c $(HEADER)
	@mkdir -p hist/usb/src
	cp $< hist/$<$(DATE)
	$(CC) --pass1 -O$@ $<
  

%.done:	%
	@mkdir -p hist
	cp $< hist/$<$(DATE)
	touch $@


