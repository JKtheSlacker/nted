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

#include "movesystemcommand.h"
#include "system.h"
#include "page.h"


NedMoveSystemCommand::NedMoveSystemCommand(NedPage *page, NedSystem *system, NedPage *other_page ) :
m_page(page), m_other_page(other_page), m_system(system) { 
	if ((m_position = g_list_index(m_page->m_systems, m_system)) < 0) {
		NedResource::Abort("NedDeleteSystemCommand::NedDeleteSystemCommand");
	}
}

void NedMoveSystemCommand::execute(bool adjust /* = false */) {
#ifdef EXECUTE_DEBUG
	printf("\tNedMoveSystemCommand::execute\n");
#endif
	m_page->m_systems = g_list_remove(m_page->m_systems, m_system);
	m_page->placeStaffs(false, 0);
	m_page->renumberSystems();
	m_other_page->m_systems = g_list_insert(m_other_page->m_systems, m_system, 0);
	m_system->changePageInfo(m_other_page);
	m_other_page->placeStaffs(false, 0);
	m_other_page->renumberSystems();
}

void NedMoveSystemCommand::unexecute(bool adjust /* = false */) {
#ifdef UNEXECUTE_DEBUG
	printf("\tNedMoveSystemCommand::unexecute\n");
#endif
	m_other_page->m_systems = g_list_remove(m_other_page->m_systems, m_system);
	m_other_page->placeStaffs(false, 0);
	m_other_page->renumberSystems();
	m_page->m_systems = g_list_insert(m_page->m_systems, m_system, m_position);
	m_system->changePageInfo(m_page);
	/*
	if ((m_system_ptr = g_list_find(*m_systems_ptr, m_system)) == NULL) {
		 NedResource::Abort("NedMoveSystemCommand::unexecute");
	}
	*/
	m_page->placeStaffs(false, 0);
	m_page->renumberSystems();
}

