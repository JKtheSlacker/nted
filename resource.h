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

#ifndef RESOURCE_H

#define RESOURCE_H
#include "config.h"
#include <sys/time.h>
#include <gtk/gtk.h>
#include <cairo.h>
#include <alsa/asoundlib.h>

#define FONTFILE "ntedfont.pfa"

#define WIDTH 1023
#define HEIGHT 767

#define ZOOMLEVELS 20
#define MAXELEMENTS  30
#define DEFAULT_BORDER 0.2
#define LEFT_RIGHT_BORDER 1.5
#define TOP_BOTTOM_BORDER 1.5

#define FONT_REL_SIZE 10.0

#define STAFF_TOP_DIST (5.0 * LINE_DIST)


#define PS_ZOOM 24.0
#define PS_ZOOM2 22.4
	

#define LINE_DIST 0.22
#define LINE_THICK 0.02
#define MEASURE_LINE_THICK 0.03
#define BEAM_THICK 0.1

#define STEM_DEFAULT_HEIGHT (3.4 * LINE_DIST)
#define STEM_THICK 0.04
#define HEAD_THICK (1.2 * LINE_DIST)
#define HEAD_THICK_HALF (1.09 * HEAD_THICK)
#define FLAG_DIST (0.87 * LINE_DIST)

#define STEM_TOP_DIST 0.09

#define HEAD_THICK_2 (HEAD_THICK / 2.0)
#define HEAD_THICK_HALF_2 (HEAD_THICK_HALF / 2.0)

#define HEAD_HIGHT (1.0 * LINE_DIST)
#define HEAD_HIGHT_HALF HEAD_HIGHT
#define HEAD_HIGHT_2 (HEAD_HIGHT / 2.0)
#define HEAD_HIGHT_HALF_2 (HEAD_HIGHT_HALF / 2.0)


#define BRACE_SPACE 0.05
#define BRACE_X_OFFSET 5.0
#define BRACKET_INDENT 0.2
#define NESTED_BRACE_INDENT 0.18

#define VOICE_COUNT 4

#define COUNT_OF_PARALLEL_ELEMENTS 1024


#define MIN_SELECTION_RECTANGLE_DIM 2.0

#define TRIPLET_VAL_SHIFT_X 0.15
#define TUPLET_XOVER 0.15
#define TUPLET_XFRONTOVER 0.1
#define TUPLET_MARKER_DIST 0.1
#define TUPLET_MARKER_DIST_UP 0.2
#define TUPLET_MARKER_DIST_DOWN 0.8
#define NUMNBER_GLYPH_WIDTH 0.4
#define NUMNBER_GLYPH_HEIGHT_UP 0.0
#define NUMNBER_GLYPH_HEIGHT_DOWN 0.6
#define NUMNBER_GLYPH_HEIGHT -0.3
#define TUPLET_LINE_THICK 0.02
#define TUPLET_BRACKET_HEIGHT 0.1
#define FIRST_TUPLET_DIGIT_DIST 0.2
#define TUPLET_MARKER_HEIGHT 0.7

#define DOT1POS (0.145 * LINE_DIST)
#define DOT2POS (0.76 * LINE_DIST)
#define DOT1_REST_POS (0.87 * LINE_DIST)
#define DOT2_REST_POS (1.0 * LINE_DIST)
#define DOT_RAD 0.05

#define WHOLE_NOTE_X_OFFS (0.43 * LINE_DIST)
#define HALF_NOTE_X_OFFS (0.43 * LINE_DIST)

#define WHOLE_NOTE_Y_OFFS (-0.217 * LINE_DIST)
#define HALF_NOTE_Y_OFFS (0.2 * LINE_DIST)

#define ACCIDENTAL_BASE_DIST (1.3 * LINE_DIST)
#define MIN_SHIFT_X_DIFF (0.87 * LINE_DIST)

#define START_ZOOM_LEVEL (ZOOMLEVELS / 2)
#define BASE 66
#define FONT_FACTOR 0.97

#define START_TEMPO_INVERSE 500.0

#define NUMBERBASE 49
#define SIG_HALF (0.4 * LINE_DIST)
#define Y_NUMRATOR_POS (-1.6 * LINE_DIST)
#define Y_DENOMINATOR_POS (0.4 * LINE_DIST)

#define AUX_LINE_OVER (0.435 * LINE_DIST)
#define AUX_LINE_THICK (0.087 * LINE_DIST)

#define MAX_LINE 30
#define MIN_LINE -MAX_LINE

#define TIE_X_START (0.456 * LINE_DIST)
#define TIE_X_START_LARGE (0.6 * LINE_DIST)
#define MIN_TIE_LEN 1.2
#define TIE_Y_OFFS1 (1.74 * LINE_DIST)
#define TIE_Y_OFFS2 (2.174 * LINE_DIST)
#define TIE_X_RIGHT_OVER (3.48 * LINE_DIST)
#define TIE_X_OFFS 0.5
#define D_TIE 0.1
#define MAX_TIE_INCR 1.5
#define MIN_TIE_INCR -1.5

#define SORTING_NONE 0
#define SORTING_LESS 1
#define SORTING_GREAT 2

#define STEM_DIR_NONE 0
#define STEM_DIR_UP 1
#define STEM_DIR_DOWN 2

#define LYRICS_FONT_SIZE 0.4
#define LYRICS_FONT "Sans"
#define LYRICS_FONT_SLANT PANGO_STYLE_NORMAL
#define LYRICS_FONT_WEIGHT PANGO_WEIGHT_NORMAL


#define TITLE_FONT_SIZE 0.8
#define TITLE_FONT "Serif"
#define TITLE_FONT_SLANT PANGO_STYLE_NORMAL
#define TITLE_FONT_WEIGHT PANGO_WEIGHT_BOLD

#define SUBJECT_FONT_SIZE 0.5
#define SUBJECT_FONT "Serif"
#define SUBJECT_FONT_SLANT PANGO_STYLE_ITALIC
#define SUBJECT_FONT_WEIGHT PANGO_WEIGHT_NORMAL

#define COMPOSER_FONT_SIZE 0.5
#define COMPOSER_FONT "Sans"
#define COMPOSER_FONT_SLANT PANGO_STYLE_NORMAL
#define COMPOSER_FONT_WEIGHT PANGO_WEIGHT_BOLD

#define ARRANGER_FONT_SIZE 0.5
#define ARRANGER_FONT "Sans"
#define ARRANGER_FONT_SLANT PANGO_STYLE_NORMAL
#define ARRANGER_FONT_WEIGHT PANGO_WEIGHT_NORMAL

#define COPYRIGHT_FONT_SIZE 0.4
#define COPYRIGHT_FONT "Sans"
#define COPYRIGHT_FONT_SLANT PANGO_STYLE_NORMAL
#define COPYRIGHT_FONT_WEIGHT PANGO_WEIGHT_NORMAL

#define STAFF_NAME_FONT_SIZE 0.4
#define STAFF_NAME_FONT "Serif"
#define STAFF_NAME_FONT_SLANT PANGO_STYLE_NORMAL
#define STAFF_NAME_FONT_WEIGHT PANGO_WEIGHT_NORMAL

#define STAFF_SHORT_NAME_FONT_SIZE 0.4
#define STAFF_SHORT_NAME_FONT "Serif"
#define STAFF_SHORT_NAME_FONT_SLANT PANGO_STYLE_NORMAL
#define STAFF_SHORT_NAME_FONT_WEIGHT PANGO_WEIGHT_NORMAL

#define TEXT_FONT_NAME_DEFAULT "Sans"
#define TEXT_FONT_SIZE_DEFAULT 0.5
#define TEXT_FONT_SLANT_DEFAULT PANGO_STYLE_NORMAL
#define TEXT_FONT_WEIGHT_DEFAULT PANGO_WEIGHT_NORMAL 

#define SYSTEM_INTENT_SEC_SPACE 0.2

#define SCORE_INFO_EXTRA_DIST 0.1

#define SCALE_GRANULARITY 50
#define SCALE_MIN 0.6
#define SCALE_MAX 1.4

#define TEXT_Y_OFFS (- 2.0 * LINE_DIST)
#define LYRICS_HEIGHT 0.35
#define MAX_LYRICS_LINES 5
#define MAX_TEXT_LENGTH 10

/* WHOLE_NOTE divisable by 13, 12, ... 5, 4, 3, 2, 1
 * and 20 * WHOLE_NOTE < 2^31 - 1
 * permits 13plets and up to 20 whole notes per staff
 */
#define FACTOR (13*11*9*8*7*5)
#define WHOLE_NOTE (128*FACTOR)
#define NOTE_2 (WHOLE_NOTE/2)
#define NOTE_4 (NOTE_2/2)
#define NOTE_8 (NOTE_4/2)
#define NOTE_16 (NOTE_8/2)
#define NOTE_32 (NOTE_16/2)
#define NOTE_64 (NOTE_32/2)
#define NOTE_128 (NOTE_64/2)

#define GRACE_DURATION NOTE_32

#define NORMAL_NOTE  0
#define CROSS_NOTE1  1
#define CROSS_NOTE2  2
#define RECT_NOTE1   3
#define RECT_NOTE2   4
#define TRIAG_NOTE1  5
#define TRIAG_NOTE2  6
#define CROSS_NOTE3  7
#define CROSS_NOTE4  8
#define TRIAG_NOTE3  9
#define QUAD_NOTE2  10 
#define GUITAR_NOTE_STEM  11 
#define GUITAR_NOTE_NO_STEM  12 
#define OPEN_HIGH_HAT 13
#define CLOSED_HIGH_HAT 14

#define COUNT_OF_DRUMS_TO_CHOOSE_DURING_MIDI_IMPORT 9
#define OFFSET_TO_BE_USED_DURING_MIDI_IMPORT 1007

#define STROKEN_GRACE (NOTE_64/2)
#define GRACE_8 (STROKEN_GRACE/2)
#define GRACE_16 (GRACE_8/2)
#define GRACE_NOTE_TIME NOTE_32

#define LENGH_FACTOR GRACE_16

#define GRACE_HEIGHT1 0.0
#define GRACE_HEIGHT2 0.1
#define GRACE_HEIGHT3 0.5
#define GRACE_HEAD_THICK 0.12
#define GRACE_HEAD_THICK_HALF (GRACE_HEAD_THICK / 2.0)
#define GRACE_STEM_THICK 0.03
#define GRACE_BEAM_THICK 0.06
#define GRACE_BEAM_DIST 0.1

#define SIGN_BREATH1 3
#define SIGN_BREATH2 4

#define LILY_BREAH_SCRIPT_RCOMMA 0
#define LILY_BREAH_SCRIPT_UPBOW 1

#define SHIFT_MODE  (1 << 0)
#define INSERT_EREASE_MODE  (1 << 1)
#define KEYBOARD_CTRL_MODE (1 << 2)

#define STAT_UP (1 << 0)
#define STAT_SHARP (1 << 1)
#define STAT_FLAT (1 << 2)
#define STAT_NATURAL ((1 << 1) | (1 << 2))
#define STAT_DSHARP (1 << 3)
#define STAT_DFLAT ((1 << 3) | (1 << 1))
#define ACCIDENTAL_MASK ((1 << 1) | (1 << 2) | (1 << 3))
#define STAT_STACC     (1 <<  4)
#define STAT_PIZZ      (1 <<  5)
#define STAT_TENUTO    (1 <<  6)
#define STAT_SFORZATO  (1 <<  7)
#define STAT_SFORZANDO (1 <<  8)
#define STAT_BOW_UP    (1 <<  9)
#define STAT_BOW_DOWN  (1 << 10)
#define STAT_PED_ON    (1 << 11)
#define STAT_PED_OFF   (1 << 12)
#define STAT_TRILL     (1 << 13)
#define STAT_PRALL     (1 << 14)
#define STAT_MORDENT   (1 << 15)
#define STAT_OPEN      (1 << 16)
#define STAT_STACCATISSIMO (1 << 17)
#define STAT_FERMATA   (1 << 18)
#define STAT_ARPEGGIO  (1 << 19)
#define STAT_BREAK_BEAM (1 << 20)
#define STAT_FORCE_BEAM (1 << 21)
#define STAT_USER_STEMDIR (1 << 22)
#define STAT_USER_HIDDEN_REST  (1 << 23)
#define STAT_USER_PLACED_REST  (1 << 24)
#define STAT_TURN      (1 << 25)
#define STAT_REV_TURN   (1 << 26)
#define STAT_TREMOLO1   (0x1 << 27)
#define STAT_TREMOLO2   (0x2 << 27)
#define STAT_TREMOLO3   (0x3 << 27)
#define STAT_TREMOLO4   (0x4 << 27)
#define TREMOLO_MASK    (0x7 << 27)
#define CHORD_STAT_MASK (STAT_STACC  | STAT_PIZZ      | STAT_TENUTO    | \
                         STAT_SFORZATO  | STAT_SFORZANDO | STAT_BOW_UP    | STAT_BOW_DOWN  | \
                         STAT_PED_ON    | STAT_PED_OFF   | STAT_TRILL     | STAT_PRALL     | \
                         STAT_MORDENT   | STAT_OPEN      | STAT_STACCATISSIMO | STAT_FERMATA | STAT_ARPEGGIO | \
			 STAT_TURN | STAT_REV_TURN | TREMOLO_MASK )

#define VIRTUAL_NATURAL_OFFS -111

#define CLEF_SPACE 0.9
#define CLEF_POS 0.5
#define TIME_SIGNATURE_SPACE 0.37
#define SIG_X_SHIFT (0.87 * LINE_DIST)
#define EXTRA_KEY_OFF 0.05
#define MEAS_BAR_SPACE (0.74 * LINE_DIST)
#define RIGHT_DIST (0.174 * LINE_DIST)
//#define EXTRA_SPACE (0.652 * LINE_DIST)
//#define EXTRA_SPACE (0.2 * LINE_DIST)
#define WHOLE_EXTRA_SPACE (2.6 * LINE_DIST)

#define TIME_SYMBOL_NONE 0
#define TIME_SYMBOL_COMMON_TIME 1
#define TIME_SYMBOL_CUT_TIME 2

// real measure numbers
#define MEASURE_NUMBER_X_POS -0.45
#define MEASURE_NUMBER_Y_POS -0.45
#define MEASURE_NUMBER_SLANT CAIRO_FONT_SLANT_ITALIC
#define MEASURE_NUMBER_WEIGHT CAIRO_FONT_WEIGHT_NORMAL
#define MEASNUMBER_SIZE 0.35

// for testing
#define MEASURE_WEIGHT NOTE_4
#define MEASURE_NUMBER_Y_DIST 0.1
#define MEASURE_NUMBER_X_DIST 0.1
#define MEASURE_NUMBER_SIZE 0.3
#define TIME_SIG_WIDTH 0.5

#define MEASURE_SPREAD_STEP 0.1
#define MEASURE_MAX_SPREAD 2.0
#define MEASURE_MIN_SPREAD 0.4

#define MIN_NEAR_DIST 0.4
#define MIN_STAFF_BORDER_DIST 0.3
#define SEC_DIST_BELOW 0.3 
#define SEC_DIST_ABOVE 0.1 
#define MIN_SEC_DIST (2 * LINE_DIST)
#define REP_NUM_XPOS 0.09
#define REP_NUM_YPOS 0.4

#define MAX_NON_FULL_REPOSITS 15

#define MAX_NOTES_PER_WITH_ACCIDENTALS_STAFF 50

#define TREBLE_CLEF 0
#define BASS_CLEF 1
#define ALTO_CLEF 2
#define SOPRAN_CLEF 3
#define TENOR_CLEF 4
#define NEUTRAL_CLEF1 5
#define NEUTRAL_CLEF2 6
#define NEUTRAL_CLEF3 7

#define MINCLEF_NUMBER TREBLE_CLEF
#define MAXCLEF_NUMBER NEUTRAL_CLEF3

#define OCTUP_Y_TREBLE_DIST_DOWN 0.8
#define OCTUP_Y_TREBLE_DIST_UP -1.05
#define OCTUP_Y_BASS_DIST_DOWN 0.25
#define OCTUP_Y_BASS_DIST_UP -0.7
#define OCTUP_Y_ALTO_DIST_DOWN 0.5
#define OCTUP_Y_ALTO_DIST_UP -0.7
#define OCTUP_X_ALTO_DIST 0.15

#define DEMO_LINE_THICK 1.4
#define DEMO_LINE_BEGIN 10.0
#define DEMO_LINE_END 80.0
#define DEMO_LINE_YPOS 46.0
#define DEMO_LINE_DIST 9.6
#define DEMO_POS_FAK 44.0

#define MAX_MIDI_INPUT_CHORD 25

#define MAX_RECENT_FILES 8

#define START_EXTRA_SPACE (1.5 * LINE_DIST)
#define OLDVALUES
#ifdef OLDVALUES
#define MINIMUM_EXTRA_SPACE (2.0 * LINE_DIST)
#define MAXIMUM_EXTRA_SPACE (10.0 * LINE_DIST)
#else
#define MINIMUM_EXTRA_SPACE (0.0 * LINE_DIST)
#define MAXIMUM_EXTRA_SPACE (5.0 * LINE_DIST)
#endif

#define MIN_SYSTEM_Y_DIST (1.2 * LINE_DIST)
#define MAX_SYSTEM_Y_DIST (1.8 * LINE_DIST)

#define REP_DOTS_X_DIST (0.8 * LINE_DIST)
#define REP_DOTS_X_EXTRA_DIST (1.0 * LINE_DIST)
#define REP_DOTS_Y1DIST (1.4 * LINE_DIST)
#define REP_DOTS_Y2DIST (2.4 * LINE_DIST)
#define REP_DOTS_RADIUS (0.2 * LINE_DIST)
#define REPEAT_MEASURE_LINE_THICK 0.1
#define REP_THICK_LINE_DIST (0.6 * LINE_DIST)
#define REP_DOUBLE_LINE_DIST (0.4 * LINE_DIST)

#define V1RED 0xf800
#define V1GREEN 0x8b00
#define V1BLUE 0x3100

#define V2RED 0x0000
#define V2GREEN 0xb333
#define V2BLUE 0x0000

#define V3RED 0x2200
#define V3GREEN 0x5800
#define V3BLUE 0xe000

#define V4RED 0xb444
#define V4GREEN 0x4443
#define V4BLUE 0x0000

#define DCOLOR(x) ((double) (x) / (double) (1 << 16))
#define DSCOLOR(x) (0.6 * (double) (x) / (double) (1 << 16))

#define HIDD_RED     255
#define HIDD_GREEN     0
#define HIDD_BLUE    255

#define NOTE_ACTIVE_STATE_NONE 0
#define NOTE_ACTIVE_STATE_ON 1
#define NOTE_ACTIVE_STATE_OFF 2


#define TOLERANCE 0.05
#define MIN_REST_Y_VOICE_DIST 0.1

#define MAX_OVER_UNDER_LINES 12

#define OFFS_ARRAY_SIZE 115
#define OFFS_ARRAY_HALF (OFFS_ARRAY_SIZE / 2)

#define X11_ICONS_DIM 25
#define X11_ICONS_BACK_DIM 35

#define X11_MINI_ICONS_DIM 20
#define X11_ICONS_BACK_MINI_DIM 25

#define MAX_SCREEN_DIM 100000

#define DBG_NO 0
#define DBG_TESTING 1
#define DBG_CRITICAL 2

#define SEGNO_SIGN           (1 << 0)
#define SEGNO_D_S_AL_CODA    (1 << 1)
#define SEGNO_D_S_AL_FINE    (1 << 2)
#define SEGNO_D_CAPO_AL_CODA (1 << 3)
#define SEGNO_D_CAPO_AL_FINE (1 << 4)
#define SEGNO_FINE           (1 << 5) 
#define SEGNO_FINE2	     (1 << 6)  // "valid" fine (during replay)
#define SEGNO_CODA	     (1 << 7)
#define SEGNO_AL_CODA	     (1 << 8)
#define SEGNO_AL_CODA2	     (1 << 9)  // "valid" al coda (during replay)


/* Debug macros */
#define LAUS(var) (var), (var) / NOTE_64, (var) / NOTE_4
/*              */
class NedNote;
class NedChordOrRest;
class NedMainWindow;
class NedSystem;
class NedStaff;
class NedPage;
class NedMeasure;
class NedTools;
class NedChordName;


struct MidiPortStruct;

struct MidiPortStruct {
	int client, port;
	char *name;
};

struct addr_ref_str {
	void *ori, *copy;
	struct addr_ref_str *next;
};

struct MidiListStruct {
	double tempoinverse;
	GList *notes;
};

struct MidiEventStruct {
	unsigned short channel, segno_sign;
	unsigned long long midi_time;
	unsigned int type;
#define PSEUDO_FLAG (0x1 << 17) // MidiEventStruct will be deleted before reply starts
#define PSEUDO2_FLAG (0x1 << 18) // MidiEventStruct is kept during replay
#define PSEUDO_MASK (0xffff << 16)
#define NOTE_MASK (0xffff)
#define PSEUDO_EVENT_MEASURE (PSEUDO_FLAG | 0x1)
#define PSEUDO_EVENT_VOL_CHANGE (PSEUDO_FLAG | 0x2)
#define PSEUDO_EVENT_RIT_START (PSEUDO2_FLAG | 0x1)
#define PSEUDO_EVENT_RIT_STOP (PSEUDO2_FLAG | 0x2)
#define PSEUDO_EVENT_CRES_START (PSEUDO2_FLAG | 0x4)
#define PSEUDO_EVENT_CRES_STOP (PSEUDO2_FLAG | 0x8)
#define PSEUDO_EVENT_TEMPO_CHANGE (PSEUDO2_FLAG | 0x10)
#define PSEUDO_EVENT_KEY_CHANGE (PSEUDO2_FLAG | 0x20)
	unsigned char data2;
	unsigned char pgm; // for notes;
	unsigned char voice;
	double tempoinverse;
	int data1;
	NedNote *note;
	NedMeasure *measure;
	bool repetition;
};

struct paper_info_struct {
	const char *name;
	int width, height;
};

class NedBbox {
	public: 
		double x, y, width, width_netto /* width without lyrics */, height;
};

struct crescendo_info {
	unsigned int symbol;
	GList *listptr;
	unsigned long long midi_time;
	int volume;
};

struct accel_info {
	unsigned int symbol;
	GList *listptr;
	unsigned long long midi_time;
	double tempo_inverse;
};

class NedResource {
	public:
		static void initialize();
		static void cleanup();
		static char *getFontFileName() {return m_font_file_name;}
		static cairo_scaled_font_t *getScaledFont(int zoom_level);
		static double getStartZoomScale() {return m_zoom_factors[START_ZOOM_LEVEL];}
		static double getZoomFactor(int zoom_level);
		static void Abort(const char *s);
		static void DbgMsg(int level, const char *format, ...);
		static void Warning(const char *s);
		static void Info(const char *s);
		static cairo_text_extents_t fontextentions[ZOOMLEVELS][MAXELEMENTS];
		static double m_braceOutline[16][6];
		static double m_bracketEndOutline[5][6];
#ifndef HAS_SET_SCALED_FONT
		static cairo_font_face_t *getFontFace() {return m_font_face;}
		static cairo_matrix_t *getFontMatrix(int zoom_level);
		static cairo_font_options_t *getFontOptions() {return m_fontoptions;}
#endif

		static GdkPixbuf *m_the_nted_icon;

		static GdkPixbuf *m_32ndnote_icon;
		static GdkPixbuf *m_64thnote_icon;
		static GdkPixbuf *m_eightnote_icon;
		static GdkPixbuf *m_eightnote_dot_icon;
		static GdkPixbuf *m_fullnote_icon;
		static GdkPixbuf *m_halfnote_icon;
		static GdkPixbuf *m_halfnote_dot_icon;
		static GdkPixbuf *m_quarternote_icon;
		static GdkPixbuf *m_quarternote_dot_icon;
		static GdkPixbuf *m_stroken_grace_icon;
		static GdkPixbuf *m_grace_eighth_icon;
		static GdkPixbuf *m_grace_sixth_icon;
		static GdkPixbuf *m_sixteenthnote_icon;
		static GdkPixbuf *m_sixteenthnote_dot_icon;

		static GdkPixbuf *m_rest_icon;

		static bool equalContent(GList *l1, GList *l2, NedChordOrRest *except, int *diff_pos);
		static GdkPixbuf *m_shift_mode_icon;
		static GdkPixbuf *m_insert_mode_icon;

		static GdkPixbuf *m_dot_icon;
		static GdkPixbuf *m_ddot_icon;
		static GdkPixbuf *m_cross_icon;
		static GdkPixbuf *m_flat_icon;
		static GdkPixbuf *m_natural_icon;
		static GdkPixbuf *m_tied_icon;

		static GdkPixbuf *m_arrow_up_icon;
		static GdkPixbuf *m_arrow_down_icon;
		static GdkPixbuf *m_arrow_left_icon;
		static GdkPixbuf *m_arrow_right_icon;

		static GdkPixbuf *m_treble_clef_icon;
		static GdkPixbuf *m_bass_clef_icon;
		static GdkPixbuf *m_alto_clef_icon;

		static GdkPixbuf *m_delete_staff_icon;
		static GdkPixbuf *m_delete_system_icon;

		static GdkPixbuf *m_color_icon;

		static GdkPixbuf *m_lyrics_icon;
		static GdkPixbuf *m_key_icon;
		static GdkPixbuf *m_tool_icon;

		static GdkPixbuf *m_normal_note_icon;
		static GdkPixbuf *m_drum1_icon;
		static GdkPixbuf *m_drum2_icon;
		static GdkPixbuf *m_drum3_icon;
		static GdkPixbuf *m_drum4_icon;
		static GdkPixbuf *m_drum5_icon;
		static GdkPixbuf *m_drum6_icon;
		static GdkPixbuf *m_drum7_icon;
		static GdkPixbuf *m_drum8_icon;
		static GdkPixbuf *m_drum9_icon;
		static GdkPixbuf *m_drum10_icon;
		static GdkPixbuf *m_drum11_icon;
		static GdkPixbuf *m_drum12_icon;
		static GdkPixbuf *m_guitar_note_icon;
		static GdkPixbuf *m_guitar_note_no_stem_icon;

		
		static GdkPixbuf *m_drum1_icon_small;
		static GdkPixbuf *m_drum2_icon_small;
		static GdkPixbuf *m_drum4_icon_small;
		static GdkPixbuf *m_drum6_icon_small;
		static GdkPixbuf *m_drum8_icon_small;
		static GdkPixbuf *m_drum3_icon_small;
		static GdkPixbuf *m_drum5_icon_small;
		static GdkPixbuf *m_drum7_icon_small;
		static GdkPixbuf *m_drum9_icon_small;
		static GdkPixbuf *m_drum10_icon_small;
		static GdkPixbuf *m_drum11_icon_small;
		static GdkPixbuf *m_drum12_icon_small;
		static GdkPixbuf *m_normal_note_icon_small;

		static GdkPixbuf *m_dcross_icon;
		static GdkPixbuf *m_dflat_icon;
		static GdkPixbuf *m_stacc_icon;
		static GdkPixbuf *m_staccatissimo_icon;
		static GdkPixbuf *m_str_pizz_icon;
		static GdkPixbuf *m_tenuto_icon;
		static GdkPixbuf *m_sforzato_icon;
		static GdkPixbuf *m_sforzando_icon;
		static GdkPixbuf *m_bow_up_icon;
		static GdkPixbuf *m_bow_down_icon;
		static GdkPixbuf *m_groupbeams_icon;
		static GdkPixbuf *m_isolate_icon;
		static GdkPixbuf *m_flipstem_icon;

		static GdkPixbuf *m_tremolo_icon1;
		static GdkPixbuf *m_tremolo_icon2;
		static GdkPixbuf *m_tremolo_icon3;
		static GdkPixbuf *m_tremolo_icon4;

		static GdkPixbuf *m_brace_icon;
		static GdkPixbuf *m_bracket_icon;

		static GdkPixbuf *m_ped_on_icon;
		static GdkPixbuf *m_ped_off_icon;
		static GdkPixbuf *m_trill_icon;
		static GdkPixbuf *m_prall_icon;
		static GdkPixbuf *m_open_icon;
		static GdkPixbuf *m_mordent_icon;
		static GdkPixbuf *m_fermata_icon;
		static GdkPixbuf *m_arpeggio_icon;
		static GdkPixbuf *m_turn_icon;
		static GdkPixbuf *m_revturn_icon;

		static GdkPixbuf *m_comm_icon;
		static GdkPixbuf *m_cut_icon;

		static GdkPixbuf *m_midikeyboard_icon;
		static GdkPixbuf *m_record_icon;

		static GdkPixbuf *m_lines;
		static int m_lines_buf_width, m_lines_buf_height;

		static GdkPixbuf *m_signs;
		static int m_signs_buf_width, m_signs_buf_height;

		static int m_drum_icons_width, m_drum_icons_height;

		static GdkPixbuf *m_background1;
		static GdkPixbuf *m_mini_background1;
		static GdkPixbuf *m_mini_background2;
		static GdkPixbuf *m_background2;
		static GdkPixbuf *m_backgroundv1;
		static GdkPixbuf *m_backgroundv2;
		static GdkPixbuf *m_backgroundh1;
		static GdkPixbuf *m_backgroundh2;


		static NedBbox m_acc_bbox;
		static double getFlagWidth() {return m_flag_width;}
		static char my_getc(FILE *fp);
		static void my_ungetc(char c, FILE *fp);
		static void unreadWord(char *word);
		static bool readWord(FILE *fp, char *word);
		static bool readWordWithNum(FILE *fp, char *word);
		static bool readWordWithAllChars(FILE *fp, char *word);
		static bool readWordOfLength(FILE *fp, char *word);
		static bool readString(FILE *fp, char *word);
		static bool readTillEnd(FILE *fp, char *word);
		static bool readInt(FILE *fp, int *val);
		static bool readUnsignedInt(FILE *fp, unsigned int *val);
		static bool readLong(FILE *fp, unsigned long long*val);
		static bool readHex(FILE *fp, int *val);
		static bool readFloat(FILE *fp, double *val);
		static bool removeWhiteSpaces(char **s);
		static void removeNonAsciiFromUTF8String(const gchar *s);
		static const char *m_error_message;
		static int m_input_line;
		static void startMidiInput();
		static void stopMidiInput();
		static void close_sequ();
		static void MidiCtrl(unsigned int ctrl_param, int channel, int val);
		static void NoteOn(int channel, int pitch, int pgm, int vol);
		static void NoteOff(int channel, int pitch);
		static void SeqReset();

		static void addMidiNoteEvent(unsigned short channel, unsigned char voice, unsigned int offset, unsigned int type, int data1, int pgm, 
			unsigned short segno_sign, NedNote *note);
		static void addMidiCtrlEvent(unsigned short channel, unsigned int offset, unsigned int ctrl_param, int val);
		static void addTempoChange(unsigned int offset, double tempoinverse);
		static void addKeysigEntry(unsigned int offset, int chn, int key);
		static void addTempoChangeAbs(unsigned long long midi_time, double tempoinverse);
		static void addMeasureEntry(NedMeasure *measure);
		static void addTempoChange(double tempoinverse);
		static void addFermata(unsigned int offset);
		static void addPseudoEvent2(unsigned int offset, int event, int chn, int data);
		static void setStartVolume(unsigned char chn, int voice_idx, unsigned char vol);
		static void correctFermataTempo();
		static void handleSegnos();
		static void prepareMidiEventList(double tempo_inverse);
		static void startReplay(NedNote *startNote, double tempo_inverse);
		static void collectTempoEvents(GList **tempo_chords);
		static void copyAllBetweenMeasures(int start_measure_number, int alternative1_measure_number, int end_measure_number);
		static bool isPlaying() {return m_playing;}
		static bool m_midi_echo;
		static void stopReplay();
		static void increaseSystemStartTime(unsigned long long system_duration);
		static const char *GM_Instruments[];
		static int getNumInstruments();
		static GList *listMidiOutDevices(int *index_of_subscribed_out_device);
		static GList *listMidiInDevices(int *index_of_subscribed_in_device);
		static void subscribe_out(int idx);
		static void subscribe_in(int idx);
		static void prepareAddrStruct();
		static int addAddr(void *addr);
		static void addAddr(int marker, void *addr);
		static void *getAdressOfMarker(int marker);
		static int getMarkerOfAddress(void *addr);
		static void startMidiExport(FILE *fp, NedMainWindow *main_window);
		static void addLastFile(char *filename);
		static char *m_recent_files[MAX_RECENT_FILES];
		static void addToRecentFiles(char *fname);
		static int getClefDist(int oldclef, int old_oct_offs, int newclef, int new_oct_offs);
		static void reset_tools();
		static void playImmediately(int channel, int pgm, int num_midi_input_notes, int *midi_input_chord, int vol);
		static bool alsaSequencerOutOk() {return m_sequ_out != NULL;}
		static bool alsaSequencerInOk() {return m_sequ_in != NULL;}
		static bool m_avoid_immadiate_play;
		static struct paper_info_struct *getPaperInfo(const char *papername);
		static const char *m_print_cmd;
		static int m_width;
		static int m_height;
		static int m_tuplet_tab[14];
		static GList *m_main_clip_board;
		static int m_number_of_first_selected_staff, m_number_of_last_selected_staff;
#ifdef YELP_PATH
		static char *m_yelparg;
#endif
		static GList *m_main_windows;
		static NedMainWindow *m_window_with_last_focus;
		static void appendTempFileName(char *fname);
		static const char *m_last_folder;
		static char *m_last_xml_dir;
		static char *m_last_midi_dir;
		static GList *clone_chords_and_rests(GList *group, bool remove_completely_hidden_voices);
		static void split_element(GList **elements, GList *sp_element, unsigned long long delta1, unsigned long long delta2);
		static bool test_for_incomplete_tuplets(GList *elements);
		static void showLicense(GtkWindow *parent);
		static bool selection_has_uncomplete_tuplets(GList *elements);
		static void setLengthField(unsigned int length_field_value) {m_length_field_value = length_field_value;}
		static unsigned int getPartLength(int *dotcount);
		static unsigned int getPartLength(int *dotcount, bool is_rest, bool build_whole_rests, unsigned int measure_duration);
		static void setLengthField2(unsigned long long measure_start, unsigned long long note_start, unsigned int length_field_value);
		static unsigned int getPartLength2(int *dotcount);
		static unsigned int getPartLength2(int *dotcount, bool is_rest, bool build_whole_rests, unsigned int measure_duration);
		static bool fittingDuration(unsigned int duration);
		static bool fittingPosition(unsigned long long position);
		static unsigned int subtime(struct timeval *future, struct timeval *now);
		static void addtime(struct timeval *now, unsigned int msecs, struct timeval *res);
		static unsigned long long getSystemStartTime() {return m_system_start_time;}
		static int determineLastLine(int treble_line, int clef);
		static const char *getLilyPondClefName(int clef_number);
		static const char *getLilyPondKeySigName(int keysig_number);
		static void writeLilyFreeChord(FILE *fp, NedChordName *chordname, unsigned int length, unsigned int duration, int dotcount, unsigned int *midi_len);
		static unsigned int getCurrentLength();
		static int getCurrentNoteHead();
		static unsigned int getStatus();
		static bool getRestMode();
		static int getDotCount();
		static bool getTiedMode();
		static void setButtons(unsigned int length, int dotcount, unsigned int chord_status, unsigned int note_status, bool isRest, bool tie_forward, int note_head);
		static void setNoteLengthButtons(unsigned int length);
		static void setToolboxTransient(NedMainWindow *w);
		static void popup_tools(NedMainWindow *w);
		static void resetSomeButtons();
		static void resetButtons();
		static void resetTieButton();
		static void toggleButton(int nr);
		static void toggleRest();
		static void toggleTie();
		static void toggleDot();
		static void toggleSharp();
		static void toggleNatural();
		static void toggleFlat();
		static NedMainWindow *getMainwWindowWithLastFocus();
		static NedStaff *m_recorded_staff;
		static void setMainwWindowWithFocus(NedMainWindow *w);
		static snd_seq_t *getMidiInSequ() {return m_sequ_in;}
		static snd_seq_t *getMidiOutSequ() {return m_sequ_out;}
		static gint m_toolboxX, m_toolboxY;
	private:
		static void printMidiEvent(MidiEventStruct *ev);
		static void handleVolAndTempoChanges(double tempo_inverse);
		static void removeAllPseudosEvents();
		static GList *findListAtMeasure(int meas_number, NedMeasure **measure);
		static void addTimeOffsetAfter(GList *end_ptr, unsigned long long time_diff);
		static void subTimeOffsetAfter(GList *end_ptr, unsigned long long time_diff);
		static GList *cloneAllMidiEventsBetween(GList *start_ptr, GList *end_ptr, unsigned long long extra_offs);
		static void deleteAllBetween(GList *start_ptr, GList *end_ptr /* , unsigned long long time_diff */);
		static void handleDSalFine(bool da_capo);
		static void handleDSalCoda(bool da_capo);
		static struct paper_info_struct m_paper_info[];
		static void process_midi_event();
		static gboolean do_poll_midi_device(void *data);
		static gboolean stopImmediate(void *data);
		static int m_number_of_midi_poll_fds;
		static bool m_run_midi_in;
		static GList *m_midi_out_devices, *m_midi_in_devices;
		static GList *m_midi_events;
		static GList *m_play_ptr;
		static int m_handler_id;
		static snd_seq_addr_t m_alsa_out_addr;
		static snd_seq_addr_t m_alsa_in_addr;
		static snd_seq_addr_t m_alsa_out_dest_addr;
		static snd_seq_addr_t m_alsa_in_src_addr;
		static unsigned long long m_system_start_time;
		static int m_channel_pgms[16];
		static int m_channel_volume[16][VOICE_COUNT];
		static struct crescendo_info m_last_crescendo_info[16];
		static double m_channel_volume_ratio[16][VOICE_COUNT];
		static struct accel_info m_last_ritardando;
		static GList *m_addr_rel_list;
		static void write_config();
		static void read_config();
		static bool permtest(snd_seq_port_info_t *pinfo, unsigned int perm);
		static void placeMidiEvent(MidiEventStruct *ev);
		static int m_marker_counter;
		static gboolean playNext(void *data);
		static bool m_playing;
		static cairo_font_options_t *m_fontoptions;
#ifndef HAS_SET_SCALED_FONT
		static cairo_matrix_t m_matrixes[ZOOMLEVELS];
#endif
		static void open_seq();
		static cairo_font_face_t *m_font_face;
		static cairo_scaled_font_t *m_scaled_fonts[ZOOMLEVELS];
		static char *m_font_file_name;
		static double m_zoom_factors[ZOOMLEVELS];
		static cairo_scaled_font_t *createScaledFont(int zoom_level);

		static unsigned int m_length_field_value;
		static unsigned int m_part_start, m_part_end;

		static void init_fonts(char *font_file_name);
		static int loadfontface(char *font_file_name);
		static void loadPixmap(const guint8 *data, GdkPixbuf **pixbuf, const char *stock_id, int *width, int *height);
		static GtkIconFactory *m_icon_factory;
		static struct timeval m_expected_time;
		static NedTools *m_tools;
		static double m_flag_width;
		static char m_word_buffer[128];
		static snd_seq_t *m_sequ_out;
		static snd_seq_t *m_sequ_in;
		static int m_count_of_last_imm_pitches;
		static snd_seq_event_t m_last_imm_notes[MAX_MIDI_INPUT_CHORD];
		static GList *m_temp_file_names;
		static double m_tempo_inverse;
		static GList *m_fermata_list;
		static bool m_ds_al_coda_available, m_ds_al_fine_available;
		static bool m_d_capo_al_fine_available, m_d_capo_al_coda_available;
};


#endif /* RESOURCE_H */
