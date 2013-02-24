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

#ifndef INSERT_CHORD_OR_REST_GROUP_COMMAND_H

#define INSERT_CHORD_OR_REST_GROUP_COMMAND_H
#include "config.h"
#include "command.h"
#include <gtk/gtk.h>

class NedChordOrRest;
class NedVoice;


class NedInsertChordOrRestGroupCommand : public NedCommand {
	public:	
		NedInsertChordOrRestGroupCommand(NedVoice *voice, GList *chords_or_rests, int pos);
		virtual void execute(bool adjust = false);
		virtual void unexecute(bool adjust = false);
	private:
		NedVoice *m_voice;
		int m_pos;
		GList *m_chord_or_rests;
};

#endif /* INSERT_CHORD_OR_REST_GROUP_COMMAND_H */
