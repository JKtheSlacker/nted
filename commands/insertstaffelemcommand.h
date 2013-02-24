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

#ifndef INSERT_STAFF_ELEM_COMMAND_H

#define INSERT_STAFF_ELEM_COMMAND_H
#include "config.h"
#include "command.h"
#include <gtk/gtk.h>

class NedChordOrRest;
class NedStaff;

class NedInsertStaffElemCommand : public NedCommand {
	public:	
		NedInsertStaffElemCommand(NedChordOrRest *staffelem, int clef_number_so_far, int octave_shift_so_far, NedStaff *staff, bool do_adjust);
		virtual void execute(bool adjust = false);
		virtual void unexecute(bool adjust = false);
	private:
		int m_staff_nr;
		NedStaff *m_staff;
		NedChordOrRest *m_staff_elem;
		int m_line_dist;
		bool m_do_adjust;

};

#endif /* INSERT_STAFF_ELEM_COMMAND_H */
