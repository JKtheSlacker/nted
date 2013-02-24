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

#ifndef MIDI_RECORDER_H

#define MIDI_RECORDER_H

#include <stdio.h>
#include <gtk/gtk.h>
#include "importer.h"

class NedIdiotsEditor;

class NedMidiRecorder : public NedImporter  {
	public:
		NedMidiRecorder(int num, int denom, int key, int midi_pgm,
		                        int tempo_inverse, int metro_volume, bool triplet, bool f_piano, bool dnt_split);
		virtual ~NedMidiRecorder();
		virtual bool import();
		virtual bool convertToScore() {return true;}
		virtual const char *getErrorString() {return m_error_string;}
		void startMetronom();
		void stopMetronom();
	private:
		static gboolean OnMetronomTick(void *data);
		void process_midi_event(unsigned long long midi_time);
		const char *m_error_string;
		unsigned long long m_current_time;
		unsigned long long m_last_event_time;
		bool m_with_triplets;
		NedIdiotsEditor *m_idiots_editor;
		int m_current_midi_instrument[16];
		bool m_metronom_running;
		unsigned int m_metronom_count;
		struct timeval m_last_recorder_metronom_tick, m_next_metronome_tick;
		bool m_record_started;
		int m_num, m_denom;
		int m_midi_pgm, m_tempo_inverse, m_metro_volume;
		struct timeval m_metronom_intervall;
		bool m_triplet_recognition;
		int m_number_of_midi_poll_fds;
		unsigned long long m_intervall_long;
};

#endif /* MIDI_RECORDER_H */
