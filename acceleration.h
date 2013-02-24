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

#ifndef ACCELERATION_H

#define ACCELERATION_H
#include "line3.h"

class NedAcceleration : public NedLine3 {
	public:
		NedAcceleration(NedLinePoint *start, NedLinePoint *mid, NedLinePoint *end, bool ritardando);
		NedAcceleration(NedLinePoint *start, NedLinePoint *mid, NedLinePoint *end, bool ritardando, double tempodiff);
		virtual void draw(cairo_t *cr, double leftx, double topy, double zoom_factor, int zoom_level);
		virtual int getLineType() {return m_ritardando ? LINE_RITARDANDO : LINE_ACCELERANDO;}
		virtual void startContextDialog(GtkWidget *ref);
		bool isRitardando() {return m_ritardando;}
		double m_tempodiff;
	private:
		bool m_ritardando;
};

#endif /* ACCELERATION_H */
