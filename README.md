# IOTA Client Application with NB-IoT Connectivity

This application is a proof-of-concept implementation of an IOTA payment system where IOTA transactions are triggered from a device connected to the Internet via an NB-IoT network.

The system works with the NB-IoT device that talks via UDP to a proxy server, which acts as an IOTA client and executes the IOTA transactions triggered by the NB-IoT device.

## NB-IoT Device Application

Communication with the NB-IoT network is done by means of an u-blox SARA-N2 modem.
The NB-IoT application that implements the UDP client runs on Linux, and expects the modem serial interface to be available at the `/dev/ttyUSB0` path (defined in the application Makefile). In order to trigger an IOTA transfer of a given amount, the client sends a simple JSON-formatted message to a UDP server at a certain IP address; both the amount of the transfer and the server IP address can be specified as command line options.

The format of the command line is as below:

`iota-nb-iot [-i <IOTA amount>] [-s <server IP address>]`

The application has been developed using code available at
<https://os.mbed.com/teams/ublox/code/ublox-at-cellular-interface/>,
<https://os.mbed.com/teams/ublox/code/ublox-cellular-base/>,
<https://os.mbed.com/teams/ublox/code/ublox-at-cellular-interface-n2xx/>,
<https://os.mbed.com/teams/ublox/code/ublox-cellular-base-n2xx/>, and
<https://github.com/ARMmbed/mbed-os>. It uses the JSON library at
<https://github.com/json-c/json-c>, which should normally be
installed by default in any modern Linux distribution.

This code has been tested with an u-blox SARA-N211 EVK running firmware version V100R100C10B656, connected to the Deutsche Telekom NB-IoT network.

## UDP Proxy Application

The UDP server application is written in Node.js; it implements an IOTA client by using the IOTA javascript library at <https://github.com/iotaledger/iota.lib.js>; upon receiving a message from the NB-IoT device on the UDP interface, this application executes an IOTA
transfer of the amount specified in the UDP message. The seed from which IOTA funds are taken and the address of the recipient of the transfers are hard-coded in the Javascript source.
