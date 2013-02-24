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

#ifndef LINE_H

#define LINE_H
#include <gtk/gtk.h>
#include <cairo.h>

#define LINE_CRESCENDO (1 << 0)
#define LINE_DECRESCENDO (1 << 1)
#define LINE_OCTAVATION1 (1 << 3)
#define LINE_OCTAVATION_1 (1 << 4)
#define LINE_OCTAVATION2 (1 << 5)
#define LINE_OCTAVATION_2 (1 << 6)
#define LINE_OCTAVIATION_MASK (LINE_OCTAVATION1 | LINE_OCTAVATION_1 | LINE_OCTAVATION2 | LINE_OCTAVATION_2)
#define LINE_ACCELERANDO (1 << 7)
#define LINE_RITARDANDO (1 << 8)
#define LINE_TEMPO_CHANGE (LINE_ACCELERANDO | LINE_RITARDANDO)

class NedLinePoint;

class NedLine {
	public:
		NedLine(NedLinePoint *start, NedLinePoint *end);
		virtual ~NedLine() {}
		virtual void draw(cairo_t *cr, double leftx, double topy, double zoom_factor, int zoom_level) = 0;
		virtual void computeParams();
		virtual void startContextDialog(GtkWidget *ref) {}
		virtual bool isActive();
		virtual bool isLine3() {return false;}
		NedLinePoint *getLineStartPoint() {return m_line_start_point;}
		NedLinePoint *getLineEndPoint() {return m_line_end_point;}
		virtual int getLineType() = 0;
		virtual unsigned long long computeMidiDuration();
		virtual void setAllPointsToRel(NedLinePoint *initiator);
	protected:
		NedLinePoint *m_line_start_point, *m_line_end_point;
		double m_start_x, m_start_y;
		double m_end_x, m_end_y;
		GList *m_intermediate_staves;
		bool m_distributed;
		bool m_allow_drawing;
	friend class NedLinePoint;
};

#endif /* LINE_H */
