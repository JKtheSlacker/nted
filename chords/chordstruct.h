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

#define ROOT_BITS 5
#define MODI_BITS 5
#define ROOT_MASK ((1 << ROOT_BITS) - 1)
#define MOFI_MASK (((1 << MODI_BITS) - 1) << ROOT_BITS)
#define GUITAR_CHORD_DRAW_DIAGRAM (1 << 16)
#define GUITAR_CHORD_DRAW_TEXT (1 << 17)

struct chord_str {
	const char *id;
	const char *chordname1;
	const char *chordname2;
	int fret;
	int numfrets;
	const char *fret_s;
#define CHORD_MUTE (1 << 5)
#define CHORD_END_OF_BARREE_RIGHT (1 << 6)
#define CHORD_END_OF_BARREE_LEFT (1 << 7)
	unsigned char s[6];
};

#define F_DIST 30.0
#define F_COUNT 5
#define S_DIST 22.0
#define GUITAR_WIDTH (5 * S_DIST)

extern struct chord_str ned_chords[];
