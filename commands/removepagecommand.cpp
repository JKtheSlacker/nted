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

#include "removepagecommand.h"
#include "page.h"
#include "mainwindow.h"
#include "resource.h"
NedRemovePageCommand::NedRemovePageCommand(NedMainWindow *main_window, NedPage *page) :
m_removed_page(page), m_main_window(main_window) {}


void NedRemovePageCommand::execute(bool adjust /* = false */) {
#ifdef EXECUTE_DEBUG
	printf("\tNedRemovePageCommand::execute\n");
#endif

	GList *lptr;

	if ((lptr = g_list_find(m_main_window->m_pages, m_removed_page)) == NULL) { // friend !!
		NedResource::Abort("NedAppendNewPageCommand::execute");
	}
	m_main_window->m_pages = g_list_delete_link(m_main_window->m_pages, lptr);
	m_main_window->setAndUpdateClefTypeAndKeySig();
	m_main_window->updatePageCounter();
}


void NedRemovePageCommand::unexecute(bool adjust /* = false */) {
#ifdef UNEXECUTE_DEBUG
	printf("\tNedRemovePageCommand::unexecute\n");
#endif
	m_main_window->m_pages = g_list_append(m_main_window->m_pages, m_removed_page);
	m_main_window->setAndUpdateClefTypeAndKeySig();
	m_main_window->updatePageCounter();
}
