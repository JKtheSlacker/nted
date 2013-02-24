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

#ifndef MIDI_IMPORT_DIALOG_H

#define MIDI_IMPORT_DIALOG_H
#include "config.h"
#include <gtk/gtk.h>



class NedMidiImportDialog {
	public:
		NedMidiImportDialog(GtkWindow *parent, char **folder, char **filename);
		void getValues(bool *state, bool *with_triplets, int *tempo_change_density, int *volume_change_density,
			bool *force_piano, bool *sort_instruments, bool *dont_split);
	private:
		static void OnClose(GtkDialog *dialog, gint result, gpointer data);
		static void OnPianoForcePressed(GtkButton *button, gpointer data);
		static void OnDontSplitPressed(GtkButton *button, gpointer data);
		GtkWidget *m_with_triplet_button;
		GtkWidget *m_tempo_change_density_scale;
		GtkWidget *m_volume_change_density_scale;
		GtkWidget *m_file_chooser;
		GtkWidget *m_force_piano_button;
		GtkWidget *m_sort_instruments_button;
		GtkWidget *m_dont_split_button;
		bool m_state;
		bool m_with_triplets;
		int m_tempo_change_density;
		int m_volume_change_density;
		bool m_force_piano;
		bool m_sort_instruments;
		bool m_dont_split;
		gchar **m_last_folder;
		gchar **m_filename;
};

#endif /*  MIDI_IMPORT_DIALOG_H */
