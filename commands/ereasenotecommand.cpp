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

#include "ereasenotecommand.h"
#include "note.h"
#include "chordorrest.h"
#include "mainwindow.h"
#include "resource.h"


NedEreaseNoteCommand::NedEreaseNoteCommand(GList **notes, GList *ptr) :
m_notes(notes) {
	if ((m_pos = g_list_position(*m_notes, ptr)) < 0) {
		NedResource::Abort("NedEreaseNoteCommand");
	}
	m_note = ((NedNote *) ptr->data);
}

void NedEreaseNoteCommand::execute(bool adjust /* = false */) {
#ifdef EXECUTE_DEBUG
	printf("\tNedEreaseNoteCommand::execute\n");
#endif
	GList *lptr;
	if ((lptr = g_list_nth(*m_notes, m_pos)) == NULL) {
		NedResource::Abort("NedEreaseNoteCommand::execute");
	}
	*m_notes = g_list_delete_link(*m_notes, lptr);
	m_note->getChord()->reConfigure();
	m_note->getChord()->xPositNotes();
	m_note->getChord()->computeBbox();
	if (adjust) {
		m_note->getMainWindow()->setVisible(m_note);
	}
}

void NedEreaseNoteCommand::unexecute(bool adjust /* = false */) {
#ifdef UNEXECUTE_DEBUG
	printf("\tNedEreaseNoteCommand::unexecute\n");
#endif
	*m_notes = g_list_insert(*m_notes, m_note, m_pos);
	m_note->getChord()->reConfigure();
	m_note->getChord()->xPositNotes();
	m_note->getChord()->computeBbox();
	if (adjust) {
		m_note->getMainWindow()->setVisible(m_note);
	}
}

