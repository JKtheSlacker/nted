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

#ifndef MIDI_RECORD_CONFIG_H

#define MIDI_RECORD_CONFIG_H
#include "config.h"
#include <gtk/gtk.h>
#include <cairo.h>



class NedMidiRecordConfigDialog {
	public:
		NedMidiRecordConfigDialog(GtkWindow *parent, int num, int denom, int midi_pgm,
			int tempo_inverse, int metro_volume, bool triplet, bool f_piano, bool dnt_split, int keysig);
		void getValues(bool *state, int *num, int *denom, int *midi_pgm,
			int *tempo_inverse, int *metro_volume, bool *triplet, bool *f_piano, bool *dnt_split, int *keysig);
	private:
		static void OnClose(GtkDialog *dialog, gint result, gpointer data);
		static gboolean OnScroll(GtkWidget *widget, GdkEventScroll *event, gpointer data);
		static void OnPianoForcePressed(GtkButton *button, gpointer data);
		static void OnDontSplitPressed(GtkButton *button, gpointer data);
		GtkObject *m_instrument_adjustment;
		GtkWidget *m_instrument_list;
		GtkWidget *m_numerator;
		GtkWidget *m_denominator;
		GtkWidget *m_with_triplet_button;
		GtkWidget *m_force_piano_button;
		GtkWidget *m_dont_split_button;
		GtkWidget *m_tempo_scale;
		GtkWidget *m_metro_volume_scale;
		GtkWidget *m_sig_radio_buttons[14];
		bool m_state;
		int m_num, m_denom;
		int m_pgm;
		int m_tempo_inverse;
		int m_metro_volume;
		bool m_triplet, m_f_piano, m_dnt_split;
		int m_keysig;
		
};

#endif /* MIDI_RECORD_CONFIG_H */
