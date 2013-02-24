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

//#define INPUT_DEBUG
#include <cairo.h>
#include <cairo-ft.h>
#include <iostream>
#include <exception>
#include "localization.h"
#include "mainwindow.h"
#include "chordorrest.h"
#include "note.h"
#include "resource.h"
#include "midiexport.h"
#include "measure.h"
#include "tuplet.h"
#include "tools.h"
#include "freechordname.h"
#include "druminfo.h"
#include "voice.h"

#include "icons/nted.h"

#include "icons/stroken_grace_icon.h"
#include "icons/grace_eighth_icon.h"
#include "icons/grace_sixth_icon.h"
#include "icons/n32ndnote_icon.h"
#include "icons/n64thnote_icon.h"
#include "icons/eightnote_icon.h"
#include "icons/eightnote_dot_icon.h"
#include "icons/fullnote_icon.h"
#include "icons/halfnote_icon.h"
#include "icons/halfnote_dot_icon.h"
#include "icons/quarternote_icon.h"
#include "icons/quarternote_dot_icon.h"
#include "icons/sixteenthnote_icon.h"
#include "icons/sixteenthnote_dot_icon.h"
#include "icons/insert_mode_icon.h"
#include "icons/arrow_up_icon.h"
#include "icons/arrow_down_icon.h"
#include "icons/arrow_left_icon.h"
#include "icons/arrow_right_icon.h"
#include "icons/treble_clef_icon.h"
#include "icons/alto_clef_icon.h"
#include "icons/bass_clef_icon.h"


#include "icons/rest_icon.h"

#include "icons/shift_mode_icon.h"

#include "icons/dot_icon.h"
#include "icons/ddot_icon.h"
#include "icons/cross_icon.h"
#include "icons/flat_icon.h"
#include "icons/natural_icon.h"

#include "icons/tied_icon.h"

#include "icons/delete_staff_icon.h"
#include "icons/delete_system_icon.h"

#include "icons/color_icon.h"

#include "icons/lyrics_icon.h"

#include "icons/key_icon.h"
#include "icons/tool_icon.h"

#include "icons/normal_note_icon.h"
#include "icons/drum1_icon.h"
#include "icons/drum2_icon.h"
#include "icons/drum3_icon.h"
#include "icons/drum4_icon.h"
#include "icons/drum5_icon.h"
#include "icons/drum6_icon.h"
#include "icons/drum7_icon.h"
#include "icons/drum8_icon.h"
#include "icons/drum9_icon.h"
#include "icons/drum10_icon.h"
#include "icons/drum11_icon.h"
#include "icons/drum12_icon.h"
#include "icons/guitar_note_icon.h"
#include "icons/guitar_note_no_stem_icon.h"

#include "icons/normal_note_icon_small.h"
#include "icons/drum1_icon_small.h"
#include "icons/drum2_icon_small.h"
#include "icons/drum4_icon_small.h"
#include "icons/drum6_icon_small.h"
#include "icons/drum8_icon_small.h"
#include "icons/drum3_icon_small.h"
#include "icons/drum5_icon_small.h"
#include "icons/drum7_icon_small.h"
#include "icons/drum9_icon_small.h"
#include "icons/drum10_icon_small.h"
#include "icons/drum11_icon_small.h"
#include "icons/drum12_icon_small.h"

#include "icons/dcross_icon.h"
#include "icons/dflat_icon.h"
#include "icons/stacc_icon.h"
#include "icons/staccatissimo_icon.h"
#include "icons/str_pizz_icon.h"
#include "icons/tenuto_icon.h"
#include "icons/sforzato_icon.h"
#include "icons/sforzando_icon.h"
#include "icons/bow_up_icon.h"
#include "icons/bow_down_icon.h"
#include "icons/groupbeams_icon.h"
#include "icons/isolate_icon.h"
#include "icons/flipstem_icon.h"

#include "icons/tremolo_icon1.h"
#include "icons/tremolo_icon2.h"
#include "icons/tremolo_icon3.h"
#include "icons/tremolo_icon4.h"

#include "icons/brace_icon.h"
#include "icons/bracket_icon.h"

#include "icons/ped_on_icon.h"
#include "icons/ped_off_icon.h"
#include "icons/trill_icon.h"
#include "icons/prall_icon.h"
#include "icons/open_icon.h"
#include "icons/mordent_icon.h"
#include "icons/turn_icon.h"
#include "icons/revturn_icon.h"
#include "icons/fermata_icon.h"
#include "icons/arpeggio_icon.h"

#include "icons/comm_icon.h"
#include "icons/cut_icon.h"

#include "icons/midikeyboard_icon.h"
#include "icons/record_icon.h"

#include "icons/lines.h"
#include "icons/signs.h"

#include "icons/background1.h"
#include "icons/background2.h"
#include "icons/backgroundv1.h"
#include "icons/backgroundv2.h"
#include "icons/backgroundh1.h"
#include "icons/backgroundh2.h"

#define CONFIG_DIR ".nted"
#define CONFIG_FILE "ntedrc"

GdkPixbuf *NedResource::m_the_nted_icon;

GdkPixbuf *NedResource::m_32ndnote_icon;
GdkPixbuf *NedResource::m_64thnote_icon;
GdkPixbuf *NedResource::m_eightnote_icon;
GdkPixbuf *NedResource::m_eightnote_dot_icon;
GdkPixbuf *NedResource::m_fullnote_icon;
GdkPixbuf *NedResource::m_halfnote_icon;
GdkPixbuf *NedResource::m_halfnote_dot_icon;
GdkPixbuf *NedResource::m_quarternote_icon;
GdkPixbuf *NedResource::m_quarternote_dot_icon;
GdkPixbuf *NedResource::m_sixteenthnote_icon;
GdkPixbuf *NedResource::m_sixteenthnote_dot_icon;
GdkPixbuf *NedResource::m_stroken_grace_icon;
GdkPixbuf *NedResource::m_grace_eighth_icon;
GdkPixbuf *NedResource::m_grace_sixth_icon;

GdkPixbuf *NedResource::m_rest_icon;

GdkPixbuf *NedResource::m_shift_mode_icon;
GdkPixbuf *NedResource::m_insert_mode_icon;

GdkPixbuf *NedResource::m_dot_icon;
GdkPixbuf *NedResource::m_ddot_icon;
GdkPixbuf *NedResource::m_cross_icon;
GdkPixbuf *NedResource::m_flat_icon;
GdkPixbuf *NedResource::m_natural_icon;

GdkPixbuf *NedResource::m_tied_icon;

GdkPixbuf *NedResource::m_arrow_up_icon;
GdkPixbuf *NedResource::m_arrow_down_icon;
GdkPixbuf *NedResource::m_arrow_left_icon;
GdkPixbuf *NedResource::m_arrow_right_icon;

GdkPixbuf *NedResource::m_treble_clef_icon;
GdkPixbuf *NedResource::m_bass_clef_icon;
GdkPixbuf *NedResource::m_alto_clef_icon;

GdkPixbuf *NedResource::m_delete_staff_icon;
GdkPixbuf *NedResource::m_delete_system_icon;

GdkPixbuf *NedResource::m_color_icon;

GdkPixbuf *NedResource::m_lyrics_icon;

GdkPixbuf *NedResource::m_key_icon;
GdkPixbuf *NedResource::m_tool_icon;

GdkPixbuf *NedResource::m_normal_note_icon;
GdkPixbuf *NedResource::m_drum1_icon;
GdkPixbuf *NedResource::m_drum2_icon;
GdkPixbuf *NedResource::m_drum3_icon;
GdkPixbuf *NedResource::m_drum4_icon;
GdkPixbuf *NedResource::m_drum5_icon;
GdkPixbuf *NedResource::m_drum6_icon;
GdkPixbuf *NedResource::m_drum7_icon;
GdkPixbuf *NedResource::m_drum8_icon;
GdkPixbuf *NedResource::m_drum9_icon;
GdkPixbuf *NedResource::m_drum10_icon;
GdkPixbuf *NedResource::m_drum11_icon;
GdkPixbuf *NedResource::m_drum12_icon;
GdkPixbuf *NedResource::m_guitar_note_icon;
GdkPixbuf *NedResource::m_guitar_note_no_stem_icon;

GdkPixbuf *NedResource::m_normal_note_icon_small;
GdkPixbuf *NedResource::m_drum1_icon_small;
GdkPixbuf *NedResource::m_drum2_icon_small;
GdkPixbuf *NedResource::m_drum4_icon_small;
GdkPixbuf *NedResource::m_drum6_icon_small;
GdkPixbuf *NedResource::m_drum8_icon_small;
GdkPixbuf *NedResource::m_drum3_icon_small;
GdkPixbuf *NedResource::m_drum5_icon_small;
GdkPixbuf *NedResource::m_drum7_icon_small;
GdkPixbuf *NedResource::m_drum9_icon_small;
GdkPixbuf *NedResource::m_drum10_icon_small;
GdkPixbuf *NedResource::m_drum11_icon_small;
GdkPixbuf *NedResource::m_drum12_icon_small;


GdkPixbuf *NedResource::m_dcross_icon;
GdkPixbuf *NedResource::m_dflat_icon;
GdkPixbuf *NedResource::m_stacc_icon;
GdkPixbuf *NedResource::m_staccatissimo_icon;
GdkPixbuf *NedResource::m_str_pizz_icon;
GdkPixbuf *NedResource::m_tenuto_icon;
GdkPixbuf *NedResource::m_sforzato_icon;
GdkPixbuf *NedResource::m_sforzando_icon;
GdkPixbuf *NedResource::m_bow_up_icon;
GdkPixbuf *NedResource::m_bow_down_icon;
GdkPixbuf *NedResource::m_groupbeams_icon;
GdkPixbuf *NedResource::m_isolate_icon;
GdkPixbuf *NedResource::m_flipstem_icon;

GdkPixbuf *NedResource::m_tremolo_icon1;
GdkPixbuf *NedResource::m_tremolo_icon2;
GdkPixbuf *NedResource::m_tremolo_icon3;
GdkPixbuf *NedResource::m_tremolo_icon4;

GdkPixbuf *NedResource::m_brace_icon;
GdkPixbuf *NedResource::m_bracket_icon;

GdkPixbuf *NedResource::m_ped_on_icon;
GdkPixbuf *NedResource::m_ped_off_icon;
GdkPixbuf *NedResource::m_trill_icon;
GdkPixbuf *NedResource::m_prall_icon;
GdkPixbuf *NedResource::m_open_icon;
GdkPixbuf *NedResource::m_mordent_icon;
GdkPixbuf *NedResource::m_turn_icon;
GdkPixbuf *NedResource::m_revturn_icon;
GdkPixbuf *NedResource::m_fermata_icon;
GdkPixbuf *NedResource::m_arpeggio_icon;

GdkPixbuf *NedResource::m_comm_icon;
GdkPixbuf *NedResource::m_cut_icon;

GdkPixbuf *NedResource::m_midikeyboard_icon;
GdkPixbuf *NedResource::m_record_icon;

GdkPixbuf *NedResource::m_lines;
GdkPixbuf *NedResource::m_signs;

int NedResource::m_lines_buf_width;
int NedResource::m_lines_buf_height;

int NedResource::m_signs_buf_width;
int NedResource::m_signs_buf_height;

int NedResource::m_drum_icons_width;
int NedResource::m_drum_icons_height;

GdkPixbuf *NedResource::m_background1;
GdkPixbuf *NedResource::m_mini_background1;
GdkPixbuf *NedResource::m_mini_background2;
GdkPixbuf *NedResource::m_background2;
GdkPixbuf *NedResource::m_backgroundv1;
GdkPixbuf *NedResource::m_backgroundv2;
GdkPixbuf *NedResource::m_backgroundh1;
GdkPixbuf *NedResource::m_backgroundh2;

struct timeval NedResource::m_expected_time;

snd_seq_t *NedResource::m_sequ_out = NULL;
snd_seq_t *NedResource::m_sequ_in = NULL;
char NedResource::m_word_buffer[128];

gint NedResource::m_toolboxX = 0;
gint NedResource::m_toolboxY = 0;

NedStaff *NedResource::m_recorded_staff = NULL;

GtkIconFactory *NedResource::m_icon_factory;

NedBbox NedResource::m_acc_bbox;

double NedResource::m_flag_width;

const char *NedResource::m_error_message;
int NedResource::m_input_line;

GList *NedResource::m_midi_events = NULL;
GList *NedResource::m_play_ptr;
int NedResource::m_handler_id;
unsigned long long NedResource::m_system_start_time;
bool NedResource::m_playing = FALSE;
GList *NedResource::m_midi_out_devices = NULL;
GList *NedResource::m_midi_in_devices = NULL;
snd_seq_addr_t NedResource::m_alsa_out_addr;
snd_seq_addr_t NedResource::m_alsa_in_addr;
snd_seq_addr_t NedResource::m_alsa_out_dest_addr;
snd_seq_addr_t NedResource::m_alsa_in_src_addr;
int NedResource::m_number_of_midi_poll_fds = 0;
bool NedResource::m_run_midi_in = false;
int NedResource::m_count_of_last_imm_pitches = 0;
snd_seq_event_t NedResource::m_last_imm_notes[MAX_MIDI_INPUT_CHORD];

GList *NedResource::m_addr_rel_list = NULL;
int NedResource::m_marker_counter = 1;
int NedResource::m_channel_pgms[16];
int NedResource::m_channel_volume[16][VOICE_COUNT];
double NedResource::m_channel_volume_ratio[16][VOICE_COUNT];
struct crescendo_info NedResource::m_last_crescendo_info[16];
struct accel_info NedResource::m_last_ritardando;
bool NedResource::m_midi_echo;

int NedResource::m_tuplet_tab[14]  = {0, 0, 3, 2, 3, 4, 4, 4, 12, 8, 8, 8, 8, 8};

GList *NedResource::m_main_clip_board = NULL;
int NedResource::m_number_of_first_selected_staff;
int NedResource::m_number_of_last_selected_staff;
bool NedResource::m_avoid_immadiate_play = FALSE;
char *NedResource::m_recent_files[MAX_RECENT_FILES];
const char *NedResource::m_print_cmd;
int NedResource::m_width;
int NedResource::m_height;
GList *NedResource::m_temp_file_names = NULL;

unsigned int NedResource::m_length_field_value = 0;

unsigned int NedResource::m_part_start, NedResource::m_part_end;

double NedResource::m_tempo_inverse;

NedTools *NedResource::m_tools = NULL;

const char *NedResource::m_last_folder = NTED_DATADIR "/examples";
char *NedResource::m_last_xml_dir = NULL;
char *NedResource::m_last_midi_dir = NULL;

const char *NedResource::GM_Instruments[] = {
 N_("Piano 1"),          N_("Piano 2"), N_("Piano 3"),          N_("Honky-tonk"),
 N_("E.Piano 1"),        N_("E.Piano 2"), N_("Harpsichord"),      N_("Clavinet"),
 N_("Celesta"),          N_("Glockenspiel"), N_("Music Box"),        N_("Vibraphone"),
 N_("Marimba"),          N_("Xylophone"), N_("Tubular Bells"),    N_("Dulcimer"),
 N_("Organ 1"),          N_("Organ 2"), N_("Organ 3"),          N_("Church Organ"),
 N_("Reed Organ"),       N_("Accordion"), N_("Harmonica"),        N_("Bandoneon"),
 N_("Nylon Guitar"),     N_("Steel Guitar"), N_("Jazz Guitar"),      N_("Clean Guitar"),
 N_("Guitar Mutes"),     N_("Overdrive Guitar"), N_("Guitar Harmonics"), N_("Guitar Harmonics"),
 N_("Acoustic Bass"),    N_("Fingered Bass"), N_("Picked Bass"),      N_("Fretless Bass"),
 N_("Slap Bass 1"),      N_("Slap Bass 2"), N_("Synth Bass 1"),     N_("Synth Bass 2"), 
 N_("Violin"),           N_("Viola"), N_("Cello"),            N_("Contrabass"),
 N_("Tremolo Strings"),  N_("Pizzicato"), N_("Harp"),             N_("Timpani"),
 N_("Strings"),          N_("Slow Strings"), N_("Synth Strings 1"),  N_("Synth Strings 2"),
 N_("Choir Aahs"),       N_("Voice Oohs"), N_("Synth Vox"),        N_("Orchestra Hit"),
 N_("Trumpet"),          N_("Trombone"), N_("Tuba"),             N_("Mute Trumpet"), 
 N_("French Horns"),     N_("Brass"), N_("Synth Brass 1"),    N_("Synth Brass 2"),
 N_("Soprano Sax"),      N_("Alto Sax"), N_("Tenor Sax"),        N_("Baritone Sax"),
 N_("Oboe"),             N_("English Horn"), N_("Bassoon"),          N_("Clarinet"),
 N_("Piccolo"),          N_("Flute"), N_("Recorder"),         N_("Pan Flute"),
 N_("Bottle Chiff"),     N_("Shakuhachi"), N_("Whistle"),          N_("Ocarina"),
 N_("Square Wave"),      N_("Saw Wave"), N_("Synth Calliope"),   N_("Chiffer Lead"),
 N_("Charang"),          N_("Solo Vox"), N_("5th Saw Wave"),     N_("Bass & Lead"),
 N_("Fantasia"),         N_("Warm Pad"), N_("Poly Synth"),       N_("Space Voice"),
 N_("Bowed Glass"),      N_("Metal Pad"), N_("Halo Pad"),         N_("Sweep Pad"),
 N_("Ice Rain"),         N_("Soundtrack"), N_("Crystal"),          N_("Atmosphere"), 
 N_("Brightness"),       N_("Goblin"), N_("Echo Drops"),       N_("Star Theme"),
 N_("Sitar"),            N_("Banjo"), N_("Shamisen"),         N_("Koto"),
 N_("Kalimba"),          N_("Bagpipe"), N_("Fiddle"),           N_("Shenai"),
 N_("Tinker Bell"),      N_("Agogo"), N_("Steel Drum"),       N_("Wood Block"),
 N_("Taiko Drum"),       N_("Melodic Tom"), N_("Synth Drum"),       N_("Reverse Cymbal"),
 N_("Fret Noise"),       N_("Breath Noise"), N_("Seashore"),         N_("Bird"),
 N_("Telephone"),        N_("Helicopter"), N_("Applause"),         N_("Gun Shot")
}; 

#define MAX_ZOOM 76.0
#define MIN_ZOOM 9.0

double NedResource::m_braceOutline[16][6] = {
{-42.322, 58.999, -48.185, 97.0849, -49.834, 163.292},
{5.001, 78.333, 10.499, 145.924, 8.332, 204.748},
{-1.69434, 45.9834, -16.75, 68.25, -33, 110},
{25.002, 38.665, 37.3057, 62.0166, 39, 108},
{2.16699, 58.8242, -9.333, 128, -14.334, 206.333},
{1.649, 66.207, 7.512, 104.293, 49.834, 163.292},
{-18.002, -32.335, -29.5178, -50.8487, -34.002, -87.001},
{-6.15526, -49.6221, 1.66701, -77.167, 7.33401, -128},
{5.45799, -48.962, 12.9874, -76.8682, 10.333, -126.167},
{-1.95312, -36.2705, -9.621, -56.3516, -21.999, -90.501},
{-7.03421, -19.4062, -16.6231, -27.6777, -27.251, -45.374},
{10.6289, -18.3457, 20.1052, -27.0782, 27.253, -47.039},
{12.246, -34.1964, 20.0461, -54.2305, 21.999, -90.501},
{2.65425, -49.2989, -4.875, -77.2051, -10.333, -126.167},
{-5.66701, -50.833, -13.489, -78.3779, -7.334, -128},
{4.48433, -36.1524, 16, -54.666, 34.002, -87.001}
};


double NedResource::m_bracketEndOutline[5][6] = {
{142, 6, 184, -8, 336, 106},
{103.961, 77.9707, 132.98, 153.72, 218, 252},
{-68, -104, -98.0664, -170.846, -184, -270},
{-104, -120, -204, -158, -256, -178},
{-35.9062, -13.8105, -92, -22, -115, -27}
};

struct paper_info_struct NedResource::m_paper_info[] = {
{"A5", 298, 421},
{"A4", 595, 842},
{"A3", 842, 1190},
{"B4", 729, 1032},
{"Letter", 612, 791},
{"Legal", 612, 1008},
{"230x293", 651, 677},
{NULL, 0, 0}};


cairo_font_face_t *NedResource::m_font_face;
cairo_scaled_font_t *NedResource::m_scaled_fonts[ZOOMLEVELS];
char *NedResource::m_font_file_name;
cairo_text_extents_t NedResource::fontextentions[ZOOMLEVELS][MAXELEMENTS];
double NedResource::m_zoom_factors[ZOOMLEVELS];
cairo_font_options_t *NedResource::m_fontoptions;

#ifndef HAS_SET_SCALED_FONT
cairo_matrix_t NedResource::m_matrixes[ZOOMLEVELS];
#endif

struct fermata_info {
	unsigned long long midi_time;
	double tempo_inverse;
	bool a_tempo;
};

GList *NedResource::m_fermata_list = NULL;
bool NedResource::m_ds_al_coda_available = false;
bool NedResource::m_ds_al_fine_available = false;
bool NedResource::m_d_capo_al_fine_available = false;
bool NedResource::m_d_capo_al_coda_available = false;

#ifdef YELP_PATH
char *NedResource::m_yelparg = NULL;
#endif
GList *NedResource::m_main_windows = NULL;
NedMainWindow *NedResource::m_window_with_last_focus = NULL;


void NedResource::initialize() {
	int i;
	int dummyw, dummyh;
	GtkStockItem stock_item_up;
	GtkStockItem stock_item_down;
	GtkStockItem stock_item_delete_staff;
	GtkStockItem stock_item_system_staff;
	m_midi_echo = TRUE;
	m_height = HEIGHT;
	m_width = WIDTH;
	const char *fontdir;

	for (i = 0; i < MAX_RECENT_FILES; m_recent_files[i++] = NULL);

	
	fontdir = getenv("NTED_FONT_DIR");
	if (fontdir == NULL) {
		fontdir = NTED_DATADIR;
	}

	try {
		m_font_file_name = new char[strlen(fontdir) + strlen("/") + strlen(FONTFILE) + 1];
	}
	catch(...){
		Abort("NedResource::initialize: allocation failed");
	}
	sprintf(m_font_file_name, "%s/%s", fontdir, FONTFILE);
	init_fonts(m_font_file_name);
	m_icon_factory = gtk_icon_factory_new();
	loadPixmap(nted, &m_the_nted_icon, "the-nted-icon", &dummyw, &dummyh);

	loadPixmap(n32ndnote_icon, &m_32ndnote_icon, "32ndnote-icon", &dummyw, &dummyh);
	loadPixmap(n64thnote_icon, &m_64thnote_icon, "64thnote-icon", &dummyw, &dummyh);
	loadPixmap(eightnote_icon, &m_eightnote_icon, "eighthnote-icon", &dummyw, &dummyh);
	loadPixmap(eightnote_dot_icon, &m_eightnote_dot_icon, "eighthnote-dot-icon", &dummyw, &dummyh);
	loadPixmap(fullnote_icon, &m_fullnote_icon, "fullnote_icon", &dummyw, &dummyh);
	loadPixmap(halfnote_icon, &m_halfnote_icon, "halfnote-icon",  &dummyw, &dummyh);
	loadPixmap(halfnote_dot_icon, &m_halfnote_dot_icon, "halfnote-dot-icon",  &dummyw, &dummyh);
	loadPixmap(quarternote_icon, &m_quarternote_icon, "quarternote-icon", &dummyw, &dummyh);
	loadPixmap(quarternote_dot_icon, &m_quarternote_dot_icon, "quarternote-dot-icon", &dummyw, &dummyh);
	loadPixmap(sixteenthnote_icon, &m_sixteenthnote_icon, "sixteenthnote-icon", &dummyw, &dummyh);
	loadPixmap(sixteenthnote_dot_icon, &m_sixteenthnote_dot_icon, "sixteenthnote-dot-icon", &dummyw, &dummyh);

	loadPixmap(stroken_grace_icon, &m_stroken_grace_icon, "stroken-grace-icon", &dummyw, &dummyh);
	loadPixmap(grace_eighth_icon, &m_grace_eighth_icon, "grace-eighth-icon", &dummyw, &dummyh);
	loadPixmap(grace_sixth_icon, &m_grace_sixth_icon, "grace-sixth-icon", &dummyw, &dummyh);

	loadPixmap(rest_icon, &m_rest_icon, "rest-icon", &dummyw, &dummyh);

	loadPixmap(shift_mode_icon, &m_shift_mode_icon, "shift-mode-icon", &dummyw, &dummyh);
	loadPixmap(insert_mode_icon, &m_insert_mode_icon, "insert-mode-icon", &dummyw, &dummyh);

	loadPixmap(dot_icon, &m_dot_icon, "dot-icon", &dummyw, &dummyh);
	loadPixmap(ddot_icon, &m_ddot_icon, "ddot-icon", &dummyw, &dummyh);
	loadPixmap(cross_icon, &m_cross_icon, "cross-icon", &dummyw, &dummyh);
	loadPixmap(flat_icon, &m_flat_icon, "flat-icon", &dummyw, &dummyh);
	loadPixmap(natural_icon, &m_natural_icon, "natural-icon", &dummyw, &dummyh);

	loadPixmap(tied_icon, &m_tied_icon, "tied-icon", &dummyw, &dummyh);

	loadPixmap(delete_staff_icon, &m_delete_staff_icon, "delete-staff", &dummyw, &dummyh);
	loadPixmap(delete_system_icon, &m_delete_system_icon, "delete-system", &dummyw, &dummyh);

	loadPixmap(color_icon, &m_color_icon, "color-notes", &dummyw, &dummyh);

	loadPixmap(lyrics_icon, &m_lyrics_icon, "lyrics-icon", &dummyw, &dummyh);

	loadPixmap(key_icon, &m_key_icon, "key-icon", &dummyw, &dummyh);
	loadPixmap(tool_icon, &m_tool_icon, "tool-icon", &dummyw, &dummyh);

	loadPixmap(normal_note_icon, &m_normal_note_icon, "normal-note-icon", &dummyw, &dummyh);
	loadPixmap(drum1_icon, &m_drum1_icon, "drum1-icon", &m_drum_icons_width, &m_drum_icons_height);
	loadPixmap(drum2_icon, &m_drum2_icon, "drum2-icon", &dummyw, &dummyh);
	loadPixmap(drum3_icon, &m_drum3_icon, "drum3-icon", &dummyw, &dummyh);
	loadPixmap(drum4_icon, &m_drum4_icon, "drum4-icon", &dummyw, &dummyh);
	loadPixmap(drum5_icon, &m_drum5_icon, "drum5-icon", &dummyw, &dummyh);
	loadPixmap(drum6_icon, &m_drum6_icon, "drum6-icon", &dummyw, &dummyh);
	loadPixmap(drum7_icon, &m_drum7_icon, "drum7-icon", &dummyw, &dummyh);
	loadPixmap(drum8_icon, &m_drum8_icon, "drum8-icon", &dummyw, &dummyh);
	loadPixmap(drum9_icon, &m_drum9_icon, "drum9-icon", &dummyw, &dummyh);
	loadPixmap(drum10_icon, &m_drum10_icon, "drum10-icon", &dummyw, &dummyh);
	loadPixmap(drum11_icon, &m_drum11_icon, "drum11-icon", &dummyw, &dummyh);
	loadPixmap(drum12_icon, &m_drum12_icon, "drum12-icon", &dummyw, &dummyh);
	loadPixmap(guitar_note_icon, &m_guitar_note_icon, "guitar-note-icon", &dummyw, &dummyh);
	loadPixmap(guitar_note_no_stem_icon, &m_guitar_note_no_stem_icon, "guitar-note-no-stem-icon", &dummyw, &dummyh);

	loadPixmap(normal_note_icon_small, &m_normal_note_icon_small, "normal-note-icon-small", &dummyw, &dummyh);
	loadPixmap(drum1_icon_small, &m_drum1_icon_small, "drum1-icon-small", &dummyw, &dummyh);
	loadPixmap(drum2_icon_small, &m_drum2_icon_small, "drum2-icon-small", &dummyw, &dummyh);
	loadPixmap(drum3_icon_small, &m_drum3_icon_small, "drum3-icon-small", &dummyw, &dummyh);
	loadPixmap(drum4_icon_small, &m_drum4_icon_small, "drum4-icon-small", &dummyw, &dummyh);
	loadPixmap(drum5_icon_small, &m_drum5_icon_small, "drum5-icon-small", &dummyw, &dummyh);
	loadPixmap(drum6_icon_small, &m_drum6_icon_small, "drum6-icon-small", &dummyw, &dummyh);
	loadPixmap(drum7_icon_small, &m_drum7_icon_small, "drum7-icon-small", &dummyw, &dummyh);
	loadPixmap(drum8_icon_small, &m_drum8_icon_small, "drum8-icon-small", &dummyw, &dummyh);
	loadPixmap(drum9_icon_small, &m_drum9_icon_small, "drum9-icon-small", &dummyw, &dummyh);
	loadPixmap(drum10_icon_small, &m_drum10_icon_small, "drum10-icon-small", &dummyw, &dummyh);
	loadPixmap(drum11_icon_small, &m_drum11_icon_small, "drum11-icon-small", &dummyw, &dummyh);
	loadPixmap(drum12_icon_small, &m_drum12_icon_small, "drum12-icon-small", &dummyw, &dummyh);

	loadPixmap(dcross_icon, &m_dcross_icon, "dcross-icon", &dummyw, &dummyh);
	loadPixmap(dflat_icon, &m_dflat_icon, "dflat-icon", &dummyw, &dummyh);
	loadPixmap(stacc_icon, &m_stacc_icon, "stacc-icon", &dummyw, &dummyh);
	loadPixmap(staccatissimo_icon, &m_staccatissimo_icon, "staccatissimo-icon", &dummyw, &dummyh);
	loadPixmap(str_pizz_icon, &m_str_pizz_icon, "str-pizz-icon", &dummyw, &dummyh);
	loadPixmap(tenuto_icon, &m_tenuto_icon, "tenuto-icon", &dummyw, &dummyh);
	loadPixmap(sforzato_icon, &m_sforzato_icon, "sforzato-icon", &dummyw, &dummyh);
	loadPixmap(sforzando_icon, &m_sforzando_icon, "sforzando-icon", &dummyw, &dummyh);
	loadPixmap(bow_up_icon, &m_bow_up_icon, "bow-up-icon", &dummyw, &dummyh);
	loadPixmap(bow_down_icon, &m_bow_down_icon, "bow-down-icon", &dummyw, &dummyh);
	loadPixmap(groupbeams_icon, &m_groupbeams_icon, "groupbeams-icon", &dummyw, &dummyh);
	loadPixmap(isolate_icon, &m_isolate_icon, "isolate-icon", &dummyw, &dummyh);
	loadPixmap(flipstem_icon, &m_flipstem_icon, "flipstem-icon", &dummyw, &dummyh);

	loadPixmap(tremolo_icon1, &m_tremolo_icon1, "tremolo-icon1", &dummyw, &dummyh);
	loadPixmap(tremolo_icon2, &m_tremolo_icon2, "tremolo-icon2", &dummyw, &dummyh);
	loadPixmap(tremolo_icon3, &m_tremolo_icon3, "tremolo-icon3", &dummyw, &dummyh);
	loadPixmap(tremolo_icon4, &m_tremolo_icon4, "tremolo-icon4", &dummyw, &dummyh);

	loadPixmap(brace_icon, &m_brace_icon, "brace-icon", &dummyw, &dummyh);
	loadPixmap(bracket_icon, &m_bracket_icon, "bracket-icon", &dummyw, &dummyh);

	loadPixmap(ped_on_icon, &m_ped_on_icon, "ped-on-icon", &dummyw, &dummyh);
	loadPixmap(ped_off_icon, &m_ped_off_icon, "ped-off-icon", &dummyw, &dummyh);
	loadPixmap(trill_icon, &m_trill_icon, "trill-icon", &dummyw, &dummyh);
	loadPixmap(prall_icon, &m_prall_icon, "prall-icon", &dummyw, &dummyh);
	loadPixmap(open_icon, &m_open_icon, "open-icon", &dummyw, &dummyh);
	loadPixmap(mordent_icon, &m_mordent_icon, "mordent-icon", &dummyw, &dummyh);
	loadPixmap(turn_icon, &m_turn_icon, "turn-icon", &dummyw, &dummyh);
	loadPixmap(revturn_icon, &m_revturn_icon, "revturn-icon", &dummyw, &dummyh);
	loadPixmap(fermata_icon, &m_fermata_icon, "fermata-icon", &dummyw, &dummyh);
	loadPixmap(arpeggio_icon, &m_arpeggio_icon, "arpeggio-icon", &dummyw, &dummyh);

	loadPixmap(comm_icon, &m_comm_icon, "comm-icon", &dummyw, &dummyh);
	loadPixmap(cut_icon, &m_cut_icon, "cut-icon", &dummyw, &dummyh);

	loadPixmap(midikeyboard_icon, &m_midikeyboard_icon, "midikeyboard-icon", &dummyw, &dummyh);
	loadPixmap(record_icon, &m_record_icon, "record-icon", &dummyw, &dummyh);

	loadPixmap(lines, &m_lines, "lines", &m_lines_buf_width, &m_lines_buf_height);
	loadPixmap(signs, &m_signs, "signs", &m_signs_buf_width, &m_signs_buf_height);

	loadPixmap(background1, &m_background1, "background1", &dummyw, &dummyh);
	loadPixmap(background1, &m_mini_background1, "mini-background1", &dummyw, &dummyh);
	loadPixmap(background2, &m_background2, "background1", &dummyw, &dummyh);
	loadPixmap(background2, &m_mini_background2, "mini-background2", &dummyw, &dummyh);
	loadPixmap(backgroundv1, &m_backgroundv1, "backgroundv1", &dummyw, &dummyh);
	loadPixmap(backgroundv2, &m_backgroundv2, "backgroundv1", &dummyw, &dummyh);
	loadPixmap(backgroundh1, &m_backgroundh1, "backgroundh1", &dummyw, &dummyh);
	loadPixmap(backgroundh2, &m_backgroundh2, "backgroundh1", &dummyw, &dummyh);

	m_background1 = gdk_pixbuf_scale_simple(m_background1, X11_ICONS_BACK_DIM, X11_ICONS_BACK_DIM, GDK_INTERP_HYPER);
	m_mini_background1 = gdk_pixbuf_scale_simple(m_mini_background1, X11_ICONS_BACK_MINI_DIM, X11_ICONS_BACK_MINI_DIM, GDK_INTERP_HYPER);
	m_background2 = gdk_pixbuf_scale_simple(m_background2, X11_ICONS_BACK_DIM, X11_ICONS_BACK_DIM, GDK_INTERP_HYPER);
	m_mini_background2 = gdk_pixbuf_scale_simple(m_mini_background2, X11_ICONS_BACK_MINI_DIM, X11_ICONS_BACK_MINI_DIM, GDK_INTERP_HYPER);
	m_backgroundv1 = gdk_pixbuf_scale_simple(m_backgroundv1, X11_ICONS_BACK_DIM, 2 * X11_ICONS_BACK_DIM, GDK_INTERP_HYPER);
	m_backgroundv2 = gdk_pixbuf_scale_simple(m_backgroundv2, X11_ICONS_BACK_DIM, 2 * X11_ICONS_BACK_DIM, GDK_INTERP_HYPER);
	m_backgroundh1 = gdk_pixbuf_scale_simple(m_backgroundh1, 2 * X11_ICONS_BACK_DIM, X11_ICONS_BACK_DIM, GDK_INTERP_HYPER);
	m_backgroundh2 = gdk_pixbuf_scale_simple(m_backgroundh2, 2 * X11_ICONS_BACK_DIM, X11_ICONS_BACK_DIM, GDK_INTERP_HYPER);

	loadPixmap(arrow_up_icon, &m_arrow_up_icon, "my-arrow-up-icon", &dummyw, &dummyh);
	loadPixmap(arrow_down_icon, &m_arrow_down_icon, "my-arrow-down-icon", &dummyw, &dummyh);
	loadPixmap(arrow_left_icon, &m_arrow_left_icon, "arrow-left-icon", &dummyw, &dummyh);
	loadPixmap(arrow_right_icon, &m_arrow_right_icon, "arrow-right-icon", &dummyw, &dummyh);

	loadPixmap(treble_clef_icon, &m_treble_clef_icon, "treble-clef-icon", &dummyw, &dummyh);
	loadPixmap(bass_clef_icon, &m_bass_clef_icon, "bass-clef-icon", &dummyw, &dummyh);
	loadPixmap(alto_clef_icon, &m_alto_clef_icon, "alto-clef-icon", &dummyw, &dummyh);

	stock_item_up.stock_id = (char *) "my-arrow-up-icon";
	stock_item_up.label = NULL;
	stock_item_up.modifier = (GdkModifierType) 0;
	stock_item_up.keyval = -1;
	stock_item_up.translation_domain = NULL;
	gtk_stock_add (&stock_item_up, 1);

	stock_item_down.stock_id = (char *) "my-arrow-down-icon";
	stock_item_down.label = NULL;
	stock_item_down.modifier = (GdkModifierType) 0;
	stock_item_down.keyval = -1;
	stock_item_down.translation_domain = NULL;
	gtk_stock_add (&stock_item_down, 1);

	stock_item_delete_staff.stock_id = (char *) "delete-staff";
	stock_item_delete_staff.label = (char *) "Delete Staff";
	stock_item_delete_staff.modifier = (GdkModifierType) 0;
	stock_item_delete_staff.keyval = -1;
	stock_item_delete_staff.translation_domain = NULL;
	gtk_stock_add (&stock_item_delete_staff, 1);

	stock_item_system_staff.stock_id = (char *) "delete-system";
	stock_item_system_staff.label = (char *) "Delete System";
	stock_item_system_staff.modifier = (GdkModifierType) 0;
	stock_item_system_staff.keyval = -1;
	stock_item_system_staff.translation_domain = NULL;
	gtk_stock_add (&stock_item_system_staff, 1);

	gtk_icon_factory_add_default(m_icon_factory);
	open_seq();
	m_print_cmd = "lpr %s";
	read_config();
	make_default_drums_current();
}

void NedResource::init_fonts(char *font_file_name) {
	int i;
	if (!loadfontface(font_file_name)) {
		char Str[128];
		sprintf(Str, "error in loadfontfile %s\n", font_file_name);
		Abort(Str);
	}
	for (i = 0; i < ZOOMLEVELS; i++) {
		m_zoom_factors[i] = MIN_ZOOM + (MAX_ZOOM - MIN_ZOOM) / 
				ZOOMLEVELS * i;
	}
	m_fontoptions= cairo_font_options_create();
	cairo_font_options_set_antialias(m_fontoptions, CAIRO_ANTIALIAS_SUBPIXEL);
	getScaledFont(START_ZOOM_LEVEL); // important, computes some "constants"
}

int NedResource::loadfontface(char *font_file_name) {
	FT_Library thelibrary;
	FT_Face theface;
	FT_Open_Args openargs;
	
	
	if (FT_Init_FreeType(&thelibrary) != 0) {
		DbgMsg(DBG_CRITICAL, "Error in FT_Init_FreeType\n");
		return 0;
	}
	
	openargs.flags = FT_OPEN_PATHNAME;
	openargs.pathname = font_file_name;
	if (FT_Open_Face( thelibrary, &openargs, 0, &theface) != 0) { 
		DbgMsg(DBG_CRITICAL, "Error in FT_Open_Face\n");
		return 0;
	}
	
	m_font_face = cairo_ft_font_face_create_for_ft_face(theface, 0);
	return 1;
}

#ifndef HAS_SET_SCALED_FONT
cairo_matrix_t *NedResource::getFontMatrix(int zoom_level) {
	if (zoom_level < 0 || zoom_level >= ZOOMLEVELS) {
		printf("zoom_level = %d\n", zoom_level); fflush(stdout);
		Abort("NedResource::getFontMatrix");
	}
	return &(m_matrixes[zoom_level]);
}
#endif

cairo_scaled_font_t *NedResource::createScaledFont(int zoom_level) {
	cairo_matrix_t the_font_matrix2;
#ifdef HAS_SET_SCALED_FONT
	cairo_matrix_t the_font_matrix;
#endif
	cairo_glyph_t glyph;
	cairo_scaled_font_t *scaled_font;
	int i;
	if (zoom_level < 0 || zoom_level >= ZOOMLEVELS) {
		Abort("NedResource::createScaledFont");
	}

	
#ifdef HAS_SET_SCALED_FONT
	cairo_matrix_init_scale (&the_font_matrix, FONT_FACTOR * m_zoom_factors[zoom_level],
		FONT_FACTOR * m_zoom_factors[zoom_level]);
#else
	cairo_matrix_init_scale (&(m_matrixes[zoom_level]), FONT_FACTOR * m_zoom_factors[zoom_level],
		FONT_FACTOR * m_zoom_factors[zoom_level]);
#endif
	cairo_matrix_init_identity (&the_font_matrix2);
	
	
	if (cairo_font_options_status(m_fontoptions) != CAIRO_STATUS_SUCCESS) {
		Abort("NedResource::getScaled: FontFehler in cairo_font_options_create");
	}
	
#ifdef HAS_SET_SCALED_FONT
	scaled_font = cairo_scaled_font_create(m_font_face, &the_font_matrix,&the_font_matrix2, m_fontoptions);
#else
	scaled_font = cairo_scaled_font_create(m_font_face, &(m_matrixes[zoom_level]),&the_font_matrix2, m_fontoptions);
#endif
	
	if (cairo_scaled_font_status(scaled_font) != CAIRO_STATUS_SUCCESS) {
		cairo_font_options_destroy(m_fontoptions);
		Abort("NedResource::getScaled: Fehler in cairo_scaled_font_create");
	}
	for (i = 0; i < MAXELEMENTS; i++) {
		glyph.index = BASE + i;
		glyph.y = 0;
		glyph.x = 0;
		cairo_scaled_font_glyph_extents (scaled_font, &glyph, 1, &(fontextentions[zoom_level][i]));
	}
	m_acc_bbox.x = fontextentions[zoom_level][0].x_bearing / m_zoom_factors[zoom_level];
	m_acc_bbox.y = fontextentions[zoom_level][0].y_bearing / m_zoom_factors[zoom_level];
	m_acc_bbox.width = fontextentions[zoom_level][0].width / m_zoom_factors[zoom_level];
	m_acc_bbox.height = fontextentions[zoom_level][0].height / m_zoom_factors[zoom_level];
	double xx = fontextentions[zoom_level][16].x_bearing / m_zoom_factors[zoom_level];
	double yy = fontextentions[zoom_level][16].y_bearing / m_zoom_factors[zoom_level];
	double w = fontextentions[zoom_level][16].width / m_zoom_factors[zoom_level];
	double h = fontextentions[zoom_level][16].height / m_zoom_factors[zoom_level];
	if (w > m_acc_bbox.width) m_acc_bbox.width = w;
	if (h > m_acc_bbox.height) m_acc_bbox.height = h;
	if (xx < m_acc_bbox.x) m_acc_bbox.x = xx;
	if (yy < m_acc_bbox.y) m_acc_bbox.y = yy;
	w = fontextentions[zoom_level][16].width / m_zoom_factors[zoom_level];
	h = fontextentions[zoom_level][16].height / m_zoom_factors[zoom_level];
	w = fontextentions[zoom_level][17].width / m_zoom_factors[zoom_level];
	h = fontextentions[zoom_level][17].height / m_zoom_factors[zoom_level];
	if (w > m_acc_bbox.width) m_acc_bbox.width = w;
	if (h > m_acc_bbox.height) m_acc_bbox.height = h;
	if (xx < m_acc_bbox.x) m_acc_bbox.x = xx;
	if (yy < m_acc_bbox.y) m_acc_bbox.y = yy;
	m_flag_width = (fontextentions[zoom_level][5].x_bearing + fontextentions[zoom_level][5].width) / m_zoom_factors[zoom_level];
	return scaled_font;
}

void NedResource::reset_tools() {
	if (m_tools != NULL) {
		delete m_tools;
	}
	m_tools = NULL;
}

int NedResource::getNumInstruments() {
	return (sizeof(GM_Instruments) / sizeof(char *));
}

double NedResource::getZoomFactor(int zoom_level) {
	if (zoom_level < 0 || zoom_level >= ZOOMLEVELS) {
		Abort("NedResource::getZoomLevel");
	}
	return m_zoom_factors[zoom_level];
}

void NedResource::open_seq() {
	snd_seq_client_info_t *cinfo;

	if (snd_seq_open(&m_sequ_out, "default", SND_SEQ_OPEN_DUPLEX, 0) < 0) {
		Info(_("Cannot open ALSA MIDI sequencer --> replay won't work :-((\nTry (as root)\nmodprobe snd-seq-midi\nand restart NtEd\nPerhaps this helps(?)"));
		m_sequ_out = NULL;
		return;
	}
	snd_seq_set_client_name(m_sequ_out, "Nted");
	if ((m_alsa_out_addr.port = snd_seq_create_simple_port(m_sequ_out, "Nted output",
		SND_SEQ_PORT_CAP_READ|SND_SEQ_PORT_CAP_SUBS_READ,
			SND_SEQ_PORT_TYPE_APPLICATION)) != 0) {
		Info(_("Error creating ALSA sequencer output port --> replay won't work :-(("));
		snd_seq_close(m_sequ_out);
		return;
	}
	snd_seq_client_info_alloca(&cinfo);
	snd_seq_get_client_info(m_sequ_out, cinfo);
	m_alsa_out_addr.client = snd_seq_client_info_get_client(cinfo);
	m_alsa_out_dest_addr.client = 0;

	if (snd_seq_open(&m_sequ_in, "default", SND_SEQ_OPEN_DUPLEX, 0) < 0) {
		Info(_("Cannot open ALSA MIDI sequencer --> read/record won't work :-((\nTry (as root)\nmodprobe snd-seq-midi\nand restart NtEd\nPerhaps this helps(?)"));
		m_sequ_in = NULL;
		return;
	}
	snd_seq_set_client_name(m_sequ_in, "Nted");
	if ((m_alsa_in_addr.port = snd_seq_create_simple_port(m_sequ_in, "Nted input",
		SND_SEQ_PORT_CAP_WRITE|SND_SEQ_PORT_CAP_SUBS_WRITE,
			SND_SEQ_PORT_TYPE_APPLICATION)) != 0) {
		Info(_("Error creating ALSA sequencer input port --> record won't work :-(("));
		snd_seq_close(m_sequ_in);
		return;
	}
	snd_seq_client_info_alloca(&cinfo);
	snd_seq_get_client_info(m_sequ_in, cinfo);
	m_alsa_in_addr.client = snd_seq_client_info_get_client(cinfo);
	m_alsa_in_src_addr.client = 0;
}

void NedResource::close_sequ() {
	if (m_sequ_out != NULL) {
		snd_seq_close(m_sequ_out);
	}
	m_sequ_out = NULL;
	if (m_sequ_in != NULL) {
		snd_seq_close(m_sequ_in);
	}
	m_sequ_in = NULL;
}

#define MIDI_IN_POLL_TIME 30

void NedResource::startMidiInput()  {
	int chn, pgm, i;
	snd_seq_event_t ev;

	if (m_sequ_in == NULL) {
		Info("Cannot open MIDI input device");
		return;
	}
	for (i = 0; i < m_window_with_last_focus->getStaffCount(); i++) {
		pgm = m_window_with_last_focus->m_staff_contexts[i].voices[0].m_midi_program;
		chn = m_window_with_last_focus->m_staff_contexts[i].m_midi_channel;
		snd_seq_ev_clear(&ev);
		ev.type = SND_SEQ_EVENT_PGMCHANGE;
		ev.data.control.channel = chn;
		ev.data.control.value = pgm;
		snd_seq_ev_set_subs(&ev);  
		snd_seq_ev_set_direct(&ev);
		snd_seq_event_output_direct(m_sequ_out, &ev);
	}
	m_number_of_midi_poll_fds = snd_seq_poll_descriptors_count(m_sequ_in, POLLIN);
	m_run_midi_in = true;
	g_timeout_add(MIDI_IN_POLL_TIME, do_poll_midi_device, NULL);
}

gboolean NedResource::do_poll_midi_device(void *data)  {
	struct pollfd *pfd = (struct pollfd *)alloca(m_number_of_midi_poll_fds * sizeof(struct pollfd));
	if (m_run_midi_in) {
		snd_seq_poll_descriptors(m_sequ_in, pfd, m_number_of_midi_poll_fds, POLLIN);
		if (poll(pfd, m_number_of_midi_poll_fds, 0) > 0) {
			process_midi_event();
		}
		g_timeout_add(MIDI_IN_POLL_TIME, do_poll_midi_device, NULL);
		return FALSE;
	}
	return FALSE;
}


void NedResource::process_midi_event() {
	snd_seq_event_t *ev;
	int num_midi_input_notes = 0, midi_input_chord[MAX_MIDI_INPUT_CHORD];

	do {
		snd_seq_event_input(m_sequ_in, &ev);
		switch (ev->type) {
#ifdef UUU
			case SND_SEQ_EVENT_CONTROLLER: 
				fprintf(stderr, "Control event on Channel %2d: %5d       \n",
				ev->data.control.channel, ev->data.control.value);
				break;
			case SND_SEQ_EVENT_PITCHBEND:
				fprintf(stderr, "Pitchbender event on Channel %2d: %5d   \n", 
				ev->data.control.channel, ev->data.control.value);
				break;
#endif
			case SND_SEQ_EVENT_NOTEON:
#ifdef UUU
				fprintf(stderr, "Note On event on Channel %2d: %5d       \n",
				ev->data.control.channel, ev->data.note.note);
#endif 
				if (ev->data.note.velocity > 0 && num_midi_input_notes < MAX_MIDI_INPUT_CHORD) {
					midi_input_chord[num_midi_input_notes++] = ev->data.note.note;
				}
				if (m_sequ_out) {
					ev->data.control.channel = m_window_with_last_focus->setChannelFromCursor();
					snd_seq_ev_set_subs(ev);  
					snd_seq_ev_set_direct(ev);
					snd_seq_event_output_direct(m_sequ_out, ev);
				}
				break;        
			case SND_SEQ_EVENT_NOTEOFF: 
#ifdef UUU
				fprintf(stderr, "Note Off event on Channel %2d: %5d      \n",         
				ev->data.control.channel, ev->data.note.note);           
#endif
				if (m_sequ_out) {
					ev->data.control.channel = m_window_with_last_focus->setChannelFromCursor();
					snd_seq_ev_set_subs(ev);  
					snd_seq_ev_set_direct(ev);
					snd_seq_event_output_direct(m_sequ_out, ev);
				}
				break;        
		}
		snd_seq_free_event(ev);
	}
	while (snd_seq_event_input_pending(m_sequ_in, 0) > 0);
	m_window_with_last_focus->insertNoteWithPitch(num_midi_input_notes, midi_input_chord);
}

void NedResource::stopMidiInput()  {
	m_run_midi_in = false;
}

void NedResource::popup_tools(NedMainWindow *w) {
	if (m_tools == NULL) {
		m_tools = new NedTools ();
	}
	m_tools->Show();
	m_tools->setToolboxTransient(w);
}

unsigned int NedResource::getCurrentLength() {
	if (m_tools == NULL) {
		return NOTE_4;
	}
	return m_tools->getCurrentLength();
}

int NedResource::getCurrentNoteHead() {
	if (m_tools == NULL) {
		return NORMAL_NOTE;
	}
	return m_tools->getCurrentNoteHead();
}

unsigned int NedResource::getStatus() {
	if (m_tools == NULL) {
		return 0;
	}
	return m_tools->getStatus();
}

bool NedResource::getRestMode() {
	if (m_tools == NULL) {
		return false;
	}
	return m_tools->getRestMode();
}

int NedResource::getDotCount() {
	if (m_tools == NULL) {
		return 0;
	}
	return m_tools->getDotCount();
}

bool NedResource::getTiedMode() {
	if (m_tools == NULL) {
		return false;
	}
	return m_tools->getTiedMode();
}

void NedResource::setButtons(unsigned int length, int dotcount, unsigned int chord_status, unsigned int note_status, bool isRest, bool tie_forward, int note_head) {
	if (m_tools == NULL) {
		return;
	}
	m_tools->setButtons(length, dotcount, chord_status, note_status, isRest, tie_forward, note_head);
}

void NedResource::setNoteLengthButtons(unsigned int length) {
	if (m_tools == NULL) {
		return;
	}
	m_tools->setNoteLengthButtons(length);
}

void NedResource::setToolboxTransient(NedMainWindow *w) {
	if (m_tools == NULL) {
		return;
	}
	m_tools->setToolboxTransient(w);
}
void NedResource::resetSomeButtons() {
	if (m_tools == NULL) {
		return;
	}
	m_tools->resetSomeButtons();
}


void NedResource::resetButtons() {
	if (m_tools == NULL) {
		return;
	}
	m_tools->resetButtons();
}

void NedResource::resetTieButton() {
	if (m_tools == NULL) {
		return;
	}
	m_tools->resetTieButton();
}

void NedResource::toggleButton(int nr) {
	if (m_tools == NULL) {
		return;
	}
	m_tools->toggleButton(nr);
}

void NedResource::toggleRest() {
	if (m_tools == NULL) {
		return;
	}
	m_tools->toggleRest();
}

void NedResource::toggleTie() {
	if (m_tools == NULL) {
		return;
	}
	m_tools->toggleTie();
}

void NedResource::toggleDot() {
	if (m_tools == NULL) {
		return;
	}
	m_tools->toggleDot();
}

void NedResource::toggleSharp() {
	if (m_tools == NULL) {
		return;
	}
	m_tools->toggleSharp();
}

void NedResource::toggleNatural() {
	if (m_tools == NULL) {
		return;
	}
	m_tools->toggleNatural();
}

void NedResource::toggleFlat() {
	if (m_tools == NULL) {
		return;
	}
	m_tools->toggleFlat();
}

NedMainWindow *NedResource::getMainwWindowWithLastFocus() {
	return m_window_with_last_focus;
}

void NedResource::setMainwWindowWithFocus(NedMainWindow *w) {
	if (m_playing) return;
	//GList *lptr;

	/*
	if ((lptr = g_list_find(m_main_windows, w)) == NULL) {
			Abort("NedResource::setMainwWindowWithFocus");
		}
	}
	*/
	if (m_window_with_last_focus != w) {
		if (m_window_with_last_focus) {
			m_window_with_last_focus->resetMidiInput();
		}
		m_window_with_last_focus = w;
		if (m_tools != NULL) {
			m_tools->setToolboxTransient(w);
		}
	}
}

void NedResource::MidiCtrl(unsigned int ctrl_param, int channel, int val) {
	snd_seq_event_t ev;

	snd_seq_ev_clear(&ev);
	
	ev.type = SND_SEQ_EVENT_CONTROLLER;
	ev.data.control.channel = channel;
	ev.data.control.param = ctrl_param;
	ev.data.control.value = val;
	snd_seq_ev_set_subs(&ev);  
	snd_seq_ev_set_direct(&ev);
	snd_seq_event_output_direct(m_sequ_out, &ev);
}

void NedResource::NoteOn(int channel, int pitch, int pgm, int vol) {
	snd_seq_event_t ev;

	if (m_channel_pgms[channel] != pgm) {
		snd_seq_ev_clear(&ev);
		ev.type = SND_SEQ_EVENT_PGMCHANGE;
		ev.data.control.channel = channel;
		ev.data.control.value = pgm;
		snd_seq_ev_set_subs(&ev);  
		snd_seq_ev_set_direct(&ev);
		snd_seq_event_output_direct(m_sequ_out, &ev);
		m_channel_pgms[channel] = pgm;
	}

	snd_seq_ev_clear(&ev);
	ev.type = SND_SEQ_EVENT_NOTEON;
	ev.data.note.channel = channel;
	ev.data.note.velocity = vol;
	ev.data.note.note = pitch;
	snd_seq_ev_set_subs(&ev);  
	snd_seq_ev_set_direct(&ev);
	snd_seq_event_output_direct(m_sequ_out, &ev);
}

void NedResource::NoteOff(int channel, int pitch) {
	snd_seq_event_t ev;
	snd_seq_ev_clear(&ev);

	ev.type = SND_SEQ_EVENT_NOTEOFF;
	ev.data.note.channel = channel;
	ev.data.note.velocity = 0;
	ev.data.note.note = pitch;
	snd_seq_ev_set_subs(&ev);  
	snd_seq_ev_set_direct(&ev);
	snd_seq_event_output_direct(m_sequ_out, &ev);
}

void NedResource::SeqReset() {
	snd_seq_event_t ev;

	if (m_sequ_out == NULL) return;
	snd_seq_ev_clear(&ev);

	for (int i = 0; i < 16; i++) {
		MidiCtrl(MIDI_CTL_SUSTAIN, i, 1);
	}

	ev.type = SND_SEQ_EVENT_RESET;
	ev.data.note.channel = 0;
	ev.data.note.velocity = 0;
	ev.data.note.note = 0;
	snd_seq_ev_set_subs(&ev);  
	snd_seq_ev_set_direct(&ev);
	snd_seq_event_output_direct(m_sequ_out, &ev);
}


void NedResource::cleanup() {
	GList *lptr;
	write_config();
	snd_seq_port_subscribe_t *subs;
	if (m_sequ_out != NULL && m_alsa_out_dest_addr.client != 0) {
		snd_seq_port_subscribe_alloca(&subs);
		snd_seq_port_subscribe_set_sender(subs, &m_alsa_out_addr);
		snd_seq_port_subscribe_set_dest(subs, &m_alsa_out_dest_addr);
		snd_seq_unsubscribe_port(m_sequ_out, subs);
		m_alsa_out_dest_addr.client = 0;
	}
	if (m_sequ_in != NULL && m_alsa_in_src_addr.client != 0) {
		snd_seq_port_subscribe_alloca(&subs);
		snd_seq_port_subscribe_set_sender(subs, &m_alsa_in_addr);
		snd_seq_port_subscribe_set_dest(subs, &m_alsa_in_src_addr);
		snd_seq_unsubscribe_port(m_sequ_in, subs);
		m_alsa_in_src_addr.client = 0;
	}
	close_sequ();
	for (lptr = g_list_first(m_temp_file_names); lptr; lptr = g_list_next(lptr)) {
		unlink((char *) (lptr->data));
	}
}

void NedResource::write_config() {
	char fname[4096], *homedir;
	FILE *fp;
	int i;

	if ((homedir = getenv("HOME")) == NULL) {
		Warning("cannot write config: HOME not found");
		return;
	}
	strcpy(fname, homedir);
	strcat(fname, "/");
	strcat(fname, CONFIG_DIR);

	if (access(fname, R_OK | W_OK | X_OK) != 0) {
		if (access(fname, F_OK) == 0) {
			Warning("cannot write config:  cannot access dir(1)");
			return;
		}
		if (mkdir(fname, 0755) < 0) {
			Warning("cannot write config: mkdir");
			return;
		}
		if (access(fname, R_OK | W_OK | X_OK) != 0) {
			Warning("cannot write config:  cannot access dir(2)");
			return;
		}
	}
	strcat(fname, "/");
	strcat(fname, CONFIG_FILE);
	if ((fp = fopen(fname, "w")) == NULL) {
		Warning("cannot write config:  open");
		return;
	}
	if (m_tools != NULL) {
		m_tools->updatePosition();
	}
	if (m_alsa_out_dest_addr.client != 0) {
		fprintf(fp, "ALSACLIENT = %d\n", m_alsa_out_dest_addr.client);
		fprintf(fp, "ALSAPORT = %d\n", m_alsa_out_dest_addr.port);
	}
	if (m_alsa_in_src_addr.client != 0) {
		fprintf(fp, "ALSACLIENT_IN = %d\n", m_alsa_in_src_addr.client);
		fprintf(fp, "ALSAPORT_IN = %d\n", m_alsa_in_src_addr.port);
	}
	if (m_last_folder != NULL) {
		fprintf(fp, "LASTFOLDER = %s\n", m_last_folder);
	}
	if (m_last_xml_dir  != NULL) {
		fprintf(fp, "LASTXMLDIR = %s\n", m_last_xml_dir );
	}
	if (m_last_midi_dir != NULL) {
		fprintf(fp, "LASTMIDIDIR = %s\n", m_last_midi_dir );
	}
	fprintf(fp, "PRINTCMD = %s\n", m_print_cmd);
	fprintf(fp, "TOOLBOXAT %d %d\n", m_toolboxX, m_toolboxY);
	gtk_window_get_size (GTK_WINDOW(m_window_with_last_focus->getWindow()), &m_width, &m_height);
	fprintf(fp, "WIDTH = %d\n", m_width?m_width:WIDTH);
	fprintf(fp, "HEIGHT = %d\n", m_height?m_height:HEIGHT);
	for (i = 0; i < MAX_RECENT_FILES; i++) {
		if (m_recent_files[i] != NULL) {
			fprintf(fp, "RECENTFILE %d = %s\n", i, m_recent_files[i]);
		}
	}
	fclose(fp);
}

void NedResource::addToRecentFiles(char *fname) {
	int i;

	for (i = 0; i < MAX_RECENT_FILES; i++) {
		if (m_recent_files[i] != NULL) {
			if (!strcmp(m_recent_files[i], fname)) {
				return;
			}
		}
	}
	if (m_recent_files[MAX_RECENT_FILES-1] != NULL) {
		free(m_recent_files[MAX_RECENT_FILES-1]);
	}

	for (i = MAX_RECENT_FILES-1; i > 0; i--) {
		m_recent_files[i] = m_recent_files[i-1];
	}
	m_recent_files[0] = strdup(fname);
}

int NedResource::getClefDist(int oldclef, int old_oct_shift, int newclef, int new_oct_shift) {
	int oldbase, newbase;

	switch (oldclef) {
		case NEUTRAL_CLEF1:
		case NEUTRAL_CLEF3:
		case TREBLE_CLEF: oldbase = 0; break;
		case NEUTRAL_CLEF2:
		case BASS_CLEF: oldbase = 12; break;
		case ALTO_CLEF: oldbase = 6; break;
		case SOPRAN_CLEF: oldbase = 2; break;
		case TENOR_CLEF: oldbase = 8; break;
		default: oldbase = newbase = 0; Abort("NedResource::getClefDist(1)"); break;
	}
	switch(old_oct_shift) {
		case  12: oldbase -= 7; break;
		case   0: break;
		case -12: oldbase += 7; break;
		default:  Abort("NedResource::getClefDist(2)");
	}
	switch (newclef) {
		case NEUTRAL_CLEF1:
		case NEUTRAL_CLEF3:
		case TREBLE_CLEF: newbase = 0; break;
		case NEUTRAL_CLEF2:
		case BASS_CLEF: newbase = 12; break;
		case ALTO_CLEF: newbase = 6; break;
		case SOPRAN_CLEF: newbase = 2; break;
		case TENOR_CLEF: newbase = 8; break;
		default: Abort("NedResource::getClefDist(3)");
	}
	switch(new_oct_shift) {
		case  12: newbase -= 7; break;
		case   0: break;
		case -12: newbase += 7; break;
		default:  Abort("NedResource::getClefDist(4)");
	}

	return newbase - oldbase;
}


void NedResource::read_config() {
	char fname[4096], *homedir;
	char buffer[4096];
	char Str[128];
	int client = 0, port;
	int client_in = 0, port_in;
	int idx;
	bool client_set = FALSE, port_set = FALSE;
	bool client_in_set = FALSE, port_in_set = FALSE;
	FILE *fp;
	snd_seq_port_subscribe_t *subs;

	if ((homedir = getenv("HOME")) == NULL) {
		Warning("cannot read config: HOME not found");
		return;
	}
	strcpy(fname, homedir);
	strcat(fname, "/");
	strcat(fname, CONFIG_DIR);
	strcat(fname, "/");
	strcat(fname, CONFIG_FILE);
	if ((fp = fopen(fname, "r")) == NULL) {
		DbgMsg(DBG_CRITICAL, "cannot read config. Ignore this if this is the first call!\n");
		return;
	}
	m_input_line = 1;
	while(readWord(fp, buffer)) {
		if (!strcmp(buffer, "ALSACLIENT")) {
			if (!readWord(fp, buffer)) {
				sprintf(Str, "cannot read config: line %d: '=' expected", m_input_line);
				Warning(Str);
				fclose(fp);
				return;
			}
			if (!readInt(fp, &client)) {
				sprintf(Str, "cannot read config: line %d: client number expected", m_input_line);
				Warning(Str);
				fclose(fp);
				return;
			}
			client_set = TRUE;
		}
		else if (!strcmp(buffer, "ALSAPORT")) {
			if (!readWord(fp, buffer)) {
				sprintf(Str, "cannot read config: line %d: '=' expected", m_input_line);
				Warning(Str);
				fclose(fp);
				return;
			}
			if (!readInt(fp, &port)) {
				sprintf(Str, "cannot read config: line %d: port number expected", m_input_line);
				Warning(Str);
				fclose(fp);
				return;
			}
			port_set = TRUE;
		}
		else if (!strcmp(buffer, "ALSACLIENT_IN")) {
			if (!readWord(fp, buffer)) {
				sprintf(Str, "cannot read config: line %d: '=' expected", m_input_line);
				Warning(Str);
				fclose(fp);
				return;
			}
			if (!readInt(fp, &client_in)) {
				sprintf(Str, "cannot read config: line %d: client number expected", m_input_line);
				Warning(Str);
				fclose(fp);
				return;
			}
			client_in_set = TRUE;
		}
		else if (!strcmp(buffer, "ALSAPORT_IN")) {
			if (!readWord(fp, buffer)) {
				sprintf(Str, "cannot read config: line %d: '=' expected", m_input_line);
				Warning(Str);
				fclose(fp);
				return;
			}
			if (!readInt(fp, &port_in)) {
				sprintf(Str, "cannot read config: line %d: port number expected", m_input_line);
				Warning(Str);
				fclose(fp);
				return;
			}
			port_in_set = TRUE;
		}
		else if (!strcmp(buffer, "PRINTCMD")) {
			if (!readWord(fp, buffer)) {
				sprintf(Str, "cannot read config: line %d: '=' expected", m_input_line);
				Warning(Str);
				fclose(fp);
				return;
			}
			if (!readTillEnd(fp, buffer)) {
				sprintf(Str, "cannot read config: line %d: print command expected", m_input_line);
				Warning(Str);
				fclose(fp);
				return;
			}
			m_print_cmd = strdup(buffer);
		}
		else if (!strcmp(buffer, "WIDTH")) {
			if (!readWord(fp, buffer)) {
				sprintf(Str, "cannot read config: line %d: '=' expected", m_input_line);
				Warning(Str);
				fclose(fp);
				return;
			}
			if (!readInt(fp, &m_width)) {
				sprintf(Str, "cannot read config: line %d: width size expected", m_input_line);
				Warning(Str);
				fclose(fp);
				m_width = WIDTH;
				return;
			}
		}
		else if (!strcmp(buffer, "HEIGHT")) {
			if (!readWord(fp, buffer)) {
				sprintf(Str, "cannot read config: line %d: '=' expected", m_input_line);
				Warning(Str);
				fclose(fp);
				return;
			}
			if (!readInt(fp, &m_height)) {
				sprintf(Str, "cannot read config: line %d: height size expected", m_input_line);
				Warning(Str);
				fclose(fp);
                m_height = HEIGHT;
				return;
			}
		}
		else if (!strcmp(buffer, "LASTFOLDER")) {
			if (!readWord(fp, buffer)) {
				sprintf(Str, "cannot read config: line %d: '=' expected", m_input_line);
				Warning(Str);
				fclose(fp);
				return;
			}
			if (!readTillEnd(fp, buffer)) {
				sprintf(Str, "cannot read config: line %d: last folder expected", m_input_line);
				Warning(Str);
				fclose(fp);
				return;
			}
			m_last_folder = strdup(buffer);
		}
		else if (!strcmp(buffer, "LASTXMLDIR")) {
			if (!readWord(fp, buffer)) {
				sprintf(Str, "cannot read config: line %d: '=' expected", m_input_line);
				Warning(Str);
				fclose(fp);
				return;
			}
			if (!readTillEnd(fp, buffer)) {
				sprintf(Str, "cannot read config: line %d: last xml file expected", m_input_line);
				Warning(Str);
				fclose(fp);
				return;
			}
			m_last_xml_dir = strdup(buffer);
		}
		else if (!strcmp(buffer, "LASTMIDIDIR")) {
			if (!readWord(fp, buffer)) {
				sprintf(Str, "cannot read config: line %d: '=' expected", m_input_line);
				Warning(Str);
				fclose(fp);
				return;
			}
			if (!readTillEnd(fp, buffer)) {
				sprintf(Str, "cannot read config: line %d: last midi file expected", m_input_line);
				Warning(Str);
				fclose(fp);
				return;
			}
			m_last_midi_dir = strdup(buffer);
		}
		else if (!strcmp(buffer, "RECENTFILE")) {
			if (!readInt(fp, &idx) || idx < 0 || idx >= MAX_RECENT_FILES) {
				sprintf(Str, "cannot read config: line %d: index of recent file expected", m_input_line);
				Warning(Str);
				fclose(fp);
				return;
			}
			if (!readWord(fp, buffer)) {
				sprintf(Str, "cannot read config: line %d: '=' expected", m_input_line);
				Warning(Str);
				fclose(fp);
				return;
			}
			if (!readTillEnd(fp, buffer)) {
				sprintf(Str, "cannot read config: line %d: file name expected", m_input_line);
				Warning(Str);
				fclose(fp);
				return;
			}
			m_recent_files[idx] = strdup(buffer);
		}
		else if (!strcmp(buffer, "TOOLBOXAT")) {
			if (!readInt(fp, &m_toolboxX) || m_toolboxX < -MAX_SCREEN_DIM || m_toolboxX > MAX_SCREEN_DIM) {
				sprintf(Str, "cannot read config: line %d: m_toolboxX expected", m_input_line);
				Warning(Str);
				fclose(fp);
				return;
			}
			if (!readInt(fp, &m_toolboxY) || m_toolboxY < -MAX_SCREEN_DIM || m_toolboxY > MAX_SCREEN_DIM) {
				sprintf(Str, "cannot read config: line %d: m_toolboxY expected", m_input_line);
				Warning(Str);
				fclose(fp);
				return;
			}
		}
	}
	fclose(fp);

	if (m_sequ_out != NULL && client_set && port_set) {
		m_alsa_out_dest_addr.client = client;
		m_alsa_out_dest_addr.port = port;
		snd_seq_port_subscribe_alloca(&subs);
		snd_seq_port_subscribe_set_sender(subs, &m_alsa_out_addr);
		snd_seq_port_subscribe_set_dest(subs, &m_alsa_out_dest_addr);
		snd_seq_subscribe_port(m_sequ_out, subs);
	}
	if (m_sequ_in != NULL && client_in_set && port_in_set) {
		m_alsa_in_src_addr.client = client_in;
		m_alsa_in_src_addr.port = port_in;
		snd_seq_port_subscribe_alloca(&subs);
		snd_seq_port_subscribe_set_sender(subs, &m_alsa_in_src_addr);
		snd_seq_port_subscribe_set_dest(subs, &m_alsa_in_addr);
		snd_seq_subscribe_port(m_sequ_in, subs);
	}

}
	

cairo_scaled_font_t *NedResource::getScaledFont(int zoom_level) {
	if (zoom_level < 0 || zoom_level >= ZOOMLEVELS) {
		Abort("NedResource::getScaledFont");
	}
	if (m_scaled_fonts[zoom_level] == NULL) {
		m_scaled_fonts[zoom_level] = createScaledFont(zoom_level);
	}
	return m_scaled_fonts[zoom_level];
}

bool NedResource::equalContent(GList *l1, GList *l2, NedChordOrRest *except, int *diff_pos) {
	GList *lptr2;
	int lendiff;

	lendiff = g_list_length(l2) - g_list_length(l1);
	if (lendiff != 0) {
		if (except == NULL) {
			DbgMsg(DBG_TESTING, "FAIL A\n"); 
			return FALSE;
		}
		if (lendiff != 1) {
			DbgMsg(DBG_TESTING, "FAIL B\n"); 
			return FALSE;
		}
	}
	
	*diff_pos = -1;
	for (lptr2 = g_list_first(l2); lptr2; lptr2 = g_list_next(lptr2)) {
		if (g_list_find(l1, lptr2->data) == NULL) {
			if (except != NULL || *diff_pos >= 0) {
				DbgMsg(DBG_TESTING, "FAIL C\n"); 
				return FALSE;
			}
			if (lptr2->data != except) {
				DbgMsg(DBG_TESTING, "FAIL D\n"); 
				return FALSE;
			}
			*diff_pos =  g_list_position(l2, lptr2);
		}
	}
	DbgMsg(DBG_TESTING, "IDENT\n"); 
	return TRUE;
}
            
void NedResource::Abort(const char *s) {
	GtkWidget *error_dialog;
	printf("%s\n", s); fflush(stdout);
	error_dialog = gtk_message_dialog_new (NULL, 
		(GtkDialogFlags) 0,
		GTK_MESSAGE_ERROR,
		GTK_BUTTONS_OK,
		"Internal error detected: %s. Excuse the program must be terminated :-((( Please contact " PACKAGE_BUGREPORT, s);
	gtk_dialog_run (GTK_DIALOG (error_dialog));
	gtk_widget_destroy (error_dialog);	
	exit(10);
}

#define CURRENT_DEBUG_LEVEL DBG_CRITICAL
//#define CURRENT_DEBUG_LEVEL DBG_TESTING
void NedResource::DbgMsg(int level, const char *format, ...) {
	va_list ap;
	if (level < CURRENT_DEBUG_LEVEL) return;
	va_start (ap, format);
	vprintf(format, ap);
	va_end(ap);
}

void NedResource::Warning(const char *s) {
	GtkWidget *error_dialog;
	GList *lptr;

	//the mainwindow wouldn't receive the ctrl-release-event..
	for (lptr = g_list_first(m_main_windows); lptr; lptr = g_list_next(lptr)) {
		((NedMainWindow *) lptr->data)->turnOffKeyboardCtrlMode();
	}

	error_dialog = gtk_message_dialog_new (NULL, 
		(GtkDialogFlags) 0,
		GTK_MESSAGE_WARNING,
		GTK_BUTTONS_OK,
		"Internal problem detected: %s. Please contact " PACKAGE_BUGREPORT, s);
	gtk_dialog_run (GTK_DIALOG (error_dialog));
	gtk_widget_destroy (error_dialog);	
	fprintf(stderr, "Warning: %s\n", s);
}

void NedResource::Info(const char *s) {
	GtkWidget *error_dialog;
	GList *lptr;

	//the mainwindow wouldn't receive the ctrl-release-event..
	for (lptr = g_list_first(m_main_windows); lptr; lptr = g_list_next(lptr)) {
		((NedMainWindow *) lptr->data)->turnOffKeyboardCtrlMode();
	}


	error_dialog = gtk_message_dialog_new (NULL, 
		(GtkDialogFlags) 0,
		GTK_MESSAGE_INFO,
		GTK_BUTTONS_OK,
		"%s", s);
	gtk_dialog_run (GTK_DIALOG (error_dialog));
	gtk_widget_destroy (error_dialog);	
}

void NedResource::loadPixmap(const guint8 *data, GdkPixbuf **pixbuf, const char *stock_id, int *width, int *height) {
	GtkIconSet* icon_set;
        GError *err = NULL;
        *pixbuf = gdk_pixbuf_new_from_inline(-1, data, false, &err);
        if (err != NULL) {
                DbgMsg(DBG_CRITICAL, "NedResource::loadPixmap: Fehler %d beim Laden einer Pixmap: %s\n", err->code, err->message);
                exit(1);
        }
        *width = gdk_pixbuf_get_width(*pixbuf);
        *height = gdk_pixbuf_get_height(*pixbuf);
	if (stock_id != NULL) {
		icon_set = gtk_icon_set_new_from_pixbuf(*pixbuf);
		gtk_icon_factory_add(m_icon_factory, stock_id, icon_set);
	}

}


void NedResource::unreadWord(char *word) {
	strcpy(m_word_buffer, word);
}

void NedResource::my_ungetc(char c, FILE *fp) {
	if (c == '\n') m_input_line--;
	ungetc(c, fp);
}

char NedResource::my_getc(FILE *fp) {
	char c;
	if ((c = fgetc(fp)) == '\n') m_input_line++;
	return c;
}


bool NedResource::readWordWithNum(FILE *fp, char *word) {
	int pos = 0;

	if (m_word_buffer[0] != '\0') {
		strcpy(word, m_word_buffer);
		m_word_buffer[0] = '\0';
		return true;
	}

	while (((word[0] = my_getc(fp)) != EOF) && (word[0] == ' ' || word[0] == '\t' || word[0] == '\n'));
	if (feof(fp)) return FALSE;
	if (!((word[0] >= 'A' && word[0] <= 'Z') || (word[0] >= 'a' && word[0] <= 'z'))) {
		word[1] = '\0';
#ifdef INPUT_DEBUG
		printf("WORD: %s\n", word);
#endif
		return TRUE;
	}
	pos++;
	while ((word[pos] = my_getc(fp)) != EOF &&
		 (((word[pos] >= 'A' && word[pos] <= 'Z') ||
		 (word[pos] >= 'a' && word[pos] <= 'z') || word[pos] == '_') || (word[pos] >= '1' && word[pos] <= '9'))) {
		pos++;
	}
	if (!feof(fp)) {
		my_ungetc(word[pos], fp);
	}
	word[pos] = '\0';
#ifdef INPUT_DEBUG
	printf("WORD: %s\n", word);
#endif
	return TRUE;
}

bool NedResource::readWordWithAllChars(FILE *fp, char *word) {
	int pos = 0;

	while (((word[0] = my_getc(fp)) != EOF) && word[0]  <= ' ');
	if (feof(fp)) return FALSE;
	pos++;
	while ((word[pos] = my_getc(fp)) != EOF && (word[pos] & 0xff) > ' ') {
		pos++;
	}
	word[pos] = '\0';
#ifdef INPUT_DEBUG
	printf("WORD: %s\n", word);
#endif
	return TRUE;
}

bool NedResource::readWordOfLength(FILE *fp, char *word) {
	int pos;
	char c;
	int len;
	bool not_eof;

	if (m_word_buffer[0] != '\0') {
		strcpy(word, m_word_buffer);
		m_word_buffer[0] = '\0';
		return true;
	}

	if (!readInt(fp, &len) || len < 0 || len > 255) {
		return false;
	}

	pos = -1;
	while (pos < len && (not_eof = ((c = my_getc(fp)) != EOF))) {
		if (pos >= 0) {
			word[pos] = c;
		}
		pos++;
	}
	word[pos] = '\0';
#ifdef INPUT_DEBUG
	printf("WORD: %s\n", word);
#endif
	return TRUE;
}

bool NedResource::readWord(FILE *fp, char *word) {
	int pos = 0;

	if (m_word_buffer[0] != '\0') {
		strcpy(word, m_word_buffer);
		m_word_buffer[0] = '\0';
		return true;
	}

	while (((word[0] = my_getc(fp)) != EOF) && (word[0] == ' ' || word[0] == '\t' || word[0] == '\n'));
	if (feof(fp)) return FALSE;
	if (!((word[0] >= 'A' && word[0] <= 'Z') || (word[0] >= 'a' && word[0] <= 'z'))) {
		word[1] = '\0';
#ifdef INPUT_DEBUG
		printf("WORD: %s\n", word);
#endif
		return TRUE;
	}
	pos++;
	while ((word[pos] = my_getc(fp)) != EOF &&
		 ((word[pos] >= 'A' && word[pos] <= 'Z') ||
		 (word[pos] >= 'a' && word[pos] <= 'z') || word[pos] == '_')) {
		pos++;
	}
	if (!feof(fp)) {
		my_ungetc(word[pos], fp);
	}
	word[pos] = '\0';
#ifdef INPUT_DEBUG
	printf("WORD: %s\n", word);
#endif
	return TRUE;
}

bool NedResource::readTillEnd(FILE *fp, char *word) {
	int pos = 0;

	while (((word[0] = my_getc(fp)) != EOF) && (word[0] == ' ' || word[0] == '\t' || word[0] == '\n' || word[0] == '\r'));
	if (feof(fp)) return FALSE;
	pos++;
	while ((word[pos] = my_getc(fp)) != EOF &&
		 word[pos] != '\n') {
		pos++;
	}
	word[pos] = '\0';
#ifdef INPUT_DEBUG
	printf("WORD: %s\n", word);
#endif
	return TRUE;
}

void NedResource::removeNonAsciiFromUTF8String(const gchar *s) {
	gchar *cptr1, *cptr2;
	gchar *cptr = (gchar *) s;
	if (*cptr == '\0') return;
	do {
		cptr2 = g_utf8_next_char(cptr);
		if (cptr2 - cptr > 1) {
			cptr1 = cptr;
			cptr2 = g_utf8_next_char (cptr1);
			while (*cptr2 != '\0') {
				*cptr1++ = *cptr2++;
			}
			*cptr1 = '\0';
		}
		if (*cptr == '\0') break;
		cptr = g_utf8_next_char(cptr);
	}
	while (*cptr != '\0');
}

bool NedResource::readString(FILE *fp, char *word) {
	int pos = 0;

	while (((word[0] = my_getc(fp)) != EOF) && (word[0] == ' ' || word[0] == '\t' || word[0] == '\n'));
	if (feof(fp)) return FALSE;
	pos++;
	while ((word[pos] = my_getc(fp)) != EOF &&
		 word[pos] != ' ' && word[pos] != '\t' && word[pos] != '\n') {
		pos++;
	}
	word[pos] = '\0';
#ifdef INPUT_DEBUG
	printf("WORD: %s\n", word);
#endif
	return TRUE;
}


bool NedResource::readInt(FILE *fp, int *val) {
	char buffer[128];
	int pos = 0;

	while (((buffer[0] = my_getc(fp)) != EOF) && (buffer[0] == ' ' || buffer[0] == '\t' || buffer[0] == '\n'));
	if (feof(fp)) {printf("Stelle 1\n"); fflush(stdout); return FALSE;}
	if (buffer[0] != '-' && !(buffer[0] >= '0' && buffer[0] <= '9')) {
		return FALSE;
	}
		
	pos++;
	while ((buffer[pos] = my_getc(fp)) != EOF &&
		 (buffer[pos] >= '0' && buffer[pos] <= '9')) {
		pos++;
	}
	if (!feof(fp)) {
		my_ungetc(buffer[pos], fp);
	}
	buffer[pos] = '\0';
	if (sscanf(buffer, "%d", val) != 1) {
		printf("Stelle 2: %s\n", buffer); fflush(stdout);
		return FALSE;
	}
#ifdef INPUT_DEBUG
	printf("INT: %d\n", *val);
#endif
	return TRUE;
}

bool NedResource::readUnsignedInt(FILE *fp, unsigned int *val) {
	char buffer[128];
	int pos = 0;

	while (((buffer[0] = my_getc(fp)) != EOF) && (buffer[0] == ' ' || buffer[0] == '\t' || buffer[0] == '\n'));
	if (feof(fp)) return FALSE;
	if (buffer[0] != '-' && !(buffer[0] >= '0' && buffer[0] <= '9')) {
		return FALSE;
	}
		
	pos++;
	while ((buffer[pos] = my_getc(fp)) != EOF &&
		 (buffer[pos] >= '0' && buffer[pos] <= '9')) {
		pos++;
	}
	if (!feof(fp)) {
		my_ungetc(buffer[pos], fp);
	}
	buffer[pos] = '\0';
	if (sscanf(buffer, "%u", val) != 1) {
		return FALSE;
	}
#ifdef INPUT_DEBUG
	printf("UNSIGNED INT: %u\n", *val);
#endif
	return TRUE;
}

bool NedResource::readLong(FILE *fp, unsigned long long *val) {
	char buffer[1024];
	int pos = 0;

	while (((buffer[0] = my_getc(fp)) != EOF) && (buffer[0] == ' ' || buffer[0] == '\t' || buffer[0] == '\n'));
	if (feof(fp)) return FALSE;
	if (buffer[0] != '-' && !(buffer[0] >= '0' && buffer[0] <= '9')) {
		return FALSE;
	}
		
	pos++;
	while ((buffer[pos] = my_getc(fp)) != EOF &&
		 (buffer[pos] >= '0' && buffer[pos] <= '9')) {
		pos++;
	}
	if (!feof(fp)) {
		my_ungetc(buffer[pos], fp);
	}
	buffer[pos] = '\0';
	if (sscanf(buffer, "%llu", val) != 1) {
		return FALSE;
	}
#ifdef INPUT_DEBUG
	printf("LONG: %llu\n", *val);
#endif
	return TRUE;
}

bool NedResource::readHex(FILE *fp, int *val) {
	char buffer[128];
	int pos = 0;
	bool not_eof;

	while ((not_eof = (buffer[0] = my_getc(fp)) != EOF) && (buffer[0] == ' ' || buffer[0] == '\t' || buffer[0] == '\n'));
	if (!not_eof) return FALSE;
	if (!((buffer[0] >= 'A' && buffer[0] <= 'F') || (buffer[0] >= 'a' && buffer[0] <= 'f') ||
		(buffer[0] >= '0' && buffer[0] <= '9'))) {
		DbgMsg(DBG_TESTING, "raus bei 1 buffer[0] = %c (0x%x)\n", buffer[0],buffer[0]); 
		return FALSE;
	}
		
	pos++;
	while ((buffer[pos] = my_getc(fp)) != EOF &&
		 ((buffer[pos] >= 'A' && buffer[pos] <= 'F') ||
		 (buffer[pos] >= '0' && buffer[pos] <= '9') ||
		 (buffer[pos] >= 'a' && buffer[pos] <= 'f'))) {
		pos++;
	}
	if (!feof(fp)) {
		my_ungetc(buffer[pos], fp);
	}
	buffer[pos] = '\0';
	if (sscanf(buffer, "%x", val) != 1) {
		return FALSE;
	}
#ifdef INPUT_DEBUG
	printf("HEX: %d(0x%x)\n", *val, *val);
#endif
	return TRUE;
}

bool NedResource::readFloat(FILE *fp, double *val) {
	int pos = 0;
	char word[128];
	bool dot_seen = false;

	if (m_word_buffer[0] != '\0') {
		strcpy(word, m_word_buffer);
		m_word_buffer[0] = '\0';
	}
	else {
		while (((word[0] = my_getc(fp)) != EOF) && (word[0] == ' ' || word[0] == '\t' || word[0] == '\n'));
		if (feof(fp)) return FALSE;
		if (word[0] != '-' && word[0] != '+' && word[0] != '.' && word[0] < '0' && word[0] > '9') return false;
		if (word[0] == '.') dot_seen = true;
		pos++;
		while ((word[pos] = my_getc(fp)) != EOF && ((word[pos] >= '0' && word[pos] <= '9') || word[pos] == '.')) {
		 	if (word[pos] == '.') {
				if (dot_seen) return false;
				dot_seen = true;
			}
			pos++;
		}
		if (!feof(fp)) {
			my_ungetc(word[pos], fp);
		}
		word[pos] = '\0';
	}
	if (sscanf(word, "%lf", val) != 1) return false;
	return true;
}

bool NedResource::removeWhiteSpaces(char **s) {
	gchar *cptr;
	char buffer[4096];
	int len;

	if (g_utf8_strlen(*s, -1) < 1) return false;
	cptr = g_utf8_offset_to_pointer(*s, 0);
	while (cptr) {
		if (!g_unichar_isspace(*cptr)) break;
		cptr = g_utf8_find_next_char(cptr, NULL);
	}
	if (cptr == NULL) return false;
	strcpy(buffer, cptr);
	len = g_utf8_strlen(buffer, -1);
	if (len < 1) return false;
	cptr = g_utf8_offset_to_pointer(buffer, len - 1);
	while(cptr != NULL) {
		if (!g_unichar_isspace(*cptr)) {
			cptr = g_utf8_find_next_char(cptr, NULL);
			*cptr = '\0';
			if (*s != NULL) {
				free(*s);
			}
			*s = strdup(buffer);
			return true;
		}
		cptr = g_utf8_find_prev_char(buffer, cptr);
	}
	return false;
}




void NedResource::increaseSystemStartTime(unsigned long long system_duration) {
	m_system_start_time += system_duration;
}

bool NedResource::permtest(snd_seq_port_info_t *pinfo, unsigned int perm) {
	return (((snd_seq_port_info_get_capability(pinfo) & perm) == perm) &&
		!(snd_seq_port_info_get_capability(pinfo) & SND_SEQ_PORT_CAP_NO_EXPORT));
}

GList *NedResource::listMidiOutDevices(int *index_of_subscribed_out_device) {
	GList *lptr;
	snd_seq_port_info_t *pinfo;
	snd_seq_client_info_t *cinfo;
	MidiPortStruct *port_struct;
	int i = 0;

	*index_of_subscribed_out_device = 0;

	if (m_midi_out_devices != NULL) {
		for (lptr = g_list_first(m_midi_out_devices); lptr; lptr = g_list_next(lptr)) {
			g_free(((MidiPortStruct *) (lptr->data))->name);
			g_free(lptr->data);
		}
		g_list_free(m_midi_out_devices);
	}
	m_midi_out_devices = NULL;
	snd_seq_port_info_alloca(&pinfo);
	snd_seq_client_info_alloca(&cinfo);
	snd_seq_client_info_set_client(cinfo, -1);
	while (snd_seq_query_next_client(m_sequ_out, cinfo) >= 0) {
		snd_seq_port_info_set_client(pinfo, snd_seq_client_info_get_client(cinfo));
		snd_seq_port_info_set_port(pinfo, -1);
		while(snd_seq_query_next_port(m_sequ_out, pinfo) >= 0) {
			if (permtest(pinfo, SND_SEQ_PORT_CAP_WRITE|SND_SEQ_PORT_CAP_SUBS_WRITE)) {
				if ((port_struct = (MidiPortStruct  *) g_try_malloc(sizeof(MidiPortStruct))) == NULL) {
					Abort("NedResource::listMidiDevices: allocation failed");
				}
				port_struct->client = snd_seq_client_info_get_client(cinfo);
				port_struct->port = snd_seq_port_info_get_port(pinfo);
				port_struct->name = strdup(snd_seq_client_info_get_name(cinfo));
				if (m_alsa_out_dest_addr.client != 0) {
					if (m_alsa_out_dest_addr.client == port_struct->client && m_alsa_out_dest_addr.port == port_struct->port) {
						*index_of_subscribed_out_device = i;
					}
				}
				m_midi_out_devices = g_list_append(m_midi_out_devices, port_struct);
				i++;
			}
		}
	}
	return m_midi_out_devices;
}

GList *NedResource::listMidiInDevices(int *index_of_subscribed_in_device) {
	GList *lptr;
	snd_seq_port_info_t *pinfo;
	snd_seq_client_info_t *cinfo;
	MidiPortStruct *port_struct;
	int i = 0;

	*index_of_subscribed_in_device = 0;

	if (m_midi_in_devices != NULL) {
		for (lptr = g_list_first(m_midi_in_devices); lptr; lptr = g_list_next(lptr)) {
			g_free(((MidiPortStruct *) (lptr->data))->name);
			g_free(lptr->data);
		}
		g_list_free(m_midi_in_devices);
	}
	m_midi_in_devices = NULL;
	snd_seq_port_info_alloca(&pinfo);
	snd_seq_client_info_alloca(&cinfo);
	snd_seq_client_info_set_client(cinfo, -1);
	while (snd_seq_query_next_client(m_sequ_in, cinfo) >= 0) {
		snd_seq_port_info_set_client(pinfo, snd_seq_client_info_get_client(cinfo));
		snd_seq_port_info_set_port(pinfo, -1);
		while(snd_seq_query_next_port(m_sequ_in, pinfo) >= 0) {
			if (permtest(pinfo, SND_SEQ_PORT_CAP_WRITE|SND_SEQ_PORT_CAP_SUBS_READ)) {
				if ((port_struct = (MidiPortStruct  *) g_try_malloc(sizeof(MidiPortStruct))) == NULL) {
					Abort("NedResource::listMidiDevices: allocation failed");
				}
				port_struct->client = snd_seq_client_info_get_client(cinfo);
				port_struct->port = snd_seq_port_info_get_port(pinfo);
				port_struct->name = strdup(snd_seq_client_info_get_name(cinfo));
				if (m_alsa_in_src_addr.client != 0) {
					if (m_alsa_in_src_addr.client == port_struct->client && m_alsa_in_src_addr.port == port_struct->port) {
						*index_of_subscribed_in_device = i;
					}
				}
				m_midi_in_devices = g_list_append(m_midi_in_devices, port_struct);
				i++;
			}
		}
	}
	return m_midi_in_devices;
}


void NedResource::subscribe_out(int idx) {
	GList *lptr;
	snd_seq_port_subscribe_t *subs;


	if ((lptr = g_list_nth(m_midi_out_devices, idx)) == NULL) {
		NedResource::Abort("NedResource::subscribe_out");
	}

	snd_seq_port_subscribe_alloca(&subs);
	if (m_alsa_out_dest_addr.client != 0) {
		snd_seq_port_subscribe_set_sender(subs, &m_alsa_out_addr);
		snd_seq_port_subscribe_set_dest(subs, &m_alsa_out_dest_addr);
		snd_seq_unsubscribe_port(m_sequ_out, subs);
		m_alsa_out_dest_addr.client = 0;
	}
	m_alsa_out_dest_addr.client = ((MidiPortStruct *) lptr->data)->client;
	m_alsa_out_dest_addr.port = ((MidiPortStruct *) lptr->data)->port;
	snd_seq_port_subscribe_set_sender(subs, &m_alsa_out_addr);
	snd_seq_port_subscribe_set_dest(subs, &m_alsa_out_dest_addr);
	snd_seq_subscribe_port(m_sequ_out, subs);
}
					
void NedResource::subscribe_in(int idx) {
	GList *lptr;
	snd_seq_port_subscribe_t *subs;


	if ((lptr = g_list_nth(m_midi_in_devices, idx)) == NULL) {
		NedResource::Abort("NedResource::subscribe_in");
	}

	snd_seq_port_subscribe_alloca(&subs);
	if (m_alsa_in_src_addr.client != 0) {
		snd_seq_port_subscribe_set_sender(subs, &m_alsa_in_src_addr);
		snd_seq_port_subscribe_set_dest(subs, &m_alsa_in_addr);
		snd_seq_unsubscribe_port(m_sequ_in, subs);
		m_alsa_in_src_addr.client = 0;
	}
	m_alsa_in_src_addr.client = ((MidiPortStruct *) lptr->data)->client;
	m_alsa_in_src_addr.port = ((MidiPortStruct *) lptr->data)->port;
	snd_seq_port_subscribe_set_sender(subs, &m_alsa_in_src_addr);
	snd_seq_port_subscribe_set_dest(subs, &m_alsa_in_addr);
	snd_seq_subscribe_port(m_sequ_in, subs);
}
					


void NedResource::prepareMidiEventList(double tempo_inverse) {
	GList *lptr1, *lptr2;

	if (m_midi_events != NULL) {
		for (lptr1 = g_list_first(m_midi_events); lptr1; lptr1 = g_list_next(lptr1)) {
			for (lptr2 =  g_list_first(((MidiListStruct *) lptr1->data)->notes); lptr2; lptr2 = g_list_next(lptr2)) {
				g_free(lptr2->data);
			}
			g_list_free(g_list_first(((MidiListStruct *) lptr1->data)->notes));
			g_free(lptr1->data);
		}
		g_list_free(m_midi_events);
	}
	for (lptr1 = g_list_first(m_fermata_list); lptr1; lptr1 = g_list_next(lptr1)) {
		g_free(lptr1->data);
	}
	g_list_free(m_fermata_list);
	m_fermata_list = NULL;
	m_midi_events = NULL;
	m_system_start_time = 0;
	m_tempo_inverse = tempo_inverse;
	m_d_capo_al_coda_available = m_d_capo_al_fine_available = m_ds_al_coda_available = m_ds_al_fine_available = false;
}

void NedResource::setStartVolume(unsigned char chn, int voice_idx, unsigned char vol) {
	m_channel_volume[chn][voice_idx] = vol;
}

void NedResource::addMidiNoteEvent(unsigned short channel, unsigned char voice, unsigned int offset, unsigned int type, int data1, int pgm, 
			unsigned short segno_sign, NedNote *note) {
	MidiEventStruct *ev;
	if ((ev = (MidiEventStruct *) g_try_malloc(sizeof(MidiEventStruct))) == NULL) {
		Abort("NedResource::addMidiNoteEvent: allocation failed \n");
	}
	ev->channel = channel;
	ev->voice = voice;
	ev->midi_time = m_system_start_time + offset;
	ev->type = type;
	ev->segno_sign = segno_sign;
	ev->data1 = data1;
	ev->data2 = (type == SND_SEQ_EVENT_NOTEON) ? 64 /* will be corrected */: 0; 
	ev->pgm = pgm;
	ev->note = note;
	ev->repetition = false;
	if ((segno_sign & SEGNO_D_S_AL_CODA) != 0) m_ds_al_coda_available = true;
	if ((segno_sign & SEGNO_D_S_AL_FINE) != 0) m_ds_al_fine_available = true;
	if ((segno_sign & SEGNO_D_CAPO_AL_CODA) != 0) m_d_capo_al_coda_available = true;
	if ((segno_sign & SEGNO_D_CAPO_AL_FINE) != 0) m_d_capo_al_fine_available = true;

	placeMidiEvent(ev);
}

void NedResource::addMidiCtrlEvent(unsigned short channel, unsigned int offset, unsigned int ctrl_param, int val) {
	MidiEventStruct *ev;
	if ((ev = (MidiEventStruct *) g_try_malloc(sizeof(MidiEventStruct))) == NULL) {
		Abort("NedResource::addMidiCtrlEvent: allocation failed");
	}
	ev->channel = channel;
	ev->midi_time = m_system_start_time + offset;
	ev->type = SND_SEQ_EVENT_CONTROLLER;
	ev->data1 = ctrl_param;
	ev->data2 = val;
	ev->segno_sign = 0; /* dummy */

	placeMidiEvent(ev);
}

void NedResource::addMeasureEntry(NedMeasure *measure) {
	MidiEventStruct *ev;

	if ((ev = (MidiEventStruct *) g_try_malloc(sizeof(MidiEventStruct))) == NULL) {
		Abort("NedResource::addMeasureEntry: allocation failed");
	}
	ev->channel = 0; /* dummy */
	ev->midi_time = m_system_start_time + measure->midi_start;
	ev->type = PSEUDO_EVENT_MEASURE;
	ev->measure = measure;
	ev->segno_sign = 0;

	placeMidiEvent(ev);
}

void NedResource::addTempoChange(unsigned int offset, double tempoinverse) {
	MidiEventStruct *ev;

	if ((ev = (MidiEventStruct *) g_try_malloc(sizeof(MidiEventStruct))) == NULL) {
		Abort("NedResource::addTempoChange: allocation failed");
	}
	ev->channel = 0; /* dummy */
	ev->segno_sign = 0;
	ev->midi_time = m_system_start_time + offset;
	ev->type = PSEUDO_EVENT_TEMPO_CHANGE;
	ev->tempoinverse = tempoinverse;
	m_tempo_inverse = tempoinverse;

	placeMidiEvent(ev);
}


void NedResource::addKeysigEntry(unsigned int offset, int chn, int key) {
	MidiEventStruct *ev;
	unsigned long long time = m_system_start_time + offset;
	if (time == 0) return;

	if ((ev = (MidiEventStruct *) g_try_malloc(sizeof(MidiEventStruct))) == NULL) {
		Abort("NedResource::addKeysigEntry: allocation failed");
	}
	ev->channel = chn;
	ev->data1 = key;
	ev->midi_time = time;
	ev->type = PSEUDO_EVENT_KEY_CHANGE;

	placeMidiEvent(ev);
}


void NedResource::addTempoChangeAbs(unsigned long long midi_time, double tempoinverse) {
	MidiEventStruct *ev;

	if ((ev = (MidiEventStruct *) g_try_malloc(sizeof(MidiEventStruct))) == NULL) {
		Abort("NedResource::addTempoChangeAbs: allocation failed");
	}
	ev->channel = 0; /* dummy */
	ev->segno_sign = 0; /* dummy */
	ev->midi_time = midi_time;
	ev->type = PSEUDO_EVENT_TEMPO_CHANGE;
	ev->tempoinverse = tempoinverse;

	placeMidiEvent(ev);
}

void NedResource::addPseudoEvent2(unsigned int offset, int event, int chn, int data) {
	MidiEventStruct *ev;

	if ((ev = (MidiEventStruct *) g_try_malloc(sizeof(MidiEventStruct))) == NULL) {
		Abort("NedResource::addPseudoEvent2: allocation failed");
	}
	ev->channel = chn;
	ev->segno_sign = 0; /* dummy */
	ev->midi_time = m_system_start_time + offset;
	ev->type = event;
	ev->data1 = data;

	placeMidiEvent(ev);
}

void NedResource::addFermata(unsigned int offset) {
	struct fermata_info *fermata_info;

	if ((fermata_info = (struct fermata_info *) g_try_malloc(sizeof(struct fermata_info))) == NULL) {
		Abort("NedResource::addFermata: allocation failed");
	}
	fermata_info->midi_time = m_system_start_time + offset;
	fermata_info->tempo_inverse = m_tempo_inverse;
	fermata_info->a_tempo = true;
	m_fermata_list = g_list_append(m_fermata_list, fermata_info);
}

void NedResource::handleVolAndTempoChanges(double tempo_inverse) {
	GList *lptr1, *lptr2, *lptr3;
	GList *lptr1a, *lptr2a, *lptr3a;
	MidiEventStruct *ev, *eva;
	MidiListStruct * mls;
	unsigned long long timediff;
	int vol, voldiff, newtempo;
	double tempodiff;
	bool found;
	int i, j;

	for (i = 0; i < 16; m_last_crescendo_info[i++].symbol = 0);
	
	m_tempo_inverse = tempo_inverse;

	for (i = 0; i < 16; i++) {
		m_channel_volume_ratio[i][0] = 1.0;
		for (j = 1; j < VOICE_COUNT; j++) {
			if (m_channel_volume[i][j] > 1) {
				m_channel_volume_ratio[i][j] = (double) (m_channel_volume[i][j]) / (double) (m_channel_volume[i][0]);
			}
			else {
				m_channel_volume_ratio[i][j] = 0.0;
			}
		}
	}

	for (lptr1 = g_list_first(m_midi_events); lptr1; lptr1 = g_list_next(lptr1)) {
		lptr2 = ((MidiListStruct *) lptr1->data)->notes;
		for (lptr3 = g_list_first(lptr2); lptr3; lptr3 = g_list_next(lptr3)) {
			ev = (MidiEventStruct *) lptr3->data;
			if (ev->type == SND_SEQ_EVENT_NOTEON) {
				ev->data2 = m_channel_volume[ev->channel][ev->voice];

			}
			else if (ev->type == PSEUDO_EVENT_VOL_CHANGE) {
				for (i = 0; i < VOICE_COUNT; i++) {
					m_channel_volume[ev->channel][i] = (int) ((double) ev->data1 * m_channel_volume_ratio[ev->channel][i] + 0.5);
				}
			}
			else if (ev->type == PSEUDO_EVENT_CRES_START) {
				m_last_crescendo_info[ev->channel].symbol = PSEUDO_EVENT_CRES_START;
				m_last_crescendo_info[ev->channel].listptr = lptr1;
				m_last_crescendo_info[ev->channel].volume = m_channel_volume[ev->channel][0];
				m_last_crescendo_info[ev->channel].midi_time = ev->midi_time;
			}
			else if (ev->type == PSEUDO_EVENT_CRES_STOP) {
				if (m_last_crescendo_info[ev->channel].symbol == PSEUDO_EVENT_CRES_START) {
					voldiff = ev->data1;
					if (m_last_crescendo_info[ev->channel].volume + voldiff >= 128) {
						voldiff = 127 - m_last_crescendo_info[ev->channel].volume;
					}
					else if (m_last_crescendo_info[ev->channel].volume + voldiff < 0) {
						voldiff = -m_last_crescendo_info[ev->channel].volume;
					}
					timediff = ev->midi_time - m_last_crescendo_info[ev->channel].midi_time;
					if (timediff > 100.0) {
						found = false;
						for (lptr1a = m_last_crescendo_info[ev->channel].listptr; !found && lptr1a; lptr1a = g_list_next(lptr1a)) {
							lptr2a = ((MidiListStruct *) lptr1a->data)->notes;
							lptr3a = g_list_first(lptr2a);
							eva = (MidiEventStruct *) lptr3a->data;
							vol = m_last_crescendo_info[ev->channel].volume +
							 	(int) ((double) voldiff * (double) (eva->midi_time - m_last_crescendo_info[ev->channel].midi_time) /
								(double) timediff + 0.5);
							for (; lptr3a; lptr3a = g_list_next(lptr3a)) {
								eva = (MidiEventStruct *) lptr3a->data;
								if (eva->channel != ev->channel) continue;
								if (eva->type != SND_SEQ_EVENT_NOTEON) continue;
								eva->data2 = vol * m_channel_volume_ratio[eva->channel][eva->voice];
							}
							found = (lptr1a == lptr1);
							if (found) {
								m_channel_volume[ev->channel][0] = m_last_crescendo_info[ev->channel].volume + voldiff;
							}
						}
					}
				}
				m_last_crescendo_info[ev->channel].symbol = 0;
			}
			else if (ev->type == PSEUDO_EVENT_TEMPO_CHANGE) {
				m_tempo_inverse = ((MidiListStruct *) lptr1->data)->tempoinverse = ev->tempoinverse;
			}
			else if (ev->type == PSEUDO_EVENT_RIT_START ) {
				m_last_ritardando.symbol = PSEUDO_EVENT_RIT_START;
				m_last_ritardando.listptr = lptr1;
				m_last_ritardando.tempo_inverse = m_tempo_inverse;
				m_last_ritardando.midi_time = ev->midi_time;
			}
			else if (ev->type == PSEUDO_EVENT_RIT_STOP) {
				if (m_last_ritardando.symbol == PSEUDO_EVENT_RIT_START) {
					newtempo = (int) (60000.0 / m_last_ritardando.tempo_inverse) + ev->data1;
					if (newtempo > 300) newtempo = 300;
					if (newtempo < 20) newtempo = 20;
					timediff = ev->midi_time - m_last_ritardando.midi_time;
					tempodiff = 60000.0 / (double) newtempo - m_last_ritardando.tempo_inverse;
					if (timediff > 100.0) {
						found = false;
						for (lptr1a = m_last_ritardando.listptr; !found && lptr1a; lptr1a = g_list_next(lptr1a)) {
							mls = (MidiListStruct *) lptr1a->data;
							lptr3a = g_list_first(mls->notes);
							eva = (MidiEventStruct *) lptr3a->data;
							mls->tempoinverse = m_last_ritardando.tempo_inverse +
							 	tempodiff * (double) (eva->midi_time - m_last_ritardando.midi_time) /
								timediff;
							found = (lptr1a == lptr1);
							if (found) {
								m_tempo_inverse = mls->tempoinverse = m_last_ritardando.tempo_inverse + tempodiff;
							}
						}
					}
				}
				m_last_ritardando.symbol = 0;
			}
		}
	}

	m_tempo_inverse = tempo_inverse;
	lptr1 = g_list_first(m_midi_events); 
	if (lptr1) {
		mls = (MidiListStruct *) lptr1->data;
		if (mls->tempoinverse > 0.0) {
			m_tempo_inverse = mls->tempoinverse;
		}
	}

}

void NedResource::removeAllPseudosEvents() {
	GList *lptr1, *lptr2, *lptr3;


	lptr1 = g_list_first(m_midi_events);
	while (lptr1) {
		lptr2 = ((MidiListStruct *) lptr1->data)->notes;
		lptr3 = g_list_first(lptr2);
		while (lptr3) {
			if ((((MidiEventStruct *) lptr3->data)->type & PSEUDO_FLAG) != 0) {
				g_free(lptr3->data);
				lptr2 = g_list_delete_link(lptr2, lptr3);
				lptr3 = g_list_first(lptr2);
			}
			else {
				lptr3 = g_list_next(lptr3);
			}
		}
		if (lptr2 == NULL) {
			g_free(lptr1->data);
			m_midi_events = g_list_delete_link(m_midi_events, lptr1);
			lptr1 = g_list_first(m_midi_events);
		}
		else {
			((MidiListStruct *) lptr1->data)->notes = lptr2;
			lptr1 = g_list_next(lptr1);
		}
	}
}

void NedResource::correctFermataTempo() {
	GList *lptr1, *lptr2, *lptr3;
	unsigned long long timenote, timeferm;
	double target_tempo_inverse = 0.0, current_tempo_inverse;
	bool tempo_inserted;
#define FERMATA_MIDI_BEFORE_DIST NOTE_4
#define FERMATA_AFTER_MIDI_DIST NOTE_4
#define FERMATA_MIDI_TEMPO_RATIO 3.0

	if (m_fermata_list == NULL) return;

	for (lptr1 = g_list_first(m_midi_events);  lptr1; lptr1 = g_list_next(lptr1)) {
		lptr2 = g_list_first(((MidiListStruct *) lptr1->data)->notes);
		tempo_inserted = false;
		for (; !tempo_inserted && lptr2; lptr2 = g_list_next(lptr2)) {
			if (((MidiEventStruct *) lptr2->data)->type == SND_SEQ_EVENT_NOTEON) {
				timenote = ((MidiEventStruct *) lptr2->data)->midi_time;
				for (lptr3 = g_list_first(m_fermata_list); lptr3; lptr3 = g_list_next(lptr3)) {
					timeferm = ((struct fermata_info *) lptr3->data)->midi_time;
					if (timeferm - FERMATA_MIDI_BEFORE_DIST <= timenote && timeferm > timenote) {
						target_tempo_inverse = ((struct fermata_info *) lptr3->data)->tempo_inverse;
						current_tempo_inverse = target_tempo_inverse + (FERMATA_MIDI_TEMPO_RATIO * target_tempo_inverse - target_tempo_inverse) /
									(double) FERMATA_MIDI_BEFORE_DIST * (double) (timenote - (timeferm - FERMATA_MIDI_BEFORE_DIST));
						addTempoChangeAbs(timenote, current_tempo_inverse);
						tempo_inserted = true;
						((struct fermata_info *) lptr3->data)->a_tempo = false;
						break;
					}
					else if (timeferm <= timenote && timeferm + FERMATA_AFTER_MIDI_DIST > timenote) {
						target_tempo_inverse = ((struct fermata_info *) lptr3->data)->tempo_inverse;
						current_tempo_inverse = FERMATA_MIDI_TEMPO_RATIO * target_tempo_inverse + (target_tempo_inverse - FERMATA_MIDI_TEMPO_RATIO * target_tempo_inverse) /
									(double) FERMATA_AFTER_MIDI_DIST * (double) (timenote - timeferm);
						addTempoChangeAbs(timenote, current_tempo_inverse);
						tempo_inserted = true;
						((struct fermata_info *) lptr3->data)->a_tempo = false;
						break;
					}
					else if (!(((struct fermata_info *) lptr3->data)->a_tempo) && timenote >= timeferm + FERMATA_AFTER_MIDI_DIST) {
						addTempoChangeAbs(timenote, target_tempo_inverse);
						((struct fermata_info *) lptr3->data)->a_tempo = true;
					}
				}
			}
		}
	}
}

void NedResource::handleSegnos() {
	int xx = 0;

	if (m_ds_al_coda_available) {
		xx++;
	}
	if (m_ds_al_fine_available) {
		xx++;
	}
	if (m_d_capo_al_coda_available) {
		xx++;
	}
	if (m_d_capo_al_fine_available) {
		xx++;
	}
		
	if (xx != 1) return;

	if (m_ds_al_fine_available) {
		handleDSalFine(false);
	}
	else if (m_ds_al_coda_available) {
		handleDSalCoda(false);
	}
	else if (m_d_capo_al_fine_available) {
		handleDSalFine(true);
	}
	else if (m_d_capo_al_coda_available) {
		handleDSalCoda(true);
	}
}

void NedResource::handleDSalFine(bool da_capo) {
	GList *segno = NULL, *dal_segno_al_fine = NULL;
	unsigned long long segno_time = 0, dal_segno_al_fine_time = 0;
	GList *lptr1, *lptr2;
	GList *repeat_part;
	bool repeat_found;
	unsigned short repeat_signal = (da_capo ? SEGNO_D_CAPO_AL_FINE : SEGNO_D_S_AL_FINE);

	for (lptr1 = g_list_first(m_midi_events);  lptr1; lptr1 = g_list_next(lptr1)) {
		lptr2 = g_list_first(((MidiListStruct *) lptr1->data)->notes);
		for (; lptr2; lptr2 = g_list_next(lptr2)) {
			if ((((MidiEventStruct *) lptr2->data)->segno_sign & repeat_signal) != 0) {
				dal_segno_al_fine_time = ((MidiEventStruct *) lptr2->data)->midi_time;
				dal_segno_al_fine = lptr1;
				break;
			}
		}
	}
	if (da_capo) {
		segno = m_midi_events;
		segno_time = 0;
	}
	else {
		repeat_found = false;
		for (lptr1 = g_list_last(m_midi_events); lptr1; lptr1 = g_list_previous(lptr1)) {
			if (lptr1 == dal_segno_al_fine) repeat_found = true;
			lptr2 = g_list_first(((MidiListStruct *) lptr1->data)->notes);
			for (; lptr2; lptr2 = g_list_next(lptr2)) {
				if ((((MidiEventStruct *) lptr2->data)->segno_sign & SEGNO_SIGN) != 0 &&
					(((MidiEventStruct *) lptr2->data)->type & NOTE_MASK) == SND_SEQ_EVENT_NOTEON) { // eventually with PSEUDO2_FLAG (rest)
					segno_time = ((MidiEventStruct *) lptr2->data)->midi_time;
					segno = lptr1;
					break;
				}
			}
		}
		if (!repeat_found) {
			return;
		}
	}
	addTimeOffsetAfter(dal_segno_al_fine, dal_segno_al_fine_time - segno_time);
	repeat_part = cloneAllMidiEventsBetween(segno, dal_segno_al_fine, dal_segno_al_fine_time - segno_time);
	for (lptr1 = g_list_first(repeat_part); lptr1; lptr1 = g_list_next(lptr1)) {
		if ((((MidiEventStruct *) lptr1->data)->segno_sign & SEGNO_FINE) != 0) {
			((MidiEventStruct *) lptr1->data)->segno_sign |= SEGNO_FINE2;
		}
		placeMidiEvent((MidiEventStruct *) lptr1->data);
	}
	g_list_free(repeat_part);
}

void NedResource::handleDSalCoda(bool da_capo) {
	GList *segno = NULL, *dal_segno_al_coda = NULL, *al_coda = NULL, *coda = NULL;
	unsigned long long segno_time = 0, dal_segno_al_coda_time = 0, al_coda_time = 0, coda_time = 0;
	GList *lptr1, *lptr2, *lptr3;
	GList *repeat_part;
	bool repeat_found, al_coda_found, inside_repeat, before_segno_al_coda, sequ_error;
	unsigned short repeat_signal = (da_capo ? SEGNO_D_CAPO_AL_CODA : SEGNO_D_S_AL_CODA);

	for (lptr1 = g_list_first(m_midi_events);  lptr1; lptr1 = g_list_next(lptr1)) {
		lptr2 = g_list_first(((MidiListStruct *) lptr1->data)->notes);
		for (; lptr2; lptr2 = g_list_next(lptr2)) {
			if ((((MidiEventStruct *) lptr2->data)->segno_sign & repeat_signal) != 0 &&
				(((MidiEventStruct *) lptr2->data)->type & NOTE_MASK) == SND_SEQ_EVENT_NOTEOFF)  { // eventually with PSEUDO2_FLAG (rest)
				dal_segno_al_coda_time = ((MidiEventStruct *) lptr2->data)->midi_time;
				dal_segno_al_coda = lptr1;
				break;
			}
		}
	}
	if (dal_segno_al_coda == NULL) return;
	if (da_capo) {
		segno = m_midi_events;
		segno_time = 0; 
	}
	else {
		for (lptr1 = g_list_last(m_midi_events); lptr1; lptr1 = g_list_previous(lptr1)) {
			if (lptr1 == dal_segno_al_coda) repeat_found = true;
			lptr2 = g_list_first(((MidiListStruct *) lptr1->data)->notes);
			for (; lptr2; lptr2 = g_list_next(lptr2)) {
				if ((((MidiEventStruct *) lptr2->data)->segno_sign & SEGNO_SIGN) != 0 &&
					(((MidiEventStruct *) lptr2->data)->type & NOTE_MASK) == SND_SEQ_EVENT_NOTEON) { // eventually with PSEUDO2_FLAG (rest)
					segno_time = ((MidiEventStruct *) lptr2->data)->midi_time;
					segno = lptr1;
					break;
				}
			}
		}
		if (segno == NULL) return;
	}

	sequ_error = false;
	inside_repeat = false;
	al_coda_found = false;
	inside_repeat = false;
	for (lptr1 = g_list_last(m_midi_events); !sequ_error && lptr1; lptr1 = g_list_previous(lptr1)) {
		if (lptr1 == segno) break;
		if (lptr1 == dal_segno_al_coda) {inside_repeat = true; continue;}
		lptr2 = g_list_first(((MidiListStruct *) lptr1->data)->notes);
		for (; lptr2; lptr2 = g_list_next(lptr2)) {
			if ((((MidiEventStruct *) lptr2->data)->segno_sign & SEGNO_AL_CODA) != 0 &&
				(((MidiEventStruct *) lptr2->data)->type & NOTE_MASK) == SND_SEQ_EVENT_NOTEOFF) { // eventually with PSEUDO2_FLAG (rest)
				if (!inside_repeat) {
					sequ_error = true;
					break;
				}
				al_coda = lptr1;
				al_coda_time = ((MidiEventStruct *) lptr2->data)->midi_time;
				break;
			}
		}
	}
	if (sequ_error || al_coda == NULL) return;

	before_segno_al_coda = true;
	for (lptr1 = g_list_first(m_midi_events); !sequ_error && lptr1; lptr1 = g_list_next(lptr1)) {
		if (lptr1 == dal_segno_al_coda) {before_segno_al_coda = false;}
		lptr2 = g_list_first(((MidiListStruct *) lptr1->data)->notes);
		for (; lptr2; lptr2 = g_list_next(lptr2)) {
			if ((((MidiEventStruct *) lptr2->data)->segno_sign & SEGNO_CODA) != 0 &&
				(((MidiEventStruct *) lptr2->data)->type & NOTE_MASK) == SND_SEQ_EVENT_NOTEON) { // eventually with PSEUDO2_FLAG (rest)
				if (before_segno_al_coda) {
					sequ_error = true;
					break;
				}
				coda = lptr1;
				coda_time = ((MidiEventStruct *) lptr2->data)->midi_time;
				break;
			}
		}
	}
	if (sequ_error || coda == NULL) return;

	/*
	printf("dal_segno_al_coda = 0x%x, coda = 0x%x, dal_segno_al_coda_time = %llu(%llu), coda_time = %llu(%llu)\n",
		dal_segno_al_coda, coda, dal_segno_al_coda_time, dal_segno_al_coda_time / NOTE_4, coda_time, coda_time / NOTE_4); fflush(stdout);
		*/


	addTimeOffsetAfter(dal_segno_al_coda, al_coda_time - segno_time);
	repeat_part = cloneAllMidiEventsBetween(segno, al_coda, dal_segno_al_coda_time - segno_time);
	for (lptr1 = g_list_first(repeat_part); lptr1; lptr1 = g_list_next(lptr1)) {
		if (((MidiEventStruct *) lptr1->data)->segno_sign == SEGNO_AL_CODA && 
			(((MidiEventStruct *) lptr1->data)->type & NOTE_MASK) == SND_SEQ_EVENT_NOTEOFF) { // eventually with PSEUDO2_FLAG (rest)
			((MidiEventStruct *) lptr1->data)->segno_sign |= SEGNO_AL_CODA2;
		}
		placeMidiEvent((MidiEventStruct *) lptr1->data);
	}
	g_list_free(repeat_part);

	if (dal_segno_al_coda == coda) {
		return;
	}
	
	al_coda = NULL; coda = NULL;
	for (lptr1 = g_list_first(m_midi_events); lptr1; lptr1 = g_list_next(lptr1)) {
		lptr2 = g_list_first(((MidiListStruct *) lptr1->data)->notes);
		for (; lptr2; lptr2 = g_list_next(lptr2)) {
			if ((((MidiEventStruct *) lptr2->data)->segno_sign & SEGNO_AL_CODA2) != 0) {
				al_coda = lptr1;
				al_coda_time = ((MidiEventStruct *) lptr2->data)->midi_time;
				break;
			}
			else if ((((MidiEventStruct *) lptr2->data)->segno_sign & SEGNO_CODA) != 0 &&
					(((MidiEventStruct *) lptr2->data)->type & NOTE_MASK) == SND_SEQ_EVENT_NOTEON) { // eventually with PSEUDO2_FLAG (rest)
				coda = lptr1;
				coda_time = ((MidiEventStruct *) lptr2->data)->midi_time;
				lptr3 = g_list_previous(lptr1);
				break;
			}
		}
	}
	if (al_coda == NULL ||  coda == NULL || al_coda_time >= coda_time) {
		NedResource::Abort("handleDSalCoda(2)");
	}
	subTimeOffsetAfter(al_coda, coda_time - al_coda_time);
	deleteAllBetween(al_coda, coda);
}


GList *NedResource::findListAtMeasure(int meas_number, NedMeasure **measure) {
	GList *lptr1, *lptr2;

	for (lptr1 = g_list_first(m_midi_events);  lptr1; lptr1 = g_list_next(lptr1)) {
		lptr2 = g_list_first(((MidiListStruct *) lptr1->data)->notes);
		for (; lptr2; lptr2 = g_list_next(lptr2)) {
			if (((MidiEventStruct *) lptr2->data)->type == PSEUDO_EVENT_MEASURE) {
				if (((MidiEventStruct *) lptr2->data)->measure->getMeasureNumber() == meas_number) {
					*measure = ((MidiEventStruct *) lptr2->data)->measure;
					return lptr1;
				}
			}
		}
	}
	return g_list_last(m_midi_events);
}

void NedResource::copyAllBetweenMeasures(int start_measure_number, int alternative1_measure_number, int end_measure_number) {
	NedMeasure *start_measure, *end_measure;
	GList *start_ptr, *end_ptr, *alter1_ptr = NULL, *lptr;
	GList *repeat_part;
	GList *lptr2;
	unsigned long long extra_offs, extra_offs2;
	unsigned long long midi_start_time, midi_end_time, midi_alter1_time;
	if (start_measure_number < 0) return;
	if (end_measure_number <= start_measure_number) {
		Abort("NedResource::copyAllBetweenMeasures(1)");
	}
	if (alternative1_measure_number >= 0) {
		if (end_measure_number <= alternative1_measure_number) {
			Abort("NedResource::copyAllBetweenMeasures(2)");
		}
		if (alternative1_measure_number <= start_measure_number) {
			Abort("NedResource::copyAllBetweenMeasures(3)");
		}
		if ((alter1_ptr = findListAtMeasure(alternative1_measure_number, &start_measure)) == NULL) {
			Abort("NedResource::copyAllBetweenMeasures(4)");
		}
	}
	if ((start_ptr = findListAtMeasure(start_measure_number, &start_measure)) == NULL) {
		Abort("NedResource::copyAllBetweenMeasures(5)");
	}
	if ((end_ptr = findListAtMeasure(end_measure_number, &end_measure)) == NULL) {
		Abort("NedResource::copyAllBetweenMeasures(6)");
	}
	lptr2 = g_list_first(((MidiListStruct *) start_ptr->data)->notes);
	midi_start_time = ((MidiEventStruct *) lptr2->data)->midi_time;
	lptr2 = g_list_first(((MidiListStruct *) end_ptr->data)->notes);
	midi_end_time = ((MidiEventStruct *) lptr2->data)->midi_time;
	if (alternative1_measure_number >= 0) {
		lptr2 = g_list_first(((MidiListStruct *) alter1_ptr->data)->notes);
		midi_alter1_time = ((MidiEventStruct *) lptr2->data)->midi_time;
	}
#ifdef AAA
	if (midi_end_time <= midi_start_time) {
		if (end_measure->midi_start == 0) {
			if ((end_ptr2 = findListAtMeasure(end_measure_number - 1, &end_measure2)) == NULL) {
				Abort("NedResource::copyAllBetweenMeasures(7)");
			}
			lptr2 = g_list_first(((MidiListStruct *) end_ptr2->data)->notes);
			midi_end_time = ((MidiEventStruct *) lptr2->data)->midi_time + 
					end_measure2->midi_end - end_measure2->midi_start;
			extra_offs = midi_end_time - midi_start_time;
			/*
			DbgMsg(DBG_TESTING, "Sonderfall: end_measure->midi_end = %llu, start_measure->midi_start = %llu, extra_offs = %llu\n",
				end_measure2->midi_end / NOTE_4, start_measure->midi_start / NOTE_4,
				 extra_offs / NOTE_4); 
				 */
		}
		else {
			DbgMsg(DBG_CRITICAL, "start_measure_number = %d(%d) --> %llu, end_measure_number = %d(%d) --> %llu\n",
				start_measure_number, start_measure->getMeasureNumber(),
				start_measure->midi_start, end_measure_number,
				end_measure->getMeasureNumber(), end_measure->midi_start); 
			Abort("NedResource::copyAllBetweenMeasures(8)");
		}
	}
	else {
#endif
		if (alternative1_measure_number >= 0) {
			extra_offs2 = midi_end_time - midi_alter1_time;
		}
		extra_offs = midi_end_time - midi_start_time;
#ifdef AAA
	}
#endif
	//DbgMsg(DBG_TESTING, "end_measure->midi_start = %llu, start_measure->midi_start = %llu, extra_offs = %llu\n",
	//end_measure->midi_start / NOTE_4, start_measure->midi_start / NOTE_4, extra_offs / NOTE_4); 
	if (alternative1_measure_number >= 0) {
		addTimeOffsetAfter(end_ptr, extra_offs - extra_offs2);
	}
	else {
		addTimeOffsetAfter(end_ptr, extra_offs);
	}
	if (alternative1_measure_number >= 0) {
		repeat_part = cloneAllMidiEventsBetween(start_ptr, alter1_ptr, extra_offs);
	}
	else {
		repeat_part = cloneAllMidiEventsBetween(start_ptr, end_ptr, extra_offs);
	}
	for (lptr = g_list_first(repeat_part); lptr; lptr = g_list_next(lptr)) {
		placeMidiEvent((MidiEventStruct *) lptr->data);
	}
	g_list_free(repeat_part);
	/*
	DbgMsg(DBG_TESTING, "nach clone:\n");
	for (lptr1 = g_list_first(m_midi_events); lptr1; lptr1 = g_list_next(lptr1)) {
		DbgMsg(DBG_TESTING, "###\n");
		lptr2 = (GList *) g_list_first((GList *) (lptr1->data));
		for(; lptr2; lptr2 = g_list_next(lptr2)) {
			printMidiEvent((MidiEventStruct *) lptr2->data);
		}
	}
	*/
}

void NedResource::deleteAllBetween(GList *start_ptr, GList *end_ptr /* , unsigned long long time_diff */) {
	GList *lptr2, *lptr3;
	GList *next_ptr;
	GList *pendingOnOffs = NULL;

	if (start_ptr == end_ptr) return;

	lptr2 = ((MidiListStruct *) start_ptr->data)->notes;
	lptr3 = g_list_first(lptr2);
	while (lptr3) {
		if (((MidiEventStruct *) lptr3->data)->type != SND_SEQ_EVENT_NOTEON && ((MidiEventStruct *) lptr3->data)->type != PSEUDO_EVENT_MEASURE) {
			pendingOnOffs = g_list_append(pendingOnOffs, lptr3->data);
		}
		lptr2 = g_list_delete_link(lptr2, lptr3);
		lptr3 = g_list_first(lptr2);
	}
	((MidiListStruct *) start_ptr->data)->notes = lptr2;

	while (start_ptr != end_ptr) {
		next_ptr = g_list_next(start_ptr);
		lptr2 = ((MidiListStruct *) start_ptr->data)->notes;
		for (lptr3 = g_list_first(lptr3); lptr3; lptr3 = g_list_next(lptr3)) {
			g_free(lptr3->data);
		}
		g_list_free(lptr2);
		m_midi_events = g_list_delete_link(m_midi_events, start_ptr);
		start_ptr = next_ptr;
	}

	lptr2 = ((MidiListStruct *) start_ptr->data)->notes;
	lptr3 = g_list_first(lptr2);
	while (lptr3) {
		if (((MidiEventStruct *) lptr3->data)->type != SND_SEQ_EVENT_NOTEOFF && ((MidiEventStruct *) lptr3->data)->type != PSEUDO_EVENT_MEASURE) {
			pendingOnOffs = g_list_append(pendingOnOffs, lptr3->data);
		}
		lptr2 = g_list_delete_link(lptr2, lptr3);
		lptr3 = g_list_first(lptr2);
	}
	((MidiListStruct *) start_ptr->data)->notes = lptr2;
	for (lptr2 = g_list_first(pendingOnOffs); lptr2; lptr2 = g_list_next(lptr2)) {
		placeMidiEvent((MidiEventStruct *) lptr2->data);
	}
	g_list_free(pendingOnOffs);
}

void NedResource::addTimeOffsetAfter(GList *end_ptr, unsigned long long time_diff) {
	GList *lptr2, *lptr3;
	GList *pendingOns = NULL;

	if (g_list_position(m_midi_events, end_ptr) < 0) {
		Abort("addTimeOffsetAfter(1)");
	}
	lptr2 = ((MidiListStruct *) end_ptr->data)->notes;
	lptr3 = g_list_first(lptr2);
	while (lptr3) {
		if (((MidiEventStruct *) lptr3->data)->type != SND_SEQ_EVENT_NOTEON && ((MidiEventStruct *) lptr3->data)->type != PSEUDO_EVENT_MEASURE) {
			lptr3 = g_list_next(lptr3);
		}
		else {
			((MidiEventStruct *) lptr3->data)->midi_time += time_diff;
			pendingOns = g_list_append(pendingOns, lptr3->data);
			lptr2 = g_list_delete_link(lptr2, lptr3);
			lptr3 = g_list_first(lptr2);
		}
	}
	((MidiListStruct *) end_ptr->data)->notes = lptr2;
	/*
	lptr3 = end_ptr;
	end_ptr = g_list_next(end_ptr);
	if (lptr2 == NULL) {
		if (g_list_position(m_midi_events, lptr3) < 0) {
			Abort("addTimeOffsetAfter(3)");
		}
		m_midi_events = g_list_delete_link(m_midi_events, lptr3);
	}
	*/
	for (end_ptr = g_list_next(end_ptr); end_ptr; end_ptr = g_list_next(end_ptr)) {
		lptr2 = g_list_first(((MidiListStruct *) end_ptr->data)->notes);
		for (;lptr2; lptr2 = g_list_next(lptr2)) {
			((MidiEventStruct *) lptr2->data)->midi_time += time_diff;
		}
	}
	for (lptr2 = g_list_first(pendingOns); lptr2; lptr2 = g_list_next(lptr2)) {
		placeMidiEvent((MidiEventStruct *) lptr2->data);
	}
	g_list_free(pendingOns);
}

void NedResource::subTimeOffsetAfter(GList *end_ptr, unsigned long long time_diff) {
	GList *lptr2;

	if (g_list_position(m_midi_events, end_ptr) < 0) {
		Abort("addTimeOffsetAfter(1)");
	}
	for (end_ptr = g_list_next(end_ptr); end_ptr; end_ptr = g_list_next(end_ptr)) {
		lptr2 = g_list_first(((MidiListStruct *) end_ptr->data)->notes);
		for (;lptr2; lptr2 = g_list_next(lptr2)) {
			((MidiEventStruct *) lptr2->data)->midi_time -= time_diff;
		}
	}
}

GList * NedResource::cloneAllMidiEventsBetween(GList *start_ptr, GList *end_ptr, unsigned long long extra_offs) {
	GList *lptr2;
	int start_pos, end_pos;
	GList *repeat_part = NULL;
	MidiEventStruct *ev;


	if ((start_pos = g_list_position (m_midi_events, start_ptr)) < 0) {
		Abort("cloneAllMidiEventsBetween(1)");
	}

	if ((end_pos = g_list_position (m_midi_events, end_ptr)) < 0) {
		Abort("cloneAllMidiEventsBetween(2)");
	}

	if (end_pos <= start_pos) {
		DbgMsg(DBG_TESTING, "start_pos = %d, end_pos = %d\n", start_pos, end_pos); 
		Abort("cloneAllMidiEventsBetween(3)");
	}

	lptr2 = g_list_first(((MidiListStruct *) start_ptr->data)->notes);
	for (;lptr2; lptr2 = g_list_next(lptr2)) {
		if (((MidiEventStruct *) lptr2->data)->type == SND_SEQ_EVENT_NOTEOFF) continue;
		if (((MidiEventStruct *) lptr2->data)->type == PSEUDO_EVENT_MEASURE) continue;
		if ((ev = (MidiEventStruct *) g_try_malloc(sizeof(MidiEventStruct))) == NULL) {
			Abort("cloneAllMidiEventsBetween: allocation failed");
		}
		*ev = *(((MidiEventStruct *) lptr2->data));
		ev->midi_time += extra_offs;
		ev->repetition = true;
		repeat_part = g_list_append(repeat_part, ev);
	}
	for (start_ptr = g_list_next(start_ptr); start_ptr && (start_ptr != end_ptr); start_ptr = g_list_next(start_ptr)) {
		lptr2 = g_list_first(((MidiListStruct *) start_ptr->data)->notes);
		for (;lptr2; lptr2 = g_list_next(lptr2)) {
			if (((MidiEventStruct *) lptr2->data)->type == PSEUDO_EVENT_MEASURE) continue;
			if ((ev = (MidiEventStruct *) g_try_malloc(sizeof(MidiEventStruct))) == NULL) {
				Abort("cloneAllMidiEventsBetween: allocation failed");
			}
			*ev = *(((MidiEventStruct *) lptr2->data));
			ev->midi_time += extra_offs;
			ev->repetition = true;
			repeat_part = g_list_append(repeat_part, ev);
		}
	}
	if (start_ptr && (start_ptr == end_ptr)) {
		lptr2 = g_list_first(((MidiListStruct *) start_ptr->data)->notes);
		for (;lptr2; lptr2 = g_list_next(lptr2)) {
			if (((MidiEventStruct *) lptr2->data)->type == SND_SEQ_EVENT_NOTEON) continue;
			if (((MidiEventStruct *) lptr2->data)->type == PSEUDO_EVENT_MEASURE) continue;
			if ((ev = (MidiEventStruct *) g_try_malloc(sizeof(MidiEventStruct))) == NULL) {
				Abort("cloneAllMidiEventsBetween: allocation error");
			}
			*ev = *(((MidiEventStruct *) lptr2->data));
			ev->midi_time += extra_offs;
			ev->repetition = true;
			repeat_part = g_list_append(repeat_part, ev);
		}
	}
	return repeat_part;
}

void NedResource::printMidiEvent(MidiEventStruct *ev) {
	bool ok = true;
	if (ev->type == PSEUDO_EVENT_MEASURE) return;
	DbgMsg(DBG_TESTING, "type = ");
	switch (ev->type) {
		case SND_SEQ_EVENT_NOTEON: DbgMsg(DBG_TESTING, "SND_SEQ_EVENT_NOTEON"); break;
		case SND_SEQ_EVENT_NOTEOFF: DbgMsg(DBG_TESTING, "SND_SEQ_EVENT_NOTEOFF"); break;
		case PSEUDO_EVENT_MEASURE: DbgMsg(DBG_TESTING, "PSEUDO_EVENT_MEASURE"); ok = false; break;
		default: DbgMsg(DBG_TESTING, "unknown %d(0x%x)", ev->type, ev->type);ok = false;  break;
	}
	DbgMsg(DBG_TESTING, ", time = %llu", ev->midi_time / NOTE_4);
	if (!ok) {
		if (ev->type == PSEUDO_EVENT_MEASURE) {
			DbgMsg(DBG_TESTING, ", measnum = %d\n", ev->measure->getMeasureNumber());
		}
		DbgMsg(DBG_TESTING, "\n"); 
		return;
	}
	DbgMsg(DBG_TESTING, ", chan = %d, pitch = %d, vol = %d\n", ev->channel, ev->data1, ev->data2); 
}

void NedResource::placeMidiEvent(MidiEventStruct *ev) {
	GList *lptr1, *lptr2;
	MidiListStruct *mls;

	if (m_midi_events == NULL) {
		if ((mls = (MidiListStruct *) g_try_malloc(sizeof(MidiListStruct))) == NULL) {
			Abort("NedResource::placeMidiEvent:alloc failed(1)");
		}
		mls->tempoinverse = -1.0;
		mls->notes = g_list_append(NULL, ev);
		m_midi_events = g_list_append(m_midi_events, mls);
		return;
	}

	for (lptr1 = g_list_first(m_midi_events); lptr1; lptr1 = g_list_next(lptr1)) {
		lptr2 = g_list_first(((MidiListStruct *) lptr1->data)->notes);
		if (lptr2 == NULL) continue;
		if (((MidiEventStruct *) lptr2->data)->midi_time == ev->midi_time) {
			((MidiListStruct *) lptr1->data)->notes = g_list_append(((MidiListStruct *) lptr1->data)->notes, ev);
			return;
		}
		if (((MidiEventStruct *) lptr2->data)->midi_time > ev->midi_time) {
			if ((mls = (MidiListStruct *) g_try_malloc(sizeof(MidiListStruct))) == NULL) {
				Abort("NedResource::placeMidiEvent:alloc failed(2)");
			}
			mls->tempoinverse = -1.0;
			mls->notes = g_list_append(NULL, ev);
			m_midi_events = g_list_insert_before(m_midi_events, lptr1, mls);
			return;
		}
	}
	if ((mls = (MidiListStruct *) g_try_malloc(sizeof(MidiListStruct))) == NULL) {
		Abort("NedResource::placeMidiEvent:alloc failed(3)");
	}
	mls->tempoinverse = -1.0;
	mls->notes = g_list_append(NULL, ev);
	m_midi_events = g_list_append(m_midi_events, mls);
}

void NedResource::collectTempoEvents(GList **tempo_chords) {
	GList *lptr1, *lptr2;
	MidiEventStruct *ev;

	for (lptr1 = g_list_first(m_midi_events); lptr1; lptr1 = g_list_next(lptr1)) {
		if (((MidiListStruct *) lptr1->data)->tempoinverse < 0.0) continue;
		lptr2 = g_list_first(((MidiListStruct *) lptr1->data)->notes);
		if (lptr2 == NULL) continue;
		if ((ev = (MidiEventStruct *) g_try_malloc(sizeof(MidiEventStruct))) == NULL) {
			Abort("NedResource::addTempoChange: allocation failed");
		}
		ev->channel = 0; /* dummy */
		ev->segno_sign = 0;
		ev->midi_time = ((MidiEventStruct *) lptr2->data)->midi_time;
		ev->type = PSEUDO_EVENT_TEMPO_CHANGE;
		ev->tempoinverse = ((MidiListStruct *) lptr1->data)->tempoinverse;
		*tempo_chords = g_list_append(*tempo_chords, ev);
	}
}

void NedResource::startReplay(NedNote *startNote, double tempo_inverse) {
	GList *lptr1, *lptr2, *lptr22;
	unsigned long long time_diff;
	unsigned int sleeptime = 0;
	MidiEventStruct *m_event;
	NedNote *notes[1024];
	int num_notes = 0;
	bool found;
	bool first = true;
	struct timeval now;

	handleVolAndTempoChanges(tempo_inverse); // sets tempo inverse
	removeAllPseudosEvents();

	for (int i = 0; i < 16; m_channel_pgms[i++] = 255); // leads to midi pgm change with first note on channel


	if (m_midi_events == NULL) {
		m_window_with_last_focus->stopReplay();
		return;
	}
	m_run_midi_in = false;


	m_play_ptr = g_list_first(m_midi_events);
	if (startNote != NULL) {
		found = FALSE;
		for (m_play_ptr = g_list_first(m_midi_events); !found && m_play_ptr; m_play_ptr = g_list_next(m_play_ptr)) {
			lptr2 = g_list_first(((MidiListStruct *) m_play_ptr->data)->notes);
			for (; lptr2; lptr2 = g_list_next(lptr2)) {
				if (((MidiEventStruct *) lptr2->data)->type == SND_SEQ_EVENT_NOTEON && ((MidiEventStruct *) lptr2->data)->note == startNote) {
					// note! this excludes SND_SEQ_EVENT_NOTEONs with PSEUDO2_FLAG
					found = true;
					break;
				}
				if (((MidiListStruct *) m_play_ptr->data)->tempoinverse > 0.0) {
					m_tempo_inverse = ((MidiListStruct *) m_play_ptr->data)->tempoinverse;
				}
			}
			if (found) break;
		}
		if (!found) {
			m_play_ptr = g_list_first(m_midi_events);
		}
	}

	m_playing = TRUE;
	m_window_with_last_focus->repaint(); // repaint without aux lines;

	do {
	   lptr2 = g_list_first(((MidiListStruct *) m_play_ptr->data)->notes);
	   lptr1 = g_list_next(m_play_ptr);
	   if (lptr1 != NULL) {
		lptr22 = ((MidiListStruct *) lptr1->data)->notes;
		time_diff = ((MidiEventStruct *) lptr22->data)->midi_time - ((MidiEventStruct *) lptr2->data)->midi_time;
		sleeptime = (unsigned int) ((double) time_diff / (double) NOTE_4 * m_tempo_inverse);
	   }
	   for (; lptr2; lptr2 = g_list_next(lptr2)) {
		switch ((m_event = ((MidiEventStruct *) lptr2->data))->type) {
			// note! this excludes SND_SEQ_EVENT_NOTEONs with PSEUDO2_FLAG
			case SND_SEQ_EVENT_NOTEON: if (m_event->note->getTieBackward() == NULL) {
							NoteOn(m_event->channel, m_event->data1, m_event->pgm, m_event->data2);
						   }
							if (first) {
								first = false;
								m_window_with_last_focus->setVisible(m_event->note);
							}
							((MidiEventStruct *) lptr2->data)->note->m_active = NOTE_ACTIVE_STATE_ON;
							notes[num_notes++] = m_event->note;
							break;
			case SND_SEQ_EVENT_NOTEOFF:     if (m_event->note->getTieForward() == NULL) {
				// note! this excludes SND_SEQ_EVENT_NOTEOFFs with PSEUDO2_FLAG
								NoteOff(m_event->channel, m_event->data1);
							}
							m_event->note->m_active = NOTE_ACTIVE_STATE_OFF;
							notes[num_notes++] = m_event->note;
							break;
			case SND_SEQ_EVENT_CONTROLLER:  MidiCtrl(m_event->data1, m_event->channel, m_event->data2);
							break;
		}
	   }
	   m_play_ptr = lptr1;
	}
	while (num_notes < 1 && m_play_ptr != NULL);
	if (m_play_ptr == NULL) {
		m_window_with_last_focus->stopReplay();
		return;
	}
	//m_window_with_last_focus->setVisible(notes[0]);
	m_window_with_last_focus->repaintDuringReplay(notes, num_notes);

	if (m_play_ptr) {
		gettimeofday(&now, NULL);
		addtime(&now, sleeptime, &m_expected_time);
		m_handler_id = g_timeout_add (sleeptime, playNext, NULL);
		//m_playing = TRUE; is already done
	}
	else {
		m_playing = FALSE;
		m_window_with_last_focus->stopReplay();
	}
}

unsigned int NedResource::subtime(struct timeval *future, struct timeval *now) {
	struct timeval t;
	if (timercmp(future, now, >)) {
		timersub(future, now, &t);
		return t.tv_usec;
	}
	return 0;
}

void NedResource::addtime(struct timeval *now, unsigned int msecs, struct timeval *res) {
	struct timeval t;

	t.tv_sec = 0; 
	t.tv_usec = msecs * 1000;
	timeradd(now, &t, res);
}

gboolean NedResource::playNext(void *data) {
	GList *lptr1, *lptr2, *lptr22;
	MidiEventStruct *m_event;
	unsigned int sleeptime;
	unsigned long long time_diff = 0;
	bool stop_replay = false;
	NedNote *notes[1024];
	int num_notes = 0;
	struct timeval now;
	g_source_remove (m_handler_id);
	if (m_play_ptr == NULL) return FALSE;
	if (!m_playing) {
		if (m_midi_events != NULL) {
			for (lptr1 = g_list_first(m_midi_events); lptr1; lptr1 = g_list_next(lptr1)) {
				for (lptr2 = g_list_first(((MidiListStruct *) lptr1->data)->notes); lptr2; lptr2 = g_list_next(lptr2)) {
					if (((MidiEventStruct *) lptr2->data)->type == SND_SEQ_EVENT_NOTEON) { // note! this excludes SND_SEQ_EVENT_NOTEONs with PSEUDO2_FLAG
						NoteOff(((MidiEventStruct *) lptr2->data)->channel, ((MidiEventStruct *) lptr2->data)->data1);
						((MidiEventStruct *) lptr2->data)->note->m_active = NOTE_ACTIVE_STATE_OFF;
					}
					
				}
			}
		}
		SeqReset();
		m_window_with_last_focus->resetActiveFlags();
		m_window_with_last_focus->repaint();
		return FALSE;
	}


	lptr2 = g_list_first(((MidiListStruct *) m_play_ptr->data)->notes);
	lptr1 = g_list_next(m_play_ptr);
	if (lptr1 != NULL) {
		lptr22 = ((MidiListStruct *) lptr1->data)->notes;
		time_diff = ((MidiEventStruct *) lptr22->data)->midi_time - ((MidiEventStruct *) lptr2->data)->midi_time;
	}
	gettimeofday(&now, NULL);
	usleep(subtime(&m_expected_time, &now));
	for (; lptr2; lptr2 = g_list_next(lptr2)) {
		switch (((m_event = (MidiEventStruct *) lptr2->data))->type) {
			// note! this excludes SND_SEQ_EVENT_NOTEOFFs with PSEUDO2_FLAG
			case SND_SEQ_EVENT_NOTEOFF: if (m_event->note->getTieForward() == NULL ||
								(m_event->note->getChord()->getStatus() & STAT_TRILL)) {
							NoteOff(m_event->channel, m_event->data1);
						    }
							m_event->note->m_active = NOTE_ACTIVE_STATE_OFF;
							notes[num_notes++] = m_event->note;
							if ((m_event->segno_sign & SEGNO_FINE2) != 0) stop_replay = true;
							break;
			case (SND_SEQ_EVENT_NOTEOFF | PSEUDO2_FLAG): // rest with PSEUDO_FLAG
							if ((m_event->segno_sign & SEGNO_FINE2) != 0) stop_replay = true;
							break;
		}
	}
	if (stop_replay) {
		m_playing = FALSE;
		stopReplay();
		m_window_with_last_focus->stopReplay();
		return FALSE;
	}
	lptr2 = g_list_first(((MidiListStruct *) m_play_ptr->data)->notes);
	for (; lptr2; lptr2 = g_list_next(lptr2)) {
		switch ((m_event = ((MidiEventStruct *) lptr2->data))->type) {
			// note! this excludes SND_SEQ_EVENT_NOTEONs with PSEUDO2_FLAG
			case SND_SEQ_EVENT_NOTEON: if (m_event->note->getTieBackward() == NULL || (m_event->note->getTieBackward()->getChord()->getStatus() & STAT_TRILL)) {
								NoteOn(m_event->channel, m_event->data1, m_event->pgm, m_event->data2); 
						   }
							m_window_with_last_focus->setVisible(m_event->note);

							m_event->note->m_active = NOTE_ACTIVE_STATE_ON;
							notes[num_notes++] = m_event->note;
							break;
			case SND_SEQ_EVENT_CONTROLLER:  MidiCtrl(m_event->data1, m_event->channel, m_event->data2);
							break;
		}
		if (((MidiListStruct *) m_play_ptr->data)->tempoinverse > 0.0) {
			m_tempo_inverse = ((MidiListStruct *) m_play_ptr->data)->tempoinverse;
		}
	}
	sleeptime = (unsigned int) ((double) time_diff / (double) NOTE_4 * m_tempo_inverse);
	gettimeofday(&now, NULL);
	addtime(&now, sleeptime, &m_expected_time);
	/*
	if (num_notes > 0) {
		m_window_with_last_focus->setVisible(notes[0]);
	}
	*/

	m_window_with_last_focus->repaintDuringReplay(notes, num_notes);
	m_play_ptr = lptr1;

	if (m_play_ptr) {
		m_handler_id = g_timeout_add (sleeptime > 10 ? sleeptime - 10 : sleeptime, playNext, NULL);
	}
	else {
		m_playing = FALSE;
		m_window_with_last_focus->stopReplay();
	}
	return FALSE;
}

void NedResource::stopReplay() {
	m_playing = FALSE;
	SeqReset();
}

void NedResource::playImmediately(int channel, int pgm, int num_midi_input_notes, int *midi_input_chord, int vol) {
	snd_seq_event_t ev;

	if (m_sequ_out == NULL) return;
	if (m_avoid_immadiate_play) return;
	if (m_count_of_last_imm_pitches > 0) return;


	snd_seq_ev_clear(&ev);
	ev.type = SND_SEQ_EVENT_PGMCHANGE;
	ev.data.control.channel = channel;
	ev.data.control.value = pgm;
	snd_seq_ev_set_subs(&ev);  
	snd_seq_ev_set_direct(&ev);
	snd_seq_event_output_direct(m_sequ_out, &ev);
	for (m_count_of_last_imm_pitches = 0; m_count_of_last_imm_pitches < MAX_MIDI_INPUT_CHORD && m_count_of_last_imm_pitches < num_midi_input_notes;
			m_count_of_last_imm_pitches++) {
		snd_seq_ev_clear(&(m_last_imm_notes[m_count_of_last_imm_pitches]));
		m_last_imm_notes[m_count_of_last_imm_pitches].type = SND_SEQ_EVENT_NOTEON;
		m_last_imm_notes[m_count_of_last_imm_pitches].data.note.channel = channel;
		m_last_imm_notes[m_count_of_last_imm_pitches].data.note.velocity = vol;
		m_last_imm_notes[m_count_of_last_imm_pitches].data.note.note = midi_input_chord[m_count_of_last_imm_pitches];
		snd_seq_ev_set_subs(&(m_last_imm_notes[m_count_of_last_imm_pitches]));  
		snd_seq_ev_set_direct(&(m_last_imm_notes[m_count_of_last_imm_pitches]));
		snd_seq_event_output_direct(m_sequ_out, &(m_last_imm_notes[m_count_of_last_imm_pitches]));
	}
	g_timeout_add (200, stopImmediate, NULL);
}

gboolean NedResource::stopImmediate(void *data) {
	int i; 

	if (m_count_of_last_imm_pitches < 1) return FALSE;
	for (i = 0; i < m_count_of_last_imm_pitches; i++) {
		m_last_imm_notes[i].type = SND_SEQ_EVENT_NOTEOFF;
		m_last_imm_notes[i].data.note.velocity = 0;
		snd_seq_ev_set_subs(&(m_last_imm_notes[i]));  
		snd_seq_ev_set_direct(&(m_last_imm_notes[i]));
		snd_seq_event_output_direct(m_sequ_out, &(m_last_imm_notes[i]));
	}
	m_count_of_last_imm_pitches = 0;
	return FALSE;
}



void NedResource::startMidiExport(FILE *fp, NedMainWindow *main_window) {
	handleVolAndTempoChanges(main_window->getMidiTempoInverse());
	removeAllPseudosEvents();
	NedMidiExport::exportMidi(fp, main_window, NULL, m_midi_events);
}

void NedResource::appendTempFileName(char *fname) {
	m_temp_file_names = g_list_append(m_temp_file_names, strdup(fname));
}


struct AddrStruct {
	int marker;
	void *addr;
};

void NedResource::prepareAddrStruct() {
	GList *lptr;

	if (m_addr_rel_list != NULL) {
		for (lptr = g_list_first(m_addr_rel_list); lptr; lptr = g_list_next(lptr)) {
			g_free(lptr->data);
		}
		g_list_free(m_addr_rel_list);
		m_addr_rel_list = NULL;
	}
	m_marker_counter = 1;
}

void NedResource::addAddr(int marker, void *addr) {
	AddrStruct *adrstruct;

	if ((adrstruct = (AddrStruct *) g_try_malloc(sizeof(AddrStruct))) == NULL) {
		Abort("NedResource::addAddr: allocation failed");
	}
	adrstruct->marker = marker;
	adrstruct->addr = addr;
	m_addr_rel_list = g_list_append(m_addr_rel_list, adrstruct);
}

void *NedResource::getAdressOfMarker(int marker) {
	GList *lptr;

	for (lptr = g_list_first(m_addr_rel_list); lptr; lptr = g_list_next(lptr)) {
		if (((AddrStruct *) lptr->data)->marker == marker) {
			return ((AddrStruct *) lptr->data)->addr;
		}
	}
	DbgMsg(DBG_CRITICAL, "marker = %d\n", marker);
	Abort("NedResource::getAdressOfMarker");
	return NULL;
}

int NedResource::getMarkerOfAddress(void *addr) {
	GList *lptr;

	for (lptr = g_list_first(m_addr_rel_list); lptr; lptr = g_list_next(lptr)) {
		if (((AddrStruct *) lptr->data)->addr == addr) {
			return ((AddrStruct *) lptr->data)->marker;
		}
	}
	Abort("NedResource::getMarkerOfAddress");
	return 0;
}

int NedResource::addAddr(void *addr) {
	AddrStruct *adrstruct;

	if ((adrstruct = (AddrStruct *) g_try_malloc(sizeof(AddrStruct))) == NULL) {
		Abort("NedResource::addAddr: allocation failed");
	}
	adrstruct->marker = m_marker_counter++;
	adrstruct->addr = addr;
	m_addr_rel_list = g_list_append(m_addr_rel_list, adrstruct);
	return adrstruct->marker;
}
		
struct paper_info_struct *NedResource::getPaperInfo(const char *papername) {
	paper_info_struct *ptr;

	for (ptr = m_paper_info; ptr->name != NULL; ptr++) {
		if (!strcmp(ptr->name, papername)) return ptr;
	}
	return NULL;
}

GList *NedResource::clone_chords_and_rests(GList *group, bool remove_completely_hidden_voices) {
	GList *copylist = NULL;
	GList *lptr;
	NedVoice *voi;
	NedChordOrRest *chord_or_rest;
	struct addr_ref_str *addrlist = NULL, *addr_ptr;
	struct voice_list_struct {
		NedVoice *voice;
		bool hidden;
		struct voice_list_struct *next;
	} *voice_list = NULL, *vl_ptr;

	if (remove_completely_hidden_voices) {
		for (lptr = g_list_first(group); lptr; lptr = g_list_next(lptr)) {
			voi = ((NedChordOrRest *) lptr->data)->getVoice();
			for (vl_ptr = voice_list; vl_ptr != NULL; vl_ptr = vl_ptr->next) {
				if (vl_ptr->voice == voi) {
					break;
				}
			}
			if (vl_ptr == NULL) {
				vl_ptr = (struct voice_list_struct *) g_alloca(sizeof(struct voice_list_struct));
				vl_ptr->voice = voi;
				vl_ptr->hidden = true;
				vl_ptr->next = voice_list;
				voice_list = vl_ptr;
			}
			if (!((NedChordOrRest *) lptr->data)->isHidden()) {
				vl_ptr->hidden = false;
			}
		}
	}
					
	for (lptr = g_list_first(group); lptr; lptr = g_list_next(lptr)) {
		if (remove_completely_hidden_voices) {
			voi = ((NedChordOrRest *) lptr->data)->getVoice();
			for (vl_ptr = voice_list; vl_ptr != NULL; vl_ptr = vl_ptr->next) {
				if (vl_ptr->voice == voi) {
					break;
				}
			}
			if (vl_ptr == NULL) {
				Abort("NedResource::clone_chords_and_rests");
			}
			if (vl_ptr->hidden) continue;
		}
		chord_or_rest = ((NedChordOrRest *) lptr->data)->clone(&addrlist, false);
		copylist = g_list_append(copylist, chord_or_rest);
	}
	for (lptr = g_list_first(copylist); lptr; lptr = g_list_next(lptr)) {
		((NedChordOrRest *) lptr->data)->adjust_pointers(addrlist, NULL);
	}
	while (addrlist != NULL) {
		addr_ptr = addrlist->next;
		g_free(addrlist);
		addrlist = addr_ptr;
	}
	return copylist;
}

void NedResource::split_element(GList **elements, GList *sp_element, unsigned long long delta1, unsigned long long delta2) {
	NedChordOrRest *chord_or_rest1, *chord_or_rest2, *chord_or_rest_before;
	int pos, dotcount;
	unsigned int len2;

	if ((pos = g_list_position(*elements, sp_element)) < 0) {
		Abort("NedResource::split_element");
	}
	chord_or_rest1 = (NedChordOrRest *) sp_element->data;
	chord_or_rest1->changeDuration(delta1, 0);
	delta1 -= chord_or_rest1->getDuration();
	chord_or_rest_before = chord_or_rest1;
	setLengthField(delta1);
	while ((len2 = getPartLength(&dotcount)) > 0) {
		pos++;
		chord_or_rest2 = chord_or_rest1->cloneWithDifferentLength(len2, dotcount);
		chord_or_rest_before->tieCompleteTo(chord_or_rest2, false);
		chord_or_rest_before = chord_or_rest2;
		*elements = g_list_insert(*elements, chord_or_rest2, pos);
		delta1 -= len2;
	}
	setLengthField(delta2);
	while ((len2 = getPartLength(&dotcount)) > 0) {
		pos++;
		chord_or_rest2 = chord_or_rest1->cloneWithDifferentLength(len2, dotcount);
		chord_or_rest_before->tieCompleteTo(chord_or_rest2, false);
		chord_or_rest_before = chord_or_rest2;
		*elements = g_list_insert(*elements, chord_or_rest2, pos);
		delta2 -= len2;
	}
	chord_or_rest_before->correctTiesForward();
}

unsigned int NedResource::getPartLength(int *dotcount) {
	unsigned int ret = 0, m;
	*dotcount = 0;

		
	if (m_length_field_value >= NOTE_2) {
		if (m_length_field_value >= NOTE_2 * 7 / 4) {
			*dotcount = 2;
			ret = NOTE_2;
		}
		if (m_length_field_value >= NOTE_2 * 3 / 2) {
			*dotcount = 1;
			ret = NOTE_2;
		}
		ret = NOTE_2;
	}
	else {
		for (m = NOTE_2; m >= NOTE_64; m >>= 1) {
			if (m_length_field_value >= NOTE_16 && m_length_field_value >= m * 7 / 4) {
				*dotcount = 2;
				ret = m;
				break;
			}
			if (m_length_field_value >= NOTE_32 && m_length_field_value >= m * 3 / 2) {
				*dotcount = 1;
				ret = m;
				break;
			}
			if (m_length_field_value >= m) {
				ret = m;
				break;
			}
		}
	}
	if (*dotcount == 2) {
		m_length_field_value -= ret * 7 / 4;
	}
	else if (*dotcount == 1) {
		m_length_field_value -= ret * 3 / 2;
	}
	else {
		m_length_field_value -= ret;
	}
	return ret;
}
unsigned int NedResource::getPartLength(int *dotcount, bool is_rest, bool build_whole_rests, unsigned int measure_duration) {
	unsigned int ret = 0, m;
	*dotcount = 0;

		
	if (m_length_field_value >= NOTE_2) {
		if (is_rest && build_whole_rests && m_length_field_value >= WHOLE_NOTE) {
			m_length_field_value -= measure_duration;
			return WHOLE_NOTE;
		}
		else if (!is_rest && m_length_field_value >= WHOLE_NOTE) {
			ret = WHOLE_NOTE;
		}
		else {
			if (m_length_field_value >= NOTE_2 * 7 / 4) {
				*dotcount = 2;
				ret = NOTE_2;
			}
			if (m_length_field_value >= NOTE_2 * 3 / 2) {
				*dotcount = 1;
				ret = NOTE_2;
			}
			ret = NOTE_2;
		}
	}
	else {
		for (m = NOTE_2; m >= NOTE_64; m >>= 1) {
			if (m_length_field_value >= NOTE_16 && m_length_field_value >= m * 7 / 4) {
				*dotcount = 2;
				ret = m;
				break;
			}
			if (m_length_field_value >= NOTE_32 && m_length_field_value >= m * 3 / 2) {
				*dotcount = 1;
				ret = m;
				break;
			}
			if (m_length_field_value >= m) {
				ret = m;
				break;
			}
		}
	}
	if (*dotcount == 2) {
		m_length_field_value -= ret * 7 / 4;
	}
	else if (*dotcount == 1) {
		m_length_field_value -= ret * 3 / 2;
	}
	else {
		m_length_field_value -= ret;
	}
	return ret;
}

void NedResource::setLengthField2(unsigned long long measure_start, unsigned long long note_start, unsigned int length_field_value) {

	m_part_start = note_start - measure_start;
	m_part_end = m_part_start + length_field_value;

}


unsigned int NedResource::getPartLength2(int *dotcount) {
	unsigned int l, l1;

	*dotcount = 0;

	if (m_part_start == m_part_end) return 0;

	for (l = NOTE_2; l >= NOTE_64; l >>= 1) {
		if ((m_part_start % l) == 0 && m_part_start + l <= m_part_end) {
/*
			if (l > NOTE_16 && m_part_start + l * 7 / 4 <= m_part_end) {
				m_part_start += l * 7 / 4;
				*dotcount = 2;
			}
			else */
			if (l > NOTE_32) {
				for (l1 = NOTE_2; l1 >= NOTE_64; l1 >>= 1) {
					if (((m_part_start + l) % l1) == 0 && m_part_start + l + l1 <= m_part_end) {
						if (l == l1 << 1) {
                                			m_part_start += l + l1;
                                			*dotcount = 1;
							return l;
						}
						else {
							break;
						}
					}
				}

                        }
			m_part_start += l;
			return l;
		}
	}
	return 0;
}

unsigned int NedResource::getPartLength2(int *dotcount, bool is_rest, bool build_whole_rests, unsigned int measure_duration) {
	unsigned int l, l1;

	*dotcount = 0;

	if (m_part_start == m_part_end) return 0;

	if (m_part_end - m_part_start >= NOTE_2) {
		if (is_rest && build_whole_rests && m_part_end - m_part_start >= WHOLE_NOTE) {
			m_part_start += measure_duration;
			return WHOLE_NOTE;
		}
		else if (!is_rest && m_part_end - m_part_start >= WHOLE_NOTE) {
			m_part_start += WHOLE_NOTE;
			return WHOLE_NOTE;
		}
	}
	for (l = NOTE_2; l >= NOTE_64; l >>= 1) {
		if ((m_part_start % l) == 0 && m_part_start + l <= m_part_end) {
/*
			if (l > NOTE_16 && m_part_start + l * 7 / 4 <= m_part_end) {
				m_part_start += l * 7 / 4;
				*dotcount = 2;
			}
			else */
			if (l > NOTE_32) {
				for (l1 = NOTE_2; l1 >= NOTE_64; l1 >>= 1) {
					if (((m_part_start + l) % l1) == 0 && m_part_start + l + l1 <= m_part_end) {
						if (l == l1 << 1) {
                                			m_part_start += l + l1;
                                			*dotcount = 1;
							return l;
						}
						else {
							break;
						}
					}
				}

                        }
			m_part_start += l;
			return l;
		}
	}
	return 0;
}

bool NedResource::fittingDuration(unsigned int duration) {
	unsigned int m, d;

	for (m = WHOLE_NOTE; m >= NOTE_64; m >>= 1) {
		d = duration / m;
		if (m * d == duration) return true;
	}
	return false;
}

bool NedResource::fittingPosition(unsigned long long position) {
	unsigned long long m;

	m = position / NOTE_64;
	return m * NOTE_64 == position;
}


bool NedResource::test_for_incomplete_tuplets(GList *elements) {
	GList *lptr1, *lptr2;
	NedTuplet *tuplet;

	for (lptr1 = g_list_first(elements); lptr1; lptr1 = g_list_next(lptr1)) {
		if (((NedChordOrRest *) lptr1->data)->getTupletVal() != 0) {
			if ((tuplet = ((NedChordOrRest *) lptr1->data)->getTupletPtr()) == NULL) {
				Abort("NedResource::test_for_incomplete_tuplets");
			}
			for (lptr2 = tuplet->getElementList(); lptr2; lptr2 = g_list_next(lptr2)) {
				if (g_list_find(elements, lptr2->data) == NULL) {
					return TRUE;
				}
			}
		}
	}
	return FALSE;
}

bool NedResource::selection_has_uncomplete_tuplets(GList *elements) {
	NedTuplet *tuplet;
	GList *lptr1, *lptr2;

	for (lptr1 = g_list_first(elements); lptr1; lptr1 = g_list_next(lptr1)) {
		if ((tuplet = ((NedChordOrRest *) lptr1->data)->getTupletPtr()) == NULL) continue;
		for (lptr2 = tuplet->getElementList(); lptr2; lptr2 = g_list_next(lptr2)) {
			if (g_list_find(elements, lptr2->data) == NULL) return TRUE;
		}
	}
	return FALSE;
}

int NedResource::determineLastLine(int treble_line, int clef) {
	switch (clef) {
		case BASS_CLEF: treble_line += 12; break;
		case NEUTRAL_CLEF1:
		case NEUTRAL_CLEF2: 
		case NEUTRAL_CLEF3: 
		case ALTO_CLEF: treble_line += 6; break;
		case TENOR_CLEF: treble_line += 8; break;
		case SOPRAN_CLEF: treble_line += 2; break;
	}
	return treble_line;
}

const char *NedResource::getLilyPondClefName(int clef_number) {
	switch (clef_number) {
		case TREBLE_CLEF: return "\\clef treble";
		case BASS_CLEF: return "\\clef bass";
		case ALTO_CLEF: return "\\clef alto";
		case SOPRAN_CLEF: return "\\clef soprano";
		case TENOR_CLEF: return "\\clef tenor";
		case NEUTRAL_CLEF1: 
		case NEUTRAL_CLEF2: return "\\clef percussion";
		case NEUTRAL_CLEF3: return "";
	}
	return "\\clef treble";
}
const char *NedResource::getLilyPondKeySigName(int keysig_number) {
	switch (keysig_number) {
		case -6: return "\\key ges \\major";
		case -5: return "\\key des \\major";
		case -4: return "\\key as \\major";
		case -3: return "\\key es \\major";
		case -2: return "\\key bes \\major";
		case -1: return "\\key f \\major";
		case  1: return "\\key g \\major";
		case  2: return "\\key d \\major";
		case  3: return "\\key a \\major";
		case  4: return "\\key e \\major";
		case  5: return "\\key b \\major";
		case  6: return "\\key fis \\major";
	}
	return "";
}


void NedResource::writeLilyFreeChord(FILE *fp, NedChordName *chordname, unsigned int length, unsigned int duration, int dotcount, unsigned int *midi_len) {
	char Str[128], *cptr;
	int i;
	bool minor = false;
	bool upper = false;

	strcpy(Str, chordname->getRootText());
		
	for (cptr = Str; *cptr; cptr++) {
		*cptr = tolower(*cptr);
	}
	if (Str[1] == 'm') {
		minor = true;
		Str[1] = '\0';
	}
	for (cptr = Str; *cptr; cptr++) {
		if (*cptr == '\\') {
			switch (*(cptr + 1)) {
				case 'b': *cptr++ = 'e'; *cptr++ = 's'; break;
				case '#': *cptr++ = 'i'; *cptr++ = 's'; break;
			}
		}
	}

	fprintf(fp, "%s", Str);
	if (duration != *midi_len) {
		sprintf(Str, "%d", WHOLE_NOTE / length);
		for (i = 0; i < dotcount; i++) {
			strcat(Str, ".");
		}
		fprintf(fp, Str);
	}
	*midi_len = duration;
	if (chordname->getUpText() != '\0') {
		fprintf(fp, ":%s", chordname->getUpText());
		upper = true;
	}
	if (minor) {
		Str[0] = 'm';
		Str[1] = '\0';
		if (chordname->getDownText() != '\0') {
			strcat(Str, chordname->getDownText());
		}
		if (!upper) {
			putc(':', fp);
		}
		fprintf(fp, "%s", Str);
	}
	else {
		if (chordname->getDownText() != '\0') {
			putc(':', fp);
			fprintf(fp, "%s", chordname->getDownText());
		}
	}
	putc(' ', fp);
}
	


void NedResource::showLicense(GtkWindow *parent) {
	const gchar *the_text = "This program is free software; you can redistribute it and/or modify it under the\n"
	"terms of the GNU General Public License as published by the Free Software\n"
	"Foundation; either version 2 of the License, or (at your option) any later version.\n"
	"\n"
	"This program is distributed in the hope that it will be useful, but WITHOUT ANY\n"
	"WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A\n"
	"PARTICULAR PURPOSE. See the GNU General Public License for more details.\n"
	"\n"
	"You should have received a copy of the GNU General Public License along with this\n"
	"program; (See \"COPYING\"). If not, If not, see <http://www.gnu.org/licenses/>.";
	GtkWidget *license_text = gtk_text_view_new();
	GtkTextBuffer *textbuffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (license_text));
	gtk_text_buffer_set_text (textbuffer, the_text, -1);
	gtk_text_view_set_editable(GTK_TEXT_VIEW (license_text), FALSE);
	GtkWidget *license_dialog = gtk_dialog_new_with_buttons("LICENSE", parent, (GtkDialogFlags) (GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT),
		GTK_STOCK_OK, GTK_RESPONSE_ACCEPT, NULL);
	g_signal_connect_swapped (license_dialog, "response", G_CALLBACK (gtk_widget_destroy), license_dialog);
	gtk_container_add (GTK_CONTAINER (GTK_DIALOG(license_dialog)->vbox), license_text);
	gtk_widget_show_all(license_dialog);
}
