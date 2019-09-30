/*
 *	wiiuse
 *
 *	Written By:
 *		Michael Laforest	< para >
 *		Email: < thepara (--AT--) g m a i l [--DOT--] com >
 *
 *	Copyright 2006-2007
 *
 *	This file is part of wiiuse.
 *
 *	This program is free software; you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation; either version 3 of the License, or
 *	(at your option) any later version.
 *
 *	This program is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *	$Header$
 *
 */

/**
 *	@file
 *	@brief Handles device I/O for Windows.
 */


#include "io.h"
#include "events.h"
#include "os.h"

#include "hidapi.h"

int wiiuse_os_find(struct wiimote_t** wm, int max_wiimotes, int timeout) {
	struct hid_device_info* device_info;
	int found = 0;


    device_info = hid_enumerate(WM_VENDOR_ID, WM_PRODUCT_ID);

     while (device_info) {
        hid_device * device = hid_open(device_info->vendor_id, device_info->product_id, device_info->serial_number);

        if (!device)
        {
           
        }
		 
		 
#if defined (WIIUSE_MAC)	 
		 wm[found]->objc_wm = device;
#elif defined (WIIUSE_WIN32)
		hid_set_nonblocking (device, 1); 
		wm[found]->dev_handle = device;
        wm[found]->hid_overlap.hEvent = CreateEvent(NULL, 1, 1, "");
		wm[found]->hid_overlap.Offset = 0;
		wm[found]->hid_overlap.OffsetHigh = 0;
#endif
		 
		WIIMOTE_ENABLE_STATE(wm[found], WIIMOTE_STATE_DEV_FOUND);
		WIIMOTE_ENABLE_STATE(wm[found], WIIMOTE_STATE_CONNECTED);

		/* try to set the output report to see if the device is actually connected */
		if (!wiiuse_set_report_type(wm[found])) {
			WIIMOTE_DISABLE_STATE(wm[found], WIIMOTE_STATE_CONNECTED);
			continue;
		}

		/* do the handshake */
		wiiuse_handshake(wm[found], NULL, 0);

		WIIUSE_INFO("Connected to wiimote [id %i].", wm[found]->unid);

		++found;
		if (found >= max_wiimotes) {
			break;
		}

        device_info = device_info->next;
	}
    
	hid_free_enumeration(device_info);

	return found;
}


int wiiuse_os_connect(struct wiimote_t** wm, int wiimotes) {
	int connected = 0;
	int i = 0;

	for (; i < wiimotes; ++i) {
		if (!wm[i]) {
			continue;
		}
		if (WIIMOTE_IS_SET(wm[i], WIIMOTE_STATE_CONNECTED)) {
			++connected;
		}
	}

	return connected;
}


void wiiuse_os_disconnect(struct wiimote_t* wm) {
	if (!wm || WIIMOTE_IS_CONNECTED(wm)) {
		return;
	}

#if defined (WIIUSE_MAC)	
    hid_close(( hid_device*)wm->objc_wm); 
#elif defined (WIIUSE_WIN32)
    hid_close(( hid_device*)wm->dev_handle);
	wm->dev_handle = 0;
	ResetEvent(&wm->hid_overlap);
#endif
	
	wm->event = WIIUSE_NONE;

	WIIMOTE_DISABLE_STATE(wm, WIIMOTE_STATE_CONNECTED);
	WIIMOTE_DISABLE_STATE(wm, WIIMOTE_STATE_HANDSHAKE);
}


int wiiuse_os_poll(struct wiimote_t** wm, int wiimotes) {
	int i;
	byte read_buffer[MAX_PAYLOAD];
	int evnt = 0;

	if (!wm) {
		return 0;
	}

	for (i = 0; i < wiimotes; ++i) {
		wm[i]->event = WIIUSE_NONE;

		/* clear out the buffer */
		memset(read_buffer, 0, sizeof(read_buffer));
		/* read */
		if (wiiuse_os_read(wm[i], read_buffer, sizeof(read_buffer))) {
			/* propagate the event */
			propagate_event(wm[i], read_buffer[0], read_buffer + 1);
			evnt += (wm[i]->event != WIIUSE_NONE);
		} else {
			/* send out any waiting writes */
			wiiuse_send_next_pending_write_request(wm[i]);
			idle_cycle(wm[i]);
		}
	}

	return evnt;
}

int wiiuse_os_read(struct wiimote_t* wm, byte* buf, int len) {
    hid_device* device;

	if (!wm || !WIIMOTE_IS_CONNECTED(wm)) {
		return 0;
	}
	
#if defined (WIIUSE_MAC)	
    device = (hid_device*)wm->objc_wm; 
#elif defined (WIIUSE_WIN32)
    device = (hid_device*)wm->dev_handle;
#endif
	
    if (hid_read(device, buf, len) < 0) {			
        WIIUSE_WARNING("A wait error occurred on reading from wiimote %i.", wm->unid);
		return 0;

		/* log the received data */
//#ifdef WITH_WIIUSE_DEBUG
//		{
//			int i;
//			printf("[DEBUG] (id %i) RECV: (%.2x) ", wm->unid, buf[0]);
//			for (i = 1; i < b; i++) {
//				printf("%.2x ", buf[i]);
//			}
//			printf("\n");
//		}
//#endif
	}
	
#if defined (WIIUSE_MAC)	
    //
#elif defined (WIIUSE_WIN32)
	ResetEvent(wm->hid_overlap.hEvent);
#endif
	
	return 1;
}


int wiiuse_os_write(struct wiimote_t* wm, byte report_type, byte* buf, int len) {
	byte write_buffer[MAX_PAYLOAD];
    int result;
    hid_device* device;

	if (!wm || !WIIMOTE_IS_CONNECTED(wm)) {
		return 0;
	}

	write_buffer[0] = report_type;
	memcpy(write_buffer + 1, buf, len);
	
#if defined (WIIUSE_MAC)	
    device = (hid_device*)wm->objc_wm; 
#elif defined (WIIUSE_WIN32)
    device = (hid_device*)wm->dev_handle;
#endif
    result = hid_write(device, write_buffer, len+1);
    return (result >= 0);
}

void wiiuse_init_platform_fields(struct wiimote_t* wm) {
	
#if defined (WIIUSE_MAC)	
    //
#elif defined (WIIUSE_WIN32)
	wm->dev_handle = 0;
	wm->stack = WIIUSE_STACK_UNKNOWN;
	wm->normal_timeout = WIIMOTE_DEFAULT_TIMEOUT;
	wm->exp_timeout = WIIMOTE_EXP_TIMEOUT;
	wm->timeout = wm->normal_timeout;
#endif

    hid_init();
}

void wiiuse_cleanup_platform_fields(struct wiimote_t* wm) {
	
#if defined (WIIUSE_MAC)	
    //
#elif defined (WIIUSE_WIN32)
	wm->dev_handle = 0;
#endif
	
    hid_exit();
}

//#endif /* ifdef WIIUSE_WIN32 */
