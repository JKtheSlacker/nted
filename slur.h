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

#ifndef SLUR_H

#define SLUR_H
#include <cairo.h>

class NedSlurPoint;

class NedSlur {
	public:
		NedSlur(NedSlurPoint *points[3]);
		void draw(cairo_t *cr, double leftx, double topy, double zoom_factor, int zoom_level);
		void computeParams();
		bool isActive();
		NedSlurPoint **getSlurPoints() {return m_slur_points;}
	private:
		NedSlurPoint *m_slur_points[3];
		double m_xpos0, m_ypos0;
		double m_xpos1, m_ypos1;
		double m_xpos2, m_ypos2;
		double m_xpos11, m_xpos12, m_ypos11, m_ypos12;
		double m_xpos11_l, m_xpos12_l, m_ypos11_l, m_ypos12_l;
		double m_xpos11_r, m_xpos12_r, m_ypos11_r, m_ypos12_r;
		double m_midx1_0, m_midy1_0, m_midx1_1, m_midy1_1;
		bool m_distributed;
	friend class NedSlurPoint;
};

#endif /* SLUR_H */
