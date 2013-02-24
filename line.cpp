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

#include "line.h"
#include "linepoint.h"
#include "page.h"
#include "staff.h"
#include "chordorrest.h"
#include "system.h"
#include <math.h>


NedLine::NedLine(NedLinePoint *start, NedLinePoint *end) : m_line_start_point(start), m_line_end_point(end), m_intermediate_staves(NULL), m_distributed(false),
m_allow_drawing (false) {
	start->setLine(this);
	end->setLine(this);
	computeParams();
}

bool NedLine::isActive() {
	if (m_line_start_point->isActive()) return true;
	if (m_line_end_point->isActive()) return true;
	return false;
}


void NedLine::computeParams() {
	NedStaff *start_staff, *end_staff;
	NedLinePoint *tmp_point;

	if (m_line_end_point->less_than(m_line_start_point)) {
		tmp_point = m_line_start_point;
		m_line_start_point = m_line_end_point;
		m_line_end_point = tmp_point;
	}
	m_line_start_point->getXYPosAndStaff(&m_start_x, &m_start_y, &start_staff);
	m_line_end_point->getXYPosAndStaff(&m_end_x, &m_end_y, &end_staff);
	m_distributed = (start_staff != end_staff);

	m_allow_drawing = end_staff->getPage()->getPageNumber() - start_staff->getPage()->getPageNumber() < 2;
	if (m_allow_drawing && m_distributed) {
		g_list_free(m_intermediate_staves);
		m_intermediate_staves = NULL;
		start_staff->getPage()->getLinePoints(start_staff, end_staff, &m_intermediate_staves);
	}
}

void NedLine::setAllPointsToRel(NedLinePoint *initiator) {
	if (m_line_start_point != initiator) {
		m_line_start_point->toRel2();
	}
	if (m_line_end_point != initiator) {
		m_line_end_point->toRel2();
	}
}


unsigned long long NedLine::computeMidiDuration() {
	long long duration;
	GList *lptr, *last;

	if (m_distributed) {
		duration = m_line_start_point->getElement()->getSystem()->getSystemEndTime() - m_line_start_point->getElement()->getMidiTime();
		duration += m_line_end_point->getElement()->getMidiTime();
		last = g_list_last(m_intermediate_staves);
		lptr = g_list_first(m_intermediate_staves);
		for (lptr = g_list_next(lptr); lptr != last; lptr = g_list_next(lptr)) {
			duration += ((NedStaff *) lptr->data)->getSystem()->getSystemEndTime();
		}
		return duration;
	}
	else {
		return m_line_end_point->getElement()->getMidiTime() - m_line_start_point->getElement()->getMidiTime();
	}
}


