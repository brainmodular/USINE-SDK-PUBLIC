#ifndef _PRO_DRIVER_H
#define _PRO_DRIVER_H


#include <stdlib.h>
#include <stdio.h>
//#include <windows.h>
//#include <process.h>
#include <assert.h>
//#include <tchar.h>
#include <time.h>
//#include <conio.h>
#include "Ftd2xx.h"

//#ifdef _MSC_VER
//	#include "ms_stdint.h"
//#else
	#include <stdint.h>
//#endif

// Enttec Pro MK2 specific definitions
// THE API Key is LSB First: so if it says 11223344 .. define it as ... 44,33,22,11
unsigned char APIKey[]					= {0xAD, 0x88, 0xD0, 0xC8};
#define SET_PORT_ASSIGNMENT_LABEL	    0xCB
#define SEND_DMX_PORT2					0xA9
#define RECEIVE_DMX_PORT2				0
#define SEND_MIDI_PORT					0
#define RECEIVE_MIDI_PORT				0


// Enttec Pro definitions
#define GET_WIDGET_PARAMS		3
#define GET_WIDGET_PARAMS_REPLY 3
#define SET_WIDGET_PARAMS		4
#define RECEIVE_DMX_PORT1		5
#define SEND_DMX_PORT1			6
#define SEND_DMX_RDM_TX			7
#define RECEIVE_DMX_ON_CHANGE	8
#define RECEIVED_DMX_COS_TYPE	9
#define GET_WIDGET_SN			10
#define SET_API_KEY_LABEL				13
#define HARDWARE_VERSION_LABEL			14


#define ONE_BYTE	1
#define DMX_START_CODE 0x7E 
#define DMX_END_CODE 0xE7 
#define OFFSET 0xFF
#define DMX_HEADER_LENGTH 4
#define BYTE_LENGTH 8
#define HEADER_RDM_LABEL 5
#define NO_RESPONSE 0
#define DMX_PACKET_SIZE 512

#define RX_BUFFER_SIZE 40960
#define TX_BUFFER_SIZE 40960

/*
// Enttec Pro definitions
#define GET_WIDGET_PARAMS 3
#define GET_WIDGET_SN 10
#define GET_WIDGET_PARAMS_REPLY 3
#define SET_WIDGET_PARAMS 4
#define SET_DMX_RX_MODE 5
#define SET_DMX_TX_MODE 6
#define SEND_DMX_RDM_TX 7
#define RECEIVE_DMX_ON_CHANGE 8
#define RECEIVED_DMX_COS_TYPE 9
#define ONE_BYTE 1
#define DMX_START_CODE 0x7E 
#define DMX_END_CODE 0xE7 
#define OFFSET 0xFF
#define DMX_HEADER_LENGTH 4
#define BYTE_LENGTH 8
#define HEADER_RDM_LABEL 5
#define NO_RESPONSE 0
#define DMX_PACKET_SIZE 512
*/

#pragma pack(1)
typedef struct {
        unsigned char FirmwareLSB;
        unsigned char FirmwareMSB;
        unsigned char BreakTime;
        unsigned char MaBTime;
        unsigned char RefreshRate;
}DMXUSBPROParamsType;

typedef struct {
        unsigned char UserSizeLSB;
        unsigned char UserSizeMSB;
        unsigned char BreakTime;
        unsigned char MaBTime;
        unsigned char RefreshRate;
}DMXUSBPROSetParamsType;
#pragma pack()

struct ReceivedDmxCosStruct
{
	unsigned char start_changed_byte_number;
	unsigned char changed_byte_array[5];
	unsigned char changed_byte_data[40];
};

#define MAX_PROS 20
#define SEND_NOW 0
#define TRUE 1
#define FALSE 0
#define HEAD 0
#define IO_ERROR 9


FT_STATUS FTDI_SendData (FT_HANDLE handle, int label, unsigned char *data, int length)
{
	unsigned char end_code = DMX_END_CODE;
	FT_STATUS res = 0;
	DWORD bytes_to_write = length;
	DWORD bytes_written = 0;
	HANDLE event = NULL;
	int size=0;
	// Form Packet Header
	unsigned char header[DMX_HEADER_LENGTH];
	header[0] = DMX_START_CODE;
	header[1] = label;
	header[2] = length & OFFSET;
	header[3] = length >> BYTE_LENGTH;
	// Write The Header
	res = FT_Write(	handle, (unsigned char *) header, DMX_HEADER_LENGTH, &bytes_written);
	//if (bytes_written != DMX_HEADER_LENGTH) return  NO_RESPONSE;
	if (res != FT_OK) return  res;
	// Write The Data
	res = FT_Write(	handle, (unsigned char *) data, length, &bytes_written);
	//if (bytes_written != length) return  NO_RESPONSE;
	if (res != FT_OK) return  res;
	// Write End Code
	res = FT_Write(	handle, (unsigned char *) &end_code,ONE_BYTE, &bytes_written);
	//if (bytes_written != ONE_BYTE) return  NO_RESPONSE;
	
    return  res;
};

int FTDI_ReceiveData (FT_HANDLE handle, int label, unsigned char *data, unsigned int expected_length)
{
	FT_STATUS res = 0;
	DWORD length = 0;
	DWORD bytes_to_read = 1;
	DWORD bytes_read =0;
	unsigned char byte = 0;
	HANDLE event = NULL;
	char buffer[600];
	// Check for Start Code and matching Label
	while (byte != label)
	{
		while (byte != DMX_START_CODE)
		{
			res = FT_Read (handle, (unsigned char *)&byte, ONE_BYTE, &bytes_read);
			if(bytes_read== NO_RESPONSE) return  NO_RESPONSE;
		}
		res = FT_Read (handle, (unsigned char *)&byte, ONE_BYTE, &bytes_read);
		if (bytes_read== NO_RESPONSE) return  NO_RESPONSE;
	}
	// Read the rest of the Header Byte by Byte -- Get Length
	res = FT_Read (handle, (unsigned char *)&byte, ONE_BYTE, &bytes_read);
	if (bytes_read== NO_RESPONSE) return  NO_RESPONSE;

	length = byte;
	res = FT_Read(handle, (unsigned char *)&byte, ONE_BYTE, &bytes_read);
	if (res != FT_OK) return  NO_RESPONSE;

	length += ((uint32_t)byte) << BYTE_LENGTH;	
	// Check Length is not greater than allowed
	if (length > DMX_PACKET_SIZE)
		return  NO_RESPONSE;

	// Read the actual Response Data
	res = FT_Read (handle, buffer, length, &bytes_read);
	if(bytes_read != length) return  NO_RESPONSE;

	// Check The End Code
	res = FT_Read (handle, (unsigned char *)&byte, ONE_BYTE, &bytes_read);
	if (bytes_read== NO_RESPONSE) return  NO_RESPONSE;
	if (byte != DMX_END_CODE) return  NO_RESPONSE;

	// Copy The Data read to the buffer passed
	memcpy (data, buffer, expected_length);
	return TRUE;
};

void FTDI_PurgeBuffer (FT_HANDLE mHand)
{
	FT_Purge (mHand,FT_PURGE_TX);
	FT_Purge (mHand,FT_PURGE_RX);
};

//uint8_t FTDI_SendDataToPro(uint8_t label, unsigned char *data, uint32_t length);
//void* Get_Pro_Handle();
//uint8_t FTDI_ReceiveDataFromPro(uint8_t label, unsigned char *data, uint32_t expected_length);
//uint16_t FTDI_OpenDevice(int device_num, int* VersionMSB, int* VersionLSB,bool full_mode = true);
//int FTDI_ListDevices(uint64_t * Locations);
//void FTDI_ClosePort();
//void FTDI_PurgeBuffer();

#endif