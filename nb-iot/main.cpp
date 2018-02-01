/* IOTA NB-IoT client
 * Copyright (c) 2018 IOTA Foundation
 */

#include <platform.h>
#include <time.h>
#include <UbloxATCellularInterface.h>
#include <UbloxATCellularInterfaceN2xx.h>
#include <UDPSocket.h>

#include "json.h"

#define PIN "0000"

#define APN         "internet.nbiot.telekom.de"
#define USERNAME    NULL
#define PASSWORD    NULL

#define IOTA_NBIOT_SERVER	"104.27.157.197"

#define IOTA_NBIOT_AMOUNT	1

static int sockAddrCmp(SocketAddress &addr1, SocketAddress &addr2)
{
	nsapi_addr_t nsaddr1, nsaddr2;
	unsigned long ipv4_1, ipv4_2;

	if (addr1.get_port() != addr2.get_port()) {
		return 1;
	}
	nsaddr1 = addr1.get_addr();
	nsaddr2 = addr2.get_addr();
	if (nsaddr1.version != nsaddr2.version) {
		return 1;
	}
	memcpy(&ipv4_1, nsaddr1.bytes, sizeof(unsigned long));
	memcpy(&ipv4_2, nsaddr2.bytes, sizeof(unsigned long));
	return (ipv4_1 - ipv4_2);
}

static bool iotaNbiotTransfer(NetworkStack *interface, SocketAddress &server,
		unsigned int amount)
{
	UDPSocket socket;
	SocketAddress udpSenderAddress;
	char buf[1024];
	int msgLen;
	bool ret;

	server.set_port(14265);
	if (socket.open(interface) < 0) {
		printf("Cannot open UDP socket\n");
		return false;
	}
	msgLen = sprintf(buf, "{\"command\": \"transfer\", \"iota\": %d}", amount);
	if (socket.sendto(server, (void *)buf, msgLen) == msgLen) {
        socket.set_timeout(10000);
		msgLen = socket.recvfrom(&udpSenderAddress, buf, sizeof(buf) - 1);
		if ((msgLen > 0) && !sockAddrCmp(udpSenderAddress, server)) {
			printf("Received %d byte response from server %s\n",
					msgLen, udpSenderAddress.get_ip_address());
			buf[msgLen] = '\0';
			JsonObj jobj = jsonParse(buf);
			if (!jobj.valid) {
				printf("Cannot parse server response\n");
			}
			else {
				jsonAttr attr;

				if (jobj.getAttr("result", attr) &&
						(attr.type == jsonTypeString)) {
					if (!strcmp(attr.val.str, "ok")) {
						ret = true;
					}
					else {
						ret = false;
						if (!strcmp(attr.val.str, "error")) {
							if (jobj.getAttr("error", attr) &&
									(attr.type == jsonTypeString)) {
								printf("Error '%s' in server response\n",
										attr.val.str);
							}
							else {
								printf("Unknown error in server response\n");
							}
						}
						else {
							printf("Unknown result '%s' in server response\n",
									attr.val.str);
						}
					}
				}
				else {
					printf("Cannot get result from server response\n");
					ret = false;
				}
			}
		}
		else {
			printf("No response from server\n");
			return false;
		}
	}
	else {
		printf("Cannot send request to server\n");
		ret = false;
	}
	socket.close();
	return ret;
}

int main(int argc, char *argv[])
{
	UbloxATCellularInterfaceN2xx *interface =
			new UbloxATCellularInterfaceN2xx();
	SocketAddress udpServer;
	const char *serverAddr = IOTA_NBIOT_SERVER;
	int iotaAmount = IOTA_NBIOT_AMOUNT;
	int opt;

	while ((opt = getopt(argc, argv, "i:s:")) != -1) {
		switch (opt) {
		case 'i':
			iotaAmount = atoi(optarg);
			if (iotaAmount <= 0) {
				printf("Invalid IOTA amount\n");
				return 1;
			}
			break;
		case 's':
			serverAddr = optarg;
			break;
		case '?':
			printf("Usage: %s [-i <IOTA amount>] "
					"[-s <server IP address>]\n",
					argv[0]);
			return 1;
        }
	}
	if (!udpServer.set_ip_address(serverAddr)) {
		printf("Invalid server address\n");
		return 1;
	}
	printf("Initializing modem\n");
	if (interface->init(PIN)) {
		interface->set_credentials(APN, USERNAME, PASSWORD);
		printf("Connecting to packet network\n");
		while (interface->connect() != 0) {
			printf("Retrying\n");
        }
		printf("Transfering %d IOTA\n", iotaAmount);
		if (iotaNbiotTransfer(interface, udpServer, iotaAmount)) {
			printf("IOTA transfer executed\n");
		}
		interface->disconnect();
		interface->deinit();
	} else {
		printf("Unable to initialize interface.\n");
	}
	return 0;
}
