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

#include "movechordsandrestsreversecommand.h"
#include "clipboard.h"

NedMoveChordsAndRestsReverseCommand::NedMoveChordsAndRestsReverseCommand(NedClipBoard *board) : m_board(board) {}

void NedMoveChordsAndRestsReverseCommand::execute(bool adjust /* = false */) {
#ifdef EXECUTE_DEBUG
	printf("\tNedMoveChordsAndRestsReverseCommand::execute\n");
#endif
	m_board->execute_reverse(adjust);
}


void NedMoveChordsAndRestsReverseCommand::unexecute(bool adjust /* = false */) {
#ifdef UNEXECUTE_DEBUG
	printf("\tNedMoveChordsAndRestsReverseCommand::unexecute\n");
#endif
	m_board->unexecute_reverse(adjust);
}
