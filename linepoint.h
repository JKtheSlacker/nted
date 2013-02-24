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

#ifndef LINE_POINT_H

#define LINE_POINT_H
#include "freereplaceable.h"
#include <cairo.h>

class NedStaff;
class NedLine;
class NedCrescendo;

class NedLinePoint : public NedFreeReplaceable {
	public:
		NedLinePoint(NedLine *line, int p_nr);
		NedLinePoint(NedLine *line, int p_nr, NedChordOrRest *element);
		NedLinePoint(NedLine *line, int p_nr, NedChordOrRest *element, double x, double y, bool relative);
		void getXYPosAndStaff(double *xpos, double *ypos, NedStaff **staff);
		virtual NedFreeReplaceable *clone(struct addr_ref_str **slurlist, NedChordOrRest *newelement, GtkWidget *drawing_area = NULL, NedMainWindow *main_window = NULL);
		virtual void adjust_pointers(struct addr_ref_str *slurlist);
		NedStaff *getStaff();
		NedLinePoint *getLineStartPoint();
		NedLinePoint *getLineEndPoint();
		virtual void shift(double x, double y, double zoom_factor, double current_scale, double leftx, double topy);
		void setLine(NedLine *line) {m_line = line;}
		NedLine *getLine() {return m_line;}
		virtual void draw(cairo_t *cr, double leftx, double topy, double zoom_factor, int zoom_level, double scale);
		virtual int getType() {return FREE_PLACEABLE_LINE_POINT;}
		virtual void startContextDialog(GtkWidget *ref);
		virtual void toRel();
		virtual void toRel2();
		void virtual recompute();
		int getNumber() {return m_point_number;}
	private:
		int m_point_number;
		NedLine *m_line;
	friend class NedLine;
	friend class NedCrescendo;
};

#endif /* LINE_POINT_H */
