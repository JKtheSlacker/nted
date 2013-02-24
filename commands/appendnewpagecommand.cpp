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

#include "appendnewpagecommand.h"
#include "page.h"
#include "mainwindow.h"
#include "resource.h"

NedAppendNewPageCommand::NedAppendNewPageCommand(NedMainWindow *main_window, int number, unsigned int measure_number) :
m_main_window(main_window) {
	struct paper_info_struct *paperinfo;
	bool portrait;
	
	paperinfo = m_main_window->getCurrentPaper();
	portrait = m_main_window->getPortrait();
        double w = portrait ? paperinfo->width : paperinfo->height;
        double h = portrait ? paperinfo->height : paperinfo->width;
        w /= PS_ZOOM * m_main_window->getCurrentScale();
        h /= PS_ZOOM * m_main_window->getCurrentScale();
	m_new_page = new NedPage(main_window, w, h, number, measure_number, TRUE);
}


void NedAppendNewPageCommand::execute(bool adjust /* = false */) {
#ifdef EXECUTE_DEBUG
	printf("\tNedAppendNewPageCommand::execute\n");
#endif


	m_main_window->m_pages = g_list_append(m_main_window->m_pages, m_new_page); // friend !!
	m_main_window->updatePageCounter();
	m_main_window->setAndUpdateClefTypeAndKeySig();
}


void NedAppendNewPageCommand::unexecute(bool adjust /* = false */) {
#ifdef UNEXECUTE_DEBUG
	printf("\tNedAppendNewPageCommand::unexecute\n");
#endif
	GList *lptr;

	lptr = g_list_last(m_main_window->m_pages); // friend !!
	if (lptr->data != m_new_page) {
		NedResource::Abort("NedAppendNewPageCommand::unexecute");
	}

	m_main_window->m_pages = g_list_delete_link(m_main_window->m_pages, lptr); // friend !!
	m_main_window->updatePageCounter();
	m_main_window->setAndUpdateClefTypeAndKeySig();
}
