//
//  SIL-serial.c
//  MatrixPilot-SIL
//
//  Created by Ben Levitt on 2/10/13.
//  Copyright (c) 2013 MatrixPilot. All rights reserved.
//

#if (WIN == 1 || NIX == 1)

#include <stdio.h>
#include "SIL-udb.h"
#include "../../libUDB/libUDB.h"
//#include "../../libUDB/serialIO.h"
#include "../../libDCM/gpsParseCommon.h"
#include "../../MatrixPilot/MAVLink.h"
#include "UDBSocket.h"

UDBSocket gpsSocket;
UDBSocket telemetrySocket;

int32_t gpsRate = 0;
int32_t serialRate = 0;


#define BUFLEN 512


//////////////////////////////////////////////////////////
// GPS Serial input/output emulation
//////////////////////////////////////////////////////////

void udb_gps_set_rate(int32_t rate)
{
	gpsRate = rate;
}


boolean udb_gps_check_rate(int32_t rate)
{
	return (rate == gpsRate);
}

// Call this function to initiate sending data to the GPS
void udb_gps_start_sending_data(void)
{
	uint8_t buffer[BUFLEN];
	int16_t bytesWritten;
	int16_t c;
	int16_t pos=0;

	if (!gpsSocket) return;

	while (pos < BUFLEN && (c = udb_gps_callback_get_byte_to_send()) != -1) {
		buffer[pos++] = c;
	}
	
	bytesWritten = UDBSocket_write(gpsSocket, (uint8_t*)buffer, pos);
	
	if (bytesWritten < 0) {
		UDBSocket_close(gpsSocket);
		gpsSocket = NULL;
	}
}

//////////////////////////////////////////////////////////
// Telemetry Serial input/output emulation
//////////////////////////////////////////////////////////

void udb_serial_set_rate(int32_t rate)
{
	serialRate = rate;
}


boolean udb_serial_check_rate(int32_t rate)
{
	return (serialRate == rate);
}

void sil_telemetry_input(uint8_t* buffer, int32_t bytesRead)
{
	int16_t i;

	if (1) {
		for (i = 0; i < bytesRead; i++) {
//			udb_serial_callback_received_byte(buffer[i]);
			mavlink_callback_received_byte(buffer[i]);
		}
	}
}

// Call this function to initiate sending data to the serial port
void udb_serial_start_sending_data(void)
{
	int16_t c;
	int16_t pos = 0;

	while (pos < BUFLEN && (c = udb_serial_callback_get_byte_to_send()) != -1) {
//		buffer[pos++] = c;
	}
}

void mavlink_start_sending_data(void)
{
	uint8_t buffer[BUFLEN];
	int16_t bytesWritten;
	int16_t c;
	int16_t pos = 0;

	if (!telemetrySocket) return;

//#error here - this doesn't work when telemetry is UDB_EXTRA etc

//	while (pos < BUFLEN && (c = udb_serial_callback_get_byte_to_send()) != -1) {
	while (pos < BUFLEN && (c = mavlink_callback_get_byte_to_send()) != -1) {
		buffer[pos++] = c;
	}
	bytesWritten = UDBSocket_write(telemetrySocket, (uint8_t*)buffer, pos);
	if (bytesWritten == -1) {
		UDBSocket_close(telemetrySocket);
		telemetrySocket = NULL;
	}
}

#endif // (WIN == 1 || NIX == 1)
