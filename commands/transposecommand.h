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

#ifndef TRANSPOSE_COMMAND_H

#define TRANSPOSE_COMMAND_H
#include "config.h"
#include "command.h"
#include <gtk/gtk.h>

class NedMainWindow;

class NedTransposeCommand : public NedCommand {
	public:	
		NedTransposeCommand(NedMainWindow *main_window, int pitchdist, int staff_count, bool *staff_list, GList *selected_group);
		virtual void execute(bool adjust = false);
		virtual void unexecute(bool adjust = false);
	private:
		NedMainWindow *m_main_window;
		int m_pitch_dist;
		bool *m_staff_list;
		GList *m_selected_group;

};

#endif /* TRANSPOSE_COMMAND_H */
