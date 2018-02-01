/* IOTA UDP client using standard network socket
 * Copyright (c) 2018 IOTA Foundation
 */

#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "json.h"

#define IOTA_NBIOT_SERVER	"localhost"

#define IOTA_NBIOT_AMOUNT	1

static int sockAddrCmp(struct sockaddr *addr1, struct sockaddr *addr2)
{
	struct sockaddr_in *ip1 = (struct sockaddr_in *)addr1;
	struct sockaddr_in *ip2 = (struct sockaddr_in *)addr2;

	if ((ip1->sin_family != ip2->sin_family) ||
			(ip1->sin_port != ip2->sin_port)) {
		return 1;
	}
	return (ip1->sin_addr.s_addr - ip2->sin_addr.s_addr);
}

static bool iotaUdpTransfer(int fd, struct sockaddr *server,
		unsigned int amount)
{
	struct sockaddr udpSenderAddress;
	char buf[1024];
	int msgLen;
	socklen_t socklen;
	bool ret;

	((struct sockaddr_in *)server)->sin_port = htons(14265);
	msgLen = sprintf(buf, "{\"command\": \"transfer\", \"iota\": %d}", amount);
	if (sendto(fd, (void *)buf, msgLen, 0, server, sizeof(*server)) == msgLen) {
		struct timeval tv;

		tv.tv_sec = 10;
		tv.tv_usec = 0;
		setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
		msgLen = recvfrom(fd, buf, sizeof(buf) - 1, 0, &udpSenderAddress,
				&socklen);
		if ((msgLen > 0) && !sockAddrCmp(&udpSenderAddress, server)) {
			printf("Received %d byte response from server\n", msgLen);
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
			ret = false;
		}
	}
	else {
		printf("Cannot send request to server\n");
		ret = false;
	}
	return ret;
}

int main()
{
	struct hostent *he;
	struct in_addr *addr_list;
	int fd;
	struct sockaddr udpServer;

	printf("Getting server IP address\n");
	if ((he = gethostbyname(IOTA_NBIOT_SERVER)) == NULL) {
		printf("Cannot resolve server IP address\n");
		return 0;
	}
	addr_list = *((struct in_addr **)he->h_addr_list);
	if (addr_list == NULL) {
		printf("Cannot resolve server IP address\n");
		return 0;
	} else {
		((struct sockaddr_in *)&udpServer)->sin_addr.s_addr =
				(*addr_list).s_addr;
	}
	fd = socket(AF_INET, SOCK_DGRAM, 0);
	if (fd < 0) {
		printf("Cannot create socket\n");
		return 0;
	}
	printf("Transfering IOTA\n");
	((struct sockaddr_in *)&udpServer)->sin_family = AF_INET;
	if (iotaUdpTransfer(fd, (struct sockaddr *)&udpServer, IOTA_NBIOT_AMOUNT)) {
		printf("IOTA transfer executed\n");
	}
	else {
		printf("Unable to transfer IOTA\n");
	}
	close(fd);
	return 0;
}
