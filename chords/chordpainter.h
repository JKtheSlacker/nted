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

#ifndef CHORD_PAINTER_H

#define CHORD_PAINTER_H
#include "config.h"
#include <gtk/gtk.h>
#include <cairo.h>


struct chord_str;
class NedFreeChord;


class NedChordPainter {
	public:
		static void draw_chord(cairo_t *cr, struct chord_str *chord_descr, int chordname, unsigned int status, bool limit_fret, double xpos, double ypos, double zoom, int zoom_level);
		static void draw_chord(cairo_t *cr, struct chord_str *chord_descr, char *chord_name, unsigned int status, bool limit_fret, double xpos, double ypos, double zoom, int zoom_level);
		static struct chord_str *searchCordByCode(char *code);
		static void writeLilyChord(FILE *fp, struct chord_str *chord_descr);
		static void writeLilyChordName(FILE *fp, NedFreeChord *freechord, unsigned int length, unsigned int duration, int dotcount, unsigned int *midi_len);
		static const char *roots[];
		static const char *roots2[];
		static const char *modies[];
		static const char *modies2[];
	private:
		static const unsigned int minor[];
		static const char addoff[];
		static const char *modies3[];
		static const char *modies4[];
};

#endif /* CHORD_PAINTER_H */
