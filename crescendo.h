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

#ifndef CRESCENDO_H

#define CRESCENDO_H
#include "line.h"

class NedCrescendo : public NedLine {
	public:
		NedCrescendo(NedLinePoint *start, NedLinePoint *end, bool decrescendo);
		NedCrescendo(NedLinePoint *start, NedLinePoint *end, bool decrescendo, int volume_diff);
		virtual void startContextDialog(GtkWidget *ref);
		virtual void draw(cairo_t *cr, double leftx, double topy, double zoom_factor, int zoom_level);
		virtual int getLineType() {return m_decrescendo ? LINE_DECRESCENDO : LINE_CRESCENDO;}
		bool isDecrescendo() {return m_decrescendo;}
	private:
		bool m_decrescendo;
	public:
		int m_volume_diff;
};

#endif /* CRESCENDO_H */
