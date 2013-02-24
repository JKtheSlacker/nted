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

#ifndef IMPORTER_H

#define IMPORTER_H
#include "resource.h"
#include <stdio.h>

#include <gtk/gtk.h>
#define DEFAULT_STRING_LENGTH 512
#define MAXPARTS 32

class NedChordOrRest;
class NedMainWindow;

class part {
	public:
		part();
		void init();
		bool is_eof();
		char id[DEFAULT_STRING_LENGTH];
		char name[DEFAULT_STRING_LENGTH];
		void copy_from(part *other_part);
		int divisions;
		int num_staves;
		int volume;
		int clef, current_clef;
		int clef_octave, current_clef_octave;
		bool first_clef;
		int key_signature, current_key_signature;
		bool first_key_signature;
		int numerator, current_numerator;
		int old_numerator, old_current_numerator;
		int denominator, current_denominator;
		bool first_time_signature;
		int midi_pgm;
		int midi_channel;
		NedChordOrRest *last_chord[VOICE_COUNT];
		GList *voices[VOICE_COUNT];
		GList *start_of_last_taken[VOICE_COUNT], *end_of_last_taken[VOICE_COUNT];
		int last_keysig[VOICE_COUNT];
		bool eof[VOICE_COUNT];
		void resetTaken();
		bool isEmpty();
};

class NedImporter {
	public:
		NedImporter();
		void setImportParameters(NedMainWindow *mainwindow, FILE *fp) {
			m_main_window = mainwindow; m_FILE = fp;
		}
		virtual ~NedImporter() {};
		virtual bool import() = 0;
		virtual bool convertToScore() = 0;
		virtual const char *getErrorString() = 0;
		bool is_eof();
		void resetTaken();
		part m_parts[MAXPARTS];
		int m_partcount;
		unsigned int m_upbeat_inverse;
		double m_tempo_inverse;

	protected:
		NedMainWindow *m_main_window;
		FILE *m_FILE;
};

#endif /* MUSICXML_IMPORT_H */
