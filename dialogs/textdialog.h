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

#ifndef TEXT_DIALOG_H

#define TEXT_DIALOG_H
#include "config.h"
#include "resource.h"
#include <gtk/gtk.h>
#include <cairo/cairo.h>


class NedTextDialog {
	public:
		NedTextDialog(GtkWindow *parent, char *text, int anchor, unsigned int tempo, bool enable_tempo, unsigned int volume, bool enable_volume,
			unsigned short segno_sign, int midi_program, int channel, bool enable_channel, const char *font_family, double font_size, PangoStyle font_slant, PangoWeight font_weight);
		void getValues(bool *state, char **text, int *anchor, unsigned int *tempo, bool *enable_tempo, unsigned int *volume, bool *enable_volume, 
			unsigned short *segno_sign, int *midi_program, unsigned int *channel, bool *enable_channel, const char **font_family, double *font_size, PangoStyle *font_slant, PangoWeight *font_weight);
	private:
		static void CloseCallBack(GtkDialog *dialog, gint result, gpointer data);
		static void FontCallBack(GtkButton *button, gpointer data);
		static void TempoToggled(GtkToggleButton *button, gpointer data);
		static void VolumeToggled(GtkToggleButton *button, gpointer data);
		static void InstrToggled(GtkToggleButton *button, gpointer data);
		static void ChannelToggled(GtkToggleButton *button, gpointer data);
		static gboolean OnScroll(GtkWidget *widget, GdkEventScroll *event, gpointer data);
		void retrieveFontParams();
		GtkWidget *m_text_widget;
		GtkWidget *m_font_button;
		GtkWidget *m_tempo_spin_box;
		GtkWidget *m_volume_spin_box;
		GtkWidget *m_channel_spin_box;
		GtkWidget *m_check_tempo;
		GtkWidget *m_check_volume;
		GtkWidget *m_check_instr;
		GtkWidget *m_check_channel;
		GtkWidget *m_anchor_left;
		GtkWidget *m_anchor_mid;
		GtkWidget *m_anchor_right;
		GtkWidget *m_segno_none;
		GtkWidget *m_ds_al_fine;
		GtkWidget *m_ds_al_coda;
		GtkWidget *m_d_capo_al_fine;
		GtkWidget *m_d_capo_al_coda;
		GtkWidget *m_fine;
		GtkWidget *m_al_coda;
		GtkWidget *m_instrument_list;
		GtkObject *m_instrument_adjustment;
		GtkWidget *m_main_h_box;
		GtkWidget *m_instrument_scroll;
		unsigned int m_tempo;
		unsigned int m_volume;
		bool m_enable_tempo, m_enable_volume;
		unsigned short m_segno_sign;
		int m_midi_program_number;
		int m_channel;
		bool m_enable_channel;
		int m_anchor;
		char *m_text;
		const char *m_font_family;
		double m_font_size;
		PangoStyle m_font_slant;
		PangoWeight m_font_weight;
		bool m_state;
		static const char *m_static_font_family;
		static double m_static_font_size;
		static PangoStyle m_static_font_slant;
		static PangoWeight m_static_font_weight;
};

#endif /* TEXT_DIALOG_H */
