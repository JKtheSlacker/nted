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

#include "changemeasurespread.h"
#include "measure.h"
#include "resource.h"

NedChangeMeasureSpreadCommand::NedChangeMeasureSpreadCommand(NedMeasure *measure, int spread_direction) :
m_measure(measure) {
	m_old_spread = m_measure->m_spread_fac;
	m_old_special_spread = m_measure->m_special_spread;
	m_new_spread = m_old_spread + spread_direction * MEASURE_SPREAD_STEP;
}

void NedChangeMeasureSpreadCommand::execute(bool adjust /* = false */) {
#ifdef EXECUTE_DEBUG
	printf("\tNedChangeMeasureSpreadCommand::execute\n");
#endif
	m_measure->m_spread_fac = m_new_spread;
	m_measure->m_special_spread = true;
}


void NedChangeMeasureSpreadCommand::unexecute(bool adjust /* = false */) {
#ifdef UNEXECUTE_DEBUG
	printf("\tNedChangeMeasureSpreadCommand::unexecute\n");
#endif
	m_measure->m_spread_fac = m_old_spread;
	m_measure->m_special_spread = m_old_special_spread;
}
