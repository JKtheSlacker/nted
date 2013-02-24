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

#include "midiimporter.h"
#include "idiotseditor.h"
#include "resource.h"
#include <string.h>

#define NOTE_OFF               0x80
#define NOTE_ON                0x90
#define META_EVENT             0xFF
#define SYS_EXCLUSIVE_MESSAGE1 0xF0
#define SYS_EXCLUSIVE_MESSAGE2 0xF7
#define PGM_CHANGE             0xC0
#define AFTER_TOUCH            0xA0
#define CTRL_CHANGE            0xB0
#define CHANNEL_PRESSURE       0xD0
#define PITCH_BEND             0xE0
#define SONG_POS_PTR           0xF2
#define SONG_SELECT            0xF3
#define TUNE_REQU              0xF6

#define META_TEXT              0x01
#define META_TRACK_NAME        0x03
#define META_TIMESIG           0x58
#define META_KEYSIG            0x59
#define META_TEMPO             0x51

NedMidiImporter::NedMidiImporter(bool with_triplets, int tempo_change_density, int volume_change_density, bool force_piano, bool sort_instruments, bool dont_split, bool use_id_ed) :
	NedImporter(), m_error_string(NULL), m_current_time(0), m_last_event_time(0), m_with_triplets(with_triplets), m_use_id_ed(use_id_ed)
{
	m_idiots_editor = new NedIdiotsEditor(tempo_change_density, volume_change_density, force_piano, sort_instruments, dont_split, use_id_ed);
	for (int i = 0; i < 16; m_current_midi_instrument[i++] = 0);
}

bool NedMidiImporter::import() {
	int track_nr;

	read_header(m_FILE);
	for (track_nr = 0; track_nr < m_ntracks; track_nr++) {
		readTrack(m_FILE, track_nr, 0);
		if (m_current_time > m_last_event_time) m_last_event_time = m_current_time;
		if (m_error_string != NULL) break;
	}

	if (m_error_string != NULL) {
		NedResource::DbgMsg(DBG_TESTING, "%s\n", m_error_string);
		return false;
	}
	m_idiots_editor->setLastTime(m_last_event_time);
	return true;
}

bool NedMidiImporter::convertToScore() {
	int i, j;
	char *cptr;

	m_idiots_editor->computeTimeSigMeasureNumbers(m_main_window);
	m_idiots_editor->computeTempo();
	m_idiots_editor->computeVolumes();
	m_idiots_editor->snapStartOfNotes();
	if (m_with_triplets) {
		m_idiots_editor->recognizeTriplets();
	}
	m_idiots_editor->trySplitInstruments();
	m_tempo_inverse = m_idiots_editor->m_tempo_inverse;

	m_upbeat_inverse = 0;
	m_partcount = m_idiots_editor->getPartCount();
	for (i = 0; i < m_partcount; i++) {
		m_parts[i].voices[0] = m_idiots_editor->convertToNtEdNotes(i, 0);
		for (j = 1; j < VOICE_COUNT; j++) {
			m_parts[i].voices[j] = m_idiots_editor->convertToNtEdNotes(i, j);
		}
		strcpy(m_parts[i].id, "part1");
		cptr = m_idiots_editor->getInstrumentName(i);
		if (cptr == NULL) {
			m_parts[i].name[0] = '\0';
		}
		else {
			if (*cptr == '\0') {
				m_parts[i].name[0] = '\0';
			}
			else {
				strcpy(m_parts[i].name, cptr);
			}
		}
		m_parts[i].clef = m_idiots_editor->getClef(i);
		m_parts[i].clef_octave = m_idiots_editor->getOctaveShift(i);
		m_parts[i].key_signature = m_idiots_editor->getKeySig(i);
		m_parts[i].numerator = m_idiots_editor->getNumerator();
		m_parts[i].denominator = m_idiots_editor->getDenominator();
		m_parts[i].midi_pgm = m_idiots_editor->getMidiPgm(i);
		m_parts[i].midi_channel = m_idiots_editor->getChannel(i);
		m_parts[i].volume = m_idiots_editor->getVolume(i);
	}
	return true;
}

NedMidiImporter::~NedMidiImporter() {
	//printf("NedMidiImporter::~NedMidiImporter\n"); fflush(stdout);
	delete m_idiots_editor;
}


void NedMidiImporter::read_header(FILE *fp) {
	char buffer[1024];
	int len;

	if (fread(buffer, 1, 4, fp) != 4) {
		m_error_string = "error in fread";
		return;
	}

	buffer[4] = '\0';
	if (strcmp(buffer, "MThd")) {
		if (!strcmp(buffer, "RIFF")) {
			if (fread(buffer, 1, 16, fp) != 16) {
				m_error_string = "unexpected EOF(0)";
				return;
			}
			if (fread(buffer, 1, 4, fp) != 4) {
				m_error_string = "unexpected EOF(1)";
				return;
			}
			buffer[4] = '\0';
			if (strcmp(buffer, "MThd")) {
				m_error_string = "No MIDI file";
				return;
			}
		}
		else {
			m_error_string = "No MIDI file";
			return;
		}
	}
	if (!readdword(fp, &len)) {
		m_error_string = "cannot read len";
		return;
	}
	if (len != 6) {
		m_error_string = "bad header length";
		return;
	}
	if (!readword(fp, &m_format)) {
		m_error_string = "cannot read format";
		return;
	}
	if (!readword(fp, &m_ntracks)) {
		m_error_string = "cannot read ntracks";
		return;
	}
	if (!readword(fp, &m_division)) {
		m_error_string = "cannot read division";
		return;
	}
	if (m_division < 0) {
		m_error_string = "cannot deal with negative divisions";
		return;
	}
}

void NedMidiImporter::readTrack(FILE *fp, int track_nr, int pass) {
	char buffer[1024];
	int len;
	unsigned int tempo;
	unsigned long long time_dist;
	char type;
	unsigned int meta_len;
	unsigned char status_byte, last_status;
	char sbyte, data1, data2;
	int num, denom;
	unsigned int chan;
	struct {
		long long time;
		int key;
		int state;
#define KEYSIG_INVALID 0
#define KEYSIG_SET 1
#define KEYSIG_NOT_RELEVANT 2
	} first_keysig;

	first_keysig.state = KEYSIG_INVALID;
	first_keysig.time = 0;
	first_keysig.key = 0;

	if (fread(buffer, 1, 4, fp) != 4) {
		m_error_string = "unexpected EOF(2)";
		return;
	}

	buffer[4] = '\0';
	if (strcmp(buffer, "MTrk")) {
		m_error_string = "No MIDI track";
		return;
	}
	if (!readdword(fp, &len)) {
		m_error_string = "cannot read track len";
		return;
	}

	last_status = 0;
	m_current_time = 0;
	while (len > 0) {
		time_dist = read_val_var(fp, &len);
		m_current_time += time_dist * NOTE_4 / m_division;
		sbyte = getc(fp);
		status_byte = sbyte;
		if ((status_byte & 0x80) == 0) {
			ungetc(status_byte, fp);
			status_byte = last_status;
		}
		else {
			last_status = status_byte;
			len--;
		}
		if (status_byte == META_EVENT) {
			if ((type = getc(fp)) == EOF) {
				m_error_string = "unexpected EOF(3)";
				return;
			}
			meta_len = read_val_var(fp, &len);
			if (fread(buffer, 1, meta_len, fp) != meta_len) {
				m_error_string = "unexpected EOF(4)";
				return;
			}
			len -= meta_len + 1;
			switch (type) {
				case META_TRACK_NAME: 
						  buffer[meta_len] = '\0';
						  m_idiots_editor->setInstrumentName(buffer);
						  break;
				case META_TIMESIG:
						   num  = buffer[0];
						   switch(buffer[1]) {
						   	case 0: denom =  1; break;
						   	case 1: denom =  2; break;
						   	case 2: denom =  4; break;
							case 3: denom =  8; break;
							default:  denom =16; break;
						   }
						   NedResource::DbgMsg(DBG_TESTING, "TIMESIG: %llu(%llu): num = %d, dmon = %d\n",
						   		m_current_time, m_current_time / NOTE_64, num, denom);
						   m_idiots_editor->addTimeSigChange(m_current_time, num, denom);
						   break;
				case META_TEMPO: tempo = ((0xff & buffer[0]) << 16) | ((0xff & buffer[1]) << 8) | (0xff & buffer[2]);
						 m_idiots_editor->addTempoChange(m_current_time, (int) (60000000.0 / (double) tempo));
						 break;
				case META_KEYSIG:
						if (track_nr > 0 && first_keysig.state == KEYSIG_INVALID) { // avoid keysigs of empry tracks
							first_keysig.time = m_current_time;
							first_keysig.key = buffer[0];
							first_keysig.state = KEYSIG_SET;
							break;
						   }
						   m_idiots_editor->addKeySig(m_current_time, 0, buffer[0]);
						   break;

			}
		}
		else if (status_byte == SYS_EXCLUSIVE_MESSAGE1 || status_byte == SYS_EXCLUSIVE_MESSAGE2) {
			meta_len = read_val_var(fp, &len);
			if (fread(buffer, 1, meta_len, fp) != meta_len) {
				m_error_string = "unexpected EOF(5)";
				return;
			}
			len -= meta_len;
		}
		else if ((status_byte & 0xf0) == NOTE_ON) {
			if ((data1 = getc(fp)) == EOF) {
				m_error_string = "unexpected EOF(6)";
				return;
			}
			if ((data2 = getc(fp)) == EOF) {
				m_error_string = "unexpected EOF(7)";
				return;
			}
			len -= 2;
			chan = status_byte & 0x0f;
			if (chan == 9) {
				m_current_midi_instrument[chan] = -1;
			}
			if (pass == 0) {
				if (data2 == 0) {
					m_idiots_editor->closeNote(data1, m_current_time, m_current_midi_instrument[chan], chan);
				}
				else {
				/*
					note = new NedTableNote(m_current_time, 0, data1, data2);
					m_idiots_editor->addNote(note, m_current_midi_instrument[chan]);
					*/
					if (first_keysig.state == KEYSIG_SET) {
						m_idiots_editor->addKeySig(first_keysig.time, 0, first_keysig.key);
						first_keysig.state = KEYSIG_NOT_RELEVANT;
					}
					m_idiots_editor->addNote(data1, data2, m_current_time, m_current_midi_instrument[chan], chan);
				}
			}
		}
		else if ((status_byte & 0xf0) == NOTE_OFF) {
			if ((data1 = getc(fp)) == EOF) {
				m_error_string = "unexpected EOF(8)";
				return;
			}
			if ((data2 = getc(fp)) == EOF) {
				m_error_string = "unexpected EOF(9)";
				return;
			}
			len -= 2;
			chan = status_byte & 0x0f;
			if (pass == 0) {
				m_idiots_editor->closeNote(data1, m_current_time, m_current_midi_instrument[chan], chan);
			}
		}
		else if ((status_byte & 0xf0) == AFTER_TOUCH) {
			if ((data1 = getc(fp)) == EOF) {
				m_error_string = "unexpected EOF(14)";
				return;
			}
			if ((data2 = getc(fp)) == EOF) {
				m_error_string = "unexpected EOF(15)";
				return;
			}
			len -= 2;
		}
		else if ((status_byte & 0xf0) == CTRL_CHANGE) {
			if ((data1 = getc(fp)) == EOF) {
				m_error_string = "unexpected EOF(16)";
				return;
			}
			if ((data2 = getc(fp)) == EOF) {
				m_error_string = "unexpected EOF(17)";
				return;
			}
			len -= 2;
		}
		else if ((status_byte & 0xf0) == PGM_CHANGE) {
			chan = status_byte & 0x0f;
			if ((m_current_midi_instrument[chan] = getc(fp)) == EOF) {
				m_error_string = "unexpected EOF(18)";
				return;
			}
			if (chan == 9) {
				m_current_midi_instrument[chan] = -1;
			}
			len -= 1;
		}
		else if ((status_byte & 0xf0) == CHANNEL_PRESSURE) {
			if ((data1 = getc(fp)) == EOF) {
				m_error_string = "unexpected EOF(19)";
				return;
			}
			len -= 1;
		}
		else if ((status_byte & 0xf0) == PITCH_BEND) {
			if ((data1 = getc(fp)) == EOF) {
				m_error_string = "unexpected EOF(20)";
				return;
			}
			if ((data2 = getc(fp)) == EOF) {
				m_error_string = "unexpected EOF(21)";
				return;
			}
			len -= 2;
		}
		else if ((status_byte & 0xF8) == 0xF8) {
		}
		else if (status_byte == SONG_POS_PTR) {
			if ((data1 = getc(fp)) == EOF) {
				m_error_string = "unexpected EOF(22)";
				return;
			}
			if ((data2 = getc(fp)) == EOF) {
				m_error_string = "unexpected EOF(23)";
				return;
			}
			len -= 2;
		}
		else if (status_byte == SONG_SELECT) {
			if ((data1 = getc(fp)) == EOF) {
				m_error_string = "unexpected EOF(24)";
				return;
			}
			len -= 1;
		}
		else if (status_byte == 0xF1) {
		}
		else if (status_byte == 0xF4) {
		}
		else if (status_byte == 0xF5) {
		}
		else if (status_byte == TUNE_REQU) {
		}
		if (len < 0) {
			m_error_string = "length error";
			return;
		}
	}
}
	
unsigned long NedMidiImporter::read_val_var(FILE *fp, int *len) {
	unsigned long the_val;
	char c;

	if (*len < 0) {
		m_error_string = "bad length in val(1)";
		return 0;
	}
	(*len)--;
	if ((the_val = getc(fp)) & 0x80) {
		the_val &= 0x7F;
		do {
			if (*len < 0) {
				m_error_string = "bad len in val(2)";
				return 0;
			}
			(*len)--;
			the_val = (the_val << 7) + ((c = getc(fp)) & 0x7F);
		}
		while (c & 0x80);
	}
	return the_val;
}

int NedMidiImporter::readdword(FILE *fp, int *data) {
	char c[4];
	int i;

	for (i = 0; i < 4; i++) {
		if (feof(fp)) return 0;
		c[i] = getc(fp);
	}
	*data = (c[3] & 0xff) | ((c[2] & 0xff) << 8) |
		 ((c[1] & 0xff) << 16)| ((c[0] & 0xff) << 24);
	return 1;
}

int NedMidiImporter::readword(FILE *fp, short *data) {
	char c[2];
	int i;

	for (i = 0; i < 2; i++) {
		if (feof(fp)) return 0;
		c[i] = getc(fp);
	}
	*data = ((c[1] & 0xff) << 0)| ((c[0] & 0xff) << 8);
	return 1;
}

