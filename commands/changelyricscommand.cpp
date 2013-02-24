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

#include "changelyricscommand.h"
#include "chordorrest.h"
#include "mainwindow.h"
#include "resource.h"

NedChangeLyricsCommand::NedChangeLyricsCommand(NedChordOrRest *chord, int line, NedPangoCairoText *new_lyrics) :
m_chord(chord), m_new_lyrics(new_lyrics), m_line(line) {
	m_old_lyrics = m_chord->m_lyrics[line];
}


void NedChangeLyricsCommand::execute(bool adjust /* = false */) {
#ifdef EXECUTE_DEBUG
	printf("\tNedChangeLyricsCommand::execute\n");
#endif
	m_chord->m_lyrics[m_line] = m_new_lyrics;
	m_chord->computeBbox();
	if (adjust) {
		m_chord->getMainWindow()->setVisible(m_chord);
	}
}


void NedChangeLyricsCommand::unexecute(bool adjust /* = false */) {
#ifdef UNEXECUTE_DEBUG
	printf("\tNedChangeLyricsCommand::unexecute\n");
#endif
	m_chord->m_lyrics[m_line] = m_old_lyrics;
	if (adjust) {
		m_chord->getMainWindow()->setVisible(m_chord);
	}
}
