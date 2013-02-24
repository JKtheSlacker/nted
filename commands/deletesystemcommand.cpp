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

#include "deletesystemcommand.h"
#include "page.h"
#include "resource.h"
#include "system.h"


NedDeleteSystemCommand::NedDeleteSystemCommand(NedSystem *system) :
m_system(system) {
	m_page = m_system->getPage();
	if ((m_systems_pointer = g_list_find(m_page->m_systems, system)) == NULL) {
		NedResource::Abort("NedDeleteSystemCommand::NedDeleteSystemCommand(1)");
	}
	if ((m_position = g_list_index(m_page->m_systems, system)) < 0) {
		NedResource::Abort("NedDeleteSystemCommand::NedDeleteSystemCommand(2)");
	}
}

void NedDeleteSystemCommand::execute(bool adjust /* = false */) {
#ifdef EXECUTE_DEBUG
	printf("\tNedDeleteSystemCommand::execute\n");
#endif
	m_page->m_systems = g_list_delete_link(m_page->m_systems, m_systems_pointer);
	/*
	if (adjust) {
		m_page->placeStaffs(1);
	}
	*/
}

void NedDeleteSystemCommand::unexecute(bool adjust /* = false */) {
#ifdef UNEXECUTE_DEBUG
	printf("\tNedDeleteSystemCommand::unexecute\n");
#endif
	m_page->m_systems = g_list_insert(m_page->m_systems, m_system, m_position);
	if ((m_systems_pointer = g_list_find(m_page->m_systems, m_system)) == NULL) {
		NedResource::Abort("NedDeleteSystemCommand::unexecute");
	}
	/*
	if (adjust) {
		m_page->placeStaffs(1);
	}
	*/
}

