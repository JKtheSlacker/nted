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

#include "midirecorder.h"
#include "idiotseditor.h"
#include "resource.h"
#include <string.h>


NedMidiRecorder::NedMidiRecorder(int num, int denom, int key, int midi_pgm,
                                        int tempo_inverse, int metro_volume, bool triplet, bool f_piano, bool dnt_split) :
	NedImporter(), m_error_string(NULL), m_current_time(0), m_last_event_time(0), m_metronom_running(false), m_record_started(false),
		m_num(num), m_denom(denom),
		m_midi_pgm(midi_pgm), m_tempo_inverse(tempo_inverse), m_metro_volume(metro_volume), m_triplet_recognition(triplet) 
{
	m_intervall_long = (unsigned long long) (100000000.0 * 4.0 / (double) tempo_inverse / (double) denom );
	m_metronom_intervall.tv_sec = (unsigned int) (m_intervall_long / 100000);
	m_metronom_intervall.tv_usec = (unsigned int) ((m_intervall_long % 100000) * 1000);
	m_idiots_editor = new NedIdiotsEditor(20, 20, f_piano, false, dnt_split);
	m_idiots_editor->addTimeSigChange(0, num, denom);
	m_idiots_editor->addKeySig(0, 0, key);
	for (int i = 0; i < 16; m_current_midi_instrument[i++] = 0);
}

void NedMidiRecorder::startMetronom() {
	int pitch;
	snd_seq_event_t ev;
	 struct timeval now;
	if (NedResource::isPlaying()) return;
	if (NedResource::getMidiInSequ() == NULL || NedResource::getMidiOutSequ() == NULL) return;

	snd_seq_ev_clear(&ev);
	ev.type = SND_SEQ_EVENT_PGMCHANGE;
	ev.data.control.channel = 0;
	ev.data.control.value = m_midi_pgm;
	snd_seq_ev_set_subs(&ev);  
	snd_seq_ev_set_direct(&ev);
	snd_seq_event_output_direct(NedResource::getMidiOutSequ(), &ev);

	m_number_of_midi_poll_fds = snd_seq_poll_descriptors_count(NedResource::getMidiInSequ(), POLLIN);
	NedMidiRecorder::m_metronom_count = 1;
	m_metronom_running = true;
	m_record_started = false;
	pitch = 48;
	gettimeofday(&now, NULL);
	timeradd(&now, &m_metronom_intervall, &m_next_metronome_tick);
	NedResource::playImmediately(9, 1, 1, &pitch, m_metro_volume);
	g_timeout_add(4, OnMetronomTick, this);
}

gboolean NedMidiRecorder::OnMetronomTick(void *data) {
	unsigned long long midi_time;
	struct timeval now, diff;
	int pitch = -1;
	NedMidiRecorder *recorder = (NedMidiRecorder *) data;
	if (!recorder->m_metronom_running) return FALSE;
	gettimeofday(&now, NULL);
	if (timercmp(&now, &(recorder->m_next_metronome_tick), >=)) { 
		pitch = (recorder->m_metronom_count++ % recorder->m_num) ? 40 : 48;
		//NedResource::playImmediately(9, 1, 1, &pitch, recorder->m_metro_volume);
		if (pitch == 48 && !recorder->m_record_started) {
			recorder->m_record_started = true;
			recorder->m_metronom_count = 1;
		}
		gettimeofday(&(recorder->m_last_recorder_metronom_tick), NULL);
		timeradd(&(recorder->m_last_recorder_metronom_tick), &(recorder->m_metronom_intervall), &(recorder->m_next_metronome_tick));
	}
	if (recorder->m_record_started) {
		struct pollfd *pfd = (struct pollfd *)alloca(recorder->m_number_of_midi_poll_fds * sizeof(struct pollfd));
		snd_seq_poll_descriptors(NedResource::getMidiInSequ(), pfd, recorder->m_number_of_midi_poll_fds, POLLIN);
		if (poll(pfd, recorder->m_number_of_midi_poll_fds, 0) > 0) {
			gettimeofday(&now, NULL);
			timersub(&now, &(recorder->m_last_recorder_metronom_tick), &diff);
			midi_time = recorder->m_metronom_count - 1;
			midi_time *= (unsigned long long) recorder->m_metronom_intervall.tv_sec * (unsigned long long) 1000 + (unsigned long long) recorder->m_metronom_intervall.tv_usec / (unsigned long long) 1000;
			midi_time += (unsigned long long) diff.tv_sec * (unsigned long long) 1000 + (unsigned long long) diff.tv_usec / (unsigned long long) 1000;
			midi_time *= 4 * NOTE_4;
			midi_time /= recorder->m_denom;
			midi_time /= recorder->m_intervall_long;
			recorder->process_midi_event(midi_time);
		}
	}
	if (pitch >= 0) {
		NedResource::playImmediately(9, 1, 1, &pitch, recorder->m_metro_volume);
	}
	g_timeout_add(4, OnMetronomTick, data);
	return FALSE;
}

void NedMidiRecorder::stopMetronom() {
	m_idiots_editor->changeChannelAndPgm(0, 0, m_midi_pgm);
	m_metronom_running = false;
	m_record_started = false;
}

void NedMidiRecorder::process_midi_event(unsigned long long midi_time) {
	snd_seq_event_t *ev;
	do {
		snd_seq_event_input(NedResource::getMidiInSequ(), &ev);
		switch (ev->type) {
#ifdef UUU
			case SND_SEQ_EVENT_CONTROLLER: 
				fprintf(stderr, "Control event on Channel %2d: %5d       \n",
				ev->data.control.channel, ev->data.control.value);
				break;
			case SND_SEQ_EVENT_PITCHBEND:
				fprintf(stderr, "Pitchbender event on Channel %2d: %5d   \n", 
				ev->data.control.channel, ev->data.control.value);
				break;
#endif
			case SND_SEQ_EVENT_NOTEON:
#ifdef UUU
				fprintf(stderr, "Note On event on Channel %2d: %5d       \n",
				ev->data.control.channel, ev->data.note.note);
#endif
#ifdef VVV
				printf("time = %llu(%llu, %llu): Note On event on Channel %2d: %5d, volume: %d\n",
				LAUS(midi_time),
				ev->data.control.channel, ev->data.note.note, ev->data.note.velocity);
#endif
				if (ev->data.note.velocity > 0) {
					m_idiots_editor->addNote(ev->data.note.note, 64, midi_time, 0, 0);
				}
				else {
					m_idiots_editor->closeNote(ev->data.note.note, midi_time, 0, 0);
				}
				if (NedResource::getMidiOutSequ()) {
					ev->data.control.channel = 0;
					snd_seq_ev_set_subs(ev);  
					snd_seq_ev_set_direct(ev);
					snd_seq_event_output_direct(NedResource::getMidiOutSequ(), ev);
				}
				m_last_event_time = midi_time;
				break;        
			case SND_SEQ_EVENT_NOTEOFF: 
#ifdef UUU
				fprintf(stderr, "Note Off event on Channel %2d: %5d      \n",         
				ev->data.control.channel, ev->data.note.note);           
#endif
#ifdef VVV
				printf("time = %llu(%llu, %llu): Note Off event on Channel %2d: %5d\n",
				LAUS(midi_time),
				ev->data.control.channel, ev->data.note.note);
#endif
				m_idiots_editor->closeNote(ev->data.note.note, midi_time, 0, 0);
				if (NedResource::getMidiOutSequ()) {
					ev->data.control.channel = 0;
					snd_seq_ev_set_subs(ev);  
					snd_seq_ev_set_direct(ev);
					snd_seq_event_output_direct(NedResource::getMidiOutSequ(), ev);
				}
				m_last_event_time = midi_time;
				break;        
		}
		snd_seq_free_event(ev);
	}
	while (snd_seq_event_input_pending(NedResource::getMidiInSequ(), 0) > 0);
}

bool NedMidiRecorder::import() {
	int i, j;
	char *cptr;

	m_idiots_editor->setLastTime(m_last_event_time);
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
		m_parts[i].midi_channel = 0;
		m_parts[i].volume = m_idiots_editor->getVolume(i);
	}
	return true;
}

NedMidiRecorder::~NedMidiRecorder() {
	//printf("NedMidiRecorder::~NedMidiRecorder\n"); fflush(stdout);
	delete m_idiots_editor;
}
