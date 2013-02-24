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
#include "midirecordconfigdialog.h"
#include "localization.h"
#include "resource.h"
#include "mainwindow.h"
#include "staffcontextdialog.h"

#define INSTRUMENTS_WIDGET_WIDTH 200
#define TIMESIG_FRAME_WIDTH 100

#define MIDI_RECORD_CONFIG_SHOW_WINDOW_WIDTH (INSTRUMENTS_WIDGET_WIDTH + TIMESIG_FRAME_WIDTH)
#define MIDI_RECORD_CONFIG_SHOW_WINDOW_HEIGHT 200


NedMidiRecordConfigDialog::NedMidiRecordConfigDialog(GtkWindow *parent, int num, int denom, int midi_pgm,
                        int tempo_inverse, int volume, bool triplet, bool f_piano, bool dnt_split, int keysig) :
m_num(num), m_denom(denom), m_pgm(midi_pgm), m_tempo_inverse(tempo_inverse), m_metro_volume(volume), m_triplet(triplet), m_f_piano(f_piano), m_dnt_split(dnt_split), m_keysig(keysig) {
	int i;
	GtkWidget *dialog;
	GtkWidget *time_signature_frame;
	GtkWidget *numerator_label;
	GtkWidget *denominator_label;
	GtkWidget *time_signature_vbox;
	GtkWidget *time_signature_upper_hbox;
	GtkWidget *time_signature_lower_hbox;
	GtkWidget *right_vbox;
	GtkWidget *instrument_scroll;
	GtkWidget *upper_hbox;
	GtkListStore *instrumend_list_store;
	GtkTreeIter iter;
	GtkCellRenderer *instrument_renderer;
	GtkTreeViewColumn *instrument_column;
	GtkTreeViewColumn *instrument_num_column;
	GtkTreePath* instrument_tree_path;
	GtkWidget *tempo_frame;
	GtkWidget *key_frame;
	GtkWidget *key_frame_left_vbox;
	GtkWidget *key_frame_right_vbox;
	GtkWidget *radio_hbox;
	GtkWidget *main_vbox;
	GtkWidget *metro_volume_frame;
	char Str[128];

	dialog = gtk_dialog_new_with_buttons(_("Config Record"), parent, (GtkDialogFlags) (GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT),
		GTK_STOCK_OK, GTK_RESPONSE_ACCEPT, GTK_STOCK_CANCEL, GTK_RESPONSE_REJECT, NULL);

	g_signal_connect(dialog, "response", G_CALLBACK (OnClose), (void *) this);

	time_signature_frame = gtk_frame_new(_("time signature"));
	time_signature_vbox = gtk_vbox_new(FALSE, 5);
	time_signature_upper_hbox = gtk_hbox_new(FALSE, 5);
	time_signature_lower_hbox = gtk_hbox_new(FALSE, 5);
	numerator_label = gtk_label_new(_("Numerator:"));
	denominator_label = gtk_label_new(_("Denominator:"));
	m_numerator = gtk_spin_button_new_with_range (1.0, 20.0, 1.0);
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(m_numerator), m_num);
	m_denominator = gtk_combo_box_new_text();
	gtk_combo_box_append_text(GTK_COMBO_BOX(m_denominator), "2");
	gtk_combo_box_append_text(GTK_COMBO_BOX(m_denominator), "4");
	gtk_combo_box_append_text(GTK_COMBO_BOX(m_denominator), "8");
	gtk_combo_box_append_text(GTK_COMBO_BOX(m_denominator), "16");
	if (m_denom < 4) {
		gtk_combo_box_set_active(GTK_COMBO_BOX(m_denominator), 0);
	}
	else if (m_denom < 8) {
		gtk_combo_box_set_active(GTK_COMBO_BOX(m_denominator), 1);
	}
	else if (m_denom < 16) {
		gtk_combo_box_set_active(GTK_COMBO_BOX(m_denominator), 2);
	}
	else {
		gtk_combo_box_set_active(GTK_COMBO_BOX(m_denominator), 3);
	}
	gtk_box_pack_start (GTK_BOX (time_signature_upper_hbox), numerator_label, FALSE, FALSE, 0);
	gtk_box_pack_end (GTK_BOX (time_signature_upper_hbox), m_numerator, FALSE, FALSE, 0);
	gtk_box_pack_start (GTK_BOX (time_signature_lower_hbox), denominator_label, FALSE, FALSE, 0);
	gtk_box_pack_end (GTK_BOX (time_signature_lower_hbox), m_denominator, FALSE, FALSE, 0);
	gtk_box_pack_start (GTK_BOX (time_signature_vbox), time_signature_upper_hbox, FALSE, FALSE, 0);
	gtk_box_pack_start (GTK_BOX (time_signature_vbox), time_signature_lower_hbox, FALSE, FALSE, 0);
	gtk_container_add (GTK_CONTAINER(time_signature_frame), time_signature_vbox);

	m_with_triplet_button = gtk_toggle_button_new_with_label(_("triplet recognition"));
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(m_with_triplet_button), m_triplet);
	m_force_piano_button = gtk_toggle_button_new_with_label(_("force piano"));
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(m_force_piano_button), m_f_piano);
	m_dont_split_button = gtk_toggle_button_new_with_label(_("don't split tracks"));
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(m_dont_split_button), m_dnt_split);
	if (m_dnt_split) {
		gtk_widget_set_sensitive(m_force_piano_button, FALSE);
	}
	else if (m_f_piano) {
		gtk_widget_set_sensitive(m_dont_split_button, FALSE);
	}
	g_signal_connect(m_force_piano_button, "toggled", G_CALLBACK (OnPianoForcePressed), (void *) this);
	g_signal_connect(m_dont_split_button, "toggled", G_CALLBACK (OnDontSplitPressed), (void *) this);

	right_vbox = gtk_vbox_new(FALSE, 5);
	gtk_box_pack_start (GTK_BOX(right_vbox), time_signature_frame, FALSE, FALSE, 0);

	gtk_box_pack_start (GTK_BOX(right_vbox), m_with_triplet_button, FALSE, FALSE, 0);
	gtk_box_pack_start (GTK_BOX(right_vbox), m_force_piano_button, FALSE, FALSE, 0);
	gtk_box_pack_start (GTK_BOX(right_vbox), m_dont_split_button, FALSE, FALSE, 0);

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
	gtk_widget_set_size_request(m_instrument_list, INSTRUMENTS_WIDGET_WIDTH, MIDI_RECORD_CONFIG_SHOW_WINDOW_HEIGHT);
	sprintf(Str, "%d", m_pgm);
	instrument_tree_path = gtk_tree_path_new_from_string(Str);
	gtk_tree_view_scroll_to_cell(GTK_TREE_VIEW (m_instrument_list), instrument_tree_path, NULL, FALSE, 0.0, 0.0);
	gtk_tree_view_set_cursor(GTK_TREE_VIEW (m_instrument_list), instrument_tree_path, NULL, FALSE);
	g_signal_connect(dialog, "scroll-event", G_CALLBACK (OnScroll), (void *) this);

	tempo_frame = gtk_frame_new(_("tempo"));
	m_tempo_scale = gtk_vscale_new_with_range(0.1 * 60.0, 2.4 * 60.0, 0.4 * 60.0);
	gtk_range_set_inverted(GTK_RANGE(m_tempo_scale), TRUE);
	gtk_range_set_value(GTK_RANGE(m_tempo_scale), 60.0 * (double) m_tempo_inverse / 100000.0);
	gtk_container_add (GTK_CONTAINER(tempo_frame), m_tempo_scale);

	metro_volume_frame = gtk_frame_new(_("volume"));
	m_metro_volume_scale = gtk_vscale_new_with_range(0.0, 127.0, 1.0);
	gtk_range_set_inverted(GTK_RANGE(m_metro_volume_scale), TRUE);
	gtk_range_set_value(GTK_RANGE(m_metro_volume_scale), m_metro_volume);
	gtk_container_add (GTK_CONTAINER(metro_volume_frame), m_metro_volume_scale);


	key_frame = gtk_frame_new(_("key"));
	radio_hbox = gtk_hbox_new(FALSE, 2);
	key_frame_left_vbox = gtk_vbox_new(FALSE, 2);
	key_frame_right_vbox = gtk_vbox_new(FALSE, 2);
	m_sig_radio_buttons[0] =  gtk_radio_button_new_with_label (NULL, gettext(NedStaffContextDialog::m_keyTab[0]));
	gtk_box_pack_start (GTK_BOX (key_frame_left_vbox),  m_sig_radio_buttons[0], FALSE, FALSE, 0);
	for (i = -5; i < 7; i++) {
		m_sig_radio_buttons[i+6] = gtk_radio_button_new_with_label_from_widget (GTK_RADIO_BUTTON (m_sig_radio_buttons[0]), gettext(NedStaffContextDialog::m_keyTab[i+6]));
		if (i < 0) {
			gtk_box_pack_start (GTK_BOX (key_frame_left_vbox), m_sig_radio_buttons[i+6], FALSE, FALSE, 0);
		}
		else {
			gtk_box_pack_start (GTK_BOX (key_frame_right_vbox), m_sig_radio_buttons[i+6], FALSE, FALSE, 0);
		}
	}
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (m_sig_radio_buttons[keysig+6]), TRUE);
	gtk_box_pack_start (GTK_BOX (radio_hbox), key_frame_left_vbox,  FALSE, FALSE, 0);
	gtk_box_pack_start (GTK_BOX (radio_hbox), key_frame_right_vbox,  FALSE, FALSE, 0);
	gtk_container_add (GTK_CONTAINER(key_frame), radio_hbox);

	upper_hbox = gtk_hbox_new(FALSE, 5);

	gtk_box_pack_start (GTK_BOX(upper_hbox), m_instrument_list, TRUE, TRUE, 0);
	gtk_box_pack_start (GTK_BOX(upper_hbox), instrument_scroll, FALSE, FALSE, 0);
	gtk_box_pack_start (GTK_BOX(upper_hbox), right_vbox, FALSE, FALSE, 0);
	gtk_box_pack_start (GTK_BOX(upper_hbox), tempo_frame, FALSE, FALSE, 0);
	gtk_box_pack_start (GTK_BOX(upper_hbox), metro_volume_frame, FALSE, FALSE, 0);

	main_vbox = gtk_vbox_new(FALSE, 5);
	gtk_box_pack_start (GTK_BOX(main_vbox), upper_hbox, TRUE, TRUE, 0);
	gtk_box_pack_start (GTK_BOX(main_vbox), key_frame, FALSE, FALSE, 0);

	gtk_container_add (GTK_CONTAINER (GTK_DIALOG(dialog)->vbox), main_vbox);
	gtk_widget_show_all (dialog);
	gtk_dialog_run(GTK_DIALOG(dialog));

}

void NedMidiRecordConfigDialog::OnClose(GtkDialog *dialog, gint result, gpointer data) {
	NedMidiRecordConfigDialog *midi_record_dialog = (NedMidiRecordConfigDialog *) data;
	int i;
	GtkTreeSelection* selection;
	GList *selected_rows;
	switch (result) {
		case GTK_RESPONSE_ACCEPT:
			midi_record_dialog->m_state = TRUE;
		break;
		default:
			midi_record_dialog->m_state = FALSE;
			gtk_widget_destroy (GTK_WIDGET(dialog));
			return;
		break;
	}
	midi_record_dialog->m_num = (int) gtk_spin_button_get_value(GTK_SPIN_BUTTON(midi_record_dialog->m_numerator));
	switch (gtk_combo_box_get_active(GTK_COMBO_BOX(midi_record_dialog->m_denominator))) {
		case 0: midi_record_dialog->m_denom = 2; break;
		case 1: midi_record_dialog->m_denom = 4; break;
		case 2: midi_record_dialog->m_denom = 8; break;
		default: midi_record_dialog->m_denom = 16; break;
	}
	selection = gtk_tree_view_get_selection (GTK_TREE_VIEW(midi_record_dialog->m_instrument_list));
	selected_rows = gtk_tree_selection_get_selected_rows (selection, NULL);
	if ( g_list_length(selected_rows) > 0) {
		GtkTreePath *path = (GtkTreePath *) g_list_first(selected_rows)->data;
		char *tp = gtk_tree_path_to_string(path);
		if (sscanf(tp, "%d", &(midi_record_dialog->m_pgm)) != 1) {
			NedResource::Abort("NedMidiRecordConfigDialog::OnClose: error reading tree path(1)");
		}
	}
	else {
		midi_record_dialog->m_pgm = 0;
	}
	midi_record_dialog->m_triplet = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(midi_record_dialog->m_with_triplet_button));
	midi_record_dialog->m_f_piano = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(midi_record_dialog->m_force_piano_button));
	midi_record_dialog->m_dnt_split = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(midi_record_dialog->m_dont_split_button));
	midi_record_dialog->m_tempo_inverse = (int) (100000.0 * gtk_range_get_value(GTK_RANGE(midi_record_dialog->m_tempo_scale)) / 60.0);
	midi_record_dialog->m_metro_volume = (int) gtk_range_get_value(GTK_RANGE(midi_record_dialog->m_metro_volume_scale));
	midi_record_dialog->m_keysig = 0;
	for (i = 0; i < 13; i++) {
		if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (midi_record_dialog->m_sig_radio_buttons[i]))) {
			midi_record_dialog->m_keysig = i - 6;
		}
	}
	gtk_widget_destroy (GTK_WIDGET(dialog));
}

void NedMidiRecordConfigDialog::getValues(bool *state, int *num, int *denom, int *midi_pgm,
                        int *tempo_inverse, int *metro_volume, bool *triplet, bool *f_piano, bool *dnt_split, int* keysig) {
	*state = m_state;
	*num = m_num;
	*denom = m_denom;
	*midi_pgm = m_pgm;
	*tempo_inverse = m_tempo_inverse;
	*metro_volume = m_metro_volume;
	*triplet = m_triplet;
	*f_piano = m_f_piano;
	*dnt_split = m_dnt_split;
	*keysig = m_keysig;
}


void NedMidiRecordConfigDialog::OnPianoForcePressed(GtkButton *button, gpointer data) {
	NedMidiRecordConfigDialog *midi_record_dialog = (NedMidiRecordConfigDialog *) data;
	if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(midi_record_dialog->m_force_piano_button))) {
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(midi_record_dialog->m_dont_split_button), FALSE);
		gtk_widget_set_sensitive(midi_record_dialog->m_dont_split_button, FALSE);
	}
	else {
		gtk_widget_set_sensitive(midi_record_dialog->m_dont_split_button, TRUE);
	}
}

void NedMidiRecordConfigDialog::OnDontSplitPressed(GtkButton *button, gpointer data) {
	NedMidiRecordConfigDialog *midi_record_dialog = (NedMidiRecordConfigDialog *) data;

	if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(midi_record_dialog->m_dont_split_button))) {
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(midi_record_dialog->m_force_piano_button), FALSE);
		gtk_widget_set_sensitive(midi_record_dialog->m_force_piano_button, FALSE);
	}
	else {
		 gtk_widget_set_sensitive(midi_record_dialog->m_force_piano_button, TRUE);
	}
}

gboolean NedMidiRecordConfigDialog::OnScroll(GtkWidget *widget, GdkEventScroll *event, gpointer data) {
#define SCROLL_INCR 20.0
	double adjval;
	NedMidiRecordConfigDialog *midi_record_dialog = (NedMidiRecordConfigDialog *) data;
	adjval = gtk_adjustment_get_value(GTK_ADJUSTMENT(midi_record_dialog->m_instrument_adjustment));

	if (event->direction == GDK_SCROLL_UP) {
		adjval -= SCROLL_INCR;
	}
	else if (event->direction == GDK_SCROLL_DOWN) {
		adjval += SCROLL_INCR;
	}
	gtk_adjustment_set_value(GTK_ADJUSTMENT(midi_record_dialog->m_instrument_adjustment), adjval);
	return FALSE;
}
