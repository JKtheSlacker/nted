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

#ifndef TOOLS_H

#define TOOLS_H
#include "config.h"
#include <gtk/gtk.h>
#include <gdk/gdkx.h>

class NedMainWindow;
class X11buttonGroup;

#define NUM_PANELS 5

class X11button {
	public:
		X11button(Display *dpy, Window win, GC xgc, int screen, XImage *img[2], int xpos, int ypos, int width, int height, int column, int row, int panel, int type, int scope, bool enabled, bool default_state);
		~X11button();
		void draw();
		void toggle();
		bool handle_click(gdouble x, gdouble y);
		bool handle_release(gdouble x, gdouble y);
		X11buttonGroup *m_button_group;
		bool m_active;
		int m_type;
		int m_row, m_column;
		int m_panel;
	private:
		void notifyMainWindow(NedMainWindow *current_main_window);
		Display *m_dpy;
		Window m_win;
		GC m_xgc;
		int m_screen;
#define EMPTY_BUTTON 0
#define PUSH_BUTTON 1
#define TOGGLE_BUTTON 2
		bool m_enabled;
		bool m_default_state;
		int m_scope;
#define SCOPE_NONE 0
#define SCOPE_NOTE_LENGTH 1
#define SCOPE_STATE       2 
#define SCOPE_NOTE_STATE  3 
#define SCOPE_NOTE_HEAD   4 
#define SCOPE_DOTS        5
#define SCOPE_TIES        6


		XImage *m_img[2];
		int m_xpos, m_ypos;
		int m_width, m_height;
};

class X11buttonGroup {
	public:
		X11buttonGroup(bool all_inactive_allowed);
		void removeAllButtons();
		void addX11button(X11button *button);
		void removeX11button(X11button *button);
		bool allowStateChange(X11button *button);
		void stateChanged(X11button *button);
		void resetAllButtons();
	private:
		GList *m_buttons;
		bool m_all_inactive_allowed;
};
		
			


class NedTools {
	public:
		NedTools();
		~NedTools();
		void Show();
		void getValues(bool *state);
		unsigned int getCurrentLength();
		int getCurrentNoteHead();
		unsigned int getStatus();
		bool getRestMode();
		int getDotCount();
		bool getTiedMode();
		void setButtons(unsigned int length, int dotcount, unsigned int chord_status, unsigned int note_status, bool isRest, bool tie_forward, int note_head);
		void setNoteLengthButtons(unsigned int length);
		void setToolboxTransient(NedMainWindow *m_win);
		void toggleButton(int nr);
		void toggleRest();
		void toggleTie();
		void toggleDot();
		void toggleSharp();
		void toggleNatural();
		void toggleFlat();
		void resetButtons();
		void resetTieButton();
		void resetSomeButtons();
		void updatePosition();
	private:
		void createButtons(Display *dpy, Window win, GC xgc, GdkGC *ggc,  GdkDrawable *d, int screen);
		void createMiniButtons(Display *dpy, Window win, GC xgc, GdkGC *ggc,  GdkDrawable *d, int screen);
		void determine_image_and_dimensions(Display *dpy, Window win, GdkGC *ggc, GdkDrawable *d, int screen, int panel, int row, int column, int bu_nr,
			GdkPixbuf *buf, XImage *img[2], int *w, int *h);
		void determine_mini_image_and_dimensions(Display *dpy, Window win, GdkGC *ggc, GdkDrawable *d, int screen, GdkPixbuf *buf, XImage *img[2], int type, int *w, int *h);
		void testForPanelChange(X11button *button);
		static bool close_toolbox(GtkWidget *widget, GdkEvent  *event, gpointer data);
		 class button_descr {
			public:
				GdkPixbuf **m_pixbuf;
				int m_type;
				int m_scope;
				bool m_default_state;
				X11buttonGroup *m_button_group;
		};

		static button_descr m_button_descriptions[NUM_PANELS][17];
		static button_descr m_mini_button_descriptions[8];
		static X11button *m_button_arrays[NUM_PANELS][17];
		static X11button *m_mini_button_arrays[8];
		GdkGC *m_copy_gc, *m_blue_gc;
		Display *m_dpy;
		Window m_win;
		GC m_xgc;
		int m_screen;
                                      

			
		static gboolean OnDelete(GtkWidget *widget, GdkEvent  *event, gpointer data);
		static gboolean key_press_handler(GtkWidget   *widget, GdkEventKey *event, gpointer data); 
		static bool OnButtonPress(GtkWidget *widget, GdkEventButton *event, gpointer data);
		static bool OnButtonRelease(GtkWidget *widget, GdkEventButton *event, gpointer data);
		static gboolean draw_menu(GtkWidget *widget, GdkEventExpose *event, gpointer data);
		static XImage *createX11PixmapFromGdkPixbuffer(Display *dpy, int screen, GdkDrawable *d, GdkGC *gc,
			GdkPixbuf *pixbuf, int background, bool up);
		GtkWidget *m_button_area;
		GtkWidget *m_button_frame;
		GtkWidget *m_toolbox;
		GdkPixmap *m_back_pix;
		static X11buttonGroup m_accidentals, m_note_length_group, m_note_head_group, m_dot_group, m_tremolo_group;
		bool m_state;
		int m_current_row, m_current_column;
		int m_current_panel;
		int m_buttons_created;
		static int m_back_img_width, m_back_img_height;
		static int m_back_miniimg_width, m_back_miniimg_height;
		static int m_mini_spaces;
};

#endif /* TOOLS_H */
