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

#ifndef STAFF_CONTEXT_DIALOG_H

#define STAFF_CONTEXT_DIALOG_H
#include "config.h"
#include <gtk/gtk.h>
#include <cairo.h>
#include "resource.h"

#define DELETE_STAFF_CMD 12
#define DELETE_SYSTEM_CMD 13

class NedMainWindow;


class NedStaffContextDialog {
	public:
		NedStaffContextDialog(GtkWindow *parent,  NedMainWindow *main_window, bool different_voices, bool allow_delete_system, int staff_count, int staff_pos, char *staff_name, char *staffshortname, char *group_name, char* group_short_name, int clef_number, int octave_shift, int key_signature_number, int numerator, int demoninator, unsigned int symbol, int volume[VOICE_COUNT], int midi_program[VOICE_COUNT], int channel, int tempo, int pan[VOICE_COUNT], int chorus, int play_transposed, int reverb, bool muted[VOICE_COUNT]);
		void getValues(bool *state, bool *different_voices, bool *delete_staff, bool *delete_system, int *staff_pos, char **staff_name, char **staff_short_name, char **group_name, char **group_short_name, int *clef_number, int *octave_shift, int *key_signature_number, bool *ajdust_notes, int *numerator, int *demoninator, unsigned int *symbol, int volume[VOICE_COUNT], int midi_program[VOICE_COUNT], int *channel, int *tempo, int pan[VOICE_COUNT], int *chorus, int *play_transposed, int *reverb, bool *config_changed, bool muted[VOICE_COUNT]);
	private:
		static void OnClose(GtkDialog *dialog, gint result, gpointer data);
		static gboolean OnScroll(GtkWidget *widget, GdkEventScroll *event, gpointer data);
		static gboolean draw_clefs(GtkWidget *widget,
			 GdkEventExpose *event, gpointer data);
		static void OnCommButtonToggled(GtkToggleButton *togglebutton, gpointer data);
		static void OnCutButtonToggled(GtkToggleButton *togglebutton, gpointer data);
		static void OnVoiceToggle(GtkToggleButton *togglebutton, gpointer data);
		static void OnAllMutedToggle(GtkToggleButton *togglebutton, gpointer data);
		static void OnSingleMutedToggle(GtkToggleButton *togglebutton, gpointer data);
		static void decrease_clef_number (GtkButton *button, gpointer user_data);
		static void increase_clef_number (GtkButton *button, gpointer user_data);
		static void octave_shift_change(GtkSpinButton *spinbutton, gpointer user_data);
		void SetCurrentVoiceParamsTo(int voice);
		void GetCurrentVoiceParamsFrom(int voice, int volume[VOICE_COUNT], int pan[VOICE_COUNT], int instrument[VOICE_COUNT], bool *changed);
		GtkWidget *m_clef_show_window;
		static char *m_keyTab[];
		GtkWidget *m_sig_radio_buttons[14];
		GtkWidget *m_denominator;
		GtkWidget *m_numerator;
		GtkWidget *m_com_time_bu;
		GtkWidget *m_cut_time_bu;
		GtkWidget *m_clef_bu_up;
		GtkWidget *m_clef_bu_down;
		GtkWidget *m_instrument_list;
		GtkWidget *m_volume_scale;
		GtkWidget *m_pan_scale;
		GtkWidget *m_tempo_scale;
		GtkWidget *m_chorus_scale;
		GtkWidget *m_play_transposed_scale;
		GtkWidget *m_reverb_scale;
		GtkWidget *m_channel_spin_box;
		GtkWidget *m_mute_all_check;
		GtkWidget *m_mute_1_check;
		GtkWidget *m_mute_2_check;
		GtkWidget *m_mute_3_check;
		GtkWidget *m_mute_4_check;
		GtkWidget *m_octave_shift_spin_box;
		GtkWidget *m_adjust_check_box;
		GtkWidget *m_position_list;
		GtkWidget *m_staffname;
		GtkWidget *m_radio_all;
		GtkWidget *m_radio_1;
		GtkWidget *m_radio_2;
		GtkWidget *m_radio_3;
		GtkWidget *m_radio_4;
		GtkWidget *m_staffshortname;
		GtkWidget *m_groupname;
		GtkWidget *m_groupshortname;
		GtkWidget *m_table;
		GtkObject *m_instrument_adjustment;
		int m_current_voice;
		bool m_different_voices;
		int m_octave_shift;
		int m_tempo;
		int m_chorus;
		int m_play_transposed;
		int m_reverb;
		int m_channel_value;
		bool m_state;
		int m_numerator_value;
		int m_denominator_value;
		unsigned int m_symbol;
		int m_time_symbol;
		int m_key_signature_number;
		int m_current_clef, m_newclef;
		bool m_do_adjust;
		int m_midi_program_number[VOICE_COUNT];
		int m_pan[VOICE_COUNT];
		int m_volume[VOICE_COUNT];
		bool m_config_changed;
		bool m_delete_staff;
		bool m_delete_system;
		int m_staff_pos;
		int m_staff_count;
		char *m_staff_name;
		char *m_staff_short_name;
		char *m_group_name;
		char *m_group_short_name;
		bool m_muted[VOICE_COUNT];
		gulong m_handlrs[VOICE_COUNT];

	friend class NedKeysigDialog;
	friend class NedMidiRecordConfigDialog;

		
};

#endif /* STAFF_CONTEXT_DIALOG_H */
