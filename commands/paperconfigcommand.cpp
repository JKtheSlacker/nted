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

#include "paperconfigcommand.h"
#include "mainwindow.h"
#include "resource.h"

NedPaperConfigCommand::NedPaperConfigCommand(NedMainWindow *main_window, struct paper_info_struct *old_paper_info, bool old_portrait,
	struct paper_info_struct *new_paper_info, bool new_portrait, int oldscale, int scale):
m_main_window(main_window), m_old_paper_info(old_paper_info),
m_new_paper_info(new_paper_info), m_old_portrait(old_portrait), m_new_portrait(new_portrait), m_scale(scale), m_old_scale(oldscale)  {}


void NedPaperConfigCommand::execute(bool adjust /* = false */) {
#ifdef EXECUTE_DEBUG
	printf("\tNedPaperConfigCommand::execute\n");
#endif
	m_main_window->reconfig_paper(adjust, m_new_paper_info, m_new_portrait, m_scale);
}


void NedPaperConfigCommand::unexecute(bool adjust /* = false */) {
#ifdef UNEXECUTE_DEBUG
	printf("\tNedPaperConfigCommand::unexecute\n");
#endif
	m_main_window->reconfig_paper(adjust, m_old_paper_info, m_old_portrait, m_old_scale);
}
