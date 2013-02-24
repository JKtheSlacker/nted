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

#ifndef MOVE_SPEC_MEASURE_COMMAND_H

#define MOVE_SPEC_MEASURE_COMMAND_H
#include "config.h"
#include "command.h"

#include "mainwindow.h"

class NedMoveSpecMeasureCommand : public NedCommand {
	public:	
		NedMoveSpecMeasureCommand(NedMainWindow *main_window, int measnum, int dir);
		virtual void execute(bool adjust = false);
		virtual void unexecute(bool adjust = false);
	private:
		NedMainWindow *m_main_window;
		int m_src_measure_num, m_dest_measure_num;
		SpecialMeasure m_orig_src_measure, m_copy_src_measure;
		SpecialMeasure m_orig_dest_measure, m_copy_dest_measure;
		bool m_src_measure_exists, m_dest_measure_exists;
		int m_dir;

};

#endif /* MOVE_SPEC_MEASURE_COMMAND_H */
