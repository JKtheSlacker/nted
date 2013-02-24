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

#ifndef LYRICS_EDITOR_H

#define LYRICS_EDITOR_H
#include "config.h"
#include "resource.h"
#include <gtk/gtk.h>

class NedMainWindow;


class NedLyricsEditor {
	public:
		NedLyricsEditor(GtkWindow *parent, NedMainWindow *main_window, int staff_nr);
		~NedLyricsEditor();
		void appendText(int num, const char *text);
		char *getNextWord(int line);
		bool m_newchar[MAX_LYRICS_LINES];
		void addDot(int line) { number_uf_dots[line]++;}
	private:
		static void ResponseCallBack(GtkDialog *dialog, gint result, gpointer data);
		static void OnSave (GtkButton *button, gpointer data);
		static void OnLoad (GtkButton *button, gpointer data);
		static gboolean OnMap1(GtkWidget *widget, GdkEvent *event, gpointer data);
		static gboolean OnMap2(GtkWidget *widget, GdkEvent *event, gpointer data);
		static gboolean OnMap3(GtkWidget *widget, GdkEvent *event, gpointer data);
		static gboolean OnMap4(GtkWidget *widget, GdkEvent *event, gpointer data);
		static gboolean OnMap5(GtkWidget *widget, GdkEvent *event, gpointer data);
		int number_uf_dots[MAX_LYRICS_LINES];
		GtkTextIter m_itr1[MAX_LYRICS_LINES], m_itr2[MAX_LYRICS_LINES];
		GtkTextBuffer *m_buffer[MAX_LYRICS_LINES];

		GtkWidget *m_dialog;
		GtkWidget *m_table;
		NedMainWindow *m_main_window;
		char *m_last_folder;
		int m_current_verse;
		int m_staff_nr;
};

#endif /* LYRICS_EDITOR_H */
