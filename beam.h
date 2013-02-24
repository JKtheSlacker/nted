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

#ifndef BEAM_H

#define BEAM_H
#include "config.h"
#include "resource.h"
#include "resource.h"

#include <gtk/gtk.h>
#include <cairo.h>
#define SLOPE_COUNT 7

class NedMainWindow;
class NedPage;
class NedSystem;
class NedStaff;
class NedVoice;
class NedChordOrRest;


class NedBeam {
	public:
		NedBeam(GList *beamed_chords_list);
		~NedBeam();
		void print();
		void draw(cairo_t *cr);
		void drawPostscript(cairo_t *cr);
		void insertChord(NedChordOrRest *chord, int newpos);
		void removeChord(NedChordOrRest *chord);
		bool isEqual(GList *beamlist, NedChordOrRest *except, int *diff_pos);
		void changeStemDir(int dir);
		NedMainWindow *getMainWindow();
		NedPage *getPage();
		NedSystem *getSystem();
		NedStaff *getStaff();
		double getTopOfBeam();
		double getBottomOfBeam();
		int getChordCount();
		int getPosition(NedChordOrRest *chord);
		double getYPosAt(double xpos);
		void setTupletMarker(int tuplet_marker, double *topy, double *boty);
		bool isBeamUp() {return m_up;}
		void computeBeam(int staff_voice_offs);
		void freeChords();
		void reconnect();
		void saveBeam(FILE *fp);
		NedChordOrRest *getFirst();
		NedChordOrRest *getLast();
		double getBeamLength();
		bool isFirstInBeam(NedChordOrRest *chord);
		bool isLastInBeam(NedChordOrRest *chord);
		bool m_needed; // aux.
		double getSlope() {return m_slope;}
	private:
		void computeGraceBeam(NedChordOrRest *first, NedChordOrRest *last);
		void drawGraceBeam(cairo_t *cr);
		static double m_slopes[SLOPE_COUNT];
		GList *m_beamed_chords;
		double m_slope;
		double m_start_x, m_start_y;
		double m_end_x, m_end_y;
		double m_tuplet_x, m_tuplet_y;
		bool m_up;
		int m_tuplet_marker;
		int m_beam_count;
};

#endif /* BEAM_H */
