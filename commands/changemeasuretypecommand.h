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

#ifndef  CHANGE_MEASURE_TYPE_COMMAND_H

#define  CHANGE_MEASURE_TYPE_COMMAND_H
#include "config.h"
#include "command.h"
#include <gtk/gtk.h>

class NedMainWindow;

class NedChangeMeasureTypeCommand : public NedCommand {
	public:	
		NedChangeMeasureTypeCommand(NedMainWindow *main_window, int measnum, int mtype, bool hide_following);
		virtual void execute(bool adjust = false);
		virtual void unexecute(bool adjust = false);
	private:
		NedMainWindow *m_main_window;
		int m_measnum;
		int m_mtype;
		int m_old_mtype;
		bool m_hide_following;
		bool m_old_hide_following;
		
		
};

#endif /* CHANGE_MEASURE_TYPE_COMMAND_H */
