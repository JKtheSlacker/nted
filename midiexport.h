/****************************************************************************************/
/*											*/
/* This program is free software; you can redistribute it and/or modify it under the	*/
/* terms of the GNU General Public License as published by the Free Software		*/
/* Foundation; either version 2 of the License, or (at your option) any later version.	*/
/*											*/
/* This program is distributed in the hope that it will be useful, but WITHOUT ANY	*/
/* WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A	*/
/* PARTICULAR PURPOSE. See the GNU General Public License for more details.		*/
/*											*/
/* You should have received a copy of the GNU General Public License along with this	*/
/* program; (See "COPYING"). If not, If not, see <http://www.gnu.org/licenses/>.        */
/*											*/
/*--------------------------------------------------------------------------------------*/
/*											*/
/*  Copyright   Joerg Anders, TU Chemnitz, Fakultaet fuer Informatik, GERMANY           */
/*		ja@informatik.tu-chemnitz.de						*/
/*											*/
/*											*/
/****************************************************************************************/

#ifndef MIDIEXPORT_H

#define MIDIEXPORT_H
#include "config.h"
#include <gtk/gtk.h>
#include <stdio.h>

class NedMainWindow;

class NedMidiExport {
	public: 
		static void exportMidi(FILE *fp, NedMainWindow *main_window, char *miditext, GList *midi_events);
	private:
		static void writeTrack(GList *midi_events, int track_nr, int num, int denom);
		static void writePgmChange(unsigned long long time, unsigned char chn, unsigned char pgm);
		static void writeByte(unsigned char b);
		static void writeWord(unsigned int w);
		static void writeDWord(unsigned int dw);
		static void writeString(char *s);
		static void writeTime(unsigned long long time);
		static void writeNoteOn(unsigned long long time, unsigned char ch, unsigned char ptch, unsigned char vel, unsigned char pgm);
		static void writeNoteOff(unsigned long long time, unsigned char ch, unsigned char ptch, unsigned char vel);
		static void writeCtlChange(unsigned long long time, unsigned char chn, unsigned char ctl, unsigned char pgm);
		static void writeText(unsigned long long time, const char *s);
		static void writeTrackName(unsigned long long time, const char *s);
		static void writeTimeSig(unsigned long long time, int num, int denom);
		static void writeKeySig(unsigned long long time, int sig);
		static void writeTempo(unsigned long long time, unsigned int tempo);
		static void writeCtrlTrack(char *Title, char *miditext, NedMainWindow *main_window);
		static FILE *m_midiout;
		static NedMainWindow *m_main_window;
		static bool m_first_lyrics;
		static unsigned char m_channel_pgms[16];
};


#endif // MIDIEXPORT_H
