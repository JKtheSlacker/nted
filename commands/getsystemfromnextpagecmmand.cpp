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

#include "getsystemfromnextpagecmmand.h"
#include "system.h"
#include "page.h"


NedGetSystemFromNextPageCommand::NedGetSystemFromNextPageCommand(GList *system_ptr, NedPage *page, NedPage *other_page) :
m_system_ptr(system_ptr), m_page(page),
m_other_page(other_page){ 
	m_system = (NedSystem *) m_system_ptr->data;
	m_other_systems_ptr = m_other_page->getSystemsList();
	m_systems_ptr = m_page->getSystemsList();
}

void NedGetSystemFromNextPageCommand::execute(bool adjust /* = false */) {
#ifdef EXECUTE_DEBUG
	printf("\tNedGetSystemFromNextPageCommand::execute\n");
#endif
	if (adjust) {
		if ((m_system_ptr = g_list_find(*m_other_systems_ptr, m_system)) == NULL) {
		 	NedResource::Abort("NedGetSystemFromNextPageCommand::unexecute");
		}
	}
	*m_other_systems_ptr = g_list_delete_link(*m_other_systems_ptr, m_system_ptr);
	m_other_page->placeStaffs(1);
	m_other_page->renumberSystems();
	*m_systems_ptr = g_list_append(*m_systems_ptr, m_system);
	m_system->changePageInfo(m_page);
	if ((m_system_ptr = g_list_find(*m_systems_ptr, m_system)) == NULL) {
		 NedResource::Abort("NedGetSystemFromNextPageCommand::unexecute");
	}
	m_page->placeStaffs(1);
	m_page->renumberSystems();
}

void NedGetSystemFromNextPageCommand::unexecute(bool adjust /* = false */) {
#ifdef UNEXECUTE_DEBUG
	printf("\tNedGetSystemFromNextPageCommand::unexecute\n");
#endif
	if ((m_system_ptr = g_list_find(*m_systems_ptr, m_system)) == NULL) {
		 NedResource::Abort("NedGetSystemFromNextPageCommand::unexecute");
	}
	*m_systems_ptr = g_list_remove(*m_systems_ptr, m_system);
	m_page->placeStaffs(0);
	m_page->renumberSystems();
	*m_other_systems_ptr = g_list_insert(*m_other_systems_ptr, m_system, 0);
	if ((m_system_ptr = g_list_find(*m_other_systems_ptr, m_system)) == NULL) {
		 NedResource::Abort("NedGetSystemFromNextPageCommand::unexecute");
	}
	m_system->changePageInfo(m_other_page);
	m_other_page->placeStaffs(1);
	m_other_page->renumberSystems();
}

