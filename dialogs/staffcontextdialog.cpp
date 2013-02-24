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

#include "staffcontextdialog.h"
#include "pangocairotext.h"
#include "mainwindow.h"
#include "localization.h"

#define CLEF_SHOW_WINDOW_WIDTH 90
#define CLEF_SHOW_WINDOW_HEIGHT 100

char *NedStaffContextDialog::m_keyTab[] = {
_("G flat Major; e flat minor"), _("D flat Major; b flat minor"),
_("A flat Major; f minor"), _("E flat Major; c minor"),
_("B flat Major; g minor"), _("F Major; d minor"),
_("C Major; a minor"), _("G Major; e minor"),
_("D Major; b minor"), _("A Major; f sharp minor"),
_("E Major; c sharp minor"), _("B Major; g sharp minor"),
_("F sharp Major; d sharp minor")
};


NedStaffContextDialog::NedStaffContextDialog(GtkWindow *parent, NedMainWindow *main_window, bool different_voices, bool allow_delete_system, int staff_count, int staff_pos, char *staff_name, char *staff_short_name, char *group_name, char* group_short_name, int clef_number, int octave_shift, int key_signature_number, int numerator,  int demoninator, unsigned int symbol, int volume[VOICE_COUNT], int midi_program[VOICE_COUNT], int channel, int tempo, int pan[VOICE_COUNT], int chorus, int play_transposed, int reverb, bool muted[VOICE_COUNT]) :
m_different_voices(different_voices), m_octave_shift(octave_shift), m_tempo(tempo), m_chorus(chorus), m_play_transposed(play_transposed), m_reverb(reverb), m_channel_value(channel),
m_state(false), m_numerator_value(numerator), m_denominator_value(demoninator), m_symbol(symbol),
m_key_signature_number(key_signature_number), m_current_clef(clef_number),  m_config_changed(false),
m_delete_staff(false), m_staff_pos(staff_pos), 
m_staff_count(staff_count), m_staff_name(staff_name), m_staff_short_name(staff_short_name), m_group_name(group_name), m_group_short_name(group_short_name)  {
	GtkWidget *dialog;
	GtkWidget *label_score;
	GtkWidget *label_midi;
	GtkWidget *label_position;
	GtkWidget *score_hbox;
	GtkWidget *midi_hbox;
	GtkWidget *position_hbox;
	GtkWidget *clef_hbox;
	GtkWidget *clef_buttons_vbox;
	GtkWidget *clef_frame;
	GtkWidget *key_frame;
	GtkWidget *key_frame_left_vbox;
	GtkWidget *key_frame_right_vbox;
	GtkWidget *radio_hbox;
	GtkWidget *time_signature_frame;
	GtkWidget *numerator_label;
	GtkWidget *denominator_label;
	GtkWidget *octave_shift_label;
	GtkWidget *octave_shift_vbox;
	GtkWidget *time_signature_vbox;
	GtkWidget *time_signature_upper_hbox;
	GtkWidget *time_signature_lower_hbox;
	GtkWidget *volume_frame;
	GtkWidget *tempo_frame;
	GtkWidget *pan_frame;
	GtkWidget *chorus_frame;
	GtkWidget *play_transposed_frame;
	GtkWidget *reverb_frame;
	GtkWidget *instrument_scroll;
	GtkListStore *instrumend_list_store;
	GtkTreeIter iter;
	GtkCellRenderer *instrument_renderer;
	GtkTreeViewColumn *instrument_column;
	GtkTreeViewColumn *instrument_num_column;
	GtkWidget *channel_frame;
	GtkWidget *mute_channel_vbox;
	GtkWidget *mute_frame;
	GtkWidget *mute_vbox;
	GtkWidget *mute_hbox1;
	GtkWidget *mute_hbox2;
	GtkWidget *voices_frame;
	GtkWidget *vrb_vbox;
	GtkWidget *vrb_hbox1;
	GtkWidget *vrb_hbox2;
	GdkColor bgcolor;
	GtkListStore *position_list_store;
	GtkCellRenderer *position_renderer;
	GtkTreeViewColumn *position_column;
	GtkTreeViewColumn *position_num_column;
	GtkTreePath *position_tree_path;
	GtkObject *position_adjustment;
	GtkWidget *position_scroll = NULL;
	GtkWidget *staff_name_frame;
	GtkWidget *staff_short_name_frame;
	GtkWidget *signature_name_vbox;
	GtkWidget *group_name_frame;
	GtkWidget *group_short_name_frame;
	GtkWidget *group_name_vbox;

	bool all_muted;

	int i;
	char Str[128];

	for (i = 0; i < VOICE_COUNT; i++) {
		m_midi_program_number[i] = midi_program[i];
		m_pan[i] = pan[i];
		m_volume[i] = volume[i];
		m_muted[i] = muted[i];
	}
		
	m_newclef = m_current_clef;

	dialog = gtk_dialog_new_with_buttons(_("Staff context"), parent, (GtkDialogFlags) (GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT),
		GTK_STOCK_OK, GTK_RESPONSE_ACCEPT, GTK_STOCK_CANCEL, GTK_RESPONSE_REJECT, NULL);

	if (staff_count > 1) {
		gtk_dialog_add_button(GTK_DIALOG(dialog), "delete-staff", DELETE_STAFF_CMD);
	}
	if (allow_delete_system) {
		gtk_dialog_add_button(GTK_DIALOG(dialog), "delete-system", DELETE_SYSTEM_CMD);
	}
	g_signal_connect(dialog, "response", G_CALLBACK (OnClose), (void *) this);


	clef_frame = gtk_frame_new(_("clef"));
	m_clef_show_window = gtk_drawing_area_new ();
	bgcolor.pixel = 1;
	bgcolor.red = bgcolor.green =  bgcolor.blue =  0xffff;
	gtk_widget_modify_bg(GTK_WIDGET(m_clef_show_window), GTK_STATE_NORMAL, &bgcolor);
	gtk_drawing_area_size(GTK_DRAWING_AREA(m_clef_show_window), CLEF_SHOW_WINDOW_WIDTH, CLEF_SHOW_WINDOW_HEIGHT);

	clef_hbox = gtk_hbox_new(FALSE, 2);
	gtk_box_pack_start(GTK_BOX(clef_hbox), m_clef_show_window, FALSE, FALSE, 0);
	clef_buttons_vbox = gtk_vbox_new(FALSE, 2);
	m_clef_bu_up = gtk_button_new_from_stock("my-arrow-up-icon");
	g_signal_connect (m_clef_bu_up, "pressed", G_CALLBACK(decrease_clef_number), (void *) this);
	m_clef_bu_down = gtk_button_new_from_stock ("my-arrow-down-icon");
	g_signal_connect (m_clef_bu_down, "pressed", G_CALLBACK(increase_clef_number), (void *) this);
	gtk_box_pack_start(GTK_BOX(clef_buttons_vbox), m_clef_bu_up, FALSE, FALSE, 0);
	gtk_box_pack_end(GTK_BOX(clef_buttons_vbox), m_clef_bu_down, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(clef_hbox), clef_buttons_vbox, FALSE, FALSE, 0);

	octave_shift_vbox = gtk_vbox_new(FALSE, 2);
	switch (m_octave_shift) {
		case 12: m_octave_shift = 8; break;
		case -12: m_octave_shift = -8; break;
	}
	m_octave_shift_spin_box = gtk_spin_button_new_with_range(-8.0, 8.0, 8.0);
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(m_octave_shift_spin_box), m_octave_shift);
	octave_shift_label = gtk_label_new(_("Octave shift:"));
	gtk_box_pack_start (GTK_BOX (octave_shift_vbox), octave_shift_label, FALSE, FALSE, 0);
	gtk_box_pack_start (GTK_BOX (octave_shift_vbox), m_octave_shift_spin_box, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(clef_hbox), octave_shift_vbox, FALSE, FALSE, 0);
	g_signal_connect (m_octave_shift_spin_box, "value-changed", G_CALLBACK (octave_shift_change), (void *) this);

	m_adjust_check_box = gtk_check_button_new_with_label(_("adjust notes"));
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(m_adjust_check_box), TRUE);
	gtk_box_pack_start (GTK_BOX (octave_shift_vbox), m_adjust_check_box, FALSE, FALSE, 0);

	if (m_newclef == MINCLEF_NUMBER) {
		gtk_widget_set_sensitive(m_clef_bu_up, FALSE);
	}
	if (m_newclef == MAXCLEF_NUMBER) {
		gtk_widget_set_sensitive(m_clef_bu_down, FALSE);
	}

	gtk_container_add (GTK_CONTAINER(clef_frame), clef_hbox);
	g_signal_connect (m_clef_show_window, "expose-event", G_CALLBACK (draw_clefs), (void *) this);

	key_frame = gtk_frame_new(_("key"));
	radio_hbox = gtk_hbox_new(FALSE, 2);
	key_frame_left_vbox = gtk_vbox_new(FALSE, 2);
	key_frame_right_vbox = gtk_vbox_new(FALSE, 2);
	m_sig_radio_buttons[0] =  gtk_radio_button_new_with_label (NULL, gettext(m_keyTab[0]));
	gtk_box_pack_start (GTK_BOX (key_frame_left_vbox),  m_sig_radio_buttons[0], FALSE, FALSE, 0);
	for (i = -5; i < 7; i++) {
		m_sig_radio_buttons[i+6] = gtk_radio_button_new_with_label_from_widget (GTK_RADIO_BUTTON (m_sig_radio_buttons[0]), gettext(m_keyTab[i+6]));
		if (i < 0) {
			gtk_box_pack_start (GTK_BOX (key_frame_left_vbox), m_sig_radio_buttons[i+6], FALSE, FALSE, 0);
		}
		else {
			gtk_box_pack_start (GTK_BOX (key_frame_right_vbox), m_sig_radio_buttons[i+6], FALSE, FALSE, 0);
		}
	}
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (m_sig_radio_buttons[m_key_signature_number+6]), TRUE);

	gtk_box_pack_start (GTK_BOX (radio_hbox), key_frame_left_vbox,  FALSE, FALSE, 0);
	gtk_box_pack_start (GTK_BOX (radio_hbox), key_frame_right_vbox,  FALSE, FALSE, 0);

	gtk_container_add (GTK_CONTAINER(key_frame), radio_hbox);


	time_signature_frame = gtk_frame_new(_("time signature"));
	time_signature_vbox = gtk_vbox_new(FALSE, 5);
	time_signature_upper_hbox = gtk_hbox_new(FALSE, 5);
	time_signature_lower_hbox = gtk_hbox_new(FALSE, 5);
	numerator_label = gtk_label_new(_("Numerator:"));
	denominator_label = gtk_label_new(_("Denominator:"));
	m_numerator = gtk_spin_button_new_with_range (1.0, 20.0, 1.0);
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(m_numerator), m_numerator_value);
	m_denominator = gtk_combo_box_new_text();
	gtk_combo_box_append_text(GTK_COMBO_BOX(m_denominator), "2");
	gtk_combo_box_append_text(GTK_COMBO_BOX(m_denominator), "4");
	gtk_combo_box_append_text(GTK_COMBO_BOX(m_denominator), "8");
	gtk_combo_box_append_text(GTK_COMBO_BOX(m_denominator), "16");
	if (m_denominator_value < 4) {
		gtk_combo_box_set_active(GTK_COMBO_BOX(m_denominator), 0);
	}
	else if (m_denominator_value < 8) {
		gtk_combo_box_set_active(GTK_COMBO_BOX(m_denominator), 1);
	}
	else if (m_denominator_value < 16) {
		gtk_combo_box_set_active(GTK_COMBO_BOX(m_denominator), 2);
	}
	else {
		gtk_combo_box_set_active(GTK_COMBO_BOX(m_denominator), 3);
	}

	m_com_time_bu = gtk_toggle_button_new();
	gtk_button_set_image(GTK_BUTTON(m_com_time_bu),  gtk_image_new_from_stock("comm-icon", GTK_ICON_SIZE_SMALL_TOOLBAR));
	m_cut_time_bu = gtk_toggle_button_new();
	gtk_button_set_image(GTK_BUTTON(m_cut_time_bu),  gtk_image_new_from_stock("cut-icon", GTK_ICON_SIZE_SMALL_TOOLBAR));
	g_signal_connect(m_com_time_bu, "toggled", G_CALLBACK (OnCommButtonToggled), (void *) this);
	g_signal_connect(m_cut_time_bu, "toggled", G_CALLBACK (OnCutButtonToggled), (void *) this);

	switch (symbol) {
		case TIME_SYMBOL_COMMON_TIME: gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(m_com_time_bu), TRUE);
						gtk_spin_button_set_value(GTK_SPIN_BUTTON(m_numerator), 4);
						gtk_combo_box_set_active(GTK_COMBO_BOX(m_denominator), 1);
						gtk_widget_set_sensitive(m_numerator, FALSE);
						gtk_widget_set_sensitive(m_denominator, FALSE);
						break;
		case TIME_SYMBOL_CUT_TIME: gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(m_cut_time_bu), TRUE);
						gtk_spin_button_set_value(GTK_SPIN_BUTTON(m_numerator), 2);
						gtk_combo_box_set_active(GTK_COMBO_BOX(m_denominator), 0);
						gtk_widget_set_sensitive(m_numerator, FALSE);
						gtk_widget_set_sensitive(m_denominator, FALSE);
						break;
	}

	staff_name_frame =  gtk_frame_new(_("staff name"));
	m_staffname = gtk_entry_new_with_max_length(50);
	if (m_staff_name != NULL) {
		gtk_entry_set_text(GTK_ENTRY(m_staffname), m_staff_name);
	}
	gtk_container_add (GTK_CONTAINER(staff_name_frame), m_staffname);

	staff_short_name_frame =  gtk_frame_new(_("staff short name"));
	m_staffshortname = gtk_entry_new_with_max_length(50);
	if (m_staff_short_name != NULL) {
		gtk_entry_set_text(GTK_ENTRY(m_staffshortname), m_staff_short_name);
	}
	gtk_container_add (GTK_CONTAINER(staff_short_name_frame), m_staffshortname);

	group_name_frame =  gtk_frame_new(_("group name"));
	m_groupname = gtk_entry_new_with_max_length(50);
	if (m_group_name != NULL) {
		gtk_entry_set_text(GTK_ENTRY(m_groupname), m_group_name);
	}
	gtk_container_add (GTK_CONTAINER(group_name_frame), m_groupname);

	group_short_name_frame =  gtk_frame_new(_("group short name"));
	m_groupshortname = gtk_entry_new_with_max_length(50);
	if (m_group_short_name != NULL) {
		gtk_entry_set_text(GTK_ENTRY(m_groupshortname), m_group_short_name);
	}
	gtk_container_add (GTK_CONTAINER(group_short_name_frame), m_groupshortname);



	tempo_frame = gtk_frame_new(_("tempo"));
	m_tempo_scale = gtk_vscale_new_with_range(20.0, 300.0, 1.0);
	gtk_range_set_inverted(GTK_RANGE(m_tempo_scale), TRUE);
	gtk_range_set_value(GTK_RANGE(m_tempo_scale), m_tempo);
	gtk_container_add (GTK_CONTAINER(tempo_frame), m_tempo_scale);
	chorus_frame = gtk_frame_new(_("chorus"));
	m_chorus_scale = gtk_vscale_new_with_range(20.0, 127.0, 1.0);
	gtk_range_set_inverted(GTK_RANGE(m_chorus_scale), TRUE);
	gtk_range_set_value(GTK_RANGE(m_chorus_scale), m_chorus);
	gtk_container_add (GTK_CONTAINER(chorus_frame), m_chorus_scale);

	play_transposed_frame = gtk_frame_new(_("transp."));
	m_play_transposed_scale = gtk_vscale_new_with_range(-24.0, 24.0, 1.0);
	gtk_range_set_inverted(GTK_RANGE(m_play_transposed_scale), TRUE);
	gtk_range_set_value(GTK_RANGE(m_play_transposed_scale), m_play_transposed);
	gtk_container_add (GTK_CONTAINER(play_transposed_frame), m_play_transposed_scale);

	reverb_frame = gtk_frame_new(_("reverb"));
	m_reverb_scale = gtk_vscale_new_with_range(20.0, 127.0, 1.0);
	gtk_range_set_inverted(GTK_RANGE(m_reverb_scale), TRUE);
	gtk_range_set_value(GTK_RANGE(m_reverb_scale), m_reverb);
	gtk_container_add (GTK_CONTAINER(reverb_frame), m_reverb_scale);

	volume_frame = gtk_frame_new(_("volume"));
	// see also NedStaffContextDialog::SetCurrentVoiceParamsTo
	m_volume_scale = gtk_vscale_new_with_range(0.0, 127.0, 1.0);
	gtk_range_set_inverted(GTK_RANGE(m_volume_scale), TRUE);
	gtk_container_add (GTK_CONTAINER(volume_frame), m_volume_scale);

	pan_frame = gtk_frame_new(_("pan"));
	m_pan_scale = gtk_vscale_new_with_range(20.0, 127.0, 1.0);
	gtk_range_set_inverted(GTK_RANGE(m_pan_scale), TRUE);
	// see also NedStaffContextDialog::SetCurrentVoiceParamsTo
	gtk_container_add (GTK_CONTAINER(pan_frame), m_pan_scale);

	instrumend_list_store = gtk_list_store_new (2, G_TYPE_INT, G_TYPE_STRING);

	for (i = 0; i < NedResource::getNumInstruments(); i++) {
		gtk_list_store_append (instrumend_list_store, &iter);
		gtk_list_store_set (instrumend_list_store, &iter, 0, i, 1, gettext(NedResource::GM_Instruments[i]), -1);
	}
	m_instrument_list = gtk_tree_view_new_with_model(GTK_TREE_MODEL(instrumend_list_store));
	instrument_renderer = gtk_cell_renderer_text_new ();
	instrument_num_column = gtk_tree_view_column_new_with_attributes (_("no"), instrument_renderer, "text", 0, NULL);
	gtk_tree_view_append_column (GTK_TREE_VIEW (m_instrument_list), instrument_num_column);
	instrument_column = gtk_tree_view_column_new_with_attributes (_("instruments"), instrument_renderer, "text", 1, NULL);
	gtk_tree_view_append_column (GTK_TREE_VIEW (m_instrument_list), instrument_column);
	m_instrument_adjustment = gtk_adjustment_new (0.0, 0.0, NedResource::getNumInstruments(), 1.0,  10.0, 10.0);
	instrument_scroll = gtk_vscrollbar_new(GTK_ADJUSTMENT(m_instrument_adjustment));
	gtk_tree_view_set_vadjustment (GTK_TREE_VIEW (m_instrument_list), GTK_ADJUSTMENT(m_instrument_adjustment));
	gtk_widget_set_size_request(m_instrument_list, 150, 100);
	g_signal_connect(dialog, "scroll-event", G_CALLBACK (OnScroll), (void *) this);
	// see also NedStaffContextDialog::SetCurrentVoiceParamsTo

	channel_frame = gtk_frame_new(_("channel"));
	m_channel_spin_box = gtk_spin_button_new_with_range (1.0, 16.0, 1.0);
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(m_channel_spin_box), m_channel_value + 1);
	gtk_container_add (GTK_CONTAINER(channel_frame), m_channel_spin_box);

	mute_frame = gtk_frame_new(_("muting"));
        m_mute_all_check = gtk_check_button_new_with_label (_("all"));
        m_mute_1_check = gtk_check_button_new_with_label ("1");
        m_mute_2_check = gtk_check_button_new_with_label ("2");
        m_mute_3_check = gtk_check_button_new_with_label ("3");
        m_mute_4_check = gtk_check_button_new_with_label ("4");
	
	mute_vbox = gtk_vbox_new (FALSE, 0);
	mute_hbox1 = gtk_hbox_new (FALSE, 0);
	mute_hbox2 = gtk_hbox_new (FALSE, 0);

	gtk_box_pack_start (GTK_BOX (mute_vbox), m_mute_all_check, FALSE, FALSE, 0);
	gtk_box_pack_start (GTK_BOX (mute_hbox1), m_mute_1_check, FALSE, FALSE, 0);
	gtk_box_pack_start (GTK_BOX (mute_hbox1), m_mute_2_check, FALSE, FALSE, 0);
	gtk_box_pack_start (GTK_BOX (mute_hbox2), m_mute_3_check, FALSE, FALSE, 0);
	gtk_box_pack_start (GTK_BOX (mute_hbox2), m_mute_4_check, FALSE, FALSE, 0);

	gtk_box_pack_start (GTK_BOX (mute_vbox), mute_hbox1, FALSE, FALSE, 0);
	gtk_box_pack_start (GTK_BOX (mute_vbox), mute_hbox2, FALSE, FALSE, 0);

	all_muted = true;
	for (i = 0; i < VOICE_COUNT; i++) {
		if (!muted[i]) all_muted = false;
	}

        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(m_mute_all_check), all_muted);
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(m_mute_1_check), muted[0]);
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(m_mute_2_check), muted[1]);
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(m_mute_3_check), muted[2]);
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(m_mute_4_check), muted[3]);
	gtk_container_add(GTK_CONTAINER(mute_frame), mute_vbox);

	voices_frame = gtk_frame_new(_("voices"));
	m_radio_all = gtk_radio_button_new_with_label(NULL, _("all"));
	g_signal_connect(m_mute_all_check, "toggled", G_CALLBACK (OnAllMutedToggle), (void *) this);
	m_handlrs[0] = g_signal_connect(m_mute_1_check, "toggled", G_CALLBACK (OnSingleMutedToggle), (void *) this);
	m_handlrs[1] = g_signal_connect(m_mute_2_check, "toggled", G_CALLBACK (OnSingleMutedToggle), (void *) this);
	m_handlrs[2] = g_signal_connect(m_mute_3_check, "toggled", G_CALLBACK (OnSingleMutedToggle), (void *) this);
	m_handlrs[3] = g_signal_connect(m_mute_4_check, "toggled", G_CALLBACK (OnSingleMutedToggle), (void *) this);
	m_radio_1 = gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON (m_radio_all), "1");
	m_radio_2 = gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON (m_radio_all), "2");
	m_radio_3 = gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON (m_radio_all), "3");
	m_radio_4 = gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON (m_radio_all), "4");


	if (different_voices) {
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(m_radio_1), TRUE);
		SetCurrentVoiceParamsTo(0);
		m_current_voice = 0;
	}
	else {
		SetCurrentVoiceParamsTo(-1);
		m_current_voice = -1;
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(m_radio_all), TRUE);
	}
	g_signal_connect(m_radio_all, "toggled", G_CALLBACK (OnVoiceToggle), (void *) this);
	g_signal_connect(m_radio_1, "toggled", G_CALLBACK (OnVoiceToggle), (void *) this);
	g_signal_connect(m_radio_2, "toggled", G_CALLBACK (OnVoiceToggle), (void *) this);
	g_signal_connect(m_radio_3, "toggled", G_CALLBACK (OnVoiceToggle), (void *) this);
	g_signal_connect(m_radio_4, "toggled", G_CALLBACK (OnVoiceToggle), (void *) this);

	vrb_vbox = gtk_vbox_new (FALSE, 0);
	vrb_hbox1 = gtk_hbox_new (FALSE, 0);
	vrb_hbox2 = gtk_hbox_new (FALSE, 0);

	gtk_box_pack_start (GTK_BOX (vrb_vbox), m_radio_all, FALSE, FALSE, 0);
	gtk_box_pack_start (GTK_BOX (vrb_hbox1), m_radio_1, FALSE, FALSE, 0);
	gtk_box_pack_start (GTK_BOX (vrb_hbox1), m_radio_2, FALSE, FALSE, 0);
	gtk_box_pack_start (GTK_BOX (vrb_hbox2), m_radio_3, FALSE, FALSE, 0);
	gtk_box_pack_start (GTK_BOX (vrb_hbox2), m_radio_4, FALSE, FALSE, 0);

	gtk_box_pack_start (GTK_BOX (vrb_vbox), vrb_hbox1, FALSE, FALSE, 0);
	gtk_box_pack_start (GTK_BOX (vrb_vbox), vrb_hbox2, FALSE, FALSE, 0);
	gtk_container_add(GTK_CONTAINER(voices_frame), vrb_vbox);



	mute_channel_vbox = gtk_vbox_new(FALSE, 2);
	gtk_box_pack_start(GTK_BOX(mute_channel_vbox), channel_frame,  FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(mute_channel_vbox), mute_frame, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(mute_channel_vbox), voices_frame, FALSE, FALSE, 0);

	if (staff_count > 1) {

		position_list_store = gtk_list_store_new (2, G_TYPE_STRING, G_TYPE_INT);

		for (i = 0; i < staff_count; i++) {
			gtk_list_store_append (position_list_store, &iter);
			if (main_window->m_staff_contexts[i].m_staff_name != NULL && strlen(main_window->m_staff_contexts[i].m_staff_name->getText()) > 0) {
				gtk_list_store_set (position_list_store, &iter, 0, main_window->m_staff_contexts[i].m_staff_name->getText() , 1, i, -1);
			}
			else {
				gtk_list_store_set (position_list_store, &iter, 0, _("staff") , 1, i, -1);
			}
		}

		m_position_list = gtk_tree_view_new_with_model(GTK_TREE_MODEL(position_list_store));
		position_renderer = gtk_cell_renderer_text_new ();
		position_column = gtk_tree_view_column_new_with_attributes (_("item"), position_renderer, "text", 0, NULL);
		gtk_tree_view_append_column (GTK_TREE_VIEW (m_position_list), position_column);
		position_num_column = gtk_tree_view_column_new_with_attributes (_("no"), position_renderer, "text", 1, NULL);
		gtk_tree_view_append_column (GTK_TREE_VIEW (m_position_list), position_num_column);
		position_adjustment = gtk_adjustment_new (0.0, 0.0, 4, 1.0,  10.0, 10.0);
		position_scroll = gtk_vscrollbar_new(GTK_ADJUSTMENT(position_adjustment));
		gtk_tree_view_set_vadjustment (GTK_TREE_VIEW (m_position_list), GTK_ADJUSTMENT(position_adjustment));
		gtk_widget_set_size_request(m_position_list, 150, 100);
		sprintf(Str, "%d", staff_pos);
		position_tree_path = gtk_tree_path_new_from_string(Str);
		gtk_tree_view_scroll_to_cell(GTK_TREE_VIEW (m_position_list), position_tree_path, NULL, FALSE, 0.0, 0.0);
		gtk_tree_view_set_cursor(GTK_TREE_VIEW (m_position_list), position_tree_path, NULL, FALSE);
	}

	gtk_box_pack_start (GTK_BOX (time_signature_upper_hbox), numerator_label, FALSE, FALSE, 0);
	gtk_box_pack_end (GTK_BOX (time_signature_upper_hbox), m_com_time_bu, FALSE, FALSE, 0);
	gtk_box_pack_end (GTK_BOX (time_signature_upper_hbox), m_numerator, FALSE, FALSE, 0);
	gtk_box_pack_start (GTK_BOX (time_signature_lower_hbox), denominator_label, FALSE, FALSE, 0);
	gtk_box_pack_end (GTK_BOX (time_signature_lower_hbox), m_cut_time_bu, FALSE, FALSE, 0);
	gtk_box_pack_end (GTK_BOX (time_signature_lower_hbox), m_denominator, FALSE, FALSE, 0);
	gtk_box_pack_start (GTK_BOX (time_signature_vbox), time_signature_upper_hbox, FALSE, FALSE, 0);
	gtk_box_pack_start (GTK_BOX (time_signature_vbox), time_signature_lower_hbox, FALSE, FALSE, 0);
	gtk_container_add (GTK_CONTAINER(time_signature_frame), time_signature_vbox);

	signature_name_vbox = gtk_vbox_new(FALSE, 2);
	gtk_box_pack_start(GTK_BOX(signature_name_vbox), time_signature_frame, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(signature_name_vbox), staff_name_frame, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(signature_name_vbox), staff_short_name_frame, FALSE, FALSE, 0);

	group_name_vbox = gtk_vbox_new(FALSE, 2);
	gtk_box_pack_start(GTK_BOX(group_name_vbox), group_name_frame, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(group_name_vbox), group_short_name_frame, FALSE, FALSE, 0);

	score_hbox = gtk_hbox_new(FALSE, 2);
	midi_hbox = gtk_hbox_new(FALSE, 2);
	gtk_box_pack_start(GTK_BOX(score_hbox), clef_frame, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(score_hbox), key_frame, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(score_hbox), signature_name_vbox, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(score_hbox), group_name_vbox, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(midi_hbox), tempo_frame, FALSE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(midi_hbox), chorus_frame, FALSE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(midi_hbox), reverb_frame, FALSE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(midi_hbox), play_transposed_frame, FALSE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(midi_hbox), volume_frame, FALSE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(midi_hbox), pan_frame, FALSE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(midi_hbox), m_instrument_list, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(midi_hbox), instrument_scroll, FALSE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(midi_hbox), mute_channel_vbox, FALSE, TRUE, 0);


	label_score = gtk_label_new(_("Score"));
	label_midi = gtk_label_new(_("MIDI"));
	
	m_table = gtk_notebook_new();
	gtk_notebook_append_page (GTK_NOTEBOOK(m_table), score_hbox, label_score);
	gtk_notebook_append_page (GTK_NOTEBOOK(m_table), midi_hbox, label_midi);
	if (staff_count > 1) {
		position_hbox =  gtk_hbox_new(FALSE, 2);
		gtk_box_pack_start(GTK_BOX(position_hbox), m_position_list, TRUE, TRUE, 0);
		gtk_box_pack_start(GTK_BOX(position_hbox), position_scroll, FALSE, TRUE, 0);
		label_position = gtk_label_new(_("Staff position"));
		gtk_notebook_append_page (GTK_NOTEBOOK(m_table), position_hbox, label_position);
	}

	gtk_container_add (GTK_CONTAINER (GTK_DIALOG(dialog)->vbox), m_table);
	gtk_widget_show_all (dialog);
	gtk_dialog_run(GTK_DIALOG(dialog));

}

void NedStaffContextDialog::SetCurrentVoiceParamsTo(int voice) {
	int idx = 0;
	char Str[128];
	GtkTreePath* instrument_tree_path;
	switch (voice) {
		case -1: 
		case 0: idx = 0; break;
		case 1: idx = 1; break;
		case 2: idx = 2; break;
		case 3: idx = 3; break;
		default: NedResource::Abort("NedStaffContextDialog::SetCurrentVoiceParamsTo");
	}
	gtk_range_set_value(GTK_RANGE(m_volume_scale), m_volume[idx]);
	gtk_range_set_value(GTK_RANGE(m_pan_scale), m_pan[idx]);
	sprintf(Str, "%d", m_midi_program_number[idx]);
	instrument_tree_path = gtk_tree_path_new_from_string(Str);
	gtk_tree_view_scroll_to_cell(GTK_TREE_VIEW (m_instrument_list), instrument_tree_path, NULL, FALSE, 0.0, 0.0);
	gtk_tree_view_set_cursor(GTK_TREE_VIEW (m_instrument_list), instrument_tree_path, NULL, FALSE);
}

void NedStaffContextDialog::GetCurrentVoiceParamsFrom(int voice, int volume[VOICE_COUNT], int pan[VOICE_COUNT], int instrument[VOICE_COUNT], bool *changed) {
	int idx = 0;
	GtkTreeSelection* selection;
	GList *selected_rows;
	int newvolume, newpan, newpgm;
	switch (voice) {
		case -1: 
		case 0: idx = 0; break;
		case 1: idx = 1; break;
		case 2: idx = 2; break;
		case 3: idx = 3; break;
		default: NedResource::Abort("NedStaffContextDialog::GetCurrentVoiceParamsFRom");
	}
	newvolume = (int) gtk_range_get_value(GTK_RANGE(m_volume_scale));
	if (newvolume != volume[idx]) {
		volume[idx] = newvolume;
		*changed = TRUE;
	}
	newpan = (int) gtk_range_get_value(GTK_RANGE(m_pan_scale));
	if (newpan != pan[idx]) {
		pan[idx] = newpan;
		*changed = TRUE;
	}
	selection =  gtk_tree_view_get_selection (GTK_TREE_VIEW(m_instrument_list));
	selected_rows = gtk_tree_selection_get_selected_rows (selection, NULL);
	if ( g_list_length(selected_rows) > 0) {
		GtkTreePath *path = (GtkTreePath *) g_list_first(selected_rows)->data;
		char *tp = gtk_tree_path_to_string(path);
		if (sscanf(tp, "%d", &(newpgm)) != 1) {
			NedResource::Abort("NedStaffContextDialog::OnClose: error reading tree path(1)");
		}
	}
	else {
		newpgm = -1;
	}
	if (instrument[idx] != newpgm) {
		instrument[idx] = newpgm;
		*changed = true;
	}
}

void NedStaffContextDialog::OnClose(GtkDialog *dialog, gint result, gpointer data) {
	int i;
	bool newmute;
	int newchannel;
	int newnum, newdenom, newkeysig, newtempo;
	int newchorus, newplaytransposed, newreverb, new_octave_shift;
	const char *newname, *newshortname;
	const char *newgroupname, *newshortgroupname;
	bool diff, newdifferentvoices;
	GtkTreeSelection* selection;
	GList *selected_rows;

	NedStaffContextDialog *context_dialog = (NedStaffContextDialog *) data;
	context_dialog->m_delete_staff = FALSE;
	context_dialog->m_delete_system = FALSE;
	switch (result) {
		case GTK_RESPONSE_ACCEPT:
			context_dialog->m_state = TRUE;
		break;
		case DELETE_STAFF_CMD:
			context_dialog->m_delete_staff = TRUE;
			context_dialog->m_state = FALSE;
			break;
		case DELETE_SYSTEM_CMD:
			context_dialog->m_delete_system = TRUE;
			context_dialog->m_state = FALSE;
			break;
		default:
			context_dialog->m_state = FALSE;
		break;
	}
	newkeysig = 0;
	context_dialog->m_key_signature_number = 0;
	for (i = 0; i < 13; i++) {
		if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (context_dialog->m_sig_radio_buttons[i]))) {
			newkeysig = i - 6;
		}
	}
	if (newkeysig != context_dialog->m_key_signature_number) {
		context_dialog->m_key_signature_number = newkeysig;
		context_dialog->m_config_changed = TRUE;
	}
	newnum = (int) gtk_spin_button_get_value(GTK_SPIN_BUTTON(context_dialog->m_numerator));
	if (newnum != context_dialog->m_numerator_value) {
		context_dialog->m_numerator_value = newnum;
		context_dialog->m_config_changed = TRUE;
	}
	switch (gtk_combo_box_get_active(GTK_COMBO_BOX(context_dialog->m_denominator))) {
		case 0: newdenom = 2; break;
		case 1: newdenom = 4; break;
		case 2: newdenom = 8; break;
		default: newdenom = 16; break;
	}
	if (newdenom != context_dialog->m_denominator_value) {
		context_dialog->m_denominator_value = newdenom;
		context_dialog->m_config_changed = TRUE;
	}
	newtempo = (int) gtk_range_get_value(GTK_RANGE(context_dialog->m_tempo_scale));
	if (newtempo != context_dialog->m_tempo) {
		context_dialog->m_tempo = newtempo;
		context_dialog->m_config_changed = TRUE;
	}
	newchorus = (int) gtk_range_get_value(GTK_RANGE(context_dialog->m_chorus_scale));
	if (newchorus != context_dialog->m_chorus) {
		context_dialog->m_chorus = newchorus;
		context_dialog->m_config_changed = TRUE;
	}
	newplaytransposed = (int) gtk_range_get_value(GTK_RANGE(context_dialog->m_play_transposed_scale));
	if (newplaytransposed != context_dialog->m_play_transposed) {
		context_dialog->m_play_transposed = newplaytransposed;
		context_dialog->m_config_changed = TRUE;
	}
	newreverb = (int) gtk_range_get_value(GTK_RANGE(context_dialog->m_reverb_scale));
	if (newreverb != context_dialog->m_reverb) {
		context_dialog->m_reverb = newreverb;
		context_dialog->m_config_changed = TRUE;
	}
	newmute = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(context_dialog->m_mute_1_check));
	if (context_dialog->m_muted[0] != newmute) {
		context_dialog->m_muted[0] = newmute;
		context_dialog->m_config_changed = TRUE;
	}
	newmute = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(context_dialog->m_mute_2_check));
	if (context_dialog->m_muted[1] != newmute) {
		context_dialog->m_muted[1] = newmute;
		context_dialog->m_config_changed = TRUE;
	}
	newmute = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(context_dialog->m_mute_3_check));
	if (context_dialog->m_muted[2] != newmute) {
		context_dialog->m_muted[2] = newmute;
		context_dialog->m_config_changed = TRUE;
	}
	newmute = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(context_dialog->m_mute_4_check));
	if (context_dialog->m_muted[3] != newmute) {
		context_dialog->m_muted[3] = newmute;
		context_dialog->m_config_changed = TRUE;
	}
	context_dialog->m_do_adjust = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(context_dialog->m_adjust_check_box));
	newname = gtk_entry_get_text(GTK_ENTRY(context_dialog->m_staffname));
	if (strlen(newname) < 1) {
		newname = NULL;
	}
	diff = (newname == NULL && context_dialog->m_staff_name != NULL) || 
		(newname != NULL && context_dialog->m_staff_name == NULL);
	if (newname  != NULL && context_dialog->m_staff_name != NULL) {
		diff = diff || strcmp(newname, context_dialog->m_staff_name) != 0;
	}
	if (diff) {
		context_dialog->m_config_changed = TRUE;
	}
	if (newname != NULL && strlen(newname) > 0) {
		context_dialog->m_staff_name = strdup(newname);
	}
	else {
		context_dialog->m_staff_name = NULL;
	}
	newshortname = gtk_entry_get_text(GTK_ENTRY(context_dialog->m_staffshortname));
	diff = (newshortname == NULL && context_dialog->m_staff_short_name != NULL) || 
		(newshortname != NULL && context_dialog->m_staff_short_name == NULL);
	if (newshortname  != NULL && context_dialog->m_staff_short_name != NULL) {
		diff = diff || strcmp(newshortname, context_dialog->m_staff_short_name) != 0;
	}
	if (diff) {
		context_dialog->m_config_changed = TRUE;
	}
	if (newshortname != NULL && strlen(newshortname) > 0) {
		context_dialog->m_staff_short_name = strdup(newshortname);
	}
	else {
		context_dialog->m_staff_short_name = NULL;
	}
	newgroupname = gtk_entry_get_text(GTK_ENTRY(context_dialog->m_groupname));
	if (strlen(newgroupname) < 1) {
		newgroupname = NULL;
	}
	diff = (newgroupname == NULL && context_dialog->m_group_name != NULL) || 
		(newgroupname != NULL && context_dialog->m_group_name == NULL);
	if (newgroupname  != NULL && context_dialog->m_group_name != NULL) {
		diff = diff || strcmp(newgroupname, context_dialog->m_group_name) != 0;
	}
	if (diff) {
		context_dialog->m_config_changed = TRUE;
	}
	if (newgroupname != NULL && strlen(newgroupname) > 0) {
		context_dialog->m_group_name = strdup(newgroupname);
	}
	else {
		context_dialog->m_group_name = NULL;
	}
	newshortgroupname = gtk_entry_get_text(GTK_ENTRY(context_dialog->m_groupshortname));
	diff = (newshortgroupname == NULL && context_dialog->m_group_short_name != NULL) || 
		(newshortgroupname != NULL && context_dialog->m_group_short_name == NULL);
	if (newshortgroupname  != NULL && context_dialog->m_group_short_name != NULL) {
		diff = diff || strcmp(newshortgroupname, context_dialog->m_group_short_name) != 0;
	}
	if (diff) {
		context_dialog->m_config_changed = TRUE;
	}
	if (newshortgroupname != NULL && strlen(newshortgroupname) > 0) {
		context_dialog->m_group_short_name = strdup(newshortgroupname);
	}
	else {
		context_dialog->m_group_short_name = NULL;
	}
	if (context_dialog->m_staff_count > 1) {
		selection = gtk_tree_view_get_selection (GTK_TREE_VIEW(context_dialog->m_position_list));
		selected_rows = gtk_tree_selection_get_selected_rows (selection, NULL);
		if (g_list_length(selected_rows) > 0) {
			GtkTreePath *path = (GtkTreePath *) g_list_first(selected_rows)->data;
			char *tp = gtk_tree_path_to_string(path);
			if (sscanf(tp, "%d", &(context_dialog->m_staff_pos)) != 1) {
				NedResource::Abort("NedStaffContextDialog::OnClose: error reading tree path(2)");
			}
		}
	}
	new_octave_shift = (int) gtk_spin_button_get_value(GTK_SPIN_BUTTON(context_dialog->m_octave_shift_spin_box));
	switch (new_octave_shift) {
		case 8: new_octave_shift = 12; break;
		case -8: new_octave_shift = -12; break;
	}

	if (new_octave_shift != context_dialog->m_octave_shift) {
		context_dialog->m_octave_shift = new_octave_shift;
		context_dialog->m_config_changed = true;
	}

	newchannel = (int) gtk_spin_button_get_value(GTK_SPIN_BUTTON(context_dialog->m_channel_spin_box)) - 1;
	if (newchannel != context_dialog->m_channel_value) {
		context_dialog->m_channel_value = newchannel;
		context_dialog->m_config_changed = true;
	}
	if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(context_dialog->m_com_time_bu))) {
		context_dialog->m_symbol = TIME_SYMBOL_COMMON_TIME;
		context_dialog->m_numerator_value = context_dialog->m_denominator_value = 4;
	}
	else if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(context_dialog->m_cut_time_bu))) {
		context_dialog->m_symbol = TIME_SYMBOL_CUT_TIME;
		context_dialog->m_numerator_value = context_dialog->m_denominator_value = 2;
	}
	else {
		context_dialog->m_symbol = TIME_SYMBOL_NONE;
	}
	newdifferentvoices = !gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(context_dialog->m_radio_all));
	if (newdifferentvoices != context_dialog->m_different_voices) {
		context_dialog->m_config_changed = TRUE;
		context_dialog->m_different_voices = newdifferentvoices;
	}
	context_dialog->GetCurrentVoiceParamsFrom(context_dialog->m_current_voice, context_dialog->m_volume, context_dialog->m_pan, context_dialog->m_midi_program_number,
					 &(context_dialog->m_config_changed));
	gtk_widget_destroy (GTK_WIDGET(dialog));
}

void NedStaffContextDialog::getValues(bool *state, bool *different_voices, bool *delete_staff, bool *delete_system, int *staff_pos, char **staff_name, char **staff_short_name, char **group_name, char **group_short_name, int *clef_number, int *octave_shift, int *key_signature_number, bool *ajdust_notes, int *numerator, int *demoninator,  unsigned int *symbol, int volume[VOICE_COUNT], int midi_program[VOICE_COUNT], int *channel, int *tempo, int pan[VOICE_COUNT], int *chorus, int *play_transposed, int *reverb, bool *config_changed, bool muted[VOICE_COUNT]) {
	int i;
	*different_voices = m_different_voices;
	*delete_staff = m_delete_staff;
	*delete_system = m_delete_system;
	*state = m_state;
	*staff_pos = m_staff_pos;
	if (m_newclef != m_current_clef) {
		m_current_clef = m_newclef;
		m_config_changed = TRUE;
	}
	*ajdust_notes = m_do_adjust;
	*staff_name = m_staff_name;
	*staff_short_name = m_staff_short_name;
	*group_name = m_group_name;
	*group_short_name = m_group_short_name;
	*clef_number = m_current_clef;
	*key_signature_number = m_key_signature_number;
	*numerator = m_numerator_value;
	*demoninator = m_denominator_value;
	*tempo = m_tempo;
	*chorus = m_chorus;
	*play_transposed = m_play_transposed;
	*reverb = m_reverb;
	*octave_shift = m_octave_shift;
	*channel = m_channel_value;
	*muted = m_muted;
	*symbol = m_symbol;
	for (i = 0; i < VOICE_COUNT; i++) {
		volume[i] = m_volume[i];
		midi_program[i] = m_midi_program_number[i];
		pan[i] = m_pan[i];
		muted[i] = m_muted[i];
	}
	if (m_state) {
		*config_changed = m_config_changed;
	}
	else {
		*config_changed = FALSE;
	}
}

gboolean NedStaffContextDialog::draw_clefs(GtkWidget *widget, GdkEventExpose *event, gpointer data) {
#define ZOOM_LEVEL 9
	NedStaffContextDialog *the_dialog = (NedStaffContextDialog *) data;
	cairo_scaled_font_t *scaled_font;
	cairo_glyph_t glyph;
	int i;
	double y_offs = 0.0, oct_y_offs = 0.0, oct_x_offs = 0.0;
	bool octave_shift = false;
	
	cairo_t *cr;

	cr = gdk_cairo_create (the_dialog->m_clef_show_window->window);
	cairo_set_source_rgb (cr, 1.0, 1.0, 1.0);
	cairo_rectangle (cr,  0, 0,  the_dialog->m_clef_show_window->allocation.width, the_dialog->m_clef_show_window->allocation.height);
	cairo_fill(cr);

	cairo_set_source_rgb (cr, 0.0, 0.0, 0.0);
	scaled_font = NedResource::getScaledFont(ZOOM_LEVEL);
#ifdef HAS_SET_SCALED_FONT
	cairo_set_scaled_font (cr, scaled_font);
#else
	cairo_set_font_face(cr, NedResource::getFontFace());
	cairo_set_font_matrix(cr,  NedResource::getFontMatrix(ZOOM_LEVEL));
	cairo_set_font_options(cr, NedResource::getFontOptions());
#endif
	cairo_new_path(cr);
	cairo_set_line_width(cr, DEMO_LINE_THICK);
	for (i = 0; i < 5; i++) {
		cairo_move_to(cr, DEMO_LINE_BEGIN, DEMO_LINE_YPOS + DEMO_LINE_DIST * i);
		cairo_line_to(cr, DEMO_LINE_END, DEMO_LINE_YPOS + DEMO_LINE_DIST * i);
	}
	cairo_stroke(cr);
	cairo_new_path(cr);
	switch (the_dialog->m_newclef) {
		case TREBLE_CLEF:
				switch(the_dialog->m_octave_shift) {
					case  12: oct_y_offs = DEMO_POS_FAK * (OCTUP_Y_TREBLE_DIST_UP); octave_shift = true; break;
					case -12: oct_y_offs = DEMO_POS_FAK * (OCTUP_Y_TREBLE_DIST_DOWN); octave_shift = true; break;
				  }
				  glyph.index = BASE + 2; break;
		case BASS_CLEF: 
				switch(the_dialog->m_octave_shift) {
					case  12: oct_y_offs = DEMO_POS_FAK * (OCTUP_Y_BASS_DIST_UP); octave_shift = true; break;
					case -12: oct_y_offs = DEMO_POS_FAK * (OCTUP_Y_BASS_DIST_DOWN); octave_shift = true; break;
				  }
				glyph.index = BASE + 3; break;
		case ALTO_CLEF: 
				switch(the_dialog->m_octave_shift) {
					case  12: oct_y_offs = DEMO_POS_FAK * (OCTUP_Y_ALTO_DIST_UP);
						 oct_x_offs = DEMO_POS_FAK * (OCTUP_X_ALTO_DIST);
						 octave_shift = true; break;
					case -12: oct_y_offs = DEMO_POS_FAK * (OCTUP_Y_ALTO_DIST_DOWN);
						 oct_x_offs = DEMO_POS_FAK * (OCTUP_X_ALTO_DIST);
					         octave_shift = true; break;
				  }
				glyph.index = BASE + 1; break;
		case SOPRAN_CLEF: 
				switch(the_dialog->m_octave_shift) {
					case  12: oct_y_offs = DEMO_POS_FAK * (OCTUP_Y_ALTO_DIST_UP);
						 oct_x_offs = DEMO_POS_FAK * (OCTUP_X_ALTO_DIST);
						 octave_shift = true; break;
					case -12: oct_y_offs = DEMO_POS_FAK * (OCTUP_Y_ALTO_DIST_DOWN);
						 oct_x_offs = DEMO_POS_FAK * (OCTUP_X_ALTO_DIST);
						 octave_shift = true; break;
				  }
				glyph.index = BASE + 1; y_offs = 2 * DEMO_LINE_DIST; break;
		case TENOR_CLEF: 
				switch(the_dialog->m_octave_shift) {
					case  12: oct_y_offs = DEMO_POS_FAK * (OCTUP_Y_ALTO_DIST_UP);
						 oct_x_offs = DEMO_POS_FAK * (OCTUP_X_ALTO_DIST);
						 octave_shift = true; break;
					case -12: oct_y_offs = DEMO_POS_FAK * (OCTUP_Y_ALTO_DIST_DOWN);
						 oct_x_offs = DEMO_POS_FAK * (OCTUP_X_ALTO_DIST);
						 octave_shift = true; break;
				  }
				glyph.index = BASE + 1; y_offs = -DEMO_LINE_DIST; break;
		case NEUTRAL_CLEF1: 
		case NEUTRAL_CLEF2: 
		case NEUTRAL_CLEF3: 
				glyph.index = BASE + 27; break;
	}
	glyph.x = 50.0;
	glyph.y = 70.0 + y_offs;
	cairo_show_glyphs(cr, &glyph, 1);
	switch (the_dialog->m_newclef) {
		case NEUTRAL_CLEF1: glyph.index = NUMBERBASE + 1; 
					glyph.x = 60.0;
					glyph.y = 120.0;
					cairo_show_glyphs(cr, &glyph, 1);
					break;
		case NEUTRAL_CLEF2: glyph.index = NUMBERBASE + 2; 
					glyph.x = 60.0;
					glyph.y = 120.0;
					cairo_show_glyphs(cr, &glyph, 1);
					break;
		case NEUTRAL_CLEF3: glyph.index = NUMBERBASE + 3; 
					glyph.x = 60.0;
					glyph.y = 120.0;
					cairo_show_glyphs(cr, &glyph, 1);
					break;
	}
	if (octave_shift) {
		glyph.x = 50.0 + oct_x_offs;
		glyph.y = 70.0 + y_offs + oct_y_offs;
		glyph.index = BASE + 28;
		cairo_show_glyphs(cr, &glyph, 1);
	}
	cairo_destroy (cr);
	return FALSE;
}

void NedStaffContextDialog::decrease_clef_number (GtkButton *button, gpointer data) {
	GdkRectangle rect;
	NedStaffContextDialog *the_dialog = (NedStaffContextDialog *) data;
	if (the_dialog->m_newclef <= MINCLEF_NUMBER) return;
	the_dialog->m_newclef--;
	gtk_widget_set_sensitive(the_dialog->m_clef_bu_up, TRUE);
	gtk_widget_set_sensitive(the_dialog->m_clef_bu_down, TRUE);
	if (the_dialog->m_newclef == MINCLEF_NUMBER) {
		gtk_widget_set_sensitive(the_dialog->m_clef_bu_up, FALSE);
	}
	rect.x = rect.y = 0;
	rect.width = the_dialog->m_clef_show_window->allocation.width;
	rect.height = the_dialog->m_clef_show_window->allocation.height;
	gdk_window_invalidate_rect (the_dialog->m_clef_show_window->window, &rect, FALSE);
}

void NedStaffContextDialog::increase_clef_number (GtkButton *button, gpointer data) {
	GdkRectangle rect;
	NedStaffContextDialog *the_dialog = (NedStaffContextDialog *) data;
	if (the_dialog->m_newclef >= MAXCLEF_NUMBER) return;
	the_dialog->m_newclef++;
	gtk_widget_set_sensitive(the_dialog->m_clef_bu_up, TRUE);
	gtk_widget_set_sensitive(the_dialog->m_clef_bu_down, TRUE);
	if (the_dialog->m_newclef >= MAXCLEF_NUMBER) {
		gtk_widget_set_sensitive(the_dialog->m_clef_bu_down, FALSE);
	}
	rect.x = rect.y = 0;
	rect.width = the_dialog->m_clef_show_window->allocation.width;
	rect.height = the_dialog->m_clef_show_window->allocation.height;
	gdk_window_invalidate_rect (the_dialog->m_clef_show_window->window, &rect, FALSE);
}

gboolean NedStaffContextDialog::OnScroll(GtkWidget *widget, GdkEventScroll *event, gpointer data) {
#define SCROLL_INCR 20.0
	double adjval;
	NedStaffContextDialog *the_dialog = (NedStaffContextDialog *) data;
	if (gtk_notebook_get_current_page(GTK_NOTEBOOK(the_dialog->m_table)) != 1) return FALSE;
	adjval = gtk_adjustment_get_value(GTK_ADJUSTMENT(the_dialog->m_instrument_adjustment));

	if (event->direction == GDK_SCROLL_UP) {
		adjval -= SCROLL_INCR;
	}
	else if (event->direction == GDK_SCROLL_DOWN) {
		adjval += SCROLL_INCR;
	}
	gtk_adjustment_set_value(GTK_ADJUSTMENT(the_dialog->m_instrument_adjustment), adjval);
	return FALSE;
}

void NedStaffContextDialog::octave_shift_change(GtkSpinButton *spinbutton, gpointer data) {
	GdkRectangle rect;
	NedStaffContextDialog *context_dialog = (NedStaffContextDialog *) data;

	context_dialog->m_octave_shift = (int) gtk_spin_button_get_value(spinbutton);
	switch (context_dialog->m_octave_shift) {
		case 8: context_dialog->m_octave_shift = 12; break;
		case -8: context_dialog->m_octave_shift = -12; break;
	}
	rect.x = rect.y = 0;
	rect.width = context_dialog->m_clef_show_window->allocation.width;
	rect.height = context_dialog->m_clef_show_window->allocation.height;
	gdk_window_invalidate_rect (context_dialog->m_clef_show_window->window, &rect, FALSE);
}

void NedStaffContextDialog::OnCommButtonToggled(GtkToggleButton *togglebutton, gpointer data) {
	NedStaffContextDialog *context_dialog = (NedStaffContextDialog *) data;

	if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(context_dialog->m_com_time_bu))) {
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(context_dialog->m_cut_time_bu), FALSE);
		gtk_spin_button_set_value(GTK_SPIN_BUTTON(context_dialog->m_numerator), 4);
		gtk_combo_box_set_active(GTK_COMBO_BOX(context_dialog->m_denominator), 1);
		gtk_widget_set_sensitive(context_dialog->m_numerator, FALSE);
		gtk_widget_set_sensitive(context_dialog->m_denominator, FALSE);
	}
	else if (!gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(context_dialog->m_cut_time_bu))) {
		gtk_widget_set_sensitive(context_dialog->m_numerator, TRUE);
		gtk_widget_set_sensitive(context_dialog->m_denominator, TRUE);
	}
}

void NedStaffContextDialog::OnCutButtonToggled(GtkToggleButton *togglebutton, gpointer data) {
	NedStaffContextDialog *context_dialog = (NedStaffContextDialog *) data;

	if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(context_dialog->m_cut_time_bu))) {
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(context_dialog->m_com_time_bu), FALSE);
		gtk_spin_button_set_value(GTK_SPIN_BUTTON(context_dialog->m_numerator), 2);
		gtk_combo_box_set_active(GTK_COMBO_BOX(context_dialog->m_denominator), 0);
		gtk_widget_set_sensitive(context_dialog->m_numerator, FALSE);
		gtk_widget_set_sensitive(context_dialog->m_denominator, FALSE);
	}
	else if (!gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(context_dialog->m_com_time_bu))) {
		gtk_widget_set_sensitive(context_dialog->m_numerator, TRUE);
		gtk_widget_set_sensitive(context_dialog->m_denominator, TRUE);
	}
}

void NedStaffContextDialog::OnVoiceToggle(GtkToggleButton *togglebutton, gpointer data) {
	NedStaffContextDialog *context_dialog = (NedStaffContextDialog *) data;
	int oldvoice = context_dialog->m_current_voice;

	if (togglebutton == GTK_TOGGLE_BUTTON(context_dialog->m_radio_all)) {
		if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(context_dialog->m_radio_all))) {
			context_dialog->SetCurrentVoiceParamsTo(-1);
			context_dialog->m_current_voice = -1;
		}
		else {
			context_dialog->GetCurrentVoiceParamsFrom(-1, context_dialog->m_volume, context_dialog->m_pan,
							context_dialog->m_midi_program_number, &(context_dialog->m_config_changed));
		}
	}
	else if (togglebutton == GTK_TOGGLE_BUTTON(context_dialog->m_radio_1)) {
		if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(context_dialog->m_radio_1))) {
			context_dialog->SetCurrentVoiceParamsTo(0);
			context_dialog->m_current_voice = 0;
		}
		else {
			context_dialog->GetCurrentVoiceParamsFrom(0, context_dialog->m_volume, context_dialog->m_pan,
							context_dialog->m_midi_program_number, &(context_dialog->m_config_changed));
		}
	}
	else if (togglebutton == GTK_TOGGLE_BUTTON(context_dialog->m_radio_2)) {
		if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(context_dialog->m_radio_2))) {
			context_dialog->SetCurrentVoiceParamsTo(1);
			context_dialog->m_current_voice = 1;
		}
		else {
			context_dialog->GetCurrentVoiceParamsFrom(1, context_dialog->m_volume, context_dialog->m_pan,
							context_dialog->m_midi_program_number, &(context_dialog->m_config_changed));
		}
	}
	else if (togglebutton == GTK_TOGGLE_BUTTON(context_dialog->m_radio_3)) {
		if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(context_dialog->m_radio_3))) {
			context_dialog->SetCurrentVoiceParamsTo(2);
			context_dialog->m_current_voice = 2;
		}
		else {
			context_dialog->GetCurrentVoiceParamsFrom(2, context_dialog->m_volume, context_dialog->m_pan,
							context_dialog->m_midi_program_number, &(context_dialog->m_config_changed));
		}
	}
	else if (togglebutton == GTK_TOGGLE_BUTTON(context_dialog->m_radio_4)) {
		if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(context_dialog->m_radio_4))) {
			context_dialog->SetCurrentVoiceParamsTo(3);
			context_dialog->m_current_voice = 3;
		}
		else {
			context_dialog->GetCurrentVoiceParamsFrom(3, context_dialog->m_volume, context_dialog->m_pan,
							context_dialog->m_midi_program_number, &(context_dialog->m_config_changed));
		}
	}
}

void NedStaffContextDialog::OnAllMutedToggle(GtkToggleButton *togglebutton, gpointer data) {
	bool toggle_state;

	NedStaffContextDialog *context_dialog = (NedStaffContextDialog *) data;

	toggle_state = gtk_toggle_button_get_active (togglebutton);

	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(context_dialog->m_mute_1_check), toggle_state);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(context_dialog->m_mute_2_check), toggle_state);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(context_dialog->m_mute_3_check), toggle_state);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(context_dialog->m_mute_4_check), toggle_state);
}

void NedStaffContextDialog::OnSingleMutedToggle(GtkToggleButton *togglebutton, gpointer data) {
	int i;
	bool toggle_state;
	bool all_muted = true;
	bool none_muted = true;
	NedStaffContextDialog *context_dialog = (NedStaffContextDialog *) data;

	toggle_state = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(context_dialog->m_mute_1_check));
	if (toggle_state) {
		none_muted = false;
	}
	else {
		all_muted = false;
	}
		
	toggle_state = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(context_dialog->m_mute_2_check));
	if (toggle_state) {
		none_muted = false;
	}
	else {
		all_muted = false;
	}
	toggle_state = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(context_dialog->m_mute_3_check));
	if (toggle_state) {
		none_muted = false;
	}
	else {
		all_muted = false;
	}
	toggle_state = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(context_dialog->m_mute_4_check));
	if (toggle_state) {
		none_muted = false;
	}
	else {
		all_muted = false;
	}

	g_signal_handler_block(context_dialog->m_mute_1_check, context_dialog->m_handlrs[0]);
	g_signal_handler_block(context_dialog->m_mute_2_check, context_dialog->m_handlrs[1]);
	g_signal_handler_block(context_dialog->m_mute_3_check, context_dialog->m_handlrs[2]);
	g_signal_handler_block(context_dialog->m_mute_4_check, context_dialog->m_handlrs[3]);
	if (none_muted) {
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(context_dialog->m_mute_all_check), FALSE);
	}
	if (all_muted) {
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(context_dialog->m_mute_all_check), TRUE);
	}
	g_signal_handler_unblock(context_dialog->m_mute_1_check, context_dialog->m_handlrs[0]);
	g_signal_handler_unblock(context_dialog->m_mute_2_check, context_dialog->m_handlrs[1]);
	g_signal_handler_unblock(context_dialog->m_mute_3_check, context_dialog->m_handlrs[2]);
	g_signal_handler_unblock(context_dialog->m_mute_4_check, context_dialog->m_handlrs[3]);
	
}
