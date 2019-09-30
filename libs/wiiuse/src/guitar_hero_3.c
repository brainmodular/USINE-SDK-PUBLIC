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
 *	@brief Guitar Hero 3 expansion device.
 */

#include "guitar_hero_3.h"

#include "dynamics.h"                   /* for calc_joystick_state */
#include "events.h"                     /* for handshake_expansion */

#include <stdlib.h>                     /* for malloc */
#include <string.h>                     /* for memset */

static void guitar_hero_3_pressed_buttons(struct guitar_hero_3_t* gh3, short now);

/**
 *	@brief Handle the handshake data from the guitar.
 *
 *	@param cc		A pointer to a classic_ctrl_t structure.
 *	@param data		The data read in from the device.
 *	@param len		The length of the data block, in bytes.
 *
 *	@return	Returns 1 if handshake was successful, 0 if not.
 */
int guitar_hero_3_handshake(struct wiimote_t* wm, struct guitar_hero_3_t* gh3, byte* data, unsigned short len) {

	/*
	 *	The good fellows that made the Guitar Hero 3 controller
	 *	failed to factory calibrate the devices.  There is no
	 *	calibration data on the device.
	 */

	gh3->btns = 0;
	gh3->btns_held = 0;
	gh3->btns_released = 0;
	gh3->whammy_bar = 0.0f;

	/*
	TODO: If we're not using anything from calibration data, why are we
	even bothering here?
	*/
	if (data[0] == 0xFF) {
		/*
		 *	Sometimes the data returned here is not correct.
		 *	This might happen because the wiimote is lagging
		 *	behind our initialization sequence.
		 *	To fix this just request the handshake again.
		 *
		 *	Other times it's just the first 16 bytes are 0xFF,
		 *	but since the next 16 bytes are the same, just use
		 *	those.
		 */
		if (data[16] == 0xFF) {
			/* get the calibration data */
			byte* handshake_buf = malloc(EXP_HANDSHAKE_LEN * sizeof(byte));

			WIIUSE_DEBUG("Guitar Hero 3 handshake appears invalid, trying again.");
			wiiuse_read_data_cb(wm, handshake_expansion, handshake_buf, WM_EXP_MEM_CALIBR, EXP_HANDSHAKE_LEN);

			return 0;
		} else {
			data += 16;
		}
	}

	/* joystick stuff */
	gh3->js.max.x = GUITAR_HERO_3_JS_MAX_X;
	gh3->js.min.x = GUITAR_HERO_3_JS_MIN_X;
	gh3->js.center.x = GUITAR_HERO_3_JS_CENTER_X;
	gh3->js.max.y = GUITAR_HERO_3_JS_MAX_Y;
	gh3->js.min.y = GUITAR_HERO_3_JS_MIN_Y;
	gh3->js.center.y = GUITAR_HERO_3_JS_CENTER_Y;

	/* handshake done */
	wm->exp.type = EXP_GUITAR_HERO_3;

#ifdef WIIUSE_WIN32
	wm->timeout = WIIMOTE_DEFAULT_TIMEOUT;
#endif

	return 1;
}


/**
 *	@brief The guitar disconnected.
 *
 *	@param cc		A pointer to a classic_ctrl_t structure.
 */
void guitar_hero_3_disconnected(struct guitar_hero_3_t* gh3) {
	memset(gh3, 0, sizeof(struct guitar_hero_3_t));
}



/**
 *	@brief Handle guitar event.
 *
 *	@param cc		A pointer to a classic_ctrl_t structure.
 *	@param msg		The message specified in the event packet.
 */
void guitar_hero_3_event(struct guitar_hero_3_t* gh3, byte* msg) {

	guitar_hero_3_pressed_buttons(gh3, from_big_endian_uint16_t(msg + 4));

	/* whammy bar */
	gh3->whammy_bar = (msg[3] - GUITAR_HERO_3_WHAMMY_BAR_MIN) / (float)(GUITAR_HERO_3_WHAMMY_BAR_MAX - GUITAR_HERO_3_WHAMMY_BAR_MIN);

	/* joy stick */
	calc_joystick_state(&gh3->js, msg[0], msg[1]);
}


/**
 *	@brief Find what buttons are pressed.
 *
 *	@param cc		A pointer to a classic_ctrl_t structure.
 *	@param msg		The message byte specified in the event packet.
 */
static void guitar_hero_3_pressed_buttons(struct guitar_hero_3_t* gh3, short now) {
	/* message is inverted (0 is active, 1 is inactive) */
	now = ~now & GUITAR_HERO_3_BUTTON_ALL;

	/* pressed now & were pressed, then held */
	gh3->btns_held = (now & gh3->btns);

	/* were pressed or were held & not pressed now, then released */
	gh3->btns_released = ((gh3->btns | gh3->btns_held) & ~now);

	/* buttons pressed now */
	gh3->btns = now;
}
