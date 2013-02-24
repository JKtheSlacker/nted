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

#include "fixintermediatecommand.h"
#include "chordorrest.h"
#include "mainwindow.h"
#include "resource.h"
#include "beam.h"
#include "staff.h"
#include "voice.h"

NedFixIntermediateCommand::NedFixIntermediateCommand(NedChordOrRest *chord) :
m_chord(chord), m_stemheight_relevant(false), m_new_beam_y_offs_relevant(false), m_slope_offs_relevant(false), m_status_change_relevant(false) {
	if (chord->m_stemheight_copy > UNSET_STEM_HEIGHT) {
		m_stemheight_relevant = true;
		m_old_stemheight = chord->m_stemheight_copy;
		m_new_stemheight = chord->m_stem_height;
	}
	if (chord->m_beam_y_offs_copy > UNSET_STEM_HEIGHT) {
		m_new_beam_y_offs_relevant = true;
		m_old_beam_y_offs = chord->m_beam_y_offs_copy;
		m_new_beam_y_offs = chord->m_beam_y_offs;
	}
	if (chord->m_slope_offs_copy != UNSET_SLOPE_OFFS) {
		m_slope_offs_relevant = true;
		m_old_slope_offs = chord->m_slope_offs_copy;
		m_new_slope_offs = chord->m_slope_offs;
	}
	if (chord->m_status != chord->m_status_copy) {
		m_status_change_relevant = true;
		m_old_status = chord->m_status_copy;
		m_new_status = chord->m_status;
	}
}

void NedFixIntermediateCommand::execute(bool adjust /* = false */) {
#ifdef EXECUTE_DEBUG
	printf("\tNedFixIntermediateCommand::execute\n");
#endif
	if (m_stemheight_relevant) {
		m_chord->m_stem_height = m_new_stemheight;
	}
	if (m_new_beam_y_offs_relevant) {
		m_chord->m_beam_y_offs = m_new_beam_y_offs;
	}
	if (m_slope_offs_relevant) {
		m_chord->m_slope_offs = m_new_slope_offs;
	}
	if (m_status_change_relevant) {
		m_chord->m_status = m_new_status;
	}
	if (m_chord->m_beam != NULL) {
		m_chord->m_beam->computeBeam(m_chord->getStaff()->getStaffNumber() * VOICE_COUNT + m_chord->getVoice()->getVoiceNumber());
	}
	m_chord->xPositNotes();
	m_chord->computeBbox();
	if (adjust) {
		m_chord->getMainWindow()->setVisible(m_chord);
	}
	else { // first call
		m_chord->m_stemheight_copy = UNSET_STEM_HEIGHT - 10.0;
		m_chord->m_beam_y_offs_copy = UNSET_STEM_HEIGHT - 10.0;
		m_chord->m_slope_offs_copy = UNSET_SLOPE_OFFS;
	}
}


void NedFixIntermediateCommand::unexecute(bool adjust /* = false */) {
#ifdef UNEXECUTE_DEBUG
	printf("\tNedFixIntermediateCommand::unexecute\n");
#endif
	if (m_stemheight_relevant) {
		m_chord->m_stem_height = m_old_stemheight;
	}
	if (m_new_beam_y_offs_relevant) {
		m_chord->m_beam_y_offs = m_old_beam_y_offs;
	}
	if (m_slope_offs_relevant) {
		m_chord->m_slope_offs = m_old_slope_offs;
	}
	if (m_status_change_relevant) {
		m_chord->m_status = m_old_status;
	}
	if (m_chord->m_beam != NULL) {
		m_chord->m_beam->computeBeam(m_chord->getStaff()->getStaffNumber() * VOICE_COUNT + m_chord->getVoice()->getVoiceNumber());
	}
	m_chord->xPositNotes();
	m_chord->computeBbox();
	if (adjust) {
		m_chord->getMainWindow()->setVisible(m_chord);
	}
}
