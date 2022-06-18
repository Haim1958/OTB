/**
 ******************************************************************************
 * @file    ip_allocation.c
 * @brief   IP Allocation implementation
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; LEGAL INFORMATION</center></h2>
 *
 *  RAFAEL ADVANCED DEFENCE SYSTEMS LTD. All rights reserved. Proprietary. CONFIDENTIAL.
 *  All Rights Reserved.
 *
 *  NOTICE:  All information contained herein is, and remains
 *  the property of RAFAEL ADVANCED DEFENCE SYSTEMS LTD. and its suppliers,
 *  if any.  The intellectual and technical concepts contained
 *  herein are proprietary to RAFAEL ADVANCED DEFENCE SYSTEMS LTD.
 *  and its suppliers and may be covered by U.S. and Foreign Patents,
 *  patents in process, and are protected by trade secret or copyright law.
 *  Dissemination of this information or reproduction of this material
 *  is strictly forbidden unless prior written permission is obtained
 *  from RAFAEL ADVANCED DEFENCE SYSTEMS LTD.
 *
 ******************************************************************************
 */

/* Includes */

#include <string.h>
#include "cmsis_os.h"
#include "UserLib.h"
#include "app.h"
#include "lwip.h"
#include "lwip/api.h"
#include "lwip/tcp.h"
#include "lwip/sockets.h"
#include "lwip/inet.h"


extern struct netif gnetif;


static int32_t CreateSocketAndConnect()
{
	int32_t optval;
	int32_t sock_err;
	int32_t socket_fd;
	struct sockaddr_in clientAddr;

	// New socket
	socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (socket_fd == -1)
	{
		printf("*** IP Allocation: *** socket failed\r\n");
		return socket_fd;
	}

	// In order to reuse previously badly closed socket
	optval = 1;
	sock_err = setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(uint32_t));
	if (sock_err != 0)
	{
		printf("*** IP Allocation: *** setsockopt SO_REUSEADDR failed\r\n");
		close(socket_fd);
		return -1;
	}

	// In order to deliver messages immediately (no buffering)
	optval = 1;
	sock_err = setsockopt(socket_fd, IPPROTO_TCP, TCP_NODELAY, &optval, sizeof(uint32_t));
	if (sock_err != 0)
	{
		printf("*** IP Allocation: *** setsockopt TCP_NODELAY failed\r\n");
		close(socket_fd);
		return -1;
	}

	memset(&clientAddr, 0, sizeof(struct sockaddr_in));
	clientAddr.sin_len = sizeof(clientAddr);
	clientAddr.sin_family = AF_INET;
	clientAddr.sin_port = PP_HTONS(TCP_OTBA_PORT_FOR_OTB);
	clientAddr.sin_addr.s_addr = inet_addr(IP_ADDRESS_OTBA);

	sock_err = connect(socket_fd, (struct sockaddr*)&clientAddr, sizeof(clientAddr));
	if (0 != sock_err)
	{
		printf("*** IP Allocation: *** Error connecting to OTBA\r\n");
		close(socket_fd);
		return -1;
	}
	printf("*** IP Allocation: *** connected to OTBA on socket %ld\r\n", socket_fd);
	return socket_fd;
}

/*
 * Returns 1 if it's the first location, 0 if it's the second location
 */
static uint8_t ReceiveLocation(const char *first_location, const char* second_location)
{
	int32_t socket_fd = -1, recv_data;
	char data_buffer[10];
	uint8_t bIsFirstLocation = 0xFF;

	while (-1 == socket_fd)
	{
		socket_fd = CreateSocketAndConnect();
		if (-1 == socket_fd)
			continue;
		recv_data = recv(socket_fd, data_buffer, sizeof(data_buffer), 0);
		if (0 == recv_data)
		{
			printf("*** IP Allocation: *** disconnected from OTBA\r\n");
			close(socket_fd);
			socket_fd = -1;
			continue;
		}
		if (0 == strcmp(data_buffer, first_location))
		{
			close(socket_fd);
			bIsFirstLocation = 1;
		}
		else if (0 == strcmp(data_buffer, second_location))
		{
			close(socket_fd);
			bIsFirstLocation = 0;
		}
		else
		{
			data_buffer[9] = '\0';
			close(socket_fd);
			socket_fd = -1;
			printf("*** IP Allocation: *** Unidentified message from server: %s\r\n", data_buffer);
		}
		printf("*** IP Allocation: *** received location %s\r\n", data_buffer);
	}
	return bIsFirstLocation;
}

uint8_t GetLocationAndSetIP(const char *first_location, const char* second_location,
		const char* first_ip, const char* second_ip)
{
	uint8_t bIsFirstLocation;
	const char* selectedIP;
	ip4_addr_t newIp, newMask, newGW;

	newMask.addr = inet_addr(IP_MASK);
	newGW.addr = inet_addr(IP_DEFAULT_GATEWAY);
//	bIsFirstLocation = ReceiveLocation(first_location, second_location);
	bIsFirstLocation = 1;
	selectedIP = bIsFirstLocation ? first_ip : second_ip;
	newIp.addr = inet_addr(selectedIP);
	netif_set_addr(&gnetif, &newIp, &newMask, &newGW);
	return bIsFirstLocation;
}
