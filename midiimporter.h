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

#ifndef MIDI_IMPORTER_H

#define MIDI_IMPORTER_H

#include <stdio.h>
#include <gtk/gtk.h>
#include "importer.h"

class NedIdiotsEditor;

class NedMidiImporter : public NedImporter  {
	public:
		NedMidiImporter(bool with_triplets, int tempo_change_density, int volume_change_density, bool force_piano, bool sort_instruments, bool dont_split, bool use_id_ed);
		virtual ~NedMidiImporter();
		virtual bool import();
		virtual bool convertToScore();
		virtual const char *getErrorString() {return m_error_string;}
	private:
		void read_header(FILE *fp);
		void readTrack(FILE *fp, int track_nr, int pass);
		unsigned long read_val_var(FILE *fp, int *len);
		int readdword(FILE *fp, int *data);
		int readword(FILE *fp, short *data);
		const char *m_error_string;
		short m_format, m_ntracks, m_division;
		unsigned long long m_current_time;
		unsigned long long m_last_event_time;
		bool m_with_triplets;
		NedIdiotsEditor *m_idiots_editor;
		int m_current_midi_instrument[16];
		bool m_use_id_ed;
};

#endif /* MIDI_IMPORTER_H */
