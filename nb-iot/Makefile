TARGET = iota-nb-iot
MAP_FILE = iota-nb-iot.map

INCLUDES = \
	-Imbed-os \
	-Imbed-os/drivers \
	-Imbed-os/features \
	-Imbed-os/features/netsocket \
	-Imbed-os/platform \
	-Iublox-cellular-base \
	-Iublox-cellular-base-n2xx \
	-Iublox-at-cellular-interface \
	-Iublox-at-cellular-interface-n2xx \

LIBRARIES =

LINUX_LIBRARIES = -ljson

BUILD_FLAGS = \
	-MMD -DMDMUARTFILE=\"/dev/ttyUSB0\" \
	-DMDMTXD=0 -DMDMRXD=0 $(INCLUDES) -std=c++0x \

BUILD_FLAGS += -g

CC := $(TOOLCHAIN_PREFIX)$(CC)
CXX := $(TOOLCHAIN_PREFIX)$(CXX)

OBJS = \
	main.o \
	json.o \
	ublox-cellular-base/UbloxCellularBase.o \
	ublox-cellular-base-n2xx/UbloxCellularBaseN2xx.o \
	ublox-at-cellular-interface/UbloxATCellularInterface.o \
	ublox-at-cellular-interface-n2xx/UbloxATCellularInterfaceN2xx.o \
	mbed-os/drivers/SerialBase.o \
	mbed-os/drivers/Timer.o \
	mbed-os/drivers/UARTSerial.o \
	mbed-os/features/netsocket/NetworkInterface.o \
	mbed-os/features/netsocket/NetworkStack.o \
	mbed-os/features/netsocket/Socket.o \
	mbed-os/features/netsocket/SocketAddress.o \
	mbed-os/features/netsocket/UDPSocket.o \
	mbed-os/platform/ATCmdParser.o \
	mbed-os/platform/FileHandle.o \
	mbed-os/platform/mbed_poll.o \

DEPS = ${OBJS:.o=.d}

CFLAGS = $(BUILD_FLAGS)
CXXFLAGS = $(BUILD_FLAGS)

linux:	$(TARGET)
linux:	LIBRARIES += $(LINUX_LIBRARIES)

LINUX_SOCK_OBJS = \
	linuxSocket.o \
	json.o \

linux_socket:	iota-linux-socket
linux_socket:	LIBRARIES += $(LINUX_LIBRARIES)

all: $(TARGET)

$(TARGET): $(OBJS)
	@$(CXX) -Wl,--check-sections -Wl,--gc-sections \
		-Wl,--unresolved-symbols=report-all -Wl,--warn-common \
		-Wl,-Map,$(MAP_FILE) $(OBJS) $(LDFLAGS) $(LIBRARIES) -o $(TARGET)

iota-linux-socket: $(LINUX_SOCK_OBJS)
	@$(CXX) -Wl,--check-sections -Wl,--gc-sections \
		-Wl,--unresolved-symbols=report-all -Wl,--warn-common \
		-Wl,-Map,iota-linux-socket.map $(LINUX_SOCK_OBJS) $(LDFLAGS) \
		$(LIBRARIES) -o iota-linux-socket

clean:
	rm -f $(OBJS) $(DEPS) $(TARGET) *.map

-include ${DEPS}
