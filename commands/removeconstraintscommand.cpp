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

#include "removeconstraintscommand.h"
#include "chordorrest.h"
#include "mainwindow.h"
#include "resource.h"
#include "beam.h"
#include "staff.h"
#include "voice.h"

NedRemoveConstraintsCommand::NedRemoveConstraintsCommand(NedChordOrRest *chord) :
m_chord(chord), m_old_stem_height(chord->m_stem_height), m_old_beam_y_offs(m_chord->m_beam_y_offs), m_old_slope_offs(m_chord->m_slope_offs),
m_old_status(chord->m_status) {}

void NedRemoveConstraintsCommand::execute(bool adjust /* = false */) {
#ifdef EXECUTE_DEBUG
	printf("\tNedRemoveConstraintsCommand::execute\n");
#endif
	m_chord->m_status &= (~(STAT_USER_STEMDIR));
	m_chord->m_stem_height = STEM_DEFAULT_HEIGHT;
	m_chord->m_beam_y_offs = 0.0;
	m_chord->m_slope_offs = 0;
	if (m_chord->m_beam != NULL) {
		m_chord->m_beam->computeBeam(m_chord->getStaff()->getStaffNumber() * VOICE_COUNT + m_chord->getVoice()->getVoiceNumber());
	}
	m_chord->xPositNotes();
	m_chord->computeBbox();
	if (adjust) {
		m_chord->getMainWindow()->setVisible(m_chord);
	}
}


void NedRemoveConstraintsCommand::unexecute(bool adjust /* = false */) {
#ifdef UNEXECUTE_DEBUG
	printf("\tNedRemoveConstraintsCommand::unexecute\n");
#endif
	m_chord->m_status &= m_old_status;
	m_chord->m_stem_height = m_old_stem_height;
	m_chord->m_beam_y_offs = m_old_beam_y_offs;
	m_chord->m_slope_offs = m_old_slope_offs;
	if (m_chord->m_beam != NULL) {
		m_chord->m_beam->computeBeam(m_chord->getStaff()->getStaffNumber() * VOICE_COUNT + m_chord->getVoice()->getVoiceNumber());
	}
	m_chord->xPositNotes();
	m_chord->computeBbox();
	if (adjust) {
		m_chord->getMainWindow()->setVisible(m_chord);
	}
}
