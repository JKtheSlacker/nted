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

#ifndef FIX_INTERMEDIATE_H

#define FIX_INTERMEDIATE_H
#include "config.h"
#include "command.h"

class NedChordOrRest;

class NedFixIntermediateCommand : public NedCommand {
	public:	
		NedFixIntermediateCommand(NedChordOrRest *chord);
		virtual void execute(bool adjust = false);
		virtual void unexecute(bool adjust = false);
	private:
		NedChordOrRest *m_chord;
		bool m_stemheight_relevant;
		double m_old_stemheight;
		double m_new_stemheight;
		bool m_new_beam_y_offs_relevant;
		double m_old_beam_y_offs;
		double m_new_beam_y_offs;
		bool m_slope_offs_relevant;
		int m_old_slope_offs;
		int m_new_slope_offs;
		bool m_status_change_relevant;
		unsigned int m_old_status;
		unsigned int m_new_status;
};

#endif /* FIX_INTERMEDIATE_H */
