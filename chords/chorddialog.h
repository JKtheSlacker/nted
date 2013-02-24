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

#ifndef CHORD_DIALOG_H

#define CHORD_DIALOG_H
#include "config.h"
#include <gtk/gtk.h>
#include <cairo.h>

struct chord_str;
class NedMainWindow;


class NedChordDialog {
	public:
		NedChordDialog(GtkWindow *parent, NedMainWindow *main_window, struct chord_str *chord_struct, unsigned int status);
		~NedChordDialog();
		void getValues(bool *state, struct chord_str **chord_ptr, int *chordname, unsigned int *status);
	private:
		static void OnClose(GtkDialog *dialog, gint result, gpointer data);
		static gboolean draw_chord_window(GtkWidget *widget, GdkEventExpose *event, gpointer data);
		static gboolean draw_chord_show_window(GtkWidget *widget, GdkEventExpose *event, gpointer data);
		void updateChord(const char *chordname, int *chord_num);
		void searchChordPtr(struct chord_str *chord_struct);
		static void OnRootChange(GtkTreeView *tree_view, gpointer data);
		static void OnModiesChange(GtkTreeView *tree_view, gpointer data);
		static void OnScaleChange (GtkRange *range, gpointer data);
		static void OnSizeChanged(GtkWidget *widget, GtkAllocation *allocation, gpointer data);
		static bool OnScroll(GtkWidget *widget, GdkEventScroll *event, gpointer data);
		static bool OnNewChordSelect(GtkWidget *widget, GdkEventButton *event, gpointer data);
		GtkWidget *m_chord_show_window;
		GtkWidget *m_chord_selection_show_window;
		GtkWidget *m_root_list_view;
		GtkWidget *m_modies_list_view;
		GtkWidget *m_chord_selection_scale;
		GtkWidget *m_text_checkbox;
		GtkWidget *m_diagram_checkbox;
		GList *m_current_chords;
		int m_chordname;
		bool m_state;
		unsigned int m_status;
		NedMainWindow *m_main_window;
		char m_user_selection[128];
		double m_chords_show_window_y_offset;
		const char *current_root_name, *current_mody_name;
		struct chord_str *m_current_chord;
		static bool persistent_draw_diagram, persistent_draw_chord_name;
};

#endif /* CHORD_DIALOG_H */
