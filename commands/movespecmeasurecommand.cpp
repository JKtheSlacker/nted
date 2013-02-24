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

#include "movespecmeasurecommand.h"

NedMoveSpecMeasureCommand::NedMoveSpecMeasureCommand(NedMainWindow *main_window, int measnum, int dir) :
m_main_window(main_window), m_src_measure_num(measnum), m_src_measure_exists(false), m_dest_measure_exists(false), m_dir(dir)  {
	GList *lptr;
	bool found = false;
	m_dest_measure_num = measnum + dir;
	for (lptr = g_list_first(m_main_window->m_special_measures); lptr; lptr = g_list_next(lptr)) { // friend !!
		if (((SpecialMeasure *) lptr->data)->measure_number == m_src_measure_num) {
			found = true;
			m_orig_src_measure = (*((SpecialMeasure *) lptr->data));
  			if ((((SpecialMeasure *) lptr->data)->type & (~(REP1START | REP2END))) != 0) {
				m_copy_src_measure = (*((SpecialMeasure *) lptr->data));
				m_copy_src_measure.type &=(~(REP1START | REP2END));
				m_src_measure_exists = true;
			}
		}
	}
	if (!found) {
		NedResource::Abort("NedMoveSpecMeasureCommand::NedMoveSpecMeasureCommand");
	}
	for (lptr = g_list_first(m_main_window->m_special_measures); lptr; lptr = g_list_next(lptr)) { // friend !!
		if (((SpecialMeasure *) lptr->data)->measure_number == m_dest_measure_num) {
			m_orig_dest_measure = (*((SpecialMeasure *) lptr->data));
			m_copy_dest_measure = (*((SpecialMeasure *) lptr->data));
			m_copy_dest_measure.type |= (m_orig_src_measure.type & (REP1START | REP2END));
			m_copy_dest_measure.measure_number = measnum + dir;
			m_dest_measure_exists = true;
		}
	}
}


void NedMoveSpecMeasureCommand::execute(bool adjust /* = false */) {
	GList *lptr;
	SpecialMeasure *tmp;
#ifdef EXECUTE_DEBUG
	printf("\tNedMoveSpecMeasureCommand::execute\n");
#endif
	if (m_dest_measure_exists && m_src_measure_exists) {
		for (lptr = g_list_first(m_main_window->m_special_measures); lptr; lptr = g_list_next(lptr)) { // friend !!
			if (((SpecialMeasure *) lptr->data)->measure_number == m_src_measure_num) {
				break;
			}
		}
		if (lptr == NULL) {
			NedResource::Abort("NedMoveSpecMeasureCommand::execute(1)");
		}
		(*(SpecialMeasure *) lptr->data) = m_copy_src_measure;
		for (lptr = g_list_first(m_main_window->m_special_measures); lptr; lptr = g_list_next(lptr)) { // friend !!
			if (((SpecialMeasure *) lptr->data)->measure_number == m_dest_measure_num) {
				break;
			}
		}
		if (lptr == NULL) {
			NedResource::Abort("NedMoveSpecMeasureCommand::execute(2)");
		}
		(*(SpecialMeasure *) lptr->data) = m_copy_dest_measure;
		return;
	}
	
	if (m_src_measure_exists && !m_dest_measure_exists) {
		for (lptr = g_list_first(m_main_window->m_special_measures); lptr; lptr = g_list_next(lptr)) { // friend !!
			if (((SpecialMeasure *) lptr->data)->measure_number == m_src_measure_num) {
				break;
			}
		}
		if (lptr == NULL) {
			NedResource::Abort("NedMoveSpecMeasureCommand::execute(3)");
		}
		tmp = new SpecialMeasure();
		*tmp = (*(SpecialMeasure *) lptr->data);
		tmp->type = m_copy_src_measure.type;
		((SpecialMeasure *) lptr->data)->measure_number += m_dir;
		((SpecialMeasure *) lptr->data)->type &= (REP1START | REP2END);
		m_main_window->m_special_measures = g_list_insert_sorted(m_main_window->m_special_measures, tmp, (GCompareFunc ) SpecialMeasure::compare_spec_measures); 
		return;
	}
	if (!m_src_measure_exists && m_dest_measure_exists) {
		for (lptr = g_list_first(m_main_window->m_special_measures); lptr; lptr = g_list_next(lptr)) { // friend !!
			if (((SpecialMeasure *) lptr->data)->measure_number == m_src_measure_num) {
				break;
			}
		}
		if (lptr == NULL) {
			NedResource::Abort("NedMoveSpecMeasureCommand::execute(4)");
		}
		delete (SpecialMeasure *) lptr->data;
		m_main_window->m_special_measures = g_list_delete_link(m_main_window->m_special_measures, lptr);
		for (lptr = g_list_first(m_main_window->m_special_measures); lptr; lptr = g_list_next(lptr)) { // friend !!
			if (((SpecialMeasure *) lptr->data)->measure_number == m_dest_measure_num) {
				break;
			}
		}
		if (lptr == NULL) {
			NedResource::Abort("NedMoveSpecMeasureCommand::execute(5)");
		}
		(*((SpecialMeasure *) lptr->data)) = m_copy_dest_measure;
		return;
	}
	for (lptr = g_list_first(m_main_window->m_special_measures); lptr; lptr = g_list_next(lptr)) { // friend !!
		if (((SpecialMeasure *) lptr->data)->measure_number == m_src_measure_num) {
			((SpecialMeasure *) lptr->data)->measure_number += m_dir;
			break;
		}
	}
	if (lptr == NULL) {
		NedResource::Abort("NedMoveSpecMeasureCommand::execute(6)");
	}
}


void NedMoveSpecMeasureCommand::unexecute(bool adjust /* = false */) {
	GList *lptr;
	SpecialMeasure *tmp;
#ifdef UNEXECUTE_DEBUG
	printf("\tNedMoveSpecMeasureCommand::unexecute\n");
#endif
	if (m_dest_measure_exists && m_src_measure_exists) {
		for (lptr = g_list_first(m_main_window->m_special_measures); lptr; lptr = g_list_next(lptr)) { // friend !!
			if (((SpecialMeasure *) lptr->data)->measure_number == m_src_measure_num) {
				break;
			}
		}
		if (lptr == NULL) {
			NedResource::Abort("NedMoveSpecMeasureCommand::unexecute(1)");
		}
		(*(SpecialMeasure *) lptr->data) = m_orig_src_measure;
		for (lptr = g_list_first(m_main_window->m_special_measures); lptr; lptr = g_list_next(lptr)) { // friend !!
			if (((SpecialMeasure *) lptr->data)->measure_number == m_dest_measure_num) {
				break;
			}
		}
		if (lptr == NULL) {
			NedResource::Abort("NedMoveSpecMeasureCommand::unexecute(2)");
		}
		(*(SpecialMeasure *) lptr->data) = m_orig_dest_measure;
		return;
	}
	
	if (m_src_measure_exists && !m_dest_measure_exists) {
		for (lptr = g_list_first(m_main_window->m_special_measures); lptr; lptr = g_list_next(lptr)) { // friend !!
			if (((SpecialMeasure *) lptr->data)->measure_number == m_src_measure_num) {
				break;
			}
		}
		if (lptr == NULL) {
			NedResource::Abort("NedMoveSpecMeasureCommand::unexecute(4)");
		}
		delete (SpecialMeasure *) lptr->data;
		m_main_window->m_special_measures = g_list_delete_link(m_main_window->m_special_measures, lptr);
		for (lptr = g_list_first(m_main_window->m_special_measures); lptr; lptr = g_list_next(lptr)) { // friend !!
			if (((SpecialMeasure *) lptr->data)->measure_number == m_dest_measure_num) {
				break;
			}
		}
		if (lptr == NULL) {
			NedResource::Abort("NedMoveSpecMeasureCommand::unexecute(5)");
		}
		(*((SpecialMeasure *) lptr->data)) = m_orig_src_measure;
		return;
	}
	if (!m_src_measure_exists && m_dest_measure_exists) {
		for (lptr = g_list_first(m_main_window->m_special_measures); lptr; lptr = g_list_next(lptr)) { // friend !!
			if (((SpecialMeasure *) lptr->data)->measure_number == m_dest_measure_num) {
				break;
			}
		}
		if (lptr == NULL) {
			NedResource::Abort("NedMoveSpecMeasureCommand::unexecute(6)");
		}
		(*((SpecialMeasure *) lptr->data)) = m_orig_dest_measure;
		tmp = new SpecialMeasure();
		*tmp = m_orig_src_measure;
		m_main_window->m_special_measures = g_list_insert_sorted(m_main_window->m_special_measures, tmp, (GCompareFunc ) SpecialMeasure::compare_spec_measures); 
		return;
	}
	for (lptr = g_list_first(m_main_window->m_special_measures); lptr; lptr = g_list_next(lptr)) { // friend !!
		if (((SpecialMeasure *) lptr->data)->measure_number == m_dest_measure_num) {
			((SpecialMeasure *) lptr->data)->measure_number -= m_dir;
			break;
		}
	}
	if (lptr == NULL) {
		NedResource::Abort("NedMoveSpecMeasureCommand::unexecute(7)");
	}
}
