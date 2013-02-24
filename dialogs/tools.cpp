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

#include <math.h>
#include <X11/Xlib.h>
#include <gdk/gdkkeysyms.h>
#include "tools.h"
#include "localization.h"
#include "resource.h"
#include "mainwindow.h"

#define IS_IT(b) ((row == the_dialog->m_current_row && column == the_dialog->m_current_column) ? b[1] : b[0])
#define IS_ITV(b) (((row == the_dialog->m_current_row || row == the_dialog->m_current_row - 1) && column == the_dialog->m_current_column) ? b[1] : b[0])
#define IS_ITH(b) ((row == the_dialog->m_current_row && (column == the_dialog->m_current_column || column == the_dialog->m_current_column - 1)) ? b[1] : b[0])

#define DEFAULT_BACKGROUND 0
#define HORIZ_BACKGROUND 1
#define VERTI_BACKGROUND 2
#define MINI_BACKGROUND  3

#define MINI_BUTTON_AREA_HEIGHT (3 * m_mini_spaces + 2 * m_back_miniimg_height)

#define NOTE_64_BUTTON       [0][ 2]
#define NATURAL_BUTTON       [0][ 4]
#define SHARP_BUTTON         [0][ 5]
#define FLAT_BUTTON          [0][ 6]
#define FERMATA_BUTTON       [0][ 7]
#define WHOLE_NOTE_BUTTON    [0][ 8]
#define HALF_NOTE_BUTTON     [0][ 9]
#define QUARTER_NOTE_BUTTON  [0][10]
#define NOTE_8_BUTTON        [0][11]
#define NOTE_16_BUTTON       [0][12]
#define NOTE_32_BUTTON       [0][13]
#define TIE_BUTTON           [0][14]
#define REST_BUTTON          [0][15]
#define DOT_BUTTON           [0][16]

#define DFLAT_BUTTON         [1][ 4]
#define DSHARP_BUTTON        [1][ 5]
#define DDOT_BUTTON          [1][ 6]
#define STROKEN_GRACE_BUTTON [1][ 8]
#define GRACE_8_BUTTON       [1][ 9]
#define GRACE_16_BUTTON      [1][10]
#define SFORZANDO_BUTTON     [1][11]
#define STACCATO_BUTTON      [1][12]
#define STACCATISSIMO_BUTTON [1][13]
#define SFORZATO_BUTTON      [1][15]
#define TENUTO_BUTTON        [1][16]

#define TURN_BUTTON          [2][ 2]
#define BOW_UP_BUTTON        [2][ 4]
#define BOW_DOWN_BUTTON      [2][ 5]
#define PED_ON_BUTTON        [2][ 6]
#define PED_OFF_BUTTON       [2][ 8]
#define TRILL_BUTTON         [2][ 9]
#define PRALL_BUTTON         [2][10]
#define OPEN_BUTTON          [2][11]
#define MORDENT_BUTTON       [2][12]
#define ARPEGGIO_BUTTON      [2][13]
#define REV_TURN_BUTTON      [2][16]

#define O_HIGH_NOTE_BUTTON   [3][ 1]
#define C_HIGH_NOTE_BUTTON   [3][ 2]
#define NORMAL_NOTE_BUTTON   [3][ 4]
#define CROSS_NOTE1_BUTTON   [3][ 5]
#define CROSS_NOTE2_BUTTON   [3][ 6]
#define RECT_NOTE1_BUTTON    [3][ 7]
#define RECT_NOTE2_BUTTON    [3][ 8]
#define TRIAG_NOTE1_BUTTON   [3][ 9]
#define TRIAG_NOTE2_BUTTON   [3][10]
#define CROSS_NOTE3_BUTTON   [3][11]
#define CROSS_NOTE4_BUTTON   [3][12]
#define TRIAG_NOTE3_BUTTON   [3][13]
#define QUAD_NOTE2_BUTTON    [3][14]
#define GUIT_STEM_BUTTON     [3][15]
#define GUIT_NO_STEM_BUTTON  [3][16]

#define TEMOLO1_NOTE_BUTTON  [4][ 4]
#define TEMOLO2_NOTE_BUTTON  [4][ 5]
#define TEMOLO3_NOTE_BUTTON  [4][ 6]
#define TEMOLO4_NOTE_BUTTON  [4][ 8]


X11button *NedTools::m_button_arrays[NUM_PANELS][17];
X11button *NedTools::m_mini_button_arrays[8];

X11buttonGroup NedTools::m_accidentals(true), NedTools::m_note_length_group(false), NedTools::m_note_head_group(false), NedTools::m_dot_group(true), NedTools::m_tremolo_group(true);

static GdkPixbuf *nullbuf = NULL;

NedTools::button_descr NedTools::m_mini_button_descriptions[8] = {
	{&NedResource::m_quarternote_icon, PUSH_BUTTON, SCOPE_NONE, false, NULL},
	{&NedResource::m_cross_icon, PUSH_BUTTON, SCOPE_NONE, false, NULL},
	{&NedResource::m_bow_down_icon, PUSH_BUTTON, SCOPE_NONE, false, NULL},
	{&NedResource::m_drum7_icon, PUSH_BUTTON, SCOPE_NONE, false, NULL},
	{&NedResource::m_tremolo_icon2, PUSH_BUTTON, SCOPE_NONE, false, NULL},
	{&nullbuf, EMPTY_BUTTON, SCOPE_NONE, false, NULL},
	{&nullbuf, EMPTY_BUTTON, SCOPE_NONE, false, NULL},
	{&nullbuf, EMPTY_BUTTON, SCOPE_NONE, false, NULL}
};


NedTools::button_descr NedTools::m_button_descriptions[NUM_PANELS][17] = {
   {
	{ &nullbuf, EMPTY_BUTTON, SCOPE_NONE, false, NULL }, { &nullbuf, EMPTY_BUTTON, SCOPE_NONE, false, NULL },
		{&NedResource::m_64thnote_icon, TOGGLE_BUTTON, SCOPE_NOTE_LENGTH, false, &m_note_length_group}, {&NedResource::m_arrow_right_icon, PUSH_BUTTON, SCOPE_NONE, false, NULL }, 
	{&NedResource::m_natural_icon, TOGGLE_BUTTON, SCOPE_NOTE_STATE, false, &m_accidentals}, {&NedResource::m_cross_icon, TOGGLE_BUTTON, SCOPE_NOTE_STATE, false, &m_accidentals},
		{&NedResource::m_flat_icon, TOGGLE_BUTTON, SCOPE_NOTE_STATE, false, &m_accidentals}, {&NedResource::m_fermata_icon, TOGGLE_BUTTON, SCOPE_STATE, false, NULL},
	{&NedResource::m_fullnote_icon, TOGGLE_BUTTON, SCOPE_NOTE_LENGTH, false, &m_note_length_group}, {&NedResource::m_halfnote_icon, TOGGLE_BUTTON, SCOPE_NOTE_LENGTH, false, &m_note_length_group},
		{&NedResource::m_quarternote_icon, TOGGLE_BUTTON, SCOPE_NOTE_LENGTH, true, &m_note_length_group},
	{&NedResource::m_eightnote_icon, TOGGLE_BUTTON, SCOPE_NOTE_LENGTH, false, &m_note_length_group}, {&NedResource::m_sixteenthnote_icon, TOGGLE_BUTTON, SCOPE_NOTE_LENGTH, false, &m_note_length_group},
		{&NedResource::m_32ndnote_icon, TOGGLE_BUTTON, SCOPE_NOTE_LENGTH, false, &m_note_length_group}, {&NedResource::m_tied_icon, TOGGLE_BUTTON, SCOPE_TIES,false, NULL},
	{&NedResource::m_rest_icon, TOGGLE_BUTTON, SCOPE_NONE, false, NULL}, {&NedResource::m_dot_icon, TOGGLE_BUTTON, SCOPE_DOTS, false, &m_dot_group}
   },

   {
	{ &nullbuf, EMPTY_BUTTON, SCOPE_NONE, false, NULL }, {&NedResource::m_arrow_left_icon, PUSH_BUTTON, SCOPE_NONE, false, NULL },
	{ &nullbuf, EMPTY_BUTTON, SCOPE_NONE, false, NULL }, { &NedResource::m_arrow_right_icon, PUSH_BUTTON, SCOPE_NONE, false, NULL },
	{&NedResource::m_dflat_icon, TOGGLE_BUTTON, SCOPE_NOTE_STATE, false, &m_accidentals}, {&NedResource::m_dcross_icon, TOGGLE_BUTTON, SCOPE_NOTE_STATE, false, &m_accidentals},
		{&NedResource::m_ddot_icon, TOGGLE_BUTTON, SCOPE_DOTS, false, &m_dot_group}, {&nullbuf, EMPTY_BUTTON, SCOPE_NONE, false, NULL},
	{&NedResource::m_stroken_grace_icon, TOGGLE_BUTTON, SCOPE_NOTE_LENGTH, false, &m_note_length_group}, {&NedResource::m_grace_eighth_icon, TOGGLE_BUTTON, SCOPE_NOTE_LENGTH, false, &m_note_length_group},
		{&NedResource::m_grace_sixth_icon, TOGGLE_BUTTON, SCOPE_NOTE_LENGTH, false, &m_note_length_group},
	{ &NedResource::m_sforzando_icon, TOGGLE_BUTTON, SCOPE_STATE, false, NULL }, {&NedResource::m_stacc_icon, TOGGLE_BUTTON, SCOPE_STATE, false, NULL},
		{&NedResource::m_staccatissimo_icon, TOGGLE_BUTTON, SCOPE_STATE, false, NULL}, {&nullbuf, EMPTY_BUTTON, SCOPE_STATE, false, NULL},
	{&NedResource::m_sforzato_icon, TOGGLE_BUTTON, SCOPE_STATE, false, NULL}, {&NedResource::m_tenuto_icon, TOGGLE_BUTTON, SCOPE_STATE, false, NULL}
   },

   {
	{ &nullbuf, EMPTY_BUTTON, SCOPE_NONE, false, NULL }, {&NedResource::m_arrow_left_icon, PUSH_BUTTON, SCOPE_NONE, false, NULL }, 
		{ &NedResource::m_turn_icon, TOGGLE_BUTTON, SCOPE_STATE, false, NULL }, { &NedResource::m_arrow_right_icon, PUSH_BUTTON, SCOPE_NONE, false, NULL },
	{&NedResource::m_bow_up_icon, TOGGLE_BUTTON, SCOPE_STATE, false, NULL}, {&NedResource::m_bow_down_icon, TOGGLE_BUTTON, SCOPE_STATE, false, NULL},
		{&NedResource::m_ped_on_icon, TOGGLE_BUTTON, SCOPE_STATE, false, NULL}, {&nullbuf, EMPTY_BUTTON, SCOPE_NONE, false, NULL},
	{&NedResource::m_ped_off_icon, TOGGLE_BUTTON, SCOPE_STATE, false, NULL}, {&NedResource::m_trill_icon, TOGGLE_BUTTON, SCOPE_STATE, false, NULL},
		{&NedResource::m_prall_icon, TOGGLE_BUTTON, SCOPE_STATE, false, NULL},
	{&NedResource::m_open_icon, TOGGLE_BUTTON, SCOPE_STATE, false, NULL}, {&NedResource::m_mordent_icon, TOGGLE_BUTTON, SCOPE_STATE, false, NULL},
		{&NedResource::m_arpeggio_icon, TOGGLE_BUTTON, SCOPE_STATE, false, NULL}, {&nullbuf, EMPTY_BUTTON, SCOPE_NONE, false, NULL},
	{&nullbuf, EMPTY_BUTTON, SCOPE_NONE, false, NULL}, {&NedResource::m_revturn_icon, TOGGLE_BUTTON, SCOPE_STATE, false, NULL}
   },

   {
	{&NedResource::m_arrow_left_icon, PUSH_BUTTON, SCOPE_NONE, false, NULL }, { &NedResource::m_drum11_icon, TOGGLE_BUTTON, SCOPE_NOTE_HEAD, false, &m_note_head_group},
		 { &NedResource::m_drum12_icon, TOGGLE_BUTTON, SCOPE_NOTE_HEAD, false, &m_note_head_group }, { &NedResource::m_arrow_right_icon, PUSH_BUTTON, SCOPE_NONE, false, NULL },
	{&NedResource::m_normal_note_icon, TOGGLE_BUTTON, SCOPE_NOTE_HEAD, true, &m_note_head_group}, {&NedResource::m_drum1_icon, TOGGLE_BUTTON, SCOPE_NOTE_HEAD, false, &m_note_head_group},
		{&NedResource::m_drum2_icon, TOGGLE_BUTTON, SCOPE_NOTE_HEAD, false, &m_note_head_group}, {&NedResource::m_drum3_icon, TOGGLE_BUTTON, SCOPE_NOTE_HEAD, false, &m_note_head_group},
	{&NedResource::m_drum4_icon, TOGGLE_BUTTON, SCOPE_NOTE_HEAD, false, &m_note_head_group}, {&NedResource::m_drum5_icon, TOGGLE_BUTTON, SCOPE_NOTE_HEAD, false, &m_note_head_group},
		{&NedResource::m_drum6_icon, TOGGLE_BUTTON, SCOPE_NOTE_HEAD, false, &m_note_head_group},
	{&NedResource::m_drum7_icon, TOGGLE_BUTTON, SCOPE_NOTE_HEAD, false, &m_note_head_group}, {&NedResource::m_drum8_icon, TOGGLE_BUTTON, SCOPE_NOTE_HEAD, false, &m_note_head_group},
		{&NedResource::m_drum9_icon, TOGGLE_BUTTON, SCOPE_NOTE_HEAD, false, &m_note_head_group}, {&NedResource::m_drum10_icon, TOGGLE_BUTTON, SCOPE_NOTE_HEAD, false, &m_note_head_group},
	{&NedResource::m_guitar_note_icon, TOGGLE_BUTTON, SCOPE_NOTE_HEAD, false, &m_note_head_group}, {&NedResource::m_guitar_note_no_stem_icon, TOGGLE_BUTTON, SCOPE_NOTE_HEAD, false, &m_note_head_group}
   },

   {
	{ &nullbuf, EMPTY_BUTTON, SCOPE_NONE, false, NULL }, {&NedResource::m_arrow_left_icon, PUSH_BUTTON, SCOPE_NONE, false, NULL }, 
		{ &nullbuf, EMPTY_BUTTON, SCOPE_NONE, false, NULL }, { &nullbuf, EMPTY_BUTTON, SCOPE_NONE, false, NULL },
	{&NedResource::m_tremolo_icon1, TOGGLE_BUTTON, SCOPE_STATE, false, &m_tremolo_group}, {&NedResource::m_tremolo_icon2, TOGGLE_BUTTON, SCOPE_STATE, false, &m_tremolo_group},
		{&NedResource::m_tremolo_icon3, TOGGLE_BUTTON, SCOPE_STATE, false, &m_tremolo_group}, {&nullbuf, EMPTY_BUTTON, SCOPE_NONE, false, NULL },
	{&NedResource::m_tremolo_icon4, TOGGLE_BUTTON, SCOPE_STATE, false, &m_tremolo_group}, {&nullbuf, EMPTY_BUTTON, SCOPE_NONE, false, NULL },
		{&nullbuf, EMPTY_BUTTON, SCOPE_NONE, false, NULL },
	{&nullbuf, EMPTY_BUTTON, SCOPE_NONE, false, NULL }, {&nullbuf, EMPTY_BUTTON, SCOPE_NONE, false, NULL },
		{&nullbuf, EMPTY_BUTTON, SCOPE_NONE, false, NULL }, {&nullbuf, EMPTY_BUTTON, SCOPE_NONE, false, NULL },
	{&nullbuf, EMPTY_BUTTON, SCOPE_NONE, false, NULL }, {&nullbuf, EMPTY_BUTTON, SCOPE_NONE, false, NULL }
   }

};


int NedTools::m_back_img_width, NedTools::m_back_img_height;
int NedTools::m_back_miniimg_width, NedTools::m_back_miniimg_height;
int NedTools::m_mini_spaces;

X11button::X11button(Display *dpy, Window win, GC xgc, int screen, XImage *img[2], int xpos, int ypos, int width, int height, int column, int row, int panel, int type, int scope, bool enabled, bool default_state) :
	m_button_group(NULL), m_active(default_state), m_type(type), m_row(row), m_column(column), m_panel(panel),  m_dpy(dpy), m_win(win), m_xgc(xgc), m_screen(screen), 
	m_enabled(enabled), m_default_state(default_state), m_scope(scope), m_xpos(xpos), m_ypos(ypos), m_width(width), m_height(height) {
		m_img[0] = img[0];
		m_img[1] = img[1];
}

X11button::~X11button() {
	XDestroyImage(m_img[0]);
	XDestroyImage(m_img[1]);
	m_img[0] = NULL;
	m_img[1] = NULL;

}




void X11button::draw() {
	XPutImage(m_dpy, m_win, m_xgc, m_img[m_active ? 1 : 0], 0, 0, m_xpos, m_ypos, m_width, m_height);
}

void X11button::notifyMainWindow(NedMainWindow *current_main_window) {
	switch (m_scope) {
		case SCOPE_NOTE_LENGTH: current_main_window->changeLength(); break;
		case SCOPE_STATE: current_main_window->changeState(); break;
		case SCOPE_NOTE_STATE: current_main_window->changeNoteState(); break;
		case SCOPE_NOTE_HEAD: current_main_window->changeNoteHead(); break;
		case SCOPE_DOTS: current_main_window->changeDots(); break;
		case SCOPE_TIES: current_main_window->changeTieState(); break;
	}
}

bool X11button::handle_click(gdouble xd, gdouble yd) {
	NedMainWindow *current_main_window;
	if (!m_enabled) return false;
	int x = (int) (xd + 0.5);
	int y = (int) (yd + 0.5);
	if (x < m_xpos) return false;
	if (x > m_xpos + m_width) return false;
	if (y < m_ypos) return false;
	if (y > m_ypos + m_height) return false;
	if (m_active && m_button_group) {
		if (!m_button_group->allowStateChange(this)) return false;
	}
	m_active = !m_active;
	if (m_button_group) {
		m_button_group->stateChanged(this);
	}
	current_main_window = NedResource::getMainwWindowWithLastFocus();
	if (current_main_window != NULL) {
		notifyMainWindow(current_main_window);
	}
	return m_active;
}

void X11button::toggle() {
	NedMainWindow *current_main_window;
	if (!m_enabled) return;
	if (m_type != TOGGLE_BUTTON) return;
	if (m_active && m_button_group) {
		if (!m_button_group->allowStateChange(this)) return;
	}
	m_active = !m_active;
	if (m_button_group) {
		m_button_group->stateChanged(this);
	}
	current_main_window = NedResource::getMainwWindowWithLastFocus();
	if (current_main_window != NULL) {
		notifyMainWindow(current_main_window);
	}
}


bool X11button::handle_release(gdouble xd, gdouble yd) {
	if (m_type != PUSH_BUTTON) return false;
	if (!m_enabled) return false;
	int x = (int) (xd + 0.5);
	int y = (int) (yd + 0.5);
	if (x < m_xpos) return false;
	if (x > m_xpos + m_width) return false;
	if (y < m_ypos) return false;
	if (y > m_ypos + m_height) return false;
	m_active = false;
	return true;
}

X11buttonGroup::X11buttonGroup(bool all_inactive_allowed) : m_buttons(NULL), m_all_inactive_allowed(all_inactive_allowed) {}

void X11buttonGroup::removeAllButtons()  {
	if (m_buttons != NULL) {
/*
		while ((lptr = g_list_first(m_buttons)) != NULL) {
			delete (X11button *) lptr->data;
		}
*/
		g_list_free(m_buttons);
		m_buttons = NULL;
	}
}

void X11buttonGroup::addX11button(X11button *button) {
	if (button->m_button_group != NULL) {
		NedResource::Abort("X11buttonGroup::addX11button(1)");
	}

	if (g_list_find(m_buttons, button) != NULL) {
		NedResource::Abort("X11buttonGroup::addX11button(2)");
	}
	m_buttons = g_list_append(m_buttons, button);
	button->m_button_group = this;
}

void X11buttonGroup::removeX11button(X11button *button) {
	GList *lptr;

	if (button->m_button_group == NULL) {
		NedResource::Abort("X11buttonGroup::removeX11button(1)");
	}
	if (button->m_button_group != this) {
		NedResource::Abort("X11buttonGroup::removeX11button(2)");
	}

	if ((lptr = g_list_find(m_buttons, button)) == NULL) {
		NedResource::Abort("X11buttonGroup::removeX11button(3)");
	}
	m_buttons = g_list_delete_link(m_buttons, lptr);
	button->m_button_group = NULL;
}

bool X11buttonGroup::allowStateChange(X11button *button) {
	GList *lptr;
	int c = 0;
	bool isAmongActives = false;
	if (m_all_inactive_allowed) return true;

	for (lptr = g_list_first(m_buttons); lptr; lptr = g_list_next(lptr)) {
		if (!((X11button *) lptr->data)->m_active) continue;
		c++;
		if ((X11button *) lptr->data == button) isAmongActives = true;
	}

	if (c > 1) return true;
	return false;
}
		
void X11buttonGroup::stateChanged(X11button *button) {
	GList *lptr;
	if (!button->m_active) return;
	if (button->m_type == PUSH_BUTTON) return;

	for (lptr = g_list_first(m_buttons); lptr; lptr = g_list_next(lptr)) {
		if ((X11button *) lptr->data == button) continue;
		((X11button *) lptr->data)->m_active = false;
	}
}

void X11buttonGroup::resetAllButtons() {
	GList *lptr;

	for (lptr = g_list_first(m_buttons); lptr; lptr = g_list_next(lptr)) {
		((X11button *) lptr->data)->m_active = false;
	}
}

#define CREATE_X11(pixmap, back, up) createX11PixmapFromGdkPixbuffer(dpy, screen, d, ggc, gdk_pixbuf_scale_simple(pixmap, X11_ICONS_DIM, X11_ICONS_DIM, GDK_INTERP_HYPER), back, up);

#define CREATE_MINI_X11(pixmap, back, up) createX11PixmapFromGdkPixbuffer(dpy, screen, d, ggc, gdk_pixbuf_scale_simple(pixmap, X11_MINI_ICONS_DIM, X11_MINI_ICONS_DIM, GDK_INTERP_HYPER), back, up);
	
#define CREATE_X11_EMPTY(back, up) createX11PixmapFromGdkPixbuffer(dpy, screen, d, ggc, NULL, back, up);

#define CREATE_MINI_X11_EMPTY(back, up) createX11PixmapFromGdkPixbuffer(dpy, screen, d, ggc, NULL, back, up);

NedTools::NedTools() :
	m_back_pix(NULL), m_current_row(-1), m_current_column(-1), m_current_panel(0), m_buttons_created(false), m_dpy(NULL)
		 {
	GdkColor bgcolor;
	int wx, wy;
	NedMainWindow *last_main;
	GdkDisplay *display = NULL;
	GdkScreen *screen = NULL;

	m_toolbox = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title (GTK_WINDOW (m_toolbox), _("Toolbox"));
    	g_signal_connect (m_toolbox, "delete-event", G_CALLBACK (close_toolbox), this);

	
	g_signal_connect(m_toolbox, "delete-event", G_CALLBACK (OnDelete), (void *) this);
        g_signal_connect (m_toolbox, "key-press-event", G_CALLBACK (key_press_handler), (void *) this);

	m_back_img_width = gdk_pixbuf_get_width(NedResource::m_background1);
	m_back_img_height = gdk_pixbuf_get_height(NedResource::m_background1);
	m_back_miniimg_width = gdk_pixbuf_get_width(NedResource::m_mini_background1);
	m_back_miniimg_height = gdk_pixbuf_get_height(NedResource::m_mini_background1);
	m_mini_spaces = (4 * m_back_img_width - 4 * m_back_miniimg_width) / 5;

	if ((last_main = NedResource::getMainwWindowWithLastFocus()) != NULL) {
		display = gdk_display_get_default ();
		screen = gdk_display_get_default_screen (display);
		gdk_window_get_root_origin(GDK_WINDOW(last_main->getWindow()->window), &wx, &wy);
		gtk_window_move(GTK_WINDOW (m_toolbox), wx + NedResource::m_toolboxX, wy + NedResource::m_toolboxY);
	}

	m_button_frame = gtk_frame_new(_("Toolbox"));
	m_button_area = gtk_drawing_area_new ();
	bgcolor.pixel = 1;
	bgcolor.red = 0x0000; bgcolor.green =  0x5555; bgcolor.blue =  0xcccc;
	gtk_widget_modify_bg(GTK_WIDGET(m_button_area), GTK_STATE_NORMAL, &bgcolor);
        gtk_widget_set_double_buffered(m_button_area, FALSE);
	gtk_drawing_area_size(GTK_DRAWING_AREA(m_button_area), 4 * m_back_img_width, MINI_BUTTON_AREA_HEIGHT + 5 * m_back_img_height);
	
	
	gtk_container_add (GTK_CONTAINER(m_button_frame), m_button_area);

	g_signal_connect (m_button_area, "expose-event", G_CALLBACK (draw_menu), (void *) this);
	gtk_widget_add_events(m_button_area, GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK | GDK_KEY_PRESS_MASK);
	g_signal_connect (m_button_area, "button-press-event", G_CALLBACK (OnButtonPress), (void *) this);
	g_signal_connect (m_button_area, "button-release-event", G_CALLBACK (OnButtonRelease), (void *) this);
    	gtk_container_add (GTK_CONTAINER (m_toolbox), m_button_frame);
	gtk_window_set_resizable (GTK_WINDOW(m_toolbox), FALSE);
    	gtk_widget_show_all (m_toolbox);
}


NedTools::~NedTools() {
	int panel;
	int bu_nr;

	for (panel = 0; panel < NUM_PANELS; panel++) {
	   for (bu_nr = 0; bu_nr < 4; bu_nr++) {
	   	delete m_button_arrays[panel][bu_nr];
		m_button_arrays[panel][bu_nr] = NULL;
          }
	}
	m_accidentals.removeAllButtons();
	m_note_length_group.removeAllButtons();
	m_note_head_group.removeAllButtons();
	m_dot_group.removeAllButtons();
	m_tremolo_group.removeAllButtons();
}

void NedTools::updatePosition() {
	NedMainWindow *last_main;
	int wx, wy, tx, ty;

	if ((last_main = NedResource::getMainwWindowWithLastFocus()) != NULL) {
		gdk_window_get_root_origin(GDK_WINDOW(last_main->getWindow()->window), &wx, &wy);
		gdk_window_get_root_origin(GDK_WINDOW(m_toolbox->window), &tx, &ty);
		NedResource::m_toolboxX = tx - wx;
		NedResource::m_toolboxY = ty - wy;
	}
}

bool NedTools::close_toolbox(GtkWidget *widget, GdkEvent  *event, gpointer data) {
	NedTools *tools = (NedTools *) data;
	tools->updatePosition();

	return FALSE;
}


void NedTools::Show() {
	gtk_widget_show_all (m_toolbox);
}


void NedTools::setToolboxTransient(NedMainWindow *m_win) {
	int wx, wy;
	GdkDisplay *display = NULL;
	GdkScreen *screen = NULL;

	gtk_window_set_transient_for(GTK_WINDOW (m_toolbox), (GTK_WINDOW (m_win->getWindow())));

	display = gdk_display_get_default ();
	screen = gdk_display_get_default_screen (display);
	gdk_window_get_root_origin(GDK_WINDOW(m_win->getWindow()->window), &wx, &wy);
	gtk_window_move(GTK_WINDOW (m_toolbox), wx + NedResource::m_toolboxX, wy + NedResource::m_toolboxY);

}

unsigned int NedTools::getCurrentLength() {
	if (m_button_arrays[0][0] == NULL) return NOTE_4;

	if (m_button_arrays WHOLE_NOTE_BUTTON ->m_active) return WHOLE_NOTE;
	if (m_button_arrays HALF_NOTE_BUTTON ->m_active) return NOTE_2;
	if (m_button_arrays QUARTER_NOTE_BUTTON ->m_active) return NOTE_4;
	if (m_button_arrays NOTE_8_BUTTON ->m_active) return NOTE_8;
	if (m_button_arrays NOTE_16_BUTTON ->m_active) return NOTE_16;
	if (m_button_arrays NOTE_32_BUTTON ->m_active) return NOTE_32;
	if (m_button_arrays NOTE_64_BUTTON ->m_active) return NOTE_64;
	if (m_button_arrays STROKEN_GRACE_BUTTON ->m_active) return STROKEN_GRACE;
	if (m_button_arrays GRACE_8_BUTTON ->m_active) return GRACE_8;
	if (m_button_arrays GRACE_16_BUTTON ->m_active) return GRACE_16;
	return NOTE_4;
}

int NedTools::getCurrentNoteHead() {
	if (m_button_arrays[0][0] == NULL) return NORMAL_NOTE;

	if (m_button_arrays O_HIGH_NOTE_BUTTON ->m_active) return OPEN_HIGH_HAT;
	if (m_button_arrays C_HIGH_NOTE_BUTTON ->m_active) return CLOSED_HIGH_HAT;
	if (m_button_arrays NORMAL_NOTE_BUTTON ->m_active) return NORMAL_NOTE;
	if (m_button_arrays CROSS_NOTE1_BUTTON ->m_active) return CROSS_NOTE1;
	if (m_button_arrays CROSS_NOTE2_BUTTON ->m_active) return CROSS_NOTE2;
	if (m_button_arrays RECT_NOTE1_BUTTON ->m_active) return RECT_NOTE1;
	if (m_button_arrays RECT_NOTE2_BUTTON ->m_active) return RECT_NOTE2;
	if (m_button_arrays TRIAG_NOTE1_BUTTON ->m_active) return TRIAG_NOTE1;
	if (m_button_arrays TRIAG_NOTE2_BUTTON ->m_active) return TRIAG_NOTE2;
	if (m_button_arrays CROSS_NOTE3_BUTTON ->m_active) return CROSS_NOTE3;
	if (m_button_arrays CROSS_NOTE4_BUTTON ->m_active) return CROSS_NOTE4;
	if (m_button_arrays TRIAG_NOTE3_BUTTON ->m_active) return TRIAG_NOTE3;
	if (m_button_arrays QUAD_NOTE2_BUTTON ->m_active) return QUAD_NOTE2;
	if (m_button_arrays GUIT_STEM_BUTTON ->m_active) return GUITAR_NOTE_STEM;
	if (m_button_arrays GUIT_NO_STEM_BUTTON ->m_active) return GUITAR_NOTE_NO_STEM;
	return NORMAL_NOTE;
}

unsigned int NedTools::getStatus() {
	unsigned int status = 0;

	if (m_button_arrays[0][0] == NULL) return 0;

	if (m_button_arrays NATURAL_BUTTON ->m_active) status |= STAT_NATURAL;
	if (m_button_arrays SHARP_BUTTON ->m_active) status |= STAT_SHARP;
	if (m_button_arrays FLAT_BUTTON ->m_active) status |= STAT_FLAT;
	if (m_button_arrays FERMATA_BUTTON ->m_active) status |= STAT_FERMATA;
	if (m_button_arrays SFORZANDO_BUTTON ->m_active) status |= STAT_SFORZANDO;
	if (m_button_arrays DFLAT_BUTTON ->m_active) status |= STAT_DFLAT;
	if (m_button_arrays DSHARP_BUTTON ->m_active) status |= STAT_DSHARP;
	if (m_button_arrays STACCATO_BUTTON ->m_active) status |= STAT_STACC;
	if (m_button_arrays STACCATISSIMO_BUTTON ->m_active) status |= STAT_STACCATISSIMO;
	if (m_button_arrays SFORZATO_BUTTON ->m_active) status |= STAT_SFORZATO;
	if (m_button_arrays TENUTO_BUTTON ->m_active) status |= STAT_TENUTO;
	if (m_button_arrays TURN_BUTTON ->m_active) status |= STAT_TURN;
	if (m_button_arrays BOW_UP_BUTTON ->m_active) status |= STAT_BOW_UP;
	if (m_button_arrays BOW_DOWN_BUTTON ->m_active) status |= STAT_BOW_DOWN;
	if (m_button_arrays PED_ON_BUTTON ->m_active) status |= STAT_PED_ON;
	if (m_button_arrays PED_OFF_BUTTON ->m_active) status |= STAT_PED_OFF;
	if (m_button_arrays TRILL_BUTTON ->m_active) status |= STAT_TRILL;
	if (m_button_arrays PRALL_BUTTON ->m_active) status |= STAT_PRALL;
	if (m_button_arrays OPEN_BUTTON ->m_active) status |= STAT_OPEN;
	if (m_button_arrays MORDENT_BUTTON ->m_active) status |= STAT_MORDENT;
	if (m_button_arrays ARPEGGIO_BUTTON ->m_active) status |= STAT_ARPEGGIO;
	if (m_button_arrays REV_TURN_BUTTON ->m_active) status |= STAT_REV_TURN;
	if (m_button_arrays TEMOLO1_NOTE_BUTTON ->m_active) status |= STAT_TREMOLO1;
	if (m_button_arrays TEMOLO2_NOTE_BUTTON ->m_active) status |= STAT_TREMOLO2;
	if (m_button_arrays TEMOLO3_NOTE_BUTTON ->m_active) status |= STAT_TREMOLO3;
	if (m_button_arrays TEMOLO4_NOTE_BUTTON ->m_active) status |= STAT_TREMOLO4;
	return status;
}

bool NedTools::getRestMode() {
	if (m_button_arrays[0][0] == NULL) return false;

	return m_button_arrays REST_BUTTON ->m_active;
}

int NedTools::getDotCount() {
	if (m_button_arrays[0][0] == NULL) return 0;

	if (m_button_arrays DOT_BUTTON ->m_active) return 1;
	if (m_button_arrays DDOT_BUTTON ->m_active) return 2;
	return 0;
}

bool NedTools::getTiedMode() {
	if (m_button_arrays[0][0] == NULL) return 0;
	bool ret;

	ret = m_button_arrays TIE_BUTTON ->m_active;
	return ret;
}

void NedTools::setButtons(unsigned int length, int dotcount, unsigned int chord_status, unsigned int note_status, bool isRest, bool tie_forward, int note_head) {
	int p, i;
	if (m_button_arrays[0][0] == NULL) return;

	for (p = 0; p < NUM_PANELS; p++) {
		for (i = 0; i < 17; i++) {
			m_button_arrays[p][i]->m_active = false;
		}
	}
	m_button_arrays REST_BUTTON ->m_active = isRest;
	switch (dotcount) {
		case 2: m_button_arrays DDOT_BUTTON ->m_active = true; break;
		case 1: m_button_arrays DOT_BUTTON ->m_active = true; break;
	}
	if (!isRest) {
		switch (note_status & ACCIDENTAL_MASK) {
			case STAT_NATURAL: m_button_arrays NATURAL_BUTTON ->m_active = true; break;
			case STAT_DFLAT:  m_button_arrays DFLAT_BUTTON ->m_active = true; break;
			case STAT_FLAT: m_button_arrays FLAT_BUTTON ->m_active = true; break;
			case STAT_SHARP: m_button_arrays SHARP_BUTTON ->m_active = true; break;
			case STAT_DSHARP: m_button_arrays DSHARP_BUTTON ->m_active = true; break;
		}
	}
	
	if (chord_status & STAT_FERMATA) m_button_arrays FERMATA_BUTTON ->m_active = true;
	if (chord_status & STAT_SFORZANDO) m_button_arrays SFORZANDO_BUTTON ->m_active = true;
	if (chord_status & STAT_STACC) m_button_arrays STACCATO_BUTTON ->m_active = true;
	if (chord_status & STAT_STACCATISSIMO) m_button_arrays STACCATISSIMO_BUTTON ->m_active = true;
	if (chord_status & STAT_SFORZATO) m_button_arrays SFORZATO_BUTTON ->m_active = true;
	if (chord_status & STAT_TENUTO) m_button_arrays TENUTO_BUTTON ->m_active = true;
	if (chord_status & STAT_TURN) m_button_arrays TURN_BUTTON ->m_active = true;
	if (chord_status & STAT_BOW_UP) m_button_arrays BOW_UP_BUTTON ->m_active = true;
	if (chord_status & STAT_BOW_DOWN) m_button_arrays BOW_DOWN_BUTTON ->m_active = true;
	if (chord_status & STAT_PED_ON) m_button_arrays PED_ON_BUTTON ->m_active = true;
	if (chord_status & STAT_PED_OFF) m_button_arrays PED_OFF_BUTTON ->m_active = true;
	if (chord_status & STAT_TRILL) m_button_arrays TRILL_BUTTON ->m_active = true;
	if (chord_status & STAT_PRALL) m_button_arrays PRALL_BUTTON ->m_active = true;
	if (chord_status & STAT_OPEN) m_button_arrays OPEN_BUTTON ->m_active = true;
	if (chord_status & STAT_MORDENT) m_button_arrays MORDENT_BUTTON ->m_active = true;
	if (chord_status & STAT_ARPEGGIO) m_button_arrays ARPEGGIO_BUTTON ->m_active = true;
	if (chord_status & STAT_REV_TURN) m_button_arrays REV_TURN_BUTTON ->m_active = true;
	if ((chord_status & TREMOLO_MASK) == STAT_TREMOLO1) m_button_arrays TEMOLO1_NOTE_BUTTON ->m_active = true;
	if ((chord_status & TREMOLO_MASK) == STAT_TREMOLO2) m_button_arrays TEMOLO2_NOTE_BUTTON ->m_active = true;
	if ((chord_status & TREMOLO_MASK) == STAT_TREMOLO3) m_button_arrays TEMOLO3_NOTE_BUTTON ->m_active = true;
	if ((chord_status & TREMOLO_MASK) == STAT_TREMOLO4) m_button_arrays TEMOLO4_NOTE_BUTTON ->m_active = true;

	if (length == WHOLE_NOTE) m_button_arrays WHOLE_NOTE_BUTTON ->m_active = true;
	else if (length == NOTE_2) m_button_arrays HALF_NOTE_BUTTON ->m_active = true;
	else if (length == NOTE_4) m_button_arrays QUARTER_NOTE_BUTTON ->m_active = true;
	else if (length == NOTE_8) m_button_arrays NOTE_8_BUTTON ->m_active = true;
	else if (length == NOTE_16) m_button_arrays NOTE_16_BUTTON ->m_active = true;
	else if (length == NOTE_32) m_button_arrays NOTE_32_BUTTON ->m_active = true;
	else if (length == NOTE_64) m_button_arrays NOTE_64_BUTTON ->m_active = true;
	else if (length == STROKEN_GRACE) m_button_arrays STROKEN_GRACE_BUTTON ->m_active = true;
	else if (length == GRACE_8) m_button_arrays GRACE_8_BUTTON ->m_active = true;
	else if (length == GRACE_16) m_button_arrays GRACE_16_BUTTON ->m_active = true;
	else m_button_arrays QUARTER_NOTE_BUTTON ->m_active = true;

	if (note_head == NORMAL_NOTE) m_button_arrays NORMAL_NOTE_BUTTON ->m_active = true;
	else if (note_head == OPEN_HIGH_HAT) m_button_arrays O_HIGH_NOTE_BUTTON ->m_active = true;
	else if (note_head == CLOSED_HIGH_HAT) m_button_arrays C_HIGH_NOTE_BUTTON ->m_active = true;
	else if (note_head == CROSS_NOTE1) m_button_arrays CROSS_NOTE1_BUTTON ->m_active = true;
	else if (note_head == CROSS_NOTE2) m_button_arrays CROSS_NOTE2_BUTTON ->m_active = true;
	else if (note_head == RECT_NOTE1) m_button_arrays RECT_NOTE1_BUTTON ->m_active = true;
	else if (note_head == RECT_NOTE2) m_button_arrays RECT_NOTE2_BUTTON ->m_active = true;
	else if (note_head == TRIAG_NOTE1) m_button_arrays TRIAG_NOTE1_BUTTON ->m_active = true;
	else if (note_head == TRIAG_NOTE2) m_button_arrays TRIAG_NOTE2_BUTTON ->m_active = true;
	else if (note_head == CROSS_NOTE3) m_button_arrays CROSS_NOTE3_BUTTON ->m_active = true;
	else if (note_head == CROSS_NOTE4) m_button_arrays CROSS_NOTE4_BUTTON ->m_active = true;
	else if (note_head == TRIAG_NOTE3) m_button_arrays TRIAG_NOTE3_BUTTON ->m_active = true;
	else if (note_head == QUAD_NOTE2) m_button_arrays QUAD_NOTE2_BUTTON ->m_active = true;
	else if (note_head == GUITAR_NOTE_STEM) m_button_arrays GUIT_STEM_BUTTON ->m_active = true;
	else if (note_head == GUITAR_NOTE_NO_STEM) m_button_arrays GUIT_NO_STEM_BUTTON ->m_active = true;
	else m_button_arrays NORMAL_NOTE_BUTTON ->m_active = true;


	m_button_arrays TIE_BUTTON ->m_active = tie_forward;
	gdk_window_invalidate_rect(m_button_area->window, NULL, FALSE);

}

void NedTools::setNoteLengthButtons(unsigned int length) {
	if (m_button_arrays[0][0] == NULL) return;

	m_button_arrays WHOLE_NOTE_BUTTON ->m_active = false;
	m_button_arrays HALF_NOTE_BUTTON ->m_active = false;
	m_button_arrays QUARTER_NOTE_BUTTON ->m_active = false;
	m_button_arrays NOTE_8_BUTTON ->m_active = false;
	m_button_arrays NOTE_16_BUTTON ->m_active = false;
	m_button_arrays NOTE_32_BUTTON ->m_active = false;
	m_button_arrays NOTE_64_BUTTON ->m_active = false;
	m_button_arrays STROKEN_GRACE_BUTTON ->m_active = false;
	m_button_arrays GRACE_8_BUTTON ->m_active = false;
	m_button_arrays GRACE_16_BUTTON ->m_active = false;

	if (length == WHOLE_NOTE) m_button_arrays WHOLE_NOTE_BUTTON ->m_active = true;
	else if (length == NOTE_2) m_button_arrays HALF_NOTE_BUTTON ->m_active = true;
	else if (length == NOTE_4) m_button_arrays QUARTER_NOTE_BUTTON ->m_active = true;
	else if (length == NOTE_8) m_button_arrays NOTE_8_BUTTON ->m_active = true;
	else if (length == NOTE_16) m_button_arrays NOTE_16_BUTTON ->m_active = true;
	else if (length == NOTE_32) m_button_arrays NOTE_32_BUTTON ->m_active = true;
	else if (length == NOTE_64) m_button_arrays NOTE_64_BUTTON ->m_active = true;
	else m_button_arrays QUARTER_NOTE_BUTTON ->m_active = true;

	gdk_window_invalidate_rect(m_button_area->window, NULL, FALSE);

}

void NedTools::resetSomeButtons() {
	if (m_button_arrays[0][0] == NULL) return;
	m_button_arrays NATURAL_BUTTON ->m_active = false;
	m_button_arrays DFLAT_BUTTON ->m_active = false;
	m_button_arrays FLAT_BUTTON ->m_active = false;
	m_button_arrays SHARP_BUTTON ->m_active = false;
	m_button_arrays REST_BUTTON ->m_active = false;
	m_button_arrays DSHARP_BUTTON ->m_active = false;

	m_button_arrays DDOT_BUTTON ->m_active = false;
	m_button_arrays DOT_BUTTON ->m_active = false;

	m_button_arrays TIE_BUTTON ->m_active = false;

	m_button_arrays FERMATA_BUTTON ->m_active = false;
	m_button_arrays STACCATO_BUTTON ->m_active = false;
	m_button_arrays STACCATISSIMO_BUTTON ->m_active = false;
	m_button_arrays SFORZATO_BUTTON ->m_active = false;
	m_button_arrays SFORZANDO_BUTTON ->m_active = false;
	m_button_arrays TENUTO_BUTTON ->m_active = false;
	m_button_arrays TURN_BUTTON ->m_active = false;
	m_button_arrays BOW_UP_BUTTON ->m_active = false;
	m_button_arrays BOW_DOWN_BUTTON ->m_active = false;
	m_button_arrays PED_ON_BUTTON ->m_active = false;
	m_button_arrays PED_OFF_BUTTON ->m_active = false;
	m_button_arrays TRILL_BUTTON ->m_active = false;
	m_button_arrays PRALL_BUTTON ->m_active = false;
	m_button_arrays OPEN_BUTTON ->m_active = false;
	m_button_arrays MORDENT_BUTTON ->m_active = false;
	m_button_arrays ARPEGGIO_BUTTON ->m_active = false;
	m_button_arrays REV_TURN_BUTTON ->m_active = false;
	m_button_arrays TEMOLO1_NOTE_BUTTON ->m_active = false;
	m_button_arrays TEMOLO2_NOTE_BUTTON ->m_active = false;
	m_button_arrays TEMOLO3_NOTE_BUTTON ->m_active = false;
	m_button_arrays TEMOLO4_NOTE_BUTTON ->m_active = false;
	gdk_window_invalidate_rect(m_button_area->window, NULL, FALSE);
}

void NedTools::resetTieButton() {
	if (m_button_arrays[0][0] == NULL) return;
	m_button_arrays TIE_BUTTON ->m_active = false;
	gdk_window_invalidate_rect(m_button_area->window, NULL, FALSE);
}

void NedTools::resetButtons() {
	int p, i;
	if (m_button_arrays[0][0] == NULL) return;

	for (p = 0; p < NUM_PANELS; p++) {
		for (i = 0; i < 17; i++) {
			m_button_arrays[p][i]->m_active = m_button_descriptions[p][i].m_default_state;
		}
	}
	gdk_window_invalidate_rect(m_button_area->window, NULL, FALSE);
}

void NedTools::toggleButton(int nr) {
	m_button_arrays[m_current_panel][nr]->toggle();
	testForPanelChange(m_button_arrays[m_current_panel][nr]);
	gdk_window_invalidate_rect(m_button_area->window, NULL, FALSE);
}

void NedTools::toggleRest() {
	m_button_arrays REST_BUTTON ->m_active = !(m_button_arrays REST_BUTTON ->m_active);
	gdk_window_invalidate_rect(m_button_area->window, NULL, FALSE);
}

void NedTools::toggleTie() {
	m_button_arrays TIE_BUTTON ->m_active = !(m_button_arrays TIE_BUTTON ->m_active);
	gdk_window_invalidate_rect(m_button_area->window, NULL, FALSE);
}

void NedTools::toggleDot() {
	m_button_arrays DOT_BUTTON ->m_active = !(m_button_arrays DOT_BUTTON ->m_active);
	if (m_button_arrays DOT_BUTTON ->m_active) m_button_arrays DDOT_BUTTON ->m_active = false;
	gdk_window_invalidate_rect(m_button_area->window, NULL, FALSE);
}

void NedTools::toggleSharp() {
	m_button_arrays SHARP_BUTTON ->m_active = !(m_button_arrays SHARP_BUTTON ->m_active);
	if (m_button_arrays SHARP_BUTTON ->m_active) {
		m_button_arrays FLAT_BUTTON ->m_active = false;
		m_button_arrays NATURAL_BUTTON ->m_active = false;
		m_button_arrays DSHARP_BUTTON ->m_active = false;
		m_button_arrays DFLAT_BUTTON ->m_active = false;
	}
	gdk_window_invalidate_rect(m_button_area->window, NULL, FALSE);
}

void NedTools::toggleNatural() {
	m_button_arrays NATURAL_BUTTON ->m_active = !(m_button_arrays NATURAL_BUTTON ->m_active);
	if (m_button_arrays NATURAL_BUTTON ->m_active) {
		m_button_arrays FLAT_BUTTON ->m_active = false;
		m_button_arrays SHARP_BUTTON ->m_active = false;
		m_button_arrays DSHARP_BUTTON ->m_active = false;
		m_button_arrays DFLAT_BUTTON ->m_active = false;
	}
	gdk_window_invalidate_rect(m_button_area->window, NULL, FALSE);
}


void NedTools::toggleFlat() {
	m_button_arrays FLAT_BUTTON ->m_active = !(m_button_arrays FLAT_BUTTON ->m_active);
	if (m_button_arrays FLAT_BUTTON ->m_active) {
		m_button_arrays SHARP_BUTTON ->m_active = false;
		m_button_arrays NATURAL_BUTTON ->m_active = false;
		m_button_arrays DSHARP_BUTTON ->m_active = false;
		m_button_arrays DFLAT_BUTTON ->m_active = false;
	}
	gdk_window_invalidate_rect(m_button_area->window, NULL, FALSE);
}


gboolean NedTools::OnDelete(GtkWidget *widget, GdkEvent  *event, gpointer data) {
	NedResource::reset_tools();
	return FALSE;
}


void NedTools::getValues(bool *state) {
	*state = m_state;
}

gboolean NedTools::key_press_handler(GtkWidget *widget, GdkEventKey *event, gpointer data) {
	NedTools *the_dialog = (NedTools *) data;
	switch(event->keyval) {
		case GDK_KP_Divide:  the_dialog->toggleButton(1); break;
		case GDK_KP_Multiply:  the_dialog->toggleButton(2); break;
		case GDK_KP_Subtract:  the_dialog->toggleButton(3); break;
		case GDK_KP_Home:  the_dialog->toggleButton(4); break;
		case GDK_KP_Up:  the_dialog->toggleButton(5); break;
		case GDK_KP_Prior:  the_dialog->toggleButton(6); break;
		case GDK_KP_Add:  the_dialog->toggleButton(7); break;
		case GDK_KP_Left:  the_dialog->toggleButton(8); break;
		case GDK_KP_Begin:  the_dialog->toggleButton(9); break;
		case GDK_KP_Right:  the_dialog->toggleButton(10); break;
		case GDK_KP_End:  the_dialog->toggleButton(11); break;
		case GDK_KP_Down:  the_dialog->toggleButton(12); break;
		case GDK_KP_Next:  the_dialog->toggleButton(13); break;
		case GDK_KP_Enter:  the_dialog->toggleButton(14); break;
		case GDK_KP_Insert:  the_dialog->toggleButton(15); break;
		case GDK_KP_Delete:  the_dialog->toggleButton(16); break;
	}
	return FALSE;
}

void NedTools::determine_image_and_dimensions(Display *dpy, Window win, GdkGC *ggc, GdkDrawable *d, int screen, int panel, int row, int column, int bu_nr,  GdkPixbuf *buf, XImage *img[2], int *w, int *h) {
	if (row > 0 && column == 3) {
		*w = m_back_img_width; *h = 2 * m_back_img_height;
	}
	else if (row == 4 && column == 0) {
		*w = 2 * m_back_img_width; *h = m_back_img_height;
	}
	else {
		*w = m_back_img_width; *h = m_back_img_height;
	}
	switch (m_button_descriptions[panel][bu_nr].m_type) {
		case EMPTY_BUTTON: 
				   if (row > 0 && column == 3) {
					img[0] = CREATE_X11_EMPTY(VERTI_BACKGROUND, false);
					img[1] = CREATE_X11_EMPTY(VERTI_BACKGROUND, true);
				   }
				   else if (row == 4 && column == 0) {
					img[0] = CREATE_X11_EMPTY(HORIZ_BACKGROUND, false);
					img[1] = CREATE_X11_EMPTY(HORIZ_BACKGROUND, true);
				   }
				   else {
					img[0] = CREATE_X11_EMPTY(DEFAULT_BACKGROUND, false);
					img[1] = CREATE_X11_EMPTY(DEFAULT_BACKGROUND, true);
				   }
				   break;
		default:	   
				   if (row > 0 && column == 3) {
					img[0] = CREATE_X11(buf, VERTI_BACKGROUND, false);
					img[1] = CREATE_X11(buf, VERTI_BACKGROUND, true);
				   }
				   else if (row == 4 && column == 0) {
					img[0] = CREATE_X11(buf, HORIZ_BACKGROUND, false);
					img[1] = CREATE_X11(buf, HORIZ_BACKGROUND, true);
				   }
				   else {
					img[0] = CREATE_X11(buf, DEFAULT_BACKGROUND, false);
					img[1] = CREATE_X11(buf, DEFAULT_BACKGROUND, true);
				   }
				   break;
	}
}

void NedTools::determine_mini_image_and_dimensions(Display *dpy, Window win, GdkGC *ggc, GdkDrawable *d, int screen, GdkPixbuf *buf, XImage *img[2], int type, int *w, int *h) {
	*w = m_back_miniimg_width; *h = m_back_miniimg_height;
	if (type == EMPTY_BUTTON) {
		img[0] = CREATE_MINI_X11_EMPTY(MINI_BACKGROUND, false);
		img[1] = CREATE_MINI_X11_EMPTY(MINI_BACKGROUND, true);
	}
	else {
		img[0] = CREATE_MINI_X11(buf, MINI_BACKGROUND, false);
		img[1] = CREATE_MINI_X11(buf, MINI_BACKGROUND, true);
	}
}

void NedTools::createButtons(Display *dpy, Window win, GC xgc, GdkGC *ggc,  GdkDrawable *d, int screen) {
	int row, column;
	int bu_nr, i;
	int w, h;
	XImage *img[2];
	int panel;

	for (panel = 0; panel < NUM_PANELS; panel++) {
	   row = column = bu_nr = 0;
	   for (i = 0; i < 4; i++) {
		determine_image_and_dimensions(dpy, win, ggc, d, screen, panel, row, column, bu_nr, *(m_button_descriptions[panel][bu_nr].m_pixbuf), img, &w, &h);
		m_button_arrays[panel][bu_nr] = new X11button(dpy, win, xgc, screen, img, column * m_back_img_width, MINI_BUTTON_AREA_HEIGHT + row * m_back_img_height,
				w, h, column, row, panel, m_button_descriptions[panel][bu_nr].m_type, m_button_descriptions[panel][bu_nr].m_scope, m_button_descriptions[panel][bu_nr].m_type != EMPTY_BUTTON, m_button_descriptions[panel][bu_nr].m_default_state);

		if (m_button_descriptions[panel][bu_nr].m_button_group != NULL) {
			m_button_descriptions[panel][bu_nr].m_button_group->addX11button(m_button_arrays[panel][bu_nr]);
		}
		column++;
		bu_nr++;
	   }
	   row++; column = 0;
	   for (i = 0; i < 4; i++) {
		determine_image_and_dimensions(dpy, win, ggc, d, screen, panel, row, column, bu_nr, *(m_button_descriptions[panel][bu_nr].m_pixbuf), img, &w, &h);
		m_button_arrays[panel][bu_nr] = new X11button(dpy, win, xgc, screen, img, column * m_back_img_width, MINI_BUTTON_AREA_HEIGHT + row * m_back_img_height,
				w, h, column, row, panel, m_button_descriptions[panel][bu_nr].m_type, m_button_descriptions[panel][bu_nr].m_scope, m_button_descriptions[panel][bu_nr].m_type != EMPTY_BUTTON, m_button_descriptions[panel][bu_nr].m_default_state);
		if (m_button_descriptions[panel][bu_nr].m_button_group != NULL) {
			m_button_descriptions[panel][bu_nr].m_button_group->addX11button(m_button_arrays[panel][bu_nr]);
		}
		column++;
		bu_nr++;
	   }
	   row++; column = 0;
	   for (i = 0; i < 3; i++) {
		determine_image_and_dimensions(dpy, win, ggc, d, screen, panel, row, column, bu_nr, *(m_button_descriptions[panel][bu_nr].m_pixbuf), img, &w, &h);
		m_button_arrays[panel][bu_nr] = new X11button(dpy, win, xgc, screen, img, column * m_back_img_width, MINI_BUTTON_AREA_HEIGHT + row * m_back_img_height,
				w, h, column, row, panel, m_button_descriptions[panel][bu_nr].m_type, m_button_descriptions[panel][bu_nr].m_scope, m_button_descriptions[panel][bu_nr].m_type != EMPTY_BUTTON, m_button_descriptions[panel][bu_nr].m_default_state);
		if (m_button_descriptions[panel][bu_nr].m_button_group != NULL) {
			m_button_descriptions[panel][bu_nr].m_button_group->addX11button(m_button_arrays[panel][bu_nr]);
		}
		column++;
		bu_nr++;
	   }
	   row++; column = 0;
	   for (i = 0; i < 4; i++) {
		determine_image_and_dimensions(dpy, win, ggc, d, screen, panel, row, column, bu_nr, *(m_button_descriptions[panel][bu_nr].m_pixbuf), img, &w, &h);
		m_button_arrays[panel][bu_nr] = new X11button(dpy, win, xgc, screen, img, column * m_back_img_width, MINI_BUTTON_AREA_HEIGHT+ row * m_back_img_height,
				w, h, column, row, panel, m_button_descriptions[panel][bu_nr].m_type, m_button_descriptions[panel][bu_nr].m_scope, m_button_descriptions[panel][bu_nr].m_type != EMPTY_BUTTON, m_button_descriptions[panel][bu_nr].m_default_state);

		if (m_button_descriptions[panel][bu_nr].m_button_group != NULL) {
			m_button_descriptions[panel][bu_nr].m_button_group->addX11button(m_button_arrays[panel][bu_nr]);
		}
		column++;
		bu_nr++;
	   }
	   row++; column = 0;
	   for (i = 0; i < 2; i++) {
		determine_image_and_dimensions(dpy, win, ggc, d, screen, panel, row, column, bu_nr, *(m_button_descriptions[panel][bu_nr].m_pixbuf), img, &w, &h);
		m_button_arrays[panel][bu_nr] = new X11button(dpy, win, xgc, screen, img, column * m_back_img_width, MINI_BUTTON_AREA_HEIGHT + row * m_back_img_height,
				w, h, column, row, panel, m_button_descriptions[panel][bu_nr].m_type, m_button_descriptions[panel][bu_nr].m_scope, m_button_descriptions[panel][bu_nr].m_type != EMPTY_BUTTON, m_button_descriptions[panel][bu_nr].m_default_state);
		if (m_button_descriptions[panel][bu_nr].m_button_group != NULL) {
			m_button_descriptions[panel][bu_nr].m_button_group->addX11button(m_button_arrays[panel][bu_nr]);
		}
		column += 2;
		bu_nr++;
	   }
   }
}

void NedTools::createMiniButtons(Display *dpy, Window win, GC xgc, GdkGC *ggc,  GdkDrawable *d, int screen) {
	int column;
	int w, h;
	XImage *img[2];

	for (column = 0; column < 4; column++) {
		determine_mini_image_and_dimensions(dpy, win, ggc, d, screen, *(m_mini_button_descriptions[column].m_pixbuf), img, m_mini_button_descriptions[column].m_type, &w, &h);
		m_mini_button_arrays[column] = new X11button(dpy, win, xgc, screen, img, m_mini_spaces + column * (m_mini_spaces + m_back_miniimg_width), m_mini_spaces,
				w, h, column, 0, 0, m_mini_button_descriptions[column].m_type, SCOPE_NONE, m_mini_button_descriptions[column].m_type != EMPTY_BUTTON,
				m_mini_button_descriptions[column].m_default_state);

	}
	for (column = 4; column < 8; column++) {
		determine_mini_image_and_dimensions(dpy, win, ggc, d, screen, *(m_mini_button_descriptions[column].m_pixbuf), img, m_mini_button_descriptions[column].m_type, &w, &h);
		m_mini_button_arrays[column] = new X11button(dpy, win, xgc, screen, img, m_mini_spaces + (column - 4) * (m_mini_spaces + m_back_miniimg_width),
				m_mini_spaces + 1 * (m_mini_spaces + m_back_miniimg_height),
				w, h, column, 0, 0, m_mini_button_descriptions[column].m_type, SCOPE_NONE, m_mini_button_descriptions[column].m_type != EMPTY_BUTTON,
				m_mini_button_descriptions[column].m_default_state);

	}
}



gboolean NedTools::draw_menu(GtkWidget *widget, GdkEventExpose *event, gpointer data) {
	NedTools *the_dialog = (NedTools *) data;
	Display *dpy;
	Window win;
	GC xgc;
	GdkColor bgcolor;
	int screen;
	int bu_nr;
	
	if (m_button_arrays[0][0] == NULL) {
		the_dialog->m_back_pix = gdk_pixmap_new(the_dialog->m_button_area->window, 
	         4 * the_dialog->m_back_img_width, MINI_BUTTON_AREA_HEIGHT + 5 * the_dialog->m_back_img_height, -1);
		the_dialog->m_copy_gc = gdk_gc_new(the_dialog->m_button_area->window);
		the_dialog->m_blue_gc = gdk_gc_new(the_dialog->m_back_pix);
		bgcolor.pixel = 1;
		bgcolor.red = 0x0000; bgcolor.green =  0x5555; bgcolor.blue =  0xcccc;
		gdk_gc_set_rgb_fg_color (the_dialog->m_blue_gc, &bgcolor);
		dpy = gdk_x11_drawable_get_xdisplay(the_dialog->m_back_pix);
		win = gdk_x11_drawable_get_xid(the_dialog->m_back_pix);
		screen = DefaultScreen(dpy);
		xgc = DefaultGC(dpy, screen);
		the_dialog->createButtons(dpy, win, xgc, the_dialog->m_button_area->style->white_gc, the_dialog->m_back_pix, screen);
		the_dialog->createMiniButtons(dpy, win, xgc, the_dialog->m_button_area->style->white_gc, the_dialog->m_back_pix, screen);
	}

	if (the_dialog->m_back_pix == NULL) return TRUE;

        gdk_draw_rectangle(the_dialog->m_back_pix, the_dialog->m_blue_gc,
			TRUE, 0, 0,
	4 * the_dialog->m_back_img_width, MINI_BUTTON_AREA_HEIGHT + 5 * the_dialog->m_back_img_height);

	for (bu_nr = 0; bu_nr < 17; bu_nr++) {
		m_button_arrays[the_dialog->m_current_panel][bu_nr]->draw();
	}
	for (bu_nr = 0; bu_nr < 8; bu_nr++) {
		m_mini_button_arrays[bu_nr]->draw();
	}

	GdkImage *img1 =  gdk_drawable_get_image(the_dialog->m_back_pix, 0, 0,
	4 * the_dialog->m_back_img_width, MINI_BUTTON_AREA_HEIGHT + 5 * the_dialog->m_back_img_height);


	gdk_draw_image(the_dialog->m_button_area->window, the_dialog->m_copy_gc, img1, 0, 0, 0, 0,
	4 * the_dialog->m_back_img_width, MINI_BUTTON_AREA_HEIGHT + 5 * the_dialog->m_back_img_height);
	return TRUE;
}


XImage *NedTools::createX11PixmapFromGdkPixbuffer(Display *dpy, int screen, GdkDrawable *d, GdkGC *gc, GdkPixbuf *pixbuf, int background, bool up) {
	int depth;
	GdkImage *gdk_image;
	GdkPixmap *gdk_pixmap;
	XImage *x11_img;
	int w = 0, h = 0; 

	if (pixbuf != NULL) {
		w = gdk_pixbuf_get_width(pixbuf);
		h = gdk_pixbuf_get_height(pixbuf);
	}

	depth = DefaultDepth(dpy, screen);

	//gdk_draw_rectangle(gdk_pixmap, gc, TRUE, 0, 0, m_back_img_width, m_back_img_height);
	switch (background) {
		case HORIZ_BACKGROUND:
			gdk_pixmap = gdk_pixmap_new(d, 2 * m_back_img_width, m_back_img_height, depth);
			gdk_pixbuf_render_to_drawable (up ? NedResource::m_backgroundh1 : NedResource::m_backgroundh2, gdk_pixmap, gc,
				0, 0, 0, 0, 2 * m_back_img_width, m_back_img_height, GDK_RGB_DITHER_NONE, 0, 0);
			if (pixbuf != NULL) {
				gdk_pixbuf_render_to_drawable (pixbuf, gdk_pixmap, gc,
				0, 0, m_back_img_width - w / 2, (m_back_img_height - h) / 2, w, h, GDK_RGB_DITHER_NONE, 0, 0);
			}
			gdk_image = gdk_image_get(gdk_pixmap, 0, 0, 2 * m_back_img_width, m_back_img_height);
			break;
		case VERTI_BACKGROUND:
			gdk_pixmap = gdk_pixmap_new(d, m_back_img_width, 2 * m_back_img_height, depth);
			gdk_pixbuf_render_to_drawable (up ? NedResource::m_backgroundv1 : NedResource::m_backgroundv2, gdk_pixmap, gc,
				0, 0, 0, 0, m_back_img_width, 2 * m_back_img_height, GDK_RGB_DITHER_NONE, 0, 0);
			if (pixbuf != NULL) {
				gdk_pixbuf_render_to_drawable (pixbuf, gdk_pixmap, gc,
				0, 0, (m_back_img_width - w) / 2, m_back_img_height - h / 2, w, h, GDK_RGB_DITHER_NONE, 0, 0);
			}
			gdk_image = gdk_image_get(gdk_pixmap, 0, 0, m_back_img_width, 2 * m_back_img_height);
			break;
		case MINI_BACKGROUND:
			gdk_pixmap = gdk_pixmap_new(d, m_back_miniimg_width, m_back_miniimg_height, depth);
			gdk_pixbuf_render_to_drawable (up ? NedResource::m_mini_background1 : NedResource::m_mini_background2, gdk_pixmap, gc,
				0, 0, 0, 0, m_back_miniimg_width, m_back_miniimg_height, GDK_RGB_DITHER_NONE, 0, 0);
			if (pixbuf != NULL) {
				gdk_pixbuf_render_to_drawable (pixbuf, gdk_pixmap, gc,
				0, 0, (m_back_miniimg_width - w) / 2, (m_back_miniimg_height - h) / 2, w, h, GDK_RGB_DITHER_NONE, 0, 0);
			}
			gdk_image = gdk_image_get(gdk_pixmap, 0, 0, m_back_miniimg_width, m_back_miniimg_height);
			break;
		default:
			gdk_pixmap = gdk_pixmap_new(d, m_back_img_width, m_back_img_height, depth);
			gdk_pixbuf_render_to_drawable (up ? NedResource::m_background1 : NedResource::m_background2, gdk_pixmap, gc,
				0, 0, 0, 0, m_back_img_width, m_back_img_height, GDK_RGB_DITHER_NONE, 0, 0);
			if (pixbuf != NULL) {
				gdk_pixbuf_render_to_drawable (pixbuf, gdk_pixmap, gc,
				0, 0, (m_back_img_width - w) / 2, (m_back_img_height - h) / 2, w, h, GDK_RGB_DITHER_NONE, 0, 0);
			}
			gdk_image = gdk_image_get(gdk_pixmap, 0, 0, m_back_img_width, m_back_img_height);
			break;
	}
	x11_img = gdk_x11_image_get_ximage(gdk_image);
	//g_object_unref(gdk_pixmap);
	return x11_img;
}

bool NedTools::OnButtonPress(GtkWidget *widget, GdkEventButton *event, gpointer data) {
	NedTools *the_dialog = (NedTools *) data;
	int bu_nr;

	for (bu_nr = 0; bu_nr < 17; bu_nr++) {
		m_button_arrays[the_dialog->m_current_panel][bu_nr]->handle_click(event->x, event->y);
	}
	for (bu_nr = 0; bu_nr < 5; bu_nr++) {
		m_mini_button_arrays[bu_nr]->handle_click(event->x, event->y);
	}
	gdk_window_invalidate_rect(the_dialog->m_button_area->window, NULL, FALSE);
	return FALSE;
}

bool NedTools::OnButtonRelease(GtkWidget *widget, GdkEventButton *event, gpointer data) {
	NedTools *the_dialog = (NedTools *) data;
	int bu_nr;

	for (bu_nr = 0; bu_nr < 17; bu_nr++) {
		if (m_button_arrays[the_dialog->m_current_panel][bu_nr]->handle_release(event->x, event->y)) {
			the_dialog->testForPanelChange(m_button_arrays[the_dialog->m_current_panel][bu_nr]);
		}
	}
	for (bu_nr = 0; bu_nr < 5; bu_nr++) {
		if (m_mini_button_arrays[bu_nr]->handle_release(event->x, event->y)) {
			the_dialog->m_current_panel = bu_nr;
		}
	}
	gdk_window_invalidate_rect(the_dialog->m_button_area->window, NULL, FALSE);
	return FALSE;
}

void NedTools::testForPanelChange(X11button *button) {
	switch (m_current_panel) {
	    case 0: switch (button->m_row) {
		case 0: switch (button->m_column) {
		   	case 3: m_current_panel++;
				break;
		        }
		        break;
	            }
	            break;
	    case 1: switch (button->m_row) {
		case 0: switch (button->m_column) {
		   	case 1: m_current_panel--;
				break;
		   	case 3: m_current_panel++;
				break;
		        }
		        break;
	            }
	            break;
	    case 2: switch (button->m_row) {
		case 0: switch (button->m_column) {
		   	case 1: m_current_panel--;
				break;
		   	case 3: m_current_panel++;
				break;
		        }
		        break;
	            }
	            break;
	    case 3: switch (button->m_row) {
		case 0: switch (button->m_column) {
		   	case 0: m_current_panel--;
				break;
		   	case 3: m_current_panel++;
				break;
		        }
		        break;
	            }
	            break;
	    case 4: switch (button->m_row) {
		case 0: switch (button->m_column) {
		   	case 1: m_current_panel--;
				break;
		        }
		        break;
	            }
	            break;
	}
}
