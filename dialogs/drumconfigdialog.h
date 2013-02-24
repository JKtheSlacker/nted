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

#ifndef DRUM_CONFIG_DIALOG_H

#define DRUM_CONFIG_DIALOG_H
#include "config.h"
#include <gtk/gtk.h>
#include <cairo.h>

#define MAXBUTTONS 13


class NedDrumConfigDialog {
	public:
		NedDrumConfigDialog(GtkWindow *parent);
	private:
		static void OnClose(GtkDialog *dialog, gint result, gpointer data);
		static gboolean OnScroll(GtkWidget *widget, GdkEventScroll *event, gpointer data);
		static void OnToggle(GtkToggleButton *togglebutton, gpointer data);
		static gboolean OnExpose(GtkWidget *widget, GdkEventExpose *event, gpointer user_data);
		static void OnCursorChanged(GtkTreeView *tree_view, gpointer data);
		static void OnSave (GtkButton *button, gpointer data);
		static void OnLoad (GtkButton *button, gpointer data);
		static void OnDefault (GtkButton *button, gpointer data);
		static void OnChange (GtkButton *button, gpointer data);
		static void OnPlay (GtkButton *button, gpointer data);
		static gboolean OnEnter(GtkWidget *widget, GdkEventCrossing *event, gpointer data);  
		static gboolean OnLeave(GtkWidget *widget, GdkEventCrossing *event, gpointer data);  
		static gboolean OnKeyPress(GtkWidget *widget, GdkEventKey *event, gpointer data);
		static void increase_line (GtkButton *button, gpointer data);
		static void decrease_line (GtkButton *button, gpointer data);
		static GdkPixbuf *selectDrumIcon(int note_head);
		GtkListStore *m_drum_elements_list_store;
		GtkObject *m_drum_elements_adjustment;
		GtkWidget *m_drum_element_list;
		GtkWidget *m_preview;
		GtkWidget *m_drum_buttons[MAXBUTTONS];
		GdkColor m_black, m_red, m_white;
		int m_type, m_line;
		int m_element;
		bool m_inside_preview;
		
};

#endif /* DRUM_CONFIG_DIALOG_H */
