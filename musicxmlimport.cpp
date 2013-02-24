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

#include <string.h>
#include "localization.h"
#include "musicxmlimport.h"
#include "chordorrest.h"
#include "mainwindow.h"
#include "note.h"

#define INC_INDENT 3

#define XML_STATE_INIT 0
#define XML_STATE_NOTE 1
#define XML_STATE_PITCH 2
#define XML_STATE_STEP 3
#define XML_STATE_OCTAVE 4
#define XML_STATE_VOICE 5
#define XML_STATE_DURATION 6
#define XML_STATE_TIME_MODIFICATION 7
#define XML_STATE_ACTUAL_NOTES 8
#define XML_STATE_NORMAL_NOTES 9
#define XML_STATE_TIE_TYPE 10
#define XML_STATE_PART_LIST 11
#define XML_STATE_SCORE_PART 12
#define XML_STATE_PART_NAME 13
#define XML_STATE_PART 14
#define XML_STATE_MEASURE 15
#define XML_STATE_ATTRIBUTES 16
#define XML_STATE_DIVISIONS 17
#define XML_STATE_KEY 18
#define XML_STATE_FIFTHS 19
#define XML_STATE_TIME 20
#define XML_STATE_BEATS 21
#define XML_STATE_BEAT_TYPE 22
#define XML_STATE_CLEF 23
#define XML_STATE_SIGN 24
#define XML_STATE_LINE 25
#define XML_STATE_MIDI_INSTRUMENT 26
#define XML_STATE_MIDI_PGM 27
#define XML_STATE_MIDI_CHANNEL 28
#define XML_STATE_BACKUP 29
#define XML_STATE_FORWARD 30
#define XML_STATE_ALTER 31
#define XML_STATE_ACCIDENTAL 32
#define XML_STATE_BARLINE 33
#define XML_STATE_REPEAT 34
#define XML_STATE_ENDING 35
#define XML_STATE_LYRIC 36
#define XML_STATE_TEXT 37
#define XML_STATE_TYPE 38
#define XML_STATE_CLEF_OCTAVE_CHANGE 39
#define XML_STATE_ARTICULATIONS 40
#define XML_STATE_VOLUME 41
#define XML_STATE_DIRECTION 42
#define XML_STATE_DIR_TYPE 43
#define XML_STATE_METRONOME 44
#define XML_STATE_BEAT_UNIT 45
#define XML_STATE_PER_MINUTE 46

#define XML_TIE_TYPE_NONE 0
#define XML_TIE_TYPE_START 1
#define XML_TIE_TYPE_STOP 2

#define CHOOSE_HEAD(line) ((im->m_current_part->midi_channel == 9) ? (((line + OFFSET_TO_BE_USED_DURING_MIDI_IMPORT) % COUNT_OF_DRUMS_TO_CHOOSE_DURING_MIDI_IMPORT) + 1) : NORMAL_NOTE ) 

bool NedMusicXMLImport::import() {
#define BUFLEN 4096
	GMarkupParser parser;
	char buffer[BUFLEN];
	int len;
        m_error = NULL;
	m_error_string[0] = '\0';
	m_current_part = NULL;
	m_current_voice = 0;
	m_duration = -1;
	m_upbeat_inverse = 1;
	m_first_measure = true;
	m_beats = m_beat_type = 4;

	m_semantic_error = false;
	m_partcount = 0;
	emtpy_state_stack();
	push_state(XML_STATE_INIT);

	parser.start_element = startTag;
	parser.end_element = endTag;
	parser.text = internal_text;
	parser.passthrough = NULL;
	parser.error = NULL;

	m_parser_context = g_markup_parse_context_new(&parser, (GMarkupParseFlags) 0, (void *) this, NULL);

	len = fread(buffer, 1, BUFLEN, m_FILE);
	while (len > 0 && !m_semantic_error) {
		if (len > 0 && !m_semantic_error) {
			if (!g_markup_parse_context_parse(m_parser_context, buffer, len, &m_error)) {
				produceSyntaxErrorString();
				return false;
			}
		}
		len = fread(buffer, 1, BUFLEN, m_FILE);
	}
	if (!g_markup_parse_context_end_parse(m_parser_context, &m_error)) {
		produceSyntaxErrorString();
		return false;
	}
	g_markup_parse_context_free(m_parser_context);
	/*
	for (int i = 0; i < m_partcount; i++) {
		NedResource::DbgMsg(DBG_TESTING, "Part %d: id = %s, name = %s, divisions = %d, midi_pgm = %d, midi_channel = %d clef = ",
		 i, m_parts[i].id, m_parts[i].name, m_parts[i].divisions,
		m_parts[i].midi_pgm, m_parts[i].midi_channel);
		switch(m_parts[i].clef) {
			case TREBLE_CLEF: NedResource::DbgMsg(DBG_TESTING, "TREBLE_CLEF\n"); break; 
			case ALTO_CLEF: NedResource::DbgMsg(DBG_TESTING, "ALTO_CLEF\n"); break; 
			case BASS_CLEF: NedResource::DbgMsg(DBG_TESTING, "BASS_CLEF\n"); break; 
			case NEUTRAL_CLEF1: NedResource::DbgMsg(DBG_TESTING, "NEUTRAL_CLEF1\n"); break;
			case NEUTRAL_CLEF2: NedResource::DbgMsg(DBG_TESTING, "NEUTRAL_CLEF2\n"); break;
		}
		NedResource::DbgMsg(DBG_TESTING, "num = %d, denom = %d, keysig = %d\n",
			 m_parts[i].numerator, m_parts[i].denominator, m_parts[i].key_signature);
		
	}
	*/
	if (m_upbeat_inverse ==  1) m_upbeat_inverse = 0; 
	removeEmptyParts();
	return !m_semantic_error;
}


void NedMusicXMLImport::produceSyntaxErrorString() {
	char Str[DEFAULT_STRING_LENGTH - 20];
	strncpy(Str, m_error->message, DEFAULT_STRING_LENGTH - 20 - 1);
	Str[DEFAULT_STRING_LENGTH - 20 - 1] = '\0';
	//sprintf(m_error_string, "line %d:%d:", line, column);
	if (!m_semantic_error) {
		sprintf(m_error_string, "%s (%d)", Str, m_error->code);
	}
}

void NedMusicXMLImport::produceSementicErrorString(char *semantic_error) {
	int line, column;
	g_markup_parse_context_get_position(m_parser_context, &line, &column);
	printf("line %d:%d: %s", line, column, semantic_error);
	m_semantic_error = true;
}

int NedMusicXMLImport::pitchToLine() {
	int line, line_offs = 0, line_add = 0;
	char Str[128];
	if (m_current_part == NULL) {
		NedResource::Abort("NedMusicXMLImport::pitchToLine");
	}
	switch (m_current_part->current_clef) {
		case ALTO_CLEF: line_offs = 6; break;
		case SOPRAN_CLEF: line_offs = 2; break;
		case TENOR_CLEF: line_offs = 8; break;
		case BASS_CLEF: 
		case NEUTRAL_CLEF2: line_offs = 12; break;
	}
	if (m_pitch_name[1] != '\0') {
		sprintf(Str, _("unknown pitch: %s"), m_pitch_name);
		produceSementicErrorString(Str);
		return UNKNOWN_LINE;
	}
	if (m_octave_name[1] != '\0' || m_octave_name[0] < '0' || m_octave_name[0] > '7') {
		sprintf(Str, _("unknown octave: %s"), m_octave_name);
		produceSementicErrorString(Str);
		return UNKNOWN_LINE;
	}
	switch(m_pitch_name[0]) {
		case 'C': case 'c': line = -2; break;
		case 'D': case 'd': line = -1; break;
		case 'E': case 'e': line =  0; break;
		case 'F': case 'f': line =  1; break;
		case 'G': case 'g': line =  2; break;
		case 'A': case 'a': line =  3; break;
		case 'B': case 'b': line =  4; break;
		default: sprintf(Str, _("unknown pitch: %s"), m_pitch_name);
			produceSementicErrorString(Str);
			return UNKNOWN_LINE;
	}
	switch (m_current_part->current_clef_octave) {
		case 12: line_add = -7; break;
		case -12: line_add = 7; break;
	}
	return line + line_offs + (m_octave_name[0] - '0' - 4) * 7 + line_add;
}


void NedMusicXMLImport::startTag (GMarkupParseContext *context,
                          const gchar         *element_name,
                          const gchar        **attribute_names,
                          const gchar        **attribute_values,
                          gpointer             user_data,
                          GError             **error) {
	NedMusicXMLImport *im = (NedMusicXMLImport *) user_data;
	if (im->m_semantic_error) return;
	char buffer[DEFAULT_STRING_LENGTH];
	char Str[DEFAULT_STRING_LENGTH];

/*
	int i, j;
	for (i = 0; i < im->m_indent; i++) putchar(' ');
	NedResource::DbgMsg(DBG_TESTING, "Start: %s\n", element_name);
	j = 0;
	while (attribute_names[j] != NULL) {
		for (i = 0; i < im->m_indent+1; i++) NedResource::DbgMsg(DBG_TESTING, " ");
		NedResource::DbgMsg(DBG_TESTING, "[ %s : %s ]\n", attribute_names[j], attribute_values[j]);
		j++;
	}

	
*/
	if (!strcmp(element_name, "note")) {
		im->push_state(XML_STATE_NOTE);
		im->m_current_voice = 0;
		im->m_duration = -1;
		im->m_is_rest = false;
		im->m_tie_stop = false;
		im->m_is_grace = false;
		im->m_has_splash = false;
		im->m_staccato = false;
		im->m_staccatissimo = false;
		im->m_trill = false;
		im->m_strong_accent = false;
		im->m_tenuto = false;
		im->m_accent = false;
		im->m_note_type[0] = '\0';
		im->m_is_chord_member = false;
		im->m_alter = UNKNOWN_LINE;
		im->m_actual_notes = im->m_normal_notes = -1;
		for (int i = 0; i < MAX_LYRICS_LINES; im->m_lyrics[i++][0] = '\0');
	}
	else if (!strcmp(element_name, "pitch")) {
		im->push_state(XML_STATE_PITCH);
		im->m_pitch_name[0] = '\0';
		im->m_octave_name[0] = '\0';
	}
	else if (!strcmp(element_name, "backup")) {
		im->push_state(XML_STATE_BACKUP);
		im->m_duration = -1;
	}
	else if (!strcmp(element_name, "forward")) {
		im->push_state(XML_STATE_FORWARD);
		im->m_duration = -1;
	}
	else if (!strcmp(element_name, "step")) {
		im->push_state(XML_STATE_STEP);
	}
	else if (!strcmp(element_name, "octave")) {
		im->push_state(XML_STATE_OCTAVE);
	}
	else if (!strcmp(element_name, "articulations")) {
		im->push_state(XML_STATE_ARTICULATIONS);
	}
	else if (!strcmp(element_name, "staccato")) {
		if (!im->check_state(XML_STATE_ARTICULATIONS, XML_STATE_NOTE)) {
			im->produceSementicErrorString(_("\"articulations\" outside \"note\""));
			return;
		}
		im->m_staccato = true;
	}
	else if (!strcmp(element_name, "staccatissimo")) {
		if (!im->check_state(XML_STATE_ARTICULATIONS, XML_STATE_NOTE)) {
			im->produceSementicErrorString(_("\"articulations\" outside \"note\""));
			return;
		}
		im->m_staccatissimo = true;
	}
	else if (!strcmp(element_name, "strong-accent")) {
		if (!im->check_state(XML_STATE_ARTICULATIONS, XML_STATE_NOTE)) {
			im->produceSementicErrorString(_("\"strong-accent\" outside \"note\""));
			return;
		}
		im->m_strong_accent = true;
	}
	else if (!strcmp(element_name, "tenuto")) {
		if (!im->check_state(XML_STATE_ARTICULATIONS, XML_STATE_NOTE)) {
			im->produceSementicErrorString(_("\"tenuto\" outside \"note\""));
			return;
		}
		im->m_tenuto = true;
	}
	else if (!strcmp(element_name, "accent")) {
		if (!im->check_state(XML_STATE_ARTICULATIONS, XML_STATE_NOTE)) {
			im->produceSementicErrorString(_("\"accent\" outside \"note\""));
			return;
		}
		im->m_accent = true;
	}
	else if (!strcmp(element_name, "trill-mark")) {
		im->m_trill = true;
	}
	else if (!strcmp(element_name, "pedal")) {
		if (!im->getAttr("type", buffer, attribute_names, attribute_values)) {
			im->produceSementicErrorString(_("\"pedal\" without \"type\""));
			return;
		}
		if (!strcmp(buffer, "start")) {
			im->m_pedal_on = true;
		}
		else if (!strcmp(buffer, "stop")) {
			im->m_pedal_off = true;
		}
		else {
			im->produceSementicErrorString(_("\"pedal\" --> \"type\" is bad"));
			return;
		}
			
	}
	else if (!strcmp(element_name, "alter")) {
		im->push_state(XML_STATE_ALTER);
	}
	else if (!strcmp(element_name, "rest")) {
		im->m_is_rest = true;
	}
	else if (!strcmp(element_name, "chord")) {
		im->m_is_chord_member = true;
	}
	else if (!strcmp(element_name, "grace")) {
		im->m_is_grace = true;
		if (!im->getAttr("splash", buffer, attribute_names, attribute_values)) {
			if (!strcmp("yes", buffer)) {
				im->m_has_splash = true;
			}
		}
	}
	else if (!strcmp(element_name, "type")) {
		im->push_state(XML_STATE_TYPE);
	}
	else if (!strcmp(element_name, "tie")) {
		if (!im->getAttr("type", buffer, attribute_names, attribute_values)) {
			im->produceSementicErrorString(_("\"tie\" without \"type\""));
			return;
		}
		if (!strcmp(buffer, "stop")) {
			im->m_tie_stop = true;
		}
	}
	else if (!strcmp(element_name, "duration")) {
		im->push_state(XML_STATE_DURATION);
	}
	else if (!strcmp(element_name, "part")) {
		im->push_state(XML_STATE_PART);
		if (!im->getAttr("id", buffer, attribute_names, attribute_values)) {
			im->produceSementicErrorString(_("\"part\" without \"id\""));
			return;
		}
		if (!im->setCurrentPart(buffer)) {
			sprintf(Str, _("part \"%s\" not defined"), buffer);
			im->produceSementicErrorString(Str);
			return;
		}
		im->m_pedal_off = false;
		im->m_pedal_on = false;
		
	}
	else if (!strcmp(element_name, "midi-instrument")) {
		im->push_state(XML_STATE_MIDI_INSTRUMENT);
	}
	else if (!strcmp(element_name, "direction")) {
		im->push_state(XML_STATE_DIRECTION);
	}
	else if (!strcmp(element_name, "direction-type")) {
		im->push_state(XML_STATE_DIR_TYPE);
	}
	else if (!strcmp(element_name, "metronome")) {
		im->push_state(XML_STATE_METRONOME);
	}
	else if (!strcmp(element_name, "beat-unit")) {
		im->push_state(XML_STATE_BEAT_UNIT );
	}
	else if (!strcmp(element_name, "per-minute")) {
		im->push_state(XML_STATE_PER_MINUTE );
	}
	else if (!strcmp(element_name, "midi-program")) {
		im->push_state(XML_STATE_MIDI_PGM);
	}
	else if (!strcmp(element_name, "midi-channel")) {
		im->push_state(XML_STATE_MIDI_CHANNEL);
	}
	else if (!strcmp(element_name, "volume")) {
		im->push_state(XML_STATE_VOLUME);
	}
	else if (!strcmp(element_name, "measure")) {
		im->m_current_measure_number++;
		im->push_state(XML_STATE_MEASURE);
		if (!im->getAttr("number", buffer, attribute_names, attribute_values)) {
			im->produceSementicErrorString(_("\"measure\" without \"number\""));
			return;
		}
	}
	else if (!strcmp(element_name, "attributes")) {
		im->push_state(XML_STATE_ATTRIBUTES);
	}
	else if (!strcmp(element_name, "divisions")) {
		im->push_state(XML_STATE_DIVISIONS);
	}
	else if (!strcmp(element_name, "barline")) {
		im->push_state(XML_STATE_BARLINE);
	}
	else if (!strcmp(element_name, "repeat")) {
		im->push_state(XML_STATE_REPEAT);
		if (!im->getAttr("direction", buffer, attribute_names, attribute_values)) {
			im->produceSementicErrorString(_("\"repeat\" without \"number\""));
			return;
		}
		if (!strcmp(buffer, "forward")) {
			im->m_main_window->setSpecial(im->m_current_measure_number, REPEAT_OPEN, false, NULL, false);
		}
		else if (!strcmp(buffer, "backward")) {
			im->m_main_window->setSpecial(im->m_current_measure_number + 1, REPEAT_CLOSE, false, NULL, false);
		}
	}
	else if (!strcmp(element_name, "ending")) {
		im->push_state(XML_STATE_ENDING);
		if (!im->getAttr("type", buffer, attribute_names, attribute_values)) {
			im->produceSementicErrorString(_("\"ending\" without \"start\""));
			return;
		}
		if (!strcmp(buffer, "start")) {
			if (!im->getAttr("number", buffer, attribute_names, attribute_values)) {
				im->produceSementicErrorString(_("\"ending\" without \"number\""));
				return;
			}
			if (buffer[0] == '1' && buffer[1] == '\0') {
				im->m_main_window->setSpecial(im->m_current_measure_number, REP1START, false, NULL, false);
			}
			else if (buffer[0] == '2' && buffer[1] == '\0') {
				im->m_main_window->setSpecial(im->m_current_measure_number, REP2START, false, NULL, false);
				im->m_main_window->setSpecial(im->m_current_measure_number + 1, REP2END, false, NULL, false);
			}
		}
		else if (strcmp(buffer, "discontinue")) {
			if (buffer[0] == '1' && buffer[1] == '\0') {
				im->m_main_window->setSpecial(im->m_current_measure_number + 1, REP1END, false, NULL, false);
			}
			else if (buffer[0] == '2' && buffer[1] == '\0') {
				im->m_main_window->setSpecial(im->m_current_measure_number + 1, REP2END, false, NULL, false);
			}
		}
	}
	else if (!strcmp(element_name, "lyric")) {
		im->push_state(XML_STATE_LYRIC);
		if (!im->getAttr("number", buffer, attribute_names, attribute_values)) {
			im->produceSementicErrorString(_("\"lyric\" without \"number\""));
			return;
		}
		if (sscanf(buffer, "%d", &(im->m_lyric_number)) != 1) {
			sprintf(Str, _("bad lyricnumber: %s"), buffer);
			im->produceSementicErrorString(Str);
			return;
		}
		im->m_lyric_number--;
	}
	else if (!strcmp(element_name, "text")) {
		im->push_state(XML_STATE_TEXT);
	}
	else if (!strcmp(element_name, "key")) {
		im->push_state(XML_STATE_KEY);
		im->m_keysig = -1000;
	}
	else if (!strcmp(element_name, "fifths")) {
		im->push_state(XML_STATE_FIFTHS);
	}
	else if (!strcmp(element_name, "time")) {
		im->push_state(XML_STATE_TIME);
		im->m_beats = im->m_beat_type = -1;
	}
	else if (!strcmp(element_name, "beats")) {
		im->push_state(XML_STATE_BEATS);
	}
	else if (!strcmp(element_name, "beat-type")) {
		im->push_state(XML_STATE_BEAT_TYPE);
	}
	else if (!strcmp(element_name, "clef")) {
		im->push_state(XML_STATE_CLEF);
		im->m_clefsign[0] = '\0';
		im->m_clefline = -1;
		im->m_clefoctave = 0;
	}
	else if (!strcmp(element_name, "clef-octave-change")) {
		im->push_state(XML_STATE_CLEF_OCTAVE_CHANGE);
	}
	else if (!strcmp(element_name, "sign")) {
		im->push_state(XML_STATE_SIGN);
	}
	else if (!strcmp(element_name, "line")) {
		im->push_state(XML_STATE_LINE);
	}
	else if (!strcmp(element_name, "voice")) {
		im->push_state(XML_STATE_VOICE);
	}
	else if (!strcmp(element_name, "time-modification")) {
		im->push_state(XML_STATE_TIME_MODIFICATION);
	}
	else if (!strcmp(element_name, "actual-notes")) {
		im->push_state(XML_STATE_ACTUAL_NOTES);
	}
	else if (!strcmp(element_name, "normal-notes")) {
		im->push_state(XML_STATE_NORMAL_NOTES);
	}
	else if (!strcmp(element_name, "accidental")) {
		im->push_state(XML_STATE_ACCIDENTAL);
	}
	else if (!strcmp(element_name, "part-list")) {
		im->push_state(XML_STATE_PART_LIST);
	}
	else if (!strcmp(element_name, "score-part")) {
		if (!im->check_state(XML_STATE_PART_LIST)) {
			im->produceSementicErrorString(_("\"score-part\" outside \"part-list\""));
                        return;
		}
		im->push_state(XML_STATE_SCORE_PART);
		if (im->m_partcount >= MAXPARTS) return;
		im->m_current_part = &(im->m_parts[im->m_partcount]);
		im->m_partcount++;
		if (!im->getAttr((char *) "id", im->m_parts[im->m_partcount-1].id, attribute_names, attribute_values)) {
			im->produceSementicErrorString(_("\"score-part\" without \"id\""));
			return;
		}
	}
	else if (!strcmp(element_name, "part-name")) {
		if (im->m_partcount < MAXPARTS) {
			if (!im->check_state(XML_STATE_SCORE_PART, XML_STATE_PART_LIST)) {
				im->produceSementicErrorString(_("\"part-name\" outside \"part-list\""));
                        	return;
			}
		}
		im->push_state(XML_STATE_PART_NAME);
	}
	//im->m_indent += INC_INDENT;
}

void NedMusicXMLImport::endTag (GMarkupParseContext *context,
                          const gchar         *element_name,
                          gpointer             user_data,
                          GError             **error) {
	NedMusicXMLImport *im = (NedMusicXMLImport *) user_data;
	int line, length, dotcount;
	unsigned int state;
	int dummy;
	unsigned int dummy2;
	NedChordOrRest *chord_or_rest = NULL;
	if (im->m_semantic_error) return;
/*
	int i;
	im->m_indent -= INC_INDENT;
	for (i = 0; i < im->m_indent; i++) putchar(' ');
	NedResource::DbgMsg(DBG_TESTING, "Ende: %s\n", element_name); 
*/
	if (!strcmp(element_name, "note")) {
		im->pop_state();
		if (im->m_current_part - im->m_parts >= MAXPARTS) return;
		if (im->m_current_voice < 0 || im->m_current_voice >= VOICE_COUNT) {
			//sprintf(Str, _("voice number: %d is not supported"), im->m_current_voice + 1);
			NedResource::DbgMsg(DBG_CRITICAL, _("voice number: %d is not supported"), im->m_current_voice + 1);
			//im->produceSementicErrorString(Str);
			return;
		}
		if (im->m_is_grace) {
			im->determineGrace(&length, im->m_note_type, im->m_has_splash);
			if ((line = im->pitchToLine()) != UNKNOWN_LINE) {
				im->m_current_part->voices[im->m_current_voice] = g_list_append(
			   	im->m_current_part->voices[im->m_current_voice], 
					im->m_current_part->last_chord[im->m_current_voice] = chord_or_rest = new NedChordOrRest(NULL, TYPE_GRACE, FALSE, line, 0, length, NORMAL_NOTE, 0, 0));
			}
			return;
		}
		im->determineLength(&length, &dotcount);
/*
		if (im->m_actual_notes > 0) {
			NedResource::DbgMsg(DBG_TESTING, "TOL: %d/%d, ", im->m_actual_notes, im->m_normal_notes);
		}
		if (im->m_is_rest) {
			NedResource::DbgMsg(DBG_TESTING, "Rest: length = %u(%u), dotcount = %d, voice = %d\n", length, length/NOTE_8, dotcount, im->m_current_voice);
		}
		else if ((line = im->pitchToLine()) != UNKNOWN_LINE) {
			NedResource::DbgMsg(DBG_TESTING, "length = %u(%u), dotcount = %d, line = %d, chord_member = %d, voice = %d\n",
				 length, length/NOTE_8, dotcount, line, im->m_is_chord_member, im->m_current_voice);
		}
		
*/
		if (im->m_is_chord_member) {
			if (im->m_current_part->last_chord[im->m_current_voice] == NULL) {
				im->produceSementicErrorString((char *) "standalone chord");
				return;
			}
			state = im->determineState();
			if ((line = im->pitchToLine()) == UNKNOWN_LINE) return;
			NedNote *note = new NedNote(im->m_current_part->last_chord[im->m_current_voice], line, CHOOSE_HEAD(line), state);
			im->m_current_part->last_chord[im->m_current_voice]->insertNoteAt(note);
			if (im->m_tie_stop && im->m_current_voice >= 0 && im->m_current_voice < VOICE_COUNT) {
				im->tie(im->m_current_part->last_chord[im->m_current_voice], note);
			}
			return;
		}
		if (im->m_is_rest) {
			im->m_current_part->last_chord[im->m_current_voice] = NULL;
			im->m_current_part->voices[im->m_current_voice] = g_list_append(
			   im->m_current_part->voices[im->m_current_voice], 
			     chord_or_rest = new NedChordOrRest(NULL, TYPE_REST, FALSE, 3, dotcount, length, NORMAL_NOTE, 0, 0));
		}
		else if ((line = im->pitchToLine()) != UNKNOWN_LINE) {
			state = im->determineState();
			im->m_current_part->voices[im->m_current_voice] = g_list_append(
			   im->m_current_part->voices[im->m_current_voice], 
				im->m_current_part->last_chord[im->m_current_voice] = chord_or_rest = new NedChordOrRest(NULL, TYPE_CHORD, FALSE, line, dotcount, length, CHOOSE_HEAD(line), state, 0));
				for (int i = 0; i < MAX_LYRICS_LINES; i++) {
					chord_or_rest->setLyricsFromOldContext(im->m_main_window, i, im->m_lyrics[i]);
				}

		}
		if (im->m_tie_stop  && im->m_current_voice >= 0 && im->m_current_voice < VOICE_COUNT) {
			im->tie(im->m_current_part->last_chord[im->m_current_voice], im->m_current_part->last_chord[im->m_current_voice]->getFirstNote());
		}
		if  (im->m_actual_notes >= 0) {
			chord_or_rest->setTupletVal(im->m_actual_notes);
		}
	}
	else if (!strcmp(element_name, "forward")) {
		im->pop_state();
		if (im->m_current_part - im->m_parts >= MAXPARTS) return;
		if (im->m_current_voice < 0 || im->m_current_voice >= VOICE_COUNT) return;
		im->determineLength(&length, &dotcount);
		im->m_current_part->voices[im->m_current_voice] = g_list_append(
			im->m_current_part->voices[im->m_current_voice],
				new NedChordOrRest(NULL, TYPE_REST, TRUE, 3, dotcount, length, NORMAL_NOTE, 0, 0));
	}
	else if (!strcmp(element_name, "part-list")) {
		im->pop_state();
	}
	else if (!strcmp(element_name, "part") || !strcmp(element_name, "score-part")) {
		im->m_current_measure_number = 0;
		im->pop_state();
		im->m_current_part = NULL;
	}
	else if (!strcmp(element_name, "clef")) {
		im->pop_state();
		if (im->m_current_voice < 0 || im->m_current_voice >= VOICE_COUNT) return;
		im->determineClef();
		if (!im->m_current_part->first_clef) {
			if (im->m_current_part - im->m_parts <  MAXPARTS) {
				im->m_current_part->voices[im->m_current_voice] = g_list_append(
				im->m_current_part->voices[im->m_current_voice],
					new NedChordOrRest(NULL, TYPE_CLEF, im->m_current_part->current_clef, im->m_clefoctave, 0));
			}
		}
		im->m_current_part->first_clef = false;
	}
	else if (!strcmp(element_name, "time")) {
		im->pop_state();
		im->determineTimeSignature();
		if (!im->m_current_part->first_time_signature) {
			im->m_main_window->setSpecialTimesig(im->m_current_measure_number, im->m_current_part->current_numerator, 
				im->m_current_part->current_denominator, TIME_SYMBOL_NONE, &dummy, &dummy, &dummy2); // friend !!
		}
		im->m_current_part->first_time_signature = false;

	}
	else if (!strcmp(element_name, "key")) {
		im->pop_state();
		if (im->m_current_part - im->m_parts >= MAXPARTS) return;
		if (im->m_current_voice < 0 || im->m_current_voice >= VOICE_COUNT) return;
		im->determineKeySignature();
		if (!im->m_current_part->first_key_signature) {
			if (im->m_current_part - im->m_parts <  MAXPARTS) {
				im->m_current_part->voices[im->m_current_voice] = g_list_append(
				im->m_current_part->voices[im->m_current_voice],
					new NedChordOrRest(NULL, TYPE_KEYSIG, im->m_keysig, im->m_current_part->last_keysig[im->m_current_voice], 0));
			}
		}
		im->m_current_part->first_key_signature = false;
		im->m_current_part->last_keysig[im->m_current_voice] = im->m_keysig;
	} 
	else if (!strcmp(element_name, "measure")) {
		im->pop_state();
		if (im->m_first_measure) {
			im->m_first_measure = false;
			im->determineUpBeat();
		}
	}
	else if (!strcmp(element_name, "step") || !strcmp(element_name, "octave") ||
			!strcmp(element_name, "duration") || !strcmp(element_name, "voice") ||
			!strcmp(element_name, "pitch") || !strcmp(element_name, "time-modification") ||
			!strcmp(element_name, "actual-notes") || !strcmp(element_name, "normal-notes") ||
			!strcmp(element_name, "part-name") || !strcmp(element_name, "accidental") ||
			!strcmp(element_name, "attributes") || !strcmp(element_name, "divisions") ||
			!strcmp(element_name, "sign") || !strcmp(element_name, "fifths") ||
			!strcmp(element_name, "beats") || !strcmp(element_name, "beat-type") ||
			!strcmp(element_name, "line") || !strcmp(element_name, "voice") ||
			!strcmp(element_name, "time-modification") || !strcmp(element_name, "midi-instrument") ||
			!strcmp(element_name, "midi-program") || !strcmp(element_name, "midi-channel") ||
			!strcmp(element_name, "backup") || !strcmp(element_name, "alter") ||
			!strcmp(element_name, "barline") || !strcmp(element_name, "repeat") ||
			!strcmp(element_name, "ending") || !strcmp(element_name, "lyric") ||
			!strcmp(element_name, "text") || !strcmp(element_name, "type") ||
			!strcmp(element_name, "clef-octave-change") || !strcmp(element_name, "articulations") ||
			!strcmp(element_name, "volume") || !strcmp(element_name, "direction") ||
			!strcmp(element_name, "direction-type") || !strcmp(element_name, "metronome") ||
			!strcmp(element_name, "beat-unit") || !strcmp(element_name, "per-minute")) {
		im->pop_state();
	}
}

void NedMusicXMLImport::internal_text(GMarkupParseContext *context,
                          const gchar         *text,
                          gsize                text_len,  
                          gpointer             user_data,
                          GError             **error) {
	NedMusicXMLImport *im = (NedMusicXMLImport *) user_data;
	if (im->m_semantic_error) return;
	char Str[DEFAULT_STRING_LENGTH];
	int vol, tempo;
/*
	int i;
	NedResource::DbgMsg(DBG_TESTING, "TEXT(0): %s, state = %d\n", text, im->getState()); 
	char *cptr1, *cptr2;
	if (text_len > 0) return;
	for (cptr1 = (char *) text; *cptr1 <= ' ' && *cptr1 != '\0'; cptr1++);
	for (cptr2 = (char *) text + text_len; *cptr2 <= ' ' && cptr2 > cptr1; cptr2--);
	cptr2[1] = '\0';
	if (strlen(cptr1) < 1) return;
	for (i = 0; i < im->m_indent; i++) putchar(' ');
	NedResource::DbgMsg(DBG_TESTING, "TEXT: %s\n", cptr1); 
	NedResource::DbgMsg(DBG_TESTING, "TEXT: %s, state = %d\n", cptr1, im->getState()); 
*/
	switch(im->getState()) {
		case XML_STATE_STEP: if (im->m_pitch_name[0] != '\0') {
					im->produceSementicErrorString(_("double \"step\""));
					return;
				     }
				     strcpy(im->m_pitch_name, text);
				     break;
		case XML_STATE_OCTAVE: if (im->m_octave_name[0] != '\0') {
					im->produceSementicErrorString(_("double \"octave\""));
					return;
				     }
				     strcpy(im->m_octave_name, text);
				     break;
		case XML_STATE_DURATION: 
				     if (im->m_duration >= 0) {
					NedResource::DbgMsg(DBG_TESTING, "text = %s\n", text);
					im->produceSementicErrorString(_("double \"duration\""));
					return;
				     }
				     if (sscanf(text, "%d", &(im->m_duration)) != 1) {
					im->produceSementicErrorString(_("bad duration"));
					return;
				     }
				     break;
		case XML_STATE_PART_NAME:
					if (im->m_partcount >= MAXPARTS) break;
					if (im->m_partcount < 1) {
						NedResource::Abort("XML_STATE_PART_NAME: m_partcount < 1");
					}
					strcpy(im->m_parts[im->m_partcount-1].name, text);
					break;
		case XML_STATE_VOICE: 
				      if (im->m_current_part == NULL) {
					NedResource::Abort("im->m_current_part == NULL(1)");
				      }
				      if (sscanf(text, "%d", &(im->m_current_voice)) != 1) {
					sprintf(Str, _("Bad voice number: %s"), text);
					im->produceSementicErrorString(Str);
				      }
				      im->m_current_voice--;
				      break;
		case XML_STATE_DIVISIONS: if (!im->check_state(XML_STATE_DIVISIONS, XML_STATE_ATTRIBUTES, XML_STATE_MEASURE, XML_STATE_PART)) {
					im->produceSementicErrorString(_("\"divisions\" outside \"attributes\""));
					return;
				      }
				      if (im->m_current_part == NULL) {
					NedResource::Abort("im->m_current_part == NULL(2)");
				      }
				      if (sscanf(text, "%d", &(im->m_current_part->divisions)) != 1) {
					sprintf(Str, _("Bad divisions number: %s"), text);
					im->produceSementicErrorString(Str);
				      }
				      break;
		case XML_STATE_MIDI_PGM: if (!im->check_state(XML_STATE_MIDI_PGM, XML_STATE_MIDI_INSTRUMENT, XML_STATE_SCORE_PART)) {
					im->produceSementicErrorString(_("\"midi-programm\" outside \"midi-instrument\""));
					return;
				      }
				      if (im->m_partcount >= MAXPARTS) break;
				      if (im->m_current_part == NULL) {
					NedResource::Abort("im->m_current_part == NULL(3)");
				      }
				      if (sscanf(text, "%d", &(im->m_current_part->midi_pgm)) != 1) {
					sprintf(Str, _("Bad midi channel number: %s"), text);
					im->produceSementicErrorString(Str);
				      }
				      if (im->m_current_part->midi_pgm < 1 || im->m_current_part->midi_pgm > 128) {
					sprintf(Str, _("Bad midi-programm number: %d"), im->m_current_part->midi_pgm);
					im->produceSementicErrorString(Str);
				      }
				      im->m_current_part->midi_pgm--;
				      break;
		case XML_STATE_MIDI_CHANNEL: if (!im->check_state(XML_STATE_MIDI_CHANNEL, XML_STATE_MIDI_INSTRUMENT, XML_STATE_SCORE_PART)) {
					im->produceSementicErrorString(_("\"midi-channel\" outside \"midi-instrument\""));
					return;
				      }
				      if (im->m_partcount >= MAXPARTS) break;
				      if (im->m_current_part == NULL) {
					NedResource::Abort("im->m_current_part == NULL(4)");
				      }
				      if (sscanf(text, "%d", &(im->m_current_part->midi_channel)) != 1) {
					sprintf(Str, _("Bad midi channel number: %s"), text);
					im->produceSementicErrorString(Str);
				      }
				      if (im->m_current_part->midi_channel < 1 || im->m_current_part->midi_channel > 16) {
					sprintf(Str, _("Bad midi channel number: %d"), im->m_current_part->midi_channel);
					im->produceSementicErrorString(Str);
				      }
				      im->m_current_part->midi_channel--;
				      break;
		case XML_STATE_VOLUME: if (!im->check_state(XML_STATE_VOLUME, XML_STATE_MIDI_INSTRUMENT, XML_STATE_SCORE_PART)) {
					im->produceSementicErrorString(_("\"volume\" outside \"midi-instrument\""));
					return;
				      }
				      if (im->m_partcount >= MAXPARTS) break;
				      if (im->m_current_part == NULL) {
					NedResource::Abort("im->m_current_part == NULL(44)");
				      }
				      if (sscanf(text, "%d", &vol) != 1) {
					sprintf(Str, _("Bad midi volume number: %s"), text);
					im->produceSementicErrorString(Str);
				      }
				      if (vol < 0 || vol > 100) {
					sprintf(Str, _("Bad volume number: %d"), vol);
					im->produceSementicErrorString(Str);
				      }
				       im->m_current_part->volume = (int) ((double) vol * 127.0 / 100.0);
				      break;
		case XML_STATE_PER_MINUTE : if (!im->check_state(XML_STATE_PER_MINUTE , XML_STATE_METRONOME, XML_STATE_DIR_TYPE)) {
					im->produceSementicErrorString(_("\"per-minute\" outside \"XML_STATE_DIRECTION\""));
					return;
				      }
				      if (im->m_partcount >= MAXPARTS) break;
				      if (im->m_current_part == NULL) {
					NedResource::Abort("im->m_current_part == NULL(45)");
				      }
				      if (sscanf(text, "%d", &tempo) != 1) {
					sprintf(Str, _("Bad midi volume number: %s"), text);
					im->produceSementicErrorString(Str);
				      }
				      if (tempo < 20 || tempo > 300) {
					sprintf(Str, _("Bad temo value: %d"), tempo);
					im->produceSementicErrorString(Str);
				      }
				       im->m_tempo_inverse = 60000.0 / (double) tempo;
				      break;
		case XML_STATE_SIGN: if (!im->check_state(XML_STATE_SIGN, XML_STATE_CLEF, XML_STATE_ATTRIBUTES)) {
                                        im->produceSementicErrorString(_("\"sign\" outside \"clef\""));
                                        return;
                                      }
				     if (im->m_clefsign[0] != '\0') {
					im->produceSementicErrorString(_("double \"sign\" in \"clef\""));
					return;
				     }
				     strcpy(im->m_clefsign, text);
				     break;
		case XML_STATE_LINE: if (!im->check_state(XML_STATE_LINE, XML_STATE_CLEF, XML_STATE_ATTRIBUTES)) {
                                        im->produceSementicErrorString(_("\"line\" outside \"clef\""));
                                        return;
                                      }
				     if (im->m_clefline >= 0) {
					im->produceSementicErrorString(_("double \"line\" in \"clef\""));
					return;
				     }
				     if (!sscanf(text, "%d", &(im->m_clefline))) {
					sprintf(Str, _("bad clef line: %s"), text);
					im->produceSementicErrorString(Str);
					return;
				     }
				     break;
		case XML_STATE_BEATS: if (!im->check_state(XML_STATE_BEATS, XML_STATE_TIME, XML_STATE_ATTRIBUTES)) {
                                        im->produceSementicErrorString(_("\"beats\" outside \"time\""));
                                        return;
                                      }
				     if (im->m_beats >= 0) {
					im->produceSementicErrorString(_("double \"beats\" in \"time\""));
					return;
				     }
				     if (!sscanf(text, "%d", &(im->m_beats))) {
					sprintf(Str, _("bad beats value: %s"), text);
					im->produceSementicErrorString(Str);
					return;
				     }
				     break;
		case XML_STATE_BEAT_TYPE: if (!im->check_state(XML_STATE_BEAT_TYPE, XML_STATE_TIME, XML_STATE_ATTRIBUTES)) {
                                        im->produceSementicErrorString(_("\"beat-type\" outside \"time\""));
                                        return;
                                      }
				     if (im->m_beat_type >= 0) {
					im->produceSementicErrorString(_("double \"beat_type\" in \"time\""));
					return;
				     }
				     if (!sscanf(text, "%d", &(im->m_beat_type))) {
					sprintf(Str, _("bad beat-type value: %s"), text);
					im->produceSementicErrorString(Str);
					return;
				     }
				     break;
		case XML_STATE_FIFTHS: if (!im->check_state(XML_STATE_FIFTHS, XML_STATE_KEY, XML_STATE_ATTRIBUTES)) {
                                        im->produceSementicErrorString(_("\"fithts\" outside \"key\""));
                                        return;
                                      }
				     if (im->m_keysig > -1000) {
					im->produceSementicErrorString(_("double \"fithts\" in \"key\""));
					return;
				     }
				     if (!sscanf(text, "%d", &(im->m_keysig))) {
					sprintf(Str, _("bad keysig value: %s"), text);
					im->produceSementicErrorString(Str);
					return;
				     }
				     break;
		case XML_STATE_ACTUAL_NOTES: if (!im->check_state(XML_STATE_ACTUAL_NOTES, XML_STATE_TIME_MODIFICATION, XML_STATE_NOTE)) {
                                        im->produceSementicErrorString(_("\"actual-notes\" outside \"time-modification\""));
                                        return;
                                      }
				     if (im->m_actual_notes > 0) {
					im->produceSementicErrorString(_("double \"actual-notes\" in \"time-modification\""));
					return;
				     }
				     if (!sscanf(text, "%d", &(im->m_actual_notes))) {
					sprintf(Str, _("bad actual-notes value: %s"), text);
					im->produceSementicErrorString(Str);
					return;
				     }
				     break;
		case XML_STATE_NORMAL_NOTES: if (!im->check_state(XML_STATE_NORMAL_NOTES, XML_STATE_TIME_MODIFICATION, XML_STATE_NOTE)) {
                                        im->produceSementicErrorString(_("\"normal-notes\" outside \"time-modification\""));
                                        return;
                                      }
				     if (im->m_normal_notes > 0) {
					im->produceSementicErrorString(_("double \"normal-notes\" in \"time-modification\""));
					return;
				     }
				     if (!sscanf(text, "%d", &(im->m_normal_notes))) {
					sprintf(Str, _("bad normal-notes value: %s"), text);
					im->produceSementicErrorString(Str);
					return;
				     }
				     break;
		case XML_STATE_ALTER: if (!im->check_state(XML_STATE_ALTER, XML_STATE_PITCH, XML_STATE_NOTE)) {
                                        im->produceSementicErrorString(_("\"alter\" outside \"pitch\""));
                                        return;
                                      }
				     if (im->m_alter != UNKNOWN_LINE) {
					im->produceSementicErrorString(_("double \"alter\" in \"pitch\""));
					return;
				     }
				     if (!sscanf(text, "%d", &(im->m_alter))) {
					sprintf(Str, _("bad alter value: %s"), text);
					im->produceSementicErrorString(Str);
					return;
				     }
				     break;
		case XML_STATE_ACCIDENTAL: if (!im->check_state(XML_STATE_ACCIDENTAL, XML_STATE_NOTE)) {
                                        im->produceSementicErrorString(_("\"accidental\" outside \"note\""));
                                        return;
                                      }
				     if (!strcmp(text, "natural")) {
				        im->m_alter = 0;
				     }
				     break;
		case XML_STATE_TEXT: if (!im->check_state(XML_STATE_TEXT, XML_STATE_LYRIC, XML_STATE_NOTE)) {
					im->produceSementicErrorString(_("\"text\" outside \"lyric\""));
					return;
				      }
				      if (im->m_lyric_number < 0 || im->m_lyric_number >= MAX_LYRICS_LINES) break;
				      if (im->m_lyrics[im->m_lyric_number][0] != '\0') {
				      	im->produceSementicErrorString(_("double lyric"));
					return;
				      }
				      strcpy(im->m_lyrics[im->m_lyric_number], text);
				      break;
		case XML_STATE_TYPE: if (!im->check_state(XML_STATE_TYPE, XML_STATE_NOTE)) {
					im->produceSementicErrorString(_("\"type\" outside \"note\""));
					return;
				      }
				      strcpy(im->m_note_type, text);
				      break;
		case XML_STATE_CLEF_OCTAVE_CHANGE:
					if (!im->check_state(XML_STATE_CLEF_OCTAVE_CHANGE, XML_STATE_CLEF)) {
						im->produceSementicErrorString(_("\"clef-octave-change\" outside \"clef\""));
						return;
					}
				     if (!sscanf(text, "%d", &(im->m_clefoctave))) {
					sprintf(Str, _("bad clef octave value: %s"), text);
					im->produceSementicErrorString(Str);
					return;
				     }
				     if (im->m_clefoctave < 0) im->m_clefoctave = -12;
				     if (im->m_clefoctave > 0) im->m_clefoctave = 12;
				     break;

	}
}

void NedMusicXMLImport::emtpy_state_stack() {
	m_state_ptr = 0;
}

void NedMusicXMLImport::push_state(int state) {
	if (m_state_ptr >= MAX_STATES) {
		produceSementicErrorString((char *) "stack overflow");
		return;
	}
	m_states[m_state_ptr++] = state;
}
	
int NedMusicXMLImport::pop_state() {
	if (m_state_ptr < 1) {
		NedResource::Abort("NedMusicXMLImport::pop_state: stack undeflow");
	}
	return m_states[--m_state_ptr];
}

bool NedMusicXMLImport::check_state(int st1, int st2, int st3, int st4) {
	int idx = m_state_ptr - 1;
	if (st1 >= 0) {
		if (idx < 0) return false;
		if (st1 != m_states[idx]) return false;
	}
	idx--;
	if (st2 >= 0) {
		if (idx < 0) return false;
		if (st2 != m_states[idx]) return false;
	}
	idx--;
	if (st3 >= 0) {
		if (idx < 0) return false;
		if (st3 != m_states[idx]) return false;
	}
	idx--;
	if (st4 >= 0) {
		if (idx < 0) return false;
		if (st4 != m_states[idx]) return false;
	}
	return true;
}
	
int NedMusicXMLImport::getState() {
	if (m_state_ptr < 1) {
		NedResource::Abort("NedMusicXMLImport::getState: stack undeflow");
	}
	return m_states[m_state_ptr - 1];
}

bool NedMusicXMLImport::getAttr(const char *attr, char *res, const char **attribute_names, const char **attribute_values) {

	char **atn, **atv;

	atn = (char **) attribute_names;
	atv = (char **) attribute_values;
	while (*atn != NULL && *atv != NULL) {
		if (!strcmp(*atn, attr)) {
			strcpy(res, *atv);
			return true;
		}
		atn++; atv++;
	}
	return false;
}

bool NedMusicXMLImport::setCurrentPart(char *partid) {
	int i;
	for (i = 0; i < m_partcount; i++) {
		if (!strcmp(m_parts[i].id, partid)) {
			m_current_part = &(m_parts[i]);
			return true;
		}
	}
	return false;
}

void NedMusicXMLImport::determineClef() {
	char Str[DEFAULT_STRING_LENGTH];
	if (m_current_part == NULL) {
		NedResource::Abort("NedMusicXMLImport::determineClef");
	}
	if (m_clefsign[0] == '\0') {
		produceSementicErrorString(_("missign \"sign\" in \"clef\""));
		return;
	}
	if (m_clefline < 0) {
		produceSementicErrorString(_("missign \"line\" in \"clef\""));
		return;
	}
	if ((m_clefsign[0] == 'G' || m_clefsign[0] == 'g') && m_clefsign[1] == '\0' && m_clefline == 2) {
		m_current_part->current_clef = TREBLE_CLEF;
	}
	else if ((m_clefsign[0] == 'F' || m_clefsign[0] == 'f') && m_clefsign[1] == '\0' && m_clefline == 4) {
		m_current_part->current_clef = BASS_CLEF;
	}
	else if ((m_clefsign[0] == 'C' || m_clefsign[0] == 'c') && m_clefsign[1] == '\0' && m_clefline == 3) {
		m_current_part->current_clef = ALTO_CLEF;
	}
	else if ((m_clefsign[0] == 'C' || m_clefsign[0] == 'c') && m_clefsign[1] == '\0' && m_clefline == 1) {
		m_current_part->current_clef = SOPRAN_CLEF;
	}
	else if ((m_clefsign[0] == 'C' || m_clefsign[0] == 'c') && m_clefsign[1] == '\0' && m_clefline == 4) {
		m_current_part->current_clef = TENOR_CLEF;
	}
	else if (!strcmp(m_clefsign, "TBD") || (!strcmp(m_clefsign, "tbd") && m_clefline == 2)) {
		m_current_part->current_clef = NEUTRAL_CLEF2;
	}
	else if (!strcmp(m_clefsign, "TBD") || (!strcmp(m_clefsign, "tbd") && m_clefline == 4)) {
		m_current_part->current_clef = NEUTRAL_CLEF1;
	}
	else {
		sprintf(Str, _("unsupported clef: %s %d"), m_clefsign, m_clefline);
		produceSementicErrorString(Str);
	}
	m_current_part->current_clef_octave = m_clefoctave;
	if (m_current_part->first_clef) {
		m_current_part->clef = m_current_part->current_clef;
		m_current_part->clef_octave = m_current_part->current_clef_octave;
	}
}

void NedMusicXMLImport::determineTimeSignature() {
	char Str[DEFAULT_STRING_LENGTH];
	if (m_current_part == NULL) {
		NedResource::Abort("NedMusicXMLImport::determineTimeSignature");
	}
	if (m_beats < 1 || m_beats > 20) {
		sprintf(Str, _("unsupported time signature: %d/%d"), m_beats, m_beat_type);
		produceSementicErrorString(Str);
		return;
	}
	switch (m_beat_type) {
		case 2: case 4: case 8: case 16: break;
		default:
		sprintf(Str, _("unsupported time signature: %d/%d"), m_beats, m_beat_type);
		produceSementicErrorString(Str);
		return;
	}
	m_current_part->current_numerator = m_beats;
	m_current_part->current_denominator = m_beat_type;
	if (m_current_part->first_time_signature) {
		m_current_part->numerator = m_current_part->current_numerator;
		m_current_part->denominator = m_current_part->current_denominator;
	}
	
}

void NedMusicXMLImport::determineKeySignature() {
	char Str[DEFAULT_STRING_LENGTH];
	if (m_current_part == NULL) {
		NedResource::Abort("NedMusicXMLImport::determineKeySignature");
	}
	if (m_keysig < -7 || m_keysig > 7) {
		sprintf(Str, _("unsupported key signature: %d"), m_keysig);
		produceSementicErrorString(Str);
		return;
	}
	m_current_part->current_key_signature = m_keysig;
	if (m_current_part->first_key_signature) {
		m_current_part->key_signature = m_current_part->current_key_signature;
	}
}

void NedMusicXMLImport::determineLength(int *len, int *dotcount) {
	char Str[DEFAULT_STRING_LENGTH];
	unsigned long long ned_length, dur, div;
	unsigned int m;
	*len = NOTE_4; *dotcount = 0;
	if (m_current_part == NULL) {
		NedResource::Abort("NedMusicXMLImport::determineLength");
	}
	if (m_actual_notes >= 0) {
		if (m_actual_notes < 2 || m_actual_notes > 13) {
			sprintf(Str, _("actual notes value %d is not allowed"), m_actual_notes);
			produceSementicErrorString(Str);
			return;
		}
		if (NedResource::m_tuplet_tab[m_actual_notes] != m_normal_notes) {
			sprintf(Str, _("actual notes value %d and normal-notes value %d is not allowed"),
				 m_actual_notes, m_normal_notes);
			produceSementicErrorString(Str);
			return;
		}
	}
	dur = m_duration;
	div = m_current_part->divisions;
	ned_length = dur * NOTE_4 / div;
	for (m = NOTE_64; m <= WHOLE_NOTE; m <<= 1) {
		if (m == ned_length) {
			*len = m;
			return;
		}
		if (m * 3 / 2 == ned_length) {
			*len = m;
			*dotcount = 1;
			return;
		}
		if (m * 7 / 4 == ned_length) {
			*len = m;
			*dotcount = 2;
			return;
		}
		if (m_actual_notes > 0) {
			if (m * m_normal_notes / m_actual_notes == ned_length) {
				*len = m;
				return;
			}
		}
	}
	sprintf(Str, _("unsupported note length: %d (divisions = %d)"), m_duration, m_current_part->divisions);
	produceSementicErrorString(Str);
}

unsigned int NedMusicXMLImport::determineState() {
	char Str[DEFAULT_STRING_LENGTH];
	unsigned int state = 0;
	switch (m_alter) {
		case -2: state |= STAT_DFLAT; NedResource::DbgMsg(DBG_TESTING, "STAT_DFLAT\n"); break;
		case -1: state |= STAT_FLAT; break;
		case  0: state |=  STAT_NATURAL; break;
		case  1: state |=  STAT_SHARP; break;
		case  2: state |=  STAT_DSHARP; NedResource::DbgMsg(DBG_TESTING, "STAT_DSHARP\n"); break;
		case UNKNOWN_LINE: break;
		default:
			sprintf(Str, _("unsupported alter value %d"), m_alter);
			produceSementicErrorString(Str);
			break;
	}
	if (m_staccato) {
		state |= STAT_STACC;
	}
	if (m_staccatissimo) {
		state |= STAT_STACCATISSIMO;
	}
	if (m_strong_accent) {
		state |= STAT_SFORZATO;
	}
	if (m_tenuto) {
		state |= STAT_TENUTO;
	}
	if (m_accent) {
		state |= STAT_SFORZANDO;
	}
	if (m_trill) {
		state |= STAT_TRILL;
	}
	if (m_pedal_off) {
		m_pedal_off = false;
		state |= STAT_PED_OFF;
	}
	if (m_pedal_on) {
		m_pedal_on = false;
		state |= STAT_PED_ON;
	}
	return state;
}

void NedMusicXMLImport::determineGrace(int *length, char *type, bool splash) {
	*length = STROKEN_GRACE;
	if (!strcmp(type, "eighth")) {
		if (!splash) {
			*length = GRACE_8;
			return;
		}
	}
	else if (!strcmp(type, "16th")) {
		*length = GRACE_16;
	}
}

void NedMusicXMLImport::determineUpBeat() {
	unsigned long long measure_length = 0, voice_duration = 0;
	GList *lptr;

	switch (m_beat_type) {
		case  2: measure_length = m_beats * NOTE_2; break;
		case  4: measure_length = m_beats * NOTE_4; break;
		case  8: measure_length = m_beats * NOTE_8; break;
		case 16: measure_length = m_beats * NOTE_16; break;
	}

	for (lptr = g_list_first(m_parts[0].voices[0]); lptr; lptr = g_list_next(lptr)) {
		voice_duration += ((NedChordOrRest *)  lptr->data)->getDuration(measure_length);
	}
	if (voice_duration == 0) {
		m_first_measure = true;
		return;
	}
	if (voice_duration < measure_length) {
		if (m_upbeat_inverse != 1) {
			if (m_upbeat_inverse != measure_length - voice_duration) {
				NedResource::DbgMsg(DBG_TESTING, "contr m_upbeat_inverse\n"); 
			}
		}
		else {
			m_upbeat_inverse = measure_length - voice_duration;
		}
	}
}

void NedMusicXMLImport::removeEmptyParts() {
	int i, j;
	bool deleted;
	if (m_partcount > MAXPARTS) {
		m_partcount = MAXPARTS;
	}
	do {
		deleted = false;
		for (i = 0; i < m_partcount; i++) {
			if (m_parts[i].isEmpty()) {
				for (j = i; j < m_partcount - 1; j++) {
					m_parts[j].copy_from(&(m_parts[j + 1]));
				}
				m_partcount--;
				deleted = true;
			}
		}
	}
	while (deleted);
}

				
void NedMusicXMLImport::tie(NedChordOrRest *chord, NedNote *note) {
	GList *lptr1, *lptr2;

	if ((lptr2 = g_list_find(m_current_part->voices[m_current_voice], chord)) == NULL) {
		NedResource::Abort("NedMusicXMLImport::tie(1)");
	}
	if ((lptr1 = g_list_previous(lptr2)) == NULL) {
		produceSementicErrorString(_("no tie back possible"));
		return;
	}
	if (((NedChordOrRest *) lptr1->data)->getType() != TYPE_CHORD) {
		produceSementicErrorString(_("predecessor of tied is not a chord"));
		return;
	}
	if (!((NedChordOrRest *)lptr2->data)->tieBackward((NedChordOrRest *) lptr1->data, note)) {
		//produceSementicErrorString(_("no tie partner"));
	}
}
