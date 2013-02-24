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

#include "newlyricscommand.h"
#include "chordorrest.h"
#include "mainwindow.h"
#include "resource.h"

NedNewLyricsCommand::NedNewLyricsCommand(NedChordOrRest *chord, int line) :
m_chord(chord), m_line(line) {
	m_lyrics = m_chord->m_lyrics[line];
}


void NedNewLyricsCommand::execute(bool adjust /* = false */) {
#ifdef EXECUTE_DEBUG
	printf("\tNedNewLyricsCommand::execute\n");
#endif
	m_chord->m_lyrics[m_line] = m_lyrics;
	m_chord->computeBbox();
	if (adjust) {
		m_chord->getMainWindow()->setVisible(m_chord);
	}
}


void NedNewLyricsCommand::unexecute(bool adjust /* = false */) {
#ifdef UNEXECUTE_DEBUG
	printf("\tNedNewLyricsCommand::unexecute\n");
#endif
	m_chord->m_lyrics[m_line] = NULL;
	if (adjust) {
		m_chord->getMainWindow()->setVisible(m_chord);
	}
}
