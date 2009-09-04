/*
 * IPodEmulator.cpp
 *
 *  Created on: Aug 24, 2009
 *      Author: keary
 */

/* Modified from Rockbox for use as ipod emulation layer on top of XM radio */
/***************************************************************************
 *             __________               __   ___.
 *   Open      \______   \ ____   ____ |  | _\_ |__   _______  ___
 *   Source     |       _//  _ \_/ ___\|  |/ /| __ \ /  _ \  \/  /
 *   Jukebox    |    |   (  <_> )  \___|    < | \_\ (  <_> > <  <
 *   Firmware   |____|_  /\____/ \___  >__|_ \|___  /\____/__/\_ \
 *                     \/            \/     \/    \/            \/
 * $Id: iap.c 17400 2008-05-07 14:30:29Z xxxxxx $
 *
 * Copyright (C) 2002 by Alan Korr & Nick Robinson
 *
 * All files in this archive are subject to the GNU General Public License.
 * See the file COPYING in the source tree root for full license agreement.
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
 * KIND, either express or implied.
 *
 ****************************************************************************/

#include "podshield.h"
#include "IPodEmulator.h"
#include "RunnableManager.h"

static PROGMEM unsigned char nak_data[] = {0x04, 0x00, 0x01, 0x04, 0x00, 0x00};
#define IPOD_NAK() do { PT_WAIT_UNTIL(sender.isAvailable()); unsigned char *data = sender.getBuffer(nak_data, sizeof(nak_data)); \
	data[4] = serbuf[2]; data[5] = serbuf[3]; sender.send(); } while(0);

//IPodEmulator emulatorModule(true);

IPodEmulator::IPodEmulator(PodSerial *pserial, long baud, bool autoRegister, bool startSuspended) : Module(autoRegister, startSuspended ) {
	baudRate = baud;
	Serial = pserial;
	iap_reset();
}

void IPodEmulator::initModule() {
	Serial->setBaud(baudRate);
	iap_reset();
	sender.Serial = Serial;
	getRunnableManager().addRunnable(&sender);
}

void IPodEmulator::iap_reset() {
    iap_pollenabled = false;
    iap_updateflag = false;
    iap_changedctr = 0;
    nextPoll = 0;
	isFakePosition = false;
	nochangeTicks = 0;
	changedChannel = false;
	lastPlaylistPos = -1;
	filterPlaylist = 0;
	playlist = 0;

	last_x = 0;
	newpkt = true;
	chksum = 0;
	serbuf_i = 0;
}

void IPodEmulator::iap_notify() {
	/* called by playback when the next track starts */
	    iap_changedctr = 1;
}
bool IPodEmulator::iap_getc(unsigned char x)
{
    //static unsigned char last_x = 0;
    //static bool newpkt = true;
    //static unsigned char chksum = 0;

	bool handlePacket = false;
    /* Restart if the sync word is seen */
    if(x == 0x55 && last_x == 0xff/* && newpkt*/)
    {
        serbuf[0] = 0;
        serbuf_i = 0;
        chksum = 0;
        newpkt = false;
    }
    else
    {
        if(serbuf_i >= RX_BUFLEN)
            serbuf_i = 0;

        serbuf[serbuf_i++] = x;
        chksum += x;
    }
    last_x = x;

    /* Broadcast to queue if we have a complete message */
    if(serbuf_i && (serbuf_i == serbuf[0]+2))
    {
    	//xmlog_n("ipodCmd: ");
    	//for (int j=0;j<serbuf_i;j++)
    	//	xmlog_n("%02x ", serbuf[j]);
    	//xmlog("");
        serbuf_i = 0;
        newpkt = true;
        if(chksum == 0) {
        	handlePacket = true;
        	//iap_handlepkt();
            //queue_post(&button_queue, SYS_IAP_HANDLEPKT, 0);
        }
    }

    return handlePacket;
}

int IPodEmulator::getVirtualCurrentPlaylistPosition(bool change) {
	if (!changedChannel && !change && lastPlaylistPos != -1)
		return lastPlaylistPos;

	changedChannel = false;
	int p = getCurrentPlaylistPosition();
	if (p == lastPlaylistPos) {
        p = getPlaylistSongCount(-1); // playlist_amount();
        isFakePosition = true;
	} else
		isFakePosition = false;
	lastPlaylistPos = p;
	return p;
}


bool IPodEmulator::Run() {
	PT_BEGIN();
	while(1) {
		if (iap_pollenabled && millis() > nextPoll) {
			nextPoll = millis() + 500;
			PT_WAIT_UNTIL(sender.isAvailable());
		    PROGMEM unsigned char xdata[] = {0x04, 0x00, 0x27, 0x04, 0x00, 0x00, 0x00, 0x00};
		    unsigned char *data = sender.getBuffer(xdata, sizeof(xdata));
		    //unsigned long time_elapsed = audio_current_track()->elapsed;
		    unsigned long time_elapsed = 30000;

		    //time_elapsed += wps_state.ff_rewind_count;

		    data[3] = 0x04; // playing

		    if (iap_changedctr == 0 && isFakePosition) {
		    	nochangeTicks++;
		    } else
		    	nochangeTicks = 0;

		    /* If info has changed, don't flag it right away */
		    if(iap_changedctr > 2 || nochangeTicks > 2 || (iap_changedctr && changedChannel))
		    {
		        /* track info has changed */
		        data[3] = 0x01; // 0x02 has same effect?
		        iap_updateflag = true;
		        //long playlist_pos = getVirtualCurrentPlaylistPosition(1);
		        long playlist_pos = getVirtualCurrentPlaylistPosition(true);
		        iap_changedctr = 0;
		        /*
		        if (iap_changedctr == 1)
		        	playlist_pos = -1;
		    	if (iap_changedctr == 2)
		    		iap_changedctr = 0;
		    	else
		    		iap_changedctr = 2;
				*/
		        data[4] = playlist_pos >> 24;
		        data[5] = playlist_pos >> 16;
		        data[6] = playlist_pos >> 8;
		        data[7] = playlist_pos;
		    } else {

				data[4] = time_elapsed >> 24;
				data[5] = time_elapsed >> 16;
				data[6] = time_elapsed >> 8;
				data[7] = time_elapsed;
		    }
		    if (iap_changedctr)
		    	iap_changedctr++;

		    sender.send();
		}
		while (Serial->available()) {
			bool packetReady = iap_getc(Serial->read());
			if (packetReady) {
				// Set the mode to an unrecognized mode so below code takes no acvtion
			    if(serbuf[0] == 0)
			    	serbuf[1] = 0xff;

			    /* if we are waiting for a remote button to go out,
			       delay the handling of the new packet */
			    /*
			    if(!iap_remotetick)
			    {
			        queue_post(&button_queue, SYS_IAP_HANDLEPKT, 0);
			        return;
			    }
				*/

			    /* Handle Mode 0 */
			    if (serbuf[1] == 0x00)
			    {
			    	//xmlog("iPod: Mode switch cmd");
			        switch (serbuf[2])
			        {
			            /* get model info */
			            case 0x0D:
			            {
			            	//xmlog("iPod: GetModeInfo");
			    			PT_WAIT_UNTIL(sender.isAvailable());
			                PROGMEM unsigned char data[] = {0x00, 0x0E, 0x00, 0x0B, 0x00, 0x10,
			                                       'R', 'O', 'C', 'K', 'B', 'O', 'X', '-', 'X', 'M', 0x00};
			    		    sender.send_p(data, sizeof(data));
			                break;
			            }
			            /* No idea ??? */
			            case 0x0F:
			            {
			            	//xmlog("iPod: ?0x0f?-%02x", serbuf[3]);
			            	switch(serbuf[3]) {
								case 0x0a:
								{
					    			PT_WAIT_UNTIL(sender.isAvailable());
									PROGMEM unsigned char data[] = {0x00, 0x10, 0x0a, 0x01, 0x02};
					    		    sender.send_p(data, sizeof(data));
									break;
								}
								case 0x04:
								{
					    			PT_WAIT_UNTIL(sender.isAvailable());
									PROGMEM unsigned char data[] = {0x00, 0x10, 0x04, 0x01, 0x0c};
					    		    sender.send_p(data, sizeof(data));
									break;
								}
								case 0x00:
								default:
								{
					    			PT_WAIT_UNTIL(sender.isAvailable());
					                PROGMEM unsigned char data[] = {0x00, 0x10, 0x00, 0x01, 0x09};
					    			sender.send_p(data, sizeof(data));
									break;
								}
			            	}
			                break;
			            }
			            /* FM transmitter sends this: FF 55 06 00 01 05 00 02 01 F1 (mode switch) */
			            case 0x01:
			            {
			            	//xmlog("iPod: 0x01-%02x", serbuf[3]);
			            	switch(serbuf[3]) {
								case 0x05:
								{
					    			PT_WAIT_UNTIL(sender.isAvailable());
									PROGMEM unsigned char data[] = {0x05, 0x02};
									//xmlog("iPod: FM transmiter 0x01 0x05");
									//sleep(HZ/3);
									delay(250);
					    		    sender.send_p(data, sizeof(data));
									break;
								}
								case 0x00:
								{
					    			PT_WAIT_UNTIL(sender.isAvailable());
									PROGMEM unsigned char data[] = {0x04, 0x00, 0x01, 0x00, 0x00, 0x26};
					    		    sender.send_p(data, sizeof(data));
									break;
								}
			            	}
			                break;
			            }
			            /* FM transmitter sends this: FF 55 0E 00 13 00 00 00 35 00 00 00 04 00 00 00 00 A6 (???)*/
			            case 0x13:
			            {
			            	//xmlog("iPod: FM transmiter 0x13 0x00");
			    			PT_WAIT_UNTIL(sender.isAvailable());
			            	PROGMEM unsigned char data[] = {0x00, 0x02, 0x00, 0x13};
			    		    sender.send_p(data, sizeof(data));
			    			PT_WAIT_UNTIL(sender.isAvailable());
			    		    unsigned char data2[] = {0x00, 0x27, 0x00};
			    		    sender.send_p(data2, sizeof(data2));
			    			PT_WAIT_UNTIL(sender.isAvailable());
			    		    PROGMEM unsigned char data3[] = {0x05, 0x02};
			    		    sender.send_p(data3, sizeof(data3));
			                break;
			            }
			            /* FM transmitter sends this: FF 55 02 00 05 F9 (mode switch: AiR mode) */
			            case 0x05:
			            {
			            	//xmlog("iPod: FM transmiter 0x05 0xf9");
			    			PT_WAIT_UNTIL(sender.isAvailable());
			            	PROGMEM unsigned char data[] = {0x00, 0x02, 0x06, 0x05, 0x00, 0x00, 0x0B, 0xB8, 0x28};
			    		    sender.send_p(data, sizeof(data));
			    			PT_WAIT_UNTIL(sender.isAvailable());
			    		    PROGMEM unsigned char data2[] = {0x00, 0x02, 0x00, 0x05};
			    		    sender.send_p(data2, sizeof(data2));
			                break;
			            }
			            case 0x24:
			            {
			            	//xmlog("iPod: 0x24?");
			    			PT_WAIT_UNTIL(sender.isAvailable());
			            	PROGMEM unsigned char data[] = {0x00, 0x25, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x5f };
			    		    sender.send_p(data, sizeof(data));
			            	break;
			            }
			            case 0x2b:
			            {
			            	//xmlog("iPod: 0x2b?");
			    			PT_WAIT_UNTIL(sender.isAvailable());
			            	PROGMEM unsigned char data[] = {0x00, 0x02, 0x00, 0x2b };
			    		    sender.send_p(data, sizeof(data));
			            	break;
			            }
			            case 0x02:
			            {
			            	//xmlog("iPod: 0x02?");
			    			PT_WAIT_UNTIL(sender.isAvailable());
			            	PROGMEM unsigned char data[] = {0x04, 0x00, 0x26, 0x01 };
			    		    sender.send_p(data, sizeof(data));
			            	break;
			            }
			            case 0x07:
			            {
			            	//xmlog("iPod: 0x07?");
			                //unsigned char data[] = {0x00, 0x08, 0x6b, 0x65, 0x61, 0x72, 0x79, 0xe2, 0x80, 0x99, 0x73, 0x20, 0x69, 0x50, 0x68, 0x6f, 0x6e, 0x65, 0x00 };
			    			PT_WAIT_UNTIL(sender.isAvailable());
			            	PROGMEM unsigned char data[] = {0x00, 0x08, 'P', 'o', 'd', 'S', 'h', 'i', 'e', 'l', 'd', 0x00 };
			    		    sender.send_p(data, sizeof(data));
			            	break;
			            }
			            case 0x0b:
			            {
			            	//xmlog("iPod: 0x0b?");
			    			PT_WAIT_UNTIL(sender.isAvailable());
			            	PROGMEM unsigned char data[] = {0x00, 0x0c, 0x38, 0x38, 0x39, 0x32, 0x35, 0x57, 0x54, 0x43, 0x33, 0x4e, 0x53, 0x00};
			    		    sender.send_p(data, sizeof(data));
			            	break;
			            }

			            case 0x09:
			            {
			            	//xmlog("iPod: 0x09?");
			    			PT_WAIT_UNTIL(sender.isAvailable());
			            	PROGMEM unsigned char data[] = {0x00, 0x0a, 0x03, 0x00, 0x00};
			    		    sender.send_p(data, sizeof(data));
			            	break;
			            }

			            /* default response is with cmd ok packet */
			            default:
			            {
			            	//xmlog("iPod: Mode switch not recognized, NOT!! ACKing");
			            	/*
			                unsigned char data[] = {0x00, 0x02, 0x00, 0x00};
			                data[3] = serbuf[2]; //respond with cmd
			                iap_send_pkt(data, sizeof(data));
			                */
			                break;
			            }
			        }
			    }
			    /* Handle Mode 2 */
			    else if (serbuf[1] == 0x02)
			    {
			    	//xmlog("iPod: Mode2 cmd -- ignoring");

			//        if(serbuf[2] != 0) return;
			//        iap_remotebtn = BUTTON_NONE;
			//        iap_remotetick = false;
			//
			//        if(serbuf[0] >= 3 && serbuf[3] != 0)
			//        {
			//            if(serbuf[3] & 1)
			//                iap_remotebtn |= BUTTON_RC_PLAY;
			//            if(serbuf[3] & 2)
			//                iap_remotebtn |= BUTTON_RC_VOL_UP;
			//            if(serbuf[3] & 4)
			//                iap_remotebtn |= BUTTON_RC_VOL_DOWN;
			//            if(serbuf[3] & 8)
			//                iap_remotebtn |= BUTTON_RC_RIGHT;
			//            if(serbuf[3] & 16)
			//                iap_remotebtn |= BUTTON_RC_LEFT;
			//        }
			//        else if(serbuf[0] >= 4 && serbuf[4] != 0)
			//        {
			//            if(serbuf[4] & 1) /* play */
			//            {
			//                if (audio_status() != AUDIO_STATUS_PLAY)
			//                {
			//                    iap_remotebtn |= BUTTON_RC_PLAY;
			//                    iap_repeatbtn = 2;
			//                    iap_remotetick = false;
			//                    iap_changedctr = 1;
			//                }
			//            }
			//            if(serbuf[4] & 2) /* pause */
			//            {
			//                if (audio_status() == AUDIO_STATUS_PLAY)
			//                {
			//                    iap_remotebtn |= BUTTON_RC_PLAY;
			//                    iap_repeatbtn = 2;
			//                    iap_remotetick = false;
			//                    iap_changedctr = 1;
			//                }
			//            }
			//            if((serbuf[4] & 128) && !iap_btnshuffle) /* shuffle */
			//            {
			//                iap_btnshuffle = true;
			//                if(!global_settings.playlist_shuffle)
			//                {
			//                    global_settings.playlist_shuffle = 1;
			//                    settings_save();
			//                    settings_apply(false);
			//                    if (audio_status() & AUDIO_STATUS_PLAY)
			//                        playlist_randomise(NULL, current_tick, true);
			//                }
			//                else if(global_settings.playlist_shuffle)
			//                {
			//                    global_settings.playlist_shuffle = 0;
			//                    settings_save();
			//                    settings_apply(false);
			//                    if (audio_status() & AUDIO_STATUS_PLAY)
			//                        playlist_sort(NULL, true);
			//                }
			//            }
			//            else
			//                iap_btnshuffle = false;
			//        }
			//        else if(serbuf[0] >= 5 && serbuf[5] != 0)
			//        {
			//            if((serbuf[5] & 1) && !iap_btnrepeat) /* repeat */
			//            {
			//                int oldmode = global_settings.repeat_mode;
			//                iap_btnrepeat = true;
			//
			//                if (oldmode == REPEAT_ONE)
			//                        global_settings.repeat_mode = REPEAT_OFF;
			//                else if (oldmode == REPEAT_ALL)
			//                        global_settings.repeat_mode = REPEAT_ONE;
			//                else if (oldmode == REPEAT_OFF)
			//                        global_settings.repeat_mode = REPEAT_ALL;
			//
			//                settings_save();
			//                settings_apply(false);
			//                if (audio_status() & AUDIO_STATUS_PLAY)
			//                audio_flush_and_reload_tracks();
			//            }
			//            else
			//                iap_btnrepeat = false;
			//
			//            if(serbuf[5] & 16) /* ffwd */
			//            {
			//                iap_remotebtn |= BUTTON_RC_RIGHT;
			//            }
			//            if(serbuf[5] & 32) /* frwd */
			//            {
			//                iap_remotebtn |= BUTTON_RC_LEFT;
			//            }
			//        }
			    }
			    /* Handle Mode 3 */
			    else if (serbuf[1] == 0x03)
			    {
			        switch(serbuf[2])
			        {
			            /* some kind of status packet? */
			            case 0x01:
			            {
			            	//xmlog("iPod: status cmd (0x01)");
			    			PT_WAIT_UNTIL(sender.isAvailable());
			            	PROGMEM unsigned char data[] = {0x03, 0x02, 0x00, 0x00, 0x00, 0x00};
			    		    sender.send_p(data, sizeof(data));
			                break;
			            }
			        }
			    }
			    /* Handle Mode 4 */
			    else if (serbuf[1] == 0x04)
			    {
			    	int cmd = ((unsigned long)serbuf[2] << 8) | serbuf[3];
			        switch (cmd)
			        {
			            /* Get data updated??? flag */
			            case 0x0009:
			            {
			            	//xmlog("iPod: getDataUpdatedFlag");
			    			PT_WAIT_UNTIL(sender.isAvailable());
			            	PROGMEM unsigned char xdata[] = {0x04, 0x00, 0x0A, 0x00};
			    			unsigned char *data = sender.getBuffer(xdata, sizeof(xdata));
			                data[3] = iap_updateflag ? 0 : 1;
			    		    sender.send();
			                break;
			            }
			            /* Set data updated??? flag */
			            case 0x000B:
			            {
			            	//xmlog("iPod: setDataUpdatedFlag");
			                iap_updateflag = serbuf[4] ? 0 : 1;
			                /* respond with cmd ok packet */
			    			PT_WAIT_UNTIL(sender.isAvailable());
			                PROGMEM unsigned char data[] = {0x04, 0x00, 0x01, 0x00, 0x00, 0x0B};
			    			sender.send_p(data, sizeof(data));
			                break;
			            }
			            /* get mp3 tag */
			            case 0x000c:
			            {
			            	int tagType = serbuf[4];
			                long tracknum = (signed long)serbuf[5] << 24 |
			                                (signed long)serbuf[6] << 16 |
			                                (signed long)serbuf[7] << 8 | serbuf[8];
			                if (tracknum >= getPlaylistSongCount(-1))
			                	tracknum = getCurrentPlaylistPosition();
			                /* Genre */
			                if (tagType == 0x05) {
				    			PT_WAIT_UNTIL(sender.isAvailable());
			                	PROGMEM unsigned char xdata[48] = {0x04, 0x00, 0x0d, 0x05 };
				    			unsigned char *data = sender.getBuffer(xdata, sizeof(xdata));
			                	if (getGenre(playlist, tracknum, (char *)data+4)) {
			                		sender.send(5 + strlen((char *)data+4));
			                	} else
			                		IPOD_NAK();

			                } else {
			                	IPOD_NAK();
			                }
			            	break;
			            }
			            /* Get iPod size? */
			            case 0x0012:
			            {
			            	//xmlog("iPod: getiPodSize");
			    			PT_WAIT_UNTIL(sender.isAvailable());
			            	PROGMEM unsigned char data[] = {0x04, 0x00, 0x13, 0x01, 0x0B};
			                sender.send_p(data, sizeof(data));
			                break;
			            }
			            /* get iPodName */
			            case 0x0014:
			            {
			            	//xmlog("iPod: getiPodName");
			    			PT_WAIT_UNTIL(sender.isAvailable());
			            	PROGMEM unsigned char data[] = {0x04, 0x00, 0x15, 'P', 'o', 'd', 'S', 'h', 'i', 'e', 'l', 'd', 0x00 };
			                sender.send_p(data, sizeof(data));
			                break;
			            }
			            /* switch to main playlist */
			            case 0x0016:
			            {
			            	//xmlog("iPod: switchToMain playlist");
			                /* response is with cmd ok packet */
			    			PT_WAIT_UNTIL(sender.isAvailable());
			            	PROGMEM unsigned char xdata[] = {0x04, 0x00, 0x01, 0x00, 0x00, 0x00};
							unsigned char *data = sender.getBuffer(xdata, sizeof(xdata));
			                data[4] = serbuf[2];
			                data[5] = serbuf[3];
			                sender.send();
			                playlist = 0;
			                filterPlaylist = 0;
			                break;
			            }
			            /* switch to sometihing */
			            case 0x0038:
			            case 0x0017:
			            {
			            	int type = serbuf[4];
			                long tracknum = (signed long)serbuf[5] << 24 |
			                                (signed long)serbuf[6] << 16 |
			                                (signed long)serbuf[7] << 8 | serbuf[8];

			                if (type == 0x01) {
			                	filterPlaylist = tracknum;
			                	/*
			                	if (cmd == 0x0017)
			                		playlist = tracknum;
			                		*/
			                }
			                else {
			                	filterPlaylist = -1;
			                	//xmlog("iPod: switch to type %02x, num: %ld", type, tracknum);
			                }

			                /* response is with cmd ok packet */
			    			PT_WAIT_UNTIL(sender.isAvailable());
			                PROGMEM unsigned char xdata[] = {0x04, 0x00, 0x01, 0x00, 0x00, 0x00};
							unsigned char *data = sender.getBuffer(xdata, sizeof(xdata));
			                data[4] = serbuf[2];
			                data[5] = serbuf[3];
			                sender.send();
			                break;
			            }
			            /* Get count of given types */
			            case 0x0018:
			            {
			            	//xmlog("iPod: get count of: %02x", serbuf[4]);
			    			PT_WAIT_UNTIL(sender.isAvailable());
			            	PROGMEM unsigned char xdata[] = {0x04, 0x00, 0x19, 0x00, 0x00, 0x00, 0x00};
							unsigned char *data = sender.getBuffer(xdata, sizeof(xdata));
			                unsigned long num = 0;
			                switch(serbuf[4]) /* type number */
			                {
			                    case 0x01: /* total number of playlists */
			                        num = getPlaylistCount();
			                        break;
			                    case 0x05: /* total number of songs */
			                        num = (filterPlaylist >= 0) ? getPlaylistSongCount(filterPlaylist) : 0;
			                        num++;
			                        break;
			                }
			                data[3] = num >> 24;
			                data[4] = num >> 16;
			                data[5] = num >> 8;
			                data[6] = num;
			            	//xmlog("iPod: get count of: %02x, Result: %ld", serbuf[4], num);
			                sender.send();
			                break;
			            }
			            /* get names of types */
			            case 0x001A:
			            {
			            	static int total, type, start, cnt, send_i;
			            	type = serbuf[4];
			                start = (signed long)serbuf[5] << 24 |
			                                (signed long)serbuf[6] << 16 |
			                                (signed long)serbuf[7] << 8 | serbuf[8];
			                cnt = (signed long)serbuf[9] << 24 |
			                                (signed long)serbuf[10] << 16 |
			                                (signed long)serbuf[11] << 8 | serbuf[12];
			            	//xmlog("iPod: get names of type: %02x, from: %ld, Cnt: %ld", type, start, cnt);
			                switch(type) {
								/* playlists */
								case 0x01:
								/* songs */
								case 0x05:
								{
									if (type == 0x05 && filterPlaylist < 0) {
										IPOD_NAK();
										break;
									}

										if (type == 0x05)
											total = getPlaylistSongCount(filterPlaylist); // playlist_amount();
										else
											total = start + cnt;

										static PROGMEM unsigned char xdata[75] = {0x04, 0x00, 0x1B};
										for (send_i=start;send_i<start+cnt && send_i < total;send_i++) {
											PT_YIELD_UNTIL(sender.isAvailable());
											unsigned char *data = sender.getBuffer(xdata, sizeof(xdata));
											if (type == 0x05) {
												getSongTitleForPlaylist(filterPlaylist, send_i, (char *)&data[7]);
									    	} else {
									            getPlaylistName(send_i, (char*)&data[7]);

									    	}
									        data[3] = (long)send_i >> 24;
									        data[4] = (long)send_i>> 16;
									        data[5] = (long)send_i>> 8;
									        data[6] = (long)send_i;

											sender.send(8+strlen((char *)&data[7]));
										}


										// Add an additional "blank" song that can be used for forcing a display update
										if (type == 0x05 && send_i == total && send_i < start+cnt) {
											PT_YIELD_UNTIL(sender.isAvailable());
											unsigned char *data = sender.getBuffer(xdata, sizeof(xdata));
									        data[3] = (long)send_i >> 24;
									        data[4] = (long)send_i>> 16;
									        data[5] = (long)send_i>> 8;
									        data[6] = (long)send_i;

											strcpy((char *)&data[7], "--");
											sender.send();
										}
									break;
								}
								default:
								{
									//xmlog("iPod: No data for type requested, NAKing");
									/* send back error */
									IPOD_NAK();
									break;
								}
			                }

			            	break;
			            }
			            /* Get time and status */
			            case 0x001C:
			            {
							//xmlog("iPod: getTimeAndStatus");
			    			PT_WAIT_UNTIL(sender.isAvailable());
			            	PROGMEM unsigned char xdata[] = {0x04, 0x00, 0x1D, 0x00, 0x00, 0x00,
			                                        0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
							unsigned char *data = sender.getBuffer(xdata, sizeof(xdata));
			                //struct mp3entry *id3 = audio_current_track();
			                //unsigned long time_total = id3->length;
			                //unsigned long time_elapsed = id3->elapsed;
			                unsigned long time_total = 60000;
			                unsigned long time_elapsed = 30000;

			                //int status = audio_status();
			                data[3] = time_total >> 24;
			                data[4] = time_total >> 16;
			                data[5] = time_total >> 8;
			                data[6] = time_total;
			                data[7] = time_elapsed >> 24;
			                data[8] = time_elapsed >> 16;
			                data[9] = time_elapsed >> 8;
			                data[10] = time_elapsed;
			                if (1 /* status == AUDIO_STATUS_PLAY */)
			                    data[11] = 0x01; /* play */
			                //else if (status & AUDIO_STATUS_PAUSE)
			                //    data[11] = 0x02; /* pause */
			                sender.send();
			                break;
			            }
			            /* Get current pos in playlist */
			            case 0x001E:
			            {
							//xmlog("iPod: getCurrentPos in playlist");
			    			PT_WAIT_UNTIL(sender.isAvailable());
			            	PROGMEM unsigned char xdata[] = {0x04, 0x00, 0x1F, 0x00, 0x00, 0x00, 0x00};
							unsigned char *data = sender.getBuffer(xdata, sizeof(xdata));
			                long playlist_pos = getVirtualCurrentPlaylistPosition(false);
			                //playlist_pos -= playlist_get_first_index(NULL);
			                //if(playlist_pos < 0)
			                  //  playlist_pos += playlist_amount();
			                data[3] = playlist_pos >> 24;
			                data[4] = playlist_pos >> 16;
			                data[5] = playlist_pos >> 8;
			                data[6] = playlist_pos;
			                sender.send();
			                break;
			            }
			            /* Get title of a song number */
			            case 0x0020:
			            /* Get artist of a song number */
			            case 0x0022:
			            /* Get album of a song number */
			            case 0x0024:
			            {
							//xmlog("iPod: get title or artist or album");
			    			PT_WAIT_UNTIL(sender.isAvailable());
			            	PROGMEM unsigned char xdata[70] = {0x04, 0x00, 0xFF};
							unsigned char *data = sender.getBuffer(xdata, sizeof(xdata));
			                /*
			                struct mp3entry id3;
			                int fd;
			                */
			                long tracknum = (signed long)serbuf[4] << 24 |
			                                (signed long)serbuf[5] << 16 |
			                                (signed long)serbuf[6] << 8 | serbuf[7];


			                if (tracknum >= getPlaylistSongCount(-1))
			                	tracknum = getCurrentPlaylistPosition();

			                data[2] = serbuf[3] + 1;
			                /*
			                memcpy(&id3, audio_current_track(), sizeof(id3));
			                tracknum += playlist_get_first_index(NULL);
			                if(tracknum >= playlist_amount())
			                    tracknum -= playlist_amount();
							*/
			                /* If the tracknumber is not the current one,
			                   read id3 from disk */
			                /*
			                if(playlist_next(0) != tracknum)
			                {
			                    struct playlist_track_info info;
			                    playlist_get_track_info(NULL, tracknum, &info);
			                    fd = open(info.filename, O_RDONLY);
			                    memset(&id3, 0, sizeof(struct mp3entry));
			                    get_metadata(&id3, fd, info.filename);
			                    close(fd);
			                }
			                */

			                //boolean useExt = true;
			                /* Return the requested track data */
			                switch(serbuf[3])
			                {
			                    case 0x20:
			                    	getSongTitle(playlist, tracknum, (char *)&data[3]);
			                        //strncpy((char *)&data[3], id3.title, 64);
			                    	sender.send(4+strlen((char*)&data[3]));
			                        break;
			                    case 0x22:
			                    	getArtist(playlist, tracknum, (char *)&data[3]);
			                        //strncpy((char *)&data[3], id3.artist, 64);
			                    	sender.send(4+strlen((char*)&data[3]));
			                        break;
			                    /* Album is always zero */
			                    case 0x24:
			                        //strncpy((char *)&data[3], id3.album, 64);
			                    	//data[3] = 0;
			                    	getAlbum(playlist, tracknum, (char *)&data[3]);
			                    	sender.send(4+strlen((char*)&data[3]));
			                        break;
			                }
			                break;
			            }
			            /* Set polling mode */
			            case 0x0026:
			            {
			                iap_pollenabled = serbuf[4] ? 1 : 0;
							//xmlog("iPod: setPollMode: %d", iap_pollspeed);
			                /*responsed with cmd ok packet */
			    			PT_WAIT_UNTIL(sender.isAvailable());
			                PROGMEM unsigned char data[] = {0x04, 0x00, 0x01, 0x00, 0x00, 0x26};
			    			sender.send_p(data, sizeof(data));
			                break;
			            }
			            /* AiR playback control */
			            case 0x0029:
			            {
							//xmlog("iPod: playbackControl: %02x", serbuf[4]);

			                /* respond with cmd ok packet */
			    			PT_WAIT_UNTIL(sender.isAvailable());
			            	PROGMEM unsigned char data[] = {0x04, 0x00, 0x01, 0x00, 0x00, 0x29};
			    			sender.send_p(data, sizeof(data));
			                switch(serbuf[4])
			                {
			                    case 0x01: /* play/pause */
			                        //iap_remotebtn = BUTTON_RC_PLAY;
			                        //iap_repeatbtn = 2;
			                        //iap_remotetick = false;
			                        //iap_changedctr = 1;
			                        break;
			                    case 0x02: /* stop */
			                        //iap_remotebtn = BUTTON_RC_PLAY|BUTTON_REPEAT;
			                        //iap_repeatbtn = 2;
			                        //iap_remotetick = false;
			                        //iap_changedctr = 1;
			                        break;
			                    case 0x03: /* skip++ */
			                        //iap_remotebtn = BUTTON_RC_RIGHT;
			                        //iap_repeatbtn = 2;
			                        //iap_remotetick = false;
			                		changedChannel = true;
			                    	nextTrack();
			                        break;
			                    case 0x04: /* skip-- */
			                        //iap_remotebtn = BUTTON_RC_LEFT;
			                        //iap_repeatbtn = 2;
			                        //iap_remotetick = false;
			                		changedChannel = true;

			                    	previousTrack();
			                        break;
			                    case 0x05: /* ffwd */
			                        //iap_remotebtn = BUTTON_RC_RIGHT;
			                        //iap_remotetick = false;
			                        //if(iap_pollspeed) iap_pollspeed = 5;
			                        break;
			                    case 0x06: /* frwd */
			                        //iap_remotebtn = BUTTON_RC_LEFT;
			                        //iap_remotetick = false;
			                        //if(iap_pollspeed) iap_pollspeed = 5;
			                        break;
			                    case 0x07: /* end ffwd/frwd */
			                        //iap_remotebtn = BUTTON_NONE;
			                        //iap_remotetick = false;
			                        //if(iap_pollspeed) iap_pollspeed = 1;
			                        break;
			                }
			                break;
			            }
			            /* Get shuffle mode */
			            case 0x002C:
			            {
							//xmlog("iPod: getshuffleMode");
			    			PT_WAIT_UNTIL(sender.isAvailable());
			            	PROGMEM unsigned char xdata[] = {0x04, 0x00, 0x2D, 0x00};
			    			unsigned char *data = sender.getBuffer(xdata, sizeof(xdata));
			                data[3] = getShuffleMode();
			                sender.send();
			                break;
			            }
			            /* Set shuffle mode */
			            case 0x002E:
			            {
			            	int mode = serbuf[4];
							//xmlog("iPod: setShuffleMode");
			            	setShuffleMode(mode);
							//if (mode)
								//toggleFavorite();

			                /* respond with cmd ok packet */
			    			PT_WAIT_UNTIL(sender.isAvailable());
			            	PROGMEM unsigned char data[] = {0x04, 0x00, 0x01, 0x00, 0x00, 0x2E};
			    			sender.send_p(data, sizeof(data));
			                break;
			            }
			            /* Get repeat mode */
			            case 0x002F:
			            {
							//xmlog("iPod: getRepeatMode");
			    			PT_WAIT_UNTIL(sender.isAvailable());
			            	PROGMEM unsigned char xdata[] = {0x04, 0x00, 0x30, 0x00};
			    			unsigned char *data = sender.getBuffer(xdata, sizeof(xdata));

			                data[3] = getRepeatMode();
			                /*
			                if(global_settings.repeat_mode == REPEAT_OFF)
			                    data[3] = 0;
			                else if(global_settings.repeat_mode == REPEAT_ONE)
			                    data[3] = 1;
			                else
			                    data[3] = 2;
			                    */
			                sender.send();
			                break;
			            }
			            /* Set repeat mode */
			            case 0x0031:
			            {
							//xmlog("iPod: setRepeatMode");
			            	setRepeatMode(serbuf[4]);
			            	/*
			                int oldmode = global_settings.repeat_mode;
			                if (serbuf[4] == 0)
			                    global_settings.repeat_mode = REPEAT_OFF;
			                else if (serbuf[4] == 1)
			                    global_settings.repeat_mode = REPEAT_ONE;
			                else if (serbuf[4] == 2)
			                    global_settings.repeat_mode = REPEAT_ALL;

			                if (oldmode != global_settings.repeat_mode)
			                {
			                    settings_save();
			                    settings_apply(false);
			                    if (audio_status() & AUDIO_STATUS_PLAY)
			                        audio_flush_and_reload_tracks();
			                }
							*/
			                /* respond with cmd ok packet */
			    			PT_WAIT_UNTIL(sender.isAvailable());
			            	PROGMEM unsigned char data[] = {0x04, 0x00, 0x01, 0x00, 0x00, 0x31};
			    			sender.send_p(data, sizeof(data));
			                break;
			            }
			            /* Get Max Screen Size for Picture Upload??? */
			            case 0x0033:
			            {
							//xmlog("iPod: getScreenSize");
			    			PT_WAIT_UNTIL(sender.isAvailable());
			            	PROGMEM unsigned char data[] = {0x04, 0x00, 0x34, 0x01, 0x36, 0x00, 0xA8, 0x01};
			    			sender.send_p(data, sizeof(data));
			                break;
			            }
			            /* ??? */
			            case 0x0039:
			            {
							//xmlog("iPod: cmd 0x0039");
			    			PT_WAIT_UNTIL(sender.isAvailable());
			            	PROGMEM unsigned char data[] = {0x04, 0x00, 0x3a, 0x01, 0x40, 0x01, 0xcc, 0x02, 0x01, 0x40, 0x01, 0xcc, 0x33};
			    			sender.send_p(data, sizeof(data));
			                break;
			            }

			            /* Get number songs in current playlist */
			            case 0x0035:
			            {
							//xmlog("iPod: getNumberOfSongsInCurrentPlaylist");
			    			PT_WAIT_UNTIL(sender.isAvailable());
			            	PROGMEM unsigned char xdata[] = {0x04, 0x00, 0x36, 0x00, 0x00, 0x00, 0x00};
			    			unsigned char *data = sender.getBuffer(xdata, sizeof(xdata));
			                unsigned long playlist_amt = getPlaylistSongCount(-1); // playlist_amount();
			                //playlist_amt = 999999;
			                data[3] = playlist_amt >> 24;
			                data[4] = playlist_amt >> 16;
			                data[5] = playlist_amt >> 8;
			                data[6] = playlist_amt;
			                sender.send();
			                break;
			            }
			            /* Jump to track number in current playlist */
			            case 0x0028:
			            case 0x0037:
			            {
			                long tracknum = (signed long)serbuf[4] << 24 |
			                                (signed long)serbuf[5] << 16 |
			                                (signed long)serbuf[6] << 8 | serbuf[7];
							//xmlog("iPod: jumpToTrack in current playlist: %ld", tracknum);
			                /*
			                if (!wps_state.paused)
			                    audio_pause();
			                audio_skip(tracknum - playlist_next(0));
			                if (!wps_state.paused)
			                    audio_resume();
							*/
			            	setPlaylist(filterPlaylist);
			            	playlist = filterPlaylist;
			                if (tracknum < 0) {
			                	tracknum = 0;
			                }

			            	if (tracknum < getPlaylistSongCount(-1)) {
			            		changedChannel = true;
			            		changeTrack(tracknum);
			            	}
			                /* respond with cmd ok packet */
			    			PT_WAIT_UNTIL(sender.isAvailable());
			            	PROGMEM unsigned char xdata[] = {0x04, 0x00, 0x01, 0x00, 0x00, 0x00};
			    			unsigned char *data = sender.getBuffer(xdata, sizeof(xdata));
			                data[4] = serbuf[2];
			                data[5] = serbuf[3];
			                sender.send();
			                break;
			            }
			            default:
			            {
							//xmlog("iPod: Unknown cmd.  Acking");
			                /* default response is with cmd ok packet */
			    			PT_WAIT_UNTIL(sender.isAvailable());
			            	PROGMEM unsigned char xdata[] = {0x04, 0x00, 0x01, 0x00, 0x00, 0x00};
			    			unsigned char *data = sender.getBuffer(xdata, sizeof(xdata));
			                data[4] = serbuf[2];
			                data[5] = serbuf[3];
			                sender.send();
			                break;
			            }
			        }
			    }
			    serbuf[0] = 0;
			}
		}
		PT_YIELD();
	}
	PT_END();
}

IPodSender::IPodSender() {
	locked = false;
	startSending = false;
}

bool IPodSender::Run() {
	PT_BEGIN();
	while(1) {
		PT_WAIT_UNTIL(startSending);
		startSending = false;
		response_i = 0;

		while(response_i < responselen) {
			PT_WAIT_UNTIL(Serial->canSend());
			Serial->write(response[response_i++]);
		}

		locked = false;
	}
	PT_END();
}


unsigned char *IPodSender::getBuffer(PROGMEM unsigned char *init, int len) {
	if (init != NULL && len != 0)
		memcpy_P(response+3, init, len);
	init_len = len;
	return response+3;
}
//void IPodSender::releaseBuffer() { locked = false; }
void IPodSender::send(int len) {
	if (len == -1)
		len = init_len;
    if(len > TX_BUFLEN-4) len = TX_BUFLEN-4;

    responselen = len + 4;

    response[0] = 0xFF;
    response[1] = 0x55;

    int chksum = response[2] = len;
    for(int i = 0; i < len; i ++)
    {
        chksum += response[i+3];
        //response[i+3] = data[i];
    }

    response[len+3] = 0x100 - (chksum & 0xFF);

	startSending = true;
}

void IPodSender::send_p(PROGMEM unsigned char *data, int len) {
	getBuffer(data, len);
	send();
}
