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

#include "addnoteatcommand.h"
#include "chordorrest.h"
#include "voice.h"
#include "note.h"
#include "mainwindow.h"
#include "resource.h"

NedAddNoteAtCommand::NedAddNoteAtCommand(NedVoice *voice, NedChordOrRest *chord_or_rest, int line, int head, unsigned int status) :
m_voice(voice), m_chord(chord_or_rest), m_line(line), m_status(status), m_first_call(TRUE) {
	m_note = new NedNote(chord_or_rest, line, head, status);
}


void NedAddNoteAtCommand::execute(bool adjust /* = false */) {
#ifdef EXECUTE_DEBUG
	printf("\tNedAddNoteAtCommand::execute\n");
#endif
	m_chord->insertNoteAt(m_note, m_first_call);
	m_first_call = FALSE;
	m_chord->reConfigure();
	m_chord->xPositNotes();
	m_chord->computeBbox();
	m_chord->getMainWindow()->setSelected(m_chord, m_note);
	if (adjust) {
		m_chord->getMainWindow()->setVisible(m_chord);
	}
}


void NedAddNoteAtCommand::unexecute(bool adjust /* = false */) {
#ifdef UNEXECUTE_DEBUG
	printf("\tNedAddNoteAtCommand::unexecute\n");
#endif
	m_chord->removeNoteAt(m_line);
	m_chord->reConfigure();
	m_chord->xPositNotes();
	m_chord->computeBbox();
	m_chord->getMainWindow()->setSelected(NULL, NULL);
	if (adjust) {
		m_chord->getMainWindow()->setVisible(m_chord);
	}
}
