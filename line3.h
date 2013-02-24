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

#ifndef LINE3_H

#define LINE3_H
#include <gtk/gtk.h>
#include <cairo.h>
#include "line.h"



class NedLine3 : public NedLine {
	protected:
		NedLinePoint *m_line_mid_point;
	public:
		NedLine3(NedLinePoint *start, NedLinePoint *mid, NedLinePoint *end);
		virtual ~NedLine3() {}
		virtual void draw(cairo_t *cr, double leftx, double topy, double zoom_factor, int zoom_level) = 0;
		virtual void computeParams();
		virtual bool isLine3() {return true;}
		virtual void startContextDialog(GtkWidget *ref) {}
		virtual bool isActive();
		NedLinePoint *getLineMidPoint() {return m_line_mid_point;}
		void setAllPointsToRel(NedLinePoint *initiator);
	protected:
		GList *m_intermediate_staves1, *m_intermediate_staves2;
		bool m_distributed1, m_distributed2;
	protected:
		double m_mid_x, m_mid_y;
	friend class NedLinePoint;
};

#endif /* LINE3_H */
