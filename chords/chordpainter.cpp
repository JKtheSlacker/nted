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

#include "chordpainter.h"
#include "resource.h"
#include "chordstruct.h"
#include "freechord.h"
#include <cairo.h>
#include <math.h>
#include <ctype.h>

const char *NedChordPainter::roots[] = {"C", "C#", "Db", "D", "D#", "Eb", "E", "F", "F#", "Gb", "G", "G#", "Ab", "A", "A#", "Bb", "B", NULL};
const char *NedChordPainter::roots2[] = {"c", "cis", "des", "d", "dis", "es", "e", "f", "fis", "ges", "g", "gis", "as", "a", "ais", "bes", "b", NULL};
const char *NedChordPainter::modies[] = {"Major", "Minor", "dim", "aug", "sus", "6", "7", "maj7", "9", "add9", "m6", "m7", "mmaj7", "m9", 
					"11", "7sus4", "13", "6add9", "-5", "7-5", "7maj5", "maj9", NULL};
const char *NedChordPainter::modies2[] = {"", "m", "dim", "aug", "sus", "6", "7", "maj7", "9", "add9", "m6", "m7", "mmaj7", "m9", 
					"11", "7sus4", "13", "6add9", "-5", "7-5", "7maj5", "maj9", NULL};
const unsigned int NedChordPainter::minor[] = {0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 
					0, 0, 0, 0, 0, 0, 0, 0, 0};
const char NedChordPainter::addoff[] = {0, 1, -1, 0, 1, -1, 0, 0, 1, -1, 0, 1, -1, 0, 1, -1, 0};
const char *NedChordPainter::modies3[] = {"", "", "dim", "aug", "sus", "6", "7", "maj7", "9", "add9", "6", "7", "maj7", "9", 
					"11", "7sus4", "13", "6add9", "-5", "7-5", "7maj5", "maj9", NULL};
const char *NedChordPainter::modies4[] = {"", "m", "dim", "aug", "sus4", "6", "7", "maj7", "9", "", "m6", "m7", "", "m9", 
					"11", "7sus4", "13", "", "", "7-5", "7maj5", "maj9", NULL};

void NedChordPainter::draw_chord(cairo_t *cr, struct chord_str *chord_descr, int chordname, unsigned int status , bool limit_fret, double xpos, double ypos, double zoom, int zoom_level) {
	int i;
#define NO_BARRE 1000
	int barre = NO_BARRE;
	int barre_fret = -1;
	unsigned int block_for_barre = 0;
	double arc_y;
	bool restore_font = false;
	int fret_count = limit_fret ? chord_descr->numfrets : F_COUNT;
//#define F_COUNT (chord_descr->numfrets)
#define F_DIST_Z (F_DIST * zoom)
#define S_DIST_Z (S_DIST * zoom)
#define GUITAR_WIDTH_Z (GUITAR_WIDTH * zoom)

#define B_WIDTH (zoom * 6.0)
#define F_WIDTH (zoom * 2.0)
#define S_WIDTH (zoom * 4.0)

	if (status & GUITAR_CHORD_DRAW_DIAGRAM) {
		cairo_new_path(cr);
		cairo_set_line_width(cr, B_WIDTH);
		cairo_move_to(cr, xpos, ypos);
		cairo_line_to(cr, xpos + GUITAR_WIDTH_Z, ypos);
		cairo_stroke(cr);

		cairo_new_path(cr);
		cairo_set_line_width(cr, F_WIDTH);
		for (i = 0; i < fret_count; i++) {
			cairo_move_to(cr, xpos, ypos + i * F_DIST_Z);
			cairo_line_to(cr, xpos + GUITAR_WIDTH_Z, ypos + i * F_DIST_Z);
		}
		cairo_stroke(cr);
		cairo_new_path(cr);
		cairo_set_line_width(cr, S_WIDTH);
		for (i = 0; i < 6; i++) {
			cairo_move_to(cr, xpos + S_WIDTH / 2.0 + i * S_DIST_Z, ypos - B_WIDTH / 2.0);
			cairo_line_to(cr, xpos + S_WIDTH / 2.0 + i * S_DIST_Z, ypos + fret_count * F_DIST_Z - B_WIDTH / 2.0);
		}
		cairo_stroke(cr);

#define X_WIDTH (zoom * 2.0)
#define X_LONG (zoom * 6.0)
#define X_DIST (zoom * 8.0)

#define O_DIST (zoom * 8.0)
#define O_WIDTH (zoom * 1.4)
#define O_RAD (zoom * 6.0)

#define P_RAD (zoom * 6.0)

#define BARRE_WIDTH (zoom * 6.0)

		for (i = 0; i < 6; i++) {
			if ((chord_descr->s[i] & 0xf0) == CHORD_END_OF_BARREE_RIGHT) {
				if (barre != NO_BARRE) {
					NedResource::Abort("CHORD_END_OF_BARREE_RIGHT\n");
				}
				barre = i;
				barre_fret = (chord_descr->s[i] & 0xf);
				block_for_barre = (1 << i) - 1;
				block_for_barre = ~block_for_barre;
				cairo_new_path(cr);
				cairo_set_line_width(cr, BARRE_WIDTH);
				arc_y = ypos - B_WIDTH / 2.0 + (barre_fret - chord_descr->fret + 1) * F_DIST_Z - F_DIST_Z / 2.0;
				cairo_move_to(cr, xpos + S_WIDTH / 2.0 + i * S_DIST_Z, arc_y);
				cairo_line_to(cr, xpos + GUITAR_WIDTH_Z, arc_y);
				cairo_stroke(cr);
				cairo_new_path(cr);
				cairo_arc(cr, xpos + S_WIDTH / 2.0 + i * S_DIST_Z, arc_y, P_RAD, 0.0, 2.0 * M_PI);
				cairo_arc(cr, xpos + S_WIDTH / 2.0 + 5 * S_DIST_Z, arc_y, P_RAD, 0.0, 2.0 * M_PI);
				cairo_fill(cr);
			}
			if ((chord_descr->s[i] & 0xf0) == CHORD_END_OF_BARREE_LEFT) {
				/*
				if (barre != NO_BARRE) {
					NedResource::Abort("CHORD_END_OF_BARREE_LEFT\n");
				}
				*/
				block_for_barre = (1 << i) - 1;
				barre = -i;
				barre_fret = (chord_descr->s[i] & 0xf);
				cairo_new_path(cr);
				cairo_set_line_width(cr, BARRE_WIDTH);
				arc_y = ypos - B_WIDTH / 2.0 + (barre_fret - chord_descr->fret + 1) * F_DIST_Z - F_DIST_Z / 2.0;
				cairo_move_to(cr, xpos + S_WIDTH / 2.0 + i * S_DIST_Z, arc_y);
				cairo_line_to(cr, xpos + S_WIDTH / 2.0, arc_y); 
				cairo_stroke(cr);
				cairo_new_path(cr);
				cairo_arc(cr, xpos + S_WIDTH / 2.0 + i * S_DIST_Z, arc_y, P_RAD, 0.0, 2.0 * M_PI);
				cairo_arc(cr, xpos + S_WIDTH / 2.0 , arc_y, P_RAD, 0.0, 2.0 * M_PI);
				cairo_fill(cr);
			}
		}
		for (i = 0; i < 6; i++) {
			if ((chord_descr->s[i] & 0xf0) == CHORD_MUTE) {
				cairo_new_path(cr);
				cairo_set_line_width(cr, X_WIDTH);
				cairo_move_to(cr, xpos + S_WIDTH / 2.0 + i * S_DIST_Z - X_LONG, ypos - B_WIDTH / 2.0 - X_DIST - X_LONG);
				cairo_line_to(cr, xpos + S_WIDTH / 2.0 + i * S_DIST_Z + X_LONG, ypos - B_WIDTH / 2.0 - X_DIST + X_LONG);
				cairo_move_to(cr, xpos + S_WIDTH / 2.0 + i * S_DIST_Z + X_LONG, ypos - B_WIDTH / 2.0 - X_DIST - X_LONG);
				cairo_line_to(cr, xpos + S_WIDTH / 2.0 + i * S_DIST_Z - X_LONG, ypos - B_WIDTH / 2.0 - X_DIST + X_LONG);
				cairo_stroke(cr);
			}
			else if ((chord_descr->s[i] & 0xf) == 0) {
				cairo_new_path(cr);
				cairo_set_line_width(cr, O_WIDTH);
				cairo_arc(cr, xpos + S_WIDTH / 2.0 + i * S_DIST_Z, ypos - B_WIDTH / 2.0 - O_DIST,
					O_RAD, 0.0, 2.0 * M_PI);
				cairo_stroke(cr);
			}
			else if (chord_descr->s[i] != barre_fret || (((1 << i) & block_for_barre)) == 0) {
				cairo_new_path(cr);
				cairo_arc(cr, xpos + S_WIDTH / 2.0 + i * S_DIST_Z,
					ypos - B_WIDTH / 2.0 + ((0xf & chord_descr->s[i]) - chord_descr->fret + 1) * F_DIST_Z - F_DIST_Z / 2.0,
					P_RAD, 0.0, 2.0 * M_PI);
				cairo_fill(cr);
			}
				
		}
#define TEXT_FRET_X_DIST 12.0
#define TEXT_FRET_Y_DIST 12.0
#define TEXT_FRET_SIZE 30.0
#define TEXT_X_FRET_START (xpos + GUITAR_WIDTH_Z + TEXT_FRET_X_DIST * zoom)
#define TEXT_Y_FRET_START (ypos + TEXT_FRET_Y_DIST * zoom)
		if (chord_descr->fret != 1 ) {
			cairo_new_path(cr);
			cairo_select_font_face(cr, "Sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
			cairo_set_font_size(cr, TEXT_FRET_SIZE * zoom);
			cairo_move_to(cr, TEXT_X_FRET_START,  TEXT_Y_FRET_START);
			cairo_show_text(cr, chord_descr->fret_s);
			cairo_stroke(cr);
			restore_font = true;
		}
	}
#define CHORD_NAME_X_START 0.0
#define CHORD_NAME_Y_START -20.0
#define CHORD_NAME_SIZE1_R 30.0
#define CHORD_NAME_SIZE2_R 50.0
#define CHORD_NAME_SIZE3_R 40.0
#define CHORD_NAME_SIZE4_R 50.0
#define CHORD_NAME_SIZE2_M 30.0
#define CHORD_MODYFIER_X_DIST 35.0
#define CHORD_MODYFIER_Y_DIST 30.0
#define CHORD_HALF_TONE_Y_DIST 10.0
#define CHORD_HALF_TONE_X_DIST 35.0
#define TEXT_X_NAME_START (xpos + CHORD_NAME_X_START * zoom)
#define TEXT_Y_NAME_START (ypos + CHORD_NAME_Y_START * zoom)

	if (chordname >= 0 && (status & GUITAR_CHORD_DRAW_TEXT)) {
		cairo_new_path(cr);
		cairo_select_font_face(cr, "Sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
		cairo_set_font_size(cr, ((status & GUITAR_CHORD_DRAW_DIAGRAM) ? CHORD_NAME_SIZE1_R : CHORD_NAME_SIZE2_R) * zoom);
		cairo_move_to(cr, TEXT_X_NAME_START, TEXT_Y_NAME_START);
		cairo_show_text(cr, chordname ? chord_descr->chordname2 : chord_descr->chordname1);
		cairo_stroke(cr);
		restore_font = true;
	}
	if (restore_font) {
#ifdef HAS_SET_SCALED_FONT
		cairo_set_scaled_font (cr, scaled_font);
#else
		cairo_set_font_face(cr, NedResource::getFontFace());
		cairo_set_font_matrix(cr,  NedResource::getFontMatrix(zoom_level));
		cairo_set_font_options(cr, NedResource::getFontOptions());
#endif
	}
}

void NedChordPainter::draw_chord(cairo_t *cr, struct chord_str *chord_descr, char *chord_name, unsigned int status, bool limit_fret, double xpos, double ypos, double zoom, int zoom_level) {
	int i;
	int barre = NO_BARRE;
	int barre_fret = -1;
	int mod_idx;
	char rname[128];
	unsigned int block_for_barre = 0;
	double arc_y, xdist;
	bool restore_font = false;
	int fret_count = limit_fret ? chord_descr->numfrets : F_COUNT;
	const char *root_name, *mody_name;


	if (status & GUITAR_CHORD_DRAW_DIAGRAM) {
		cairo_new_path(cr);
		cairo_set_line_width(cr, B_WIDTH);
		cairo_move_to(cr, xpos, ypos);
		cairo_line_to(cr, xpos + GUITAR_WIDTH_Z, ypos);
		cairo_stroke(cr);

		cairo_new_path(cr);
		cairo_set_line_width(cr, F_WIDTH);
		for (i = 0; i < fret_count; i++) {
			cairo_move_to(cr, xpos, ypos + i * F_DIST_Z);
			cairo_line_to(cr, xpos + GUITAR_WIDTH_Z, ypos + i * F_DIST_Z);
		}
		cairo_stroke(cr);
		cairo_new_path(cr);
		cairo_set_line_width(cr, S_WIDTH);
		for (i = 0; i < 6; i++) {
			cairo_move_to(cr, xpos + S_WIDTH / 2.0 + i * S_DIST_Z, ypos - B_WIDTH / 2.0);
			cairo_line_to(cr, xpos + S_WIDTH / 2.0 + i * S_DIST_Z, ypos + fret_count * F_DIST_Z - B_WIDTH / 2.0);
		}
		cairo_stroke(cr);

		for (i = 0; i < 6; i++) {
			if ((chord_descr->s[i] & 0xf0) == CHORD_END_OF_BARREE_RIGHT) {
				if (barre != NO_BARRE) {
					NedResource::Abort("CHORD_END_OF_BARREE_RIGHT\n");
				}
				barre = i;
				barre_fret = (chord_descr->s[i] & 0xf);
				block_for_barre = (1 << i) - 1;
				block_for_barre = ~block_for_barre;
				cairo_new_path(cr);
				cairo_set_line_width(cr, BARRE_WIDTH);
				arc_y = ypos - B_WIDTH / 2.0 + (barre_fret - chord_descr->fret + 1) * F_DIST_Z - F_DIST_Z / 2.0;
				cairo_move_to(cr, xpos + S_WIDTH / 2.0 + i * S_DIST_Z, arc_y);
				cairo_line_to(cr, xpos + GUITAR_WIDTH_Z, arc_y);
				cairo_stroke(cr);
				cairo_new_path(cr);
				cairo_arc(cr, xpos + S_WIDTH / 2.0 + i * S_DIST_Z, arc_y, P_RAD, 0.0, 2.0 * M_PI);
				cairo_arc(cr, xpos + S_WIDTH / 2.0 + 5 * S_DIST_Z, arc_y, P_RAD, 0.0, 2.0 * M_PI);
				cairo_fill(cr);
			}
			if ((chord_descr->s[i] & 0xf0) == CHORD_END_OF_BARREE_LEFT) {
				/*
				if (barre != NO_BARRE) {
					NedResource::Abort("CHORD_END_OF_BARREE_LEFT\n");
				}
				*/
				block_for_barre = (1 << i) - 1;
				barre = -i;
				barre_fret = (chord_descr->s[i] & 0xf);
				cairo_new_path(cr);
				cairo_set_line_width(cr, BARRE_WIDTH);
				arc_y = ypos - B_WIDTH / 2.0 + (barre_fret - chord_descr->fret + 1) * F_DIST_Z - F_DIST_Z / 2.0;
				cairo_move_to(cr, xpos + S_WIDTH / 2.0 + i * S_DIST_Z, arc_y);
				cairo_line_to(cr, xpos + S_WIDTH / 2.0, arc_y); 
				cairo_stroke(cr);
				cairo_new_path(cr);
				cairo_arc(cr, xpos + S_WIDTH / 2.0 + i * S_DIST_Z, arc_y, P_RAD, 0.0, 2.0 * M_PI);
				cairo_arc(cr, xpos + S_WIDTH / 2.0 , arc_y, P_RAD, 0.0, 2.0 * M_PI);
				cairo_fill(cr);
			}
		}
		for (i = 0; i < 6; i++) {
			if ((chord_descr->s[i] & 0xf0) == CHORD_MUTE) {
				cairo_new_path(cr);
				cairo_set_line_width(cr, X_WIDTH);
				cairo_move_to(cr, xpos + S_WIDTH / 2.0 + i * S_DIST_Z - X_LONG, ypos - B_WIDTH / 2.0 - X_DIST - X_LONG);
				cairo_line_to(cr, xpos + S_WIDTH / 2.0 + i * S_DIST_Z + X_LONG, ypos - B_WIDTH / 2.0 - X_DIST + X_LONG);
				cairo_move_to(cr, xpos + S_WIDTH / 2.0 + i * S_DIST_Z + X_LONG, ypos - B_WIDTH / 2.0 - X_DIST - X_LONG);
				cairo_line_to(cr, xpos + S_WIDTH / 2.0 + i * S_DIST_Z - X_LONG, ypos - B_WIDTH / 2.0 - X_DIST + X_LONG);
				cairo_stroke(cr);
			}
			else if ((chord_descr->s[i] & 0xf) == 0) {
				cairo_new_path(cr);
				cairo_set_line_width(cr, O_WIDTH);
				cairo_arc(cr, xpos + S_WIDTH / 2.0 + i * S_DIST_Z, ypos - B_WIDTH / 2.0 - O_DIST,
					O_RAD, 0.0, 2.0 * M_PI);
				cairo_stroke(cr);
			}
			else if (chord_descr->s[i] != barre_fret || (((1 << i) & block_for_barre)) == 0) {
				cairo_new_path(cr);
				cairo_arc(cr, xpos + S_WIDTH / 2.0 + i * S_DIST_Z,
					ypos - B_WIDTH / 2.0 + ((0xf & chord_descr->s[i]) - chord_descr->fret + 1) * F_DIST_Z - F_DIST_Z / 2.0,
					P_RAD, 0.0, 2.0 * M_PI);
				cairo_fill(cr);
			}
				
		}
		if (chord_descr->fret != 1 ) {
			cairo_new_path(cr);
			cairo_select_font_face(cr, "Sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
			cairo_set_font_size(cr, TEXT_FRET_SIZE * zoom);
			cairo_move_to(cr, TEXT_X_FRET_START,  TEXT_Y_FRET_START);
			cairo_show_text(cr, chord_descr->fret_s);
			cairo_stroke(cr);
			restore_font = true;
		}
	}

	if (status & GUITAR_CHORD_DRAW_TEXT) {
		if (status & GUITAR_CHORD_DRAW_DIAGRAM) {
			cairo_new_path(cr);
			cairo_select_font_face(cr, "Sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
			cairo_set_font_size(cr, CHORD_NAME_SIZE1_R * zoom);
			cairo_move_to(cr, TEXT_X_NAME_START, TEXT_Y_NAME_START);
			cairo_show_text(cr, chord_name);
			cairo_stroke(cr);
		}
		else  {
			cairo_new_path(cr);
			cairo_select_font_face(cr, "Sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
			cairo_set_font_size(cr, CHORD_NAME_SIZE2_R * zoom);
			cairo_move_to(cr, TEXT_X_NAME_START, TEXT_Y_NAME_START);
			root_name = NedChordPainter::roots[(status & ROOT_MASK)];
			mod_idx = ((status >> ROOT_BITS) & ((1 << MODI_BITS) - 1));
			mody_name = NedChordPainter::modies3[mod_idx];
			strcpy(rname, root_name);
			xdist = 0.0;
			if (rname[1] != '\0') {
				rname[1] = '\0';
			}
			if (minor[mod_idx]) {
				if (addoff[(status & ROOT_MASK)]) {
					rname[1] = ' ';
					rname[2] = 'm';
					rname[3] = '\0';
				}
				else {
					rname[1] = 'm';
					rname[2] = '\0';
#define MINOR_X_DIST 60.0
				}
				xdist = MINOR_X_DIST;
			}
			cairo_show_text(cr, rname);
#define IDX_X_OFFS 10.0
			cairo_select_font_face(cr, "Sans", CAIRO_FONT_SLANT_ITALIC, CAIRO_FONT_WEIGHT_BOLD);
			cairo_set_font_size(cr, CHORD_NAME_SIZE3_R * zoom);
			cairo_move_to(cr, TEXT_X_NAME_START + CHORD_HALF_TONE_X_DIST * zoom, TEXT_Y_NAME_START + CHORD_HALF_TONE_Y_DIST * zoom);
			switch (addoff[(status & ROOT_MASK)]) {
				case 1: cairo_show_text(cr, "#"); break;
				case -1: cairo_show_text(cr, "b"); break;
			}
			/*
			if (root_name[1] != '\0') {
				rname[0] = root_name[1];
				cairo_select_font_face(cr, "Sans", CAIRO_FONT_SLANT_ITALIC, CAIRO_FONT_WEIGHT_BOLD);
				cairo_set_font_size(cr, (rname[0] == 'b' ? CHORD_NAME_SIZE3_R : CHORD_NAME_SIZE4_R) * zoom);
				cairo_move_to(cr, TEXT_X_NAME_START + (CHORD_HALF_TONE_X_DIST + xdist) * zoom, TEXT_Y_NAME_START + CHORD_HALF_TONE_Y_DIST * zoom);
				cairo_show_text(cr, rname);
			}
			*/
			if (addoff[(status & ROOT_MASK)]) {
				xdist += IDX_X_OFFS;
			}
			cairo_move_to(cr, TEXT_X_NAME_START + (CHORD_MODYFIER_X_DIST + xdist) * zoom, TEXT_Y_NAME_START - CHORD_MODYFIER_Y_DIST * zoom);
			cairo_set_font_size(cr, CHORD_NAME_SIZE2_M * zoom);
			cairo_show_text(cr, mody_name);
			cairo_stroke(cr);
		}
		restore_font = true;
	}
	if (restore_font) {
#ifdef HAS_SET_SCALED_FONT
		cairo_set_scaled_font (cr, scaled_font);
#else
		cairo_set_font_face(cr, NedResource::getFontFace());
		cairo_set_font_matrix(cr,  NedResource::getFontMatrix(zoom_level));
		cairo_set_font_options(cr, NedResource::getFontOptions());
#endif
	}
}

struct chord_str *NedChordPainter::searchCordByCode(char *code) {
	struct chord_str *chord_ptr;

	for (chord_ptr = ned_chords; chord_ptr->chordname1 != NULL; chord_ptr++) {
		if (!strcmp(chord_ptr->id, code)) return chord_ptr;
	}

	return NULL;
}

void NedChordPainter::writeLilyChord(FILE *fp, struct chord_str *chord_descr) {
	int i;
	int barre_start = -1, barre_end = -1, barre_fret = -1;
	unsigned int block_for_barre = 0;

	fprintf(fp, "^\\markup \\fret-diagram #\"");
	for (i = 0; i < 6; i++) {
		if ((chord_descr->s[i] & 0xf0) == CHORD_END_OF_BARREE_RIGHT) {
			barre_end = i;
			barre_start = 5;
			barre_fret = (0xf & chord_descr->s[i]);
			block_for_barre = (1 << i) - 1;
			block_for_barre = ~block_for_barre;
		}
		else if ((chord_descr->s[i] & 0xf0) == CHORD_END_OF_BARREE_LEFT) {
			barre_end = 0;
			barre_start = i;
			barre_fret = (0xf & chord_descr->s[i]);
			block_for_barre = (1 << i) - 1;
		}
	}
	if (barre_start >= 0) {
		fprintf(fp, "c:%d-%d-%d;", (6 - barre_start), (6 - barre_end), barre_fret);
	} 
	for (i = 0; i < 6; i++) {
		if ((chord_descr->s[i] & 0xf0) == CHORD_MUTE) {
			fprintf(fp, "%d-x;", 6-i);
		}
		else if ((chord_descr->s[i] & 0xf) == 0) {
			fprintf(fp, "%d-o;", 6-i);
		}
		else if (chord_descr->s[i] != barre_fret ||i == barre_start ||
				 i == barre_end || (((1 << i) & block_for_barre)) == 0) {
			fprintf(fp, "%d-%d;", 6-i, chord_descr->s[i] & 0xf);
		}
	}
	fprintf(fp, "\" ");
}

void NedChordPainter::writeLilyChordName(FILE *fp, NedFreeChord *freechord, unsigned int length, unsigned int duration, int dotcount, unsigned int *midi_len) {
	const char *root_name, *mody_name;
	unsigned int mod_idx;
	char Str[128];
	int i;

	root_name = NedChordPainter::roots2[(freechord->getStatus() & ROOT_MASK)];
	mod_idx = ((freechord->getStatus() >> ROOT_BITS) & ((1 << MODI_BITS) - 1));
	mody_name = NedChordPainter::modies4[mod_idx];

	fprintf(fp, "%s", root_name);
	if (duration != *midi_len) {
		sprintf(Str, "%d", WHOLE_NOTE / length);
		for (i = 0; i < dotcount; i++) {
			strcat(Str, ".");
		}
		fprintf(fp, Str);
	}
	*midi_len = duration;
	if (mody_name[0] != '\0') {
		fprintf(fp, ":%s", mody_name);
	}
	putc(' ', fp);
}
	
