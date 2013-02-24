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

#ifndef CHANGE_X_POSITION_COMMAND_H

#define CHANGE_X_POSITION_COMMAND_H
#include "config.h"
#include "command.h"
#include <gtk/gtk.h>

class NedChordOrRest;
class NedTuplet;

class NedChangeXPositionCommand : public NedCommand {
	public:	
		NedChangeXPositionCommand(GList **chord_an_rest_list, NedChordOrRest *sibling_chord_or_rest, NedChordOrRest *chord_or_rest, bool before);
		virtual void execute(bool adjust = false);
		virtual void unexecute(bool adjust = false);
	private:
		GList **m_chord_an_rest_list;
		int m_from_pos;
		NedChordOrRest *m_sibling_chord_or_rest;
		NedChordOrRest *m_chord_or_rest;
		NedTuplet *m_tuplet;
		bool m_before;
};

#endif /* CHANGE_X_POSITION_COMMAND_H */
