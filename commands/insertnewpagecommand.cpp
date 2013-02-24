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

#include "insertnewpagecommand.h"
#include "page.h"
#include "mainwindow.h"
#include "resource.h"

NedInsertNewPageCommand::NedInsertNewPageCommand(NedMainWindow *main_window, int pos, unsigned int start_measure_number) :
m_main_window(main_window), m_pos(pos) {
	struct paper_info_struct *paperinfo;
	bool portrait;
	
	paperinfo = m_main_window->getCurrentPaper();
	portrait = m_main_window->getPortrait();
        double w = portrait ? paperinfo->width : paperinfo->height;
        double h = portrait ? paperinfo->height : paperinfo->width;
        w /= PS_ZOOM * m_main_window->getCurrentScale();
        h /= PS_ZOOM * m_main_window->getCurrentScale();
	m_new_page = new NedPage(main_window, w, h, m_pos, start_measure_number, TRUE);
}


void NedInsertNewPageCommand::execute(bool adjust /* = false */) {
#ifdef EXECUTE_DEBUG
	printf("\tNedInsertNewPageCommand::execute\n");
#endif
	m_main_window->m_pages = g_list_insert(m_main_window->m_pages, m_new_page, m_pos); // friend !!
	m_main_window->renumberPages();
	m_main_window->setAndUpdateClefTypeAndKeySig();
	m_main_window->updatePageCounter();
}


void NedInsertNewPageCommand::unexecute(bool adjust /* = false */) {
#ifdef UNEXECUTE_DEBUG
	printf("\tNedInsertNewPageCommand::unexecute\n");
#endif
	GList *lptr;

	if ((lptr = g_list_find(m_main_window->m_pages, m_new_page)) == NULL) { // friend !!
		NedResource::Abort("NedInsertNewPageCommand::unexecute");
	}
	m_main_window->m_pages = g_list_delete_link(m_main_window->m_pages, lptr); // friend !!

	m_main_window->renumberPages();
	m_main_window->setAndUpdateClefTypeAndKeySig();
	m_main_window->updatePageCounter();
}
