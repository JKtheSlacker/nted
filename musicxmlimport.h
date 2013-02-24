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

#ifndef MUSICXML_IMPORT_H

#define MUSICXML_IMPORT_H
#include "config.h"
#include "resource.h"
#include "importer.h"
#include <stdio.h>

#include <gtk/gtk.h>
#define MAX_STATES 200
#define DEFAULT_STRING_LENGTH 512
#define MAXPARTS 32

class NedMusicXMLImport : public NedImporter {
	public:
		virtual ~NedMusicXMLImport() {};
		virtual bool import();
		virtual bool convertToScore() {return true;}
        	GError *getError() {return m_error;}
		virtual const char *getErrorString() {return m_error_string;}
	private:
		GMarkupParseContext* m_parser_context;
		void produceSyntaxErrorString();
		void produceSementicErrorString(char *semantic_error);
		bool setCurrentPart(char *partid);
		void removeEmptyParts();
		int pitchToLine();
		static void startTag (GMarkupParseContext *context,
                          const gchar         *element_name,
                          const gchar        **attribute_names,
                          const gchar        **attribute_values,
                          gpointer             user_data,
                          GError             **error);

		static void endTag (GMarkupParseContext *context,
                          const gchar         *element_name,
                          gpointer             user_data,
                          GError             **error);

		static void internal_text(GMarkupParseContext *context,
                          const gchar         *text,
                          gsize                text_len,  
                          gpointer             user_data,
                          GError             **error);

		static void errorHandler(GMarkupParseContext *context,
                          GError              *error,
                          gpointer             user_data);

		void emtpy_state_stack();
		void push_state(int state);
		void determineClef();
		void determineTimeSignature();
		void determineKeySignature();
		void determineLength(int *len, int *dotcount);
		void determineUpBeat();
		void determineGrace(int *length, char *type, bool splash);
		unsigned int determineState();
		void tie(NedChordOrRest *chord, NedNote *note);
		int pop_state();
		int getState();
		bool check_state(int st1, int st2 = -1, int st3 = -1, int st4 = -1);
		bool getAttr(const char *attr, char *res, const char **attribute_names, const char **attribute_values);
		int m_indent;
		
		int m_states[MAX_STATES];
		int m_state_ptr;
		bool m_semantic_error;
        	GError *m_error;
		char m_lyrics[MAX_LYRICS_LINES][DEFAULT_STRING_LENGTH];
		char m_error_string[DEFAULT_STRING_LENGTH];
		part *m_current_part;
		int m_lyric_number;
		int m_current_voice;
		bool m_first_measure;
		bool m_has_splash;
		bool m_is_grace;
		bool m_staccato;
		bool m_staccatissimo;
		bool m_strong_accent;
		bool m_tenuto;
		bool m_accent;
		bool m_trill;
		bool m_pedal_off;
		bool m_pedal_on;
		char m_note_type[DEFAULT_STRING_LENGTH];

		char m_pitch_name[DEFAULT_STRING_LENGTH];
		char m_octave_name[DEFAULT_STRING_LENGTH];
		char m_clefsign[DEFAULT_STRING_LENGTH];
		int m_current_measure_number;
		int m_clefline;
		int m_clefoctave;
		int m_duration;
		int m_beats;
		int m_beat_type;
		int m_keysig;
		int m_actual_notes;
		int m_normal_notes;
		int m_alter;
		bool m_is_rest;
		bool m_is_chord_member;
		bool m_tie_stop;
};

#endif /* MUSICXML_IMPORT_H */
