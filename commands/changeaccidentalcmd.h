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

#ifndef CHANGE_ACCIDENTAL_COMMAND_H

#define CHANGE_ACCIDENTAL_COMMAND_H
#include "config.h"
#include "command.h"

class NedNote;


class NedChangeAccidentalCommand : public NedCommand {
	public:	
		NedChangeAccidentalCommand(NedNote *note, unsigned int newstatus, int newline);
		virtual void execute(bool adjust = false);
		virtual void unexecute(bool adjust = false);
	private:
		NedNote *m_note;
		unsigned int m_newstatus;
		unsigned int m_oldstatus;
		int m_old_line, m_new_line;
};

#endif /* CHANGE_ACCIDENTAL_COMMAND_H */
