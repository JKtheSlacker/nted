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

#ifndef CHANGE_ID_NOTE_COMMAND_H

#define CHANGE_ID_NOTE_COMMAND_H
#include "config.h"
#include "command.h"

class NedIdNote;


class NedChangeIdNoteCommand : public NedCommand {
	public:	
		NedChangeIdNoteCommand(NedIdNote *id_note, unsigned long long new_start, unsigned long long new_stop, int new_pitch);
		virtual void execute(bool adjust = false);
		virtual void unexecute(bool adjust = false);
	private:
		NedIdNote *m_id_note;
		unsigned long long m_old_midi_start, m_new_midi_start;
		unsigned long long m_old_midi_stop, m_new_midi_stop;
		int m_old_pitch, m_new_pitch;
		bool m_old_valid, m_new_valid;
};

#endif /* CHANGE_ID_NOTE_COMMAND_H */
