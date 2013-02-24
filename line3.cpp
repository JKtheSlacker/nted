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

#include "line3.h"
#include "linepoint.h"
#include "page.h"
#include "staff.h"
#include "chordorrest.h"
#include "system.h"
#include <math.h>


NedLine3::NedLine3(NedLinePoint *start, NedLinePoint *mid, NedLinePoint *end) : NedLine(start, end),  m_line_mid_point(mid),
	m_intermediate_staves1(NULL), m_intermediate_staves2(NULL) {
	mid->setLine(this);
	computeParams();
}

bool NedLine3::isActive() {
	if (m_line_start_point->isActive()) return true;
	if (m_line_mid_point->isActive()) return true;
	if (m_line_end_point->isActive()) return true;
	return false;
}


void NedLine3::computeParams() {
	NedStaff *start_staff, *mid_staff, *end_staff;
	NedLinePoint *tmp_point;

	if (m_line_end_point->less_than(m_line_start_point)) {
		tmp_point = m_line_start_point;
		m_line_start_point = m_line_end_point;
		m_line_end_point = tmp_point;
	}
	if (m_line_mid_point->less_than(m_line_start_point)) {
		tmp_point = m_line_mid_point;
		m_line_mid_point = m_line_start_point;
		m_line_start_point = tmp_point;
	}
	if (m_line_end_point->less_than(m_line_mid_point)) {
		tmp_point = m_line_mid_point;
		m_line_mid_point = m_line_end_point;
		m_line_end_point = tmp_point;
	}
	m_line_start_point->getXYPosAndStaff(&m_start_x, &m_start_y, &start_staff);
	m_line_mid_point->getXYPosAndStaff(&m_mid_x, &m_mid_y, &mid_staff);
	m_line_end_point->getXYPosAndStaff(&m_end_x, &m_end_y, &end_staff);
	m_distributed1 = (start_staff != mid_staff);
	m_allow_drawing = true;
	if (m_distributed1) {
		g_list_free(m_intermediate_staves1);
		m_intermediate_staves1 = NULL;
		start_staff->getPage()->getLinePoints(start_staff, mid_staff, &m_intermediate_staves1);
		m_allow_drawing = mid_staff->getPage()->getPageNumber() - start_staff->getPage()->getPageNumber() < 2;
	}
	m_distributed2 = (mid_staff != end_staff);
	if (m_distributed2) {
		g_list_free(m_intermediate_staves2);
		m_intermediate_staves2 = NULL;
		mid_staff->getPage()->getLinePoints(mid_staff, end_staff, &m_intermediate_staves2);
		m_allow_drawing = m_allow_drawing && end_staff->getPage()->getPageNumber() - mid_staff->getPage()->getPageNumber() < 2;
	}
}

void NedLine3::setAllPointsToRel(NedLinePoint *initiator) {
	if (m_line_start_point != initiator) {
		m_line_start_point->toRel2();
	}
	if (m_line_mid_point != initiator) {
		m_line_mid_point->toRel2();
	}
	if (m_line_end_point != initiator) {
		m_line_end_point->toRel2();
	}
}

