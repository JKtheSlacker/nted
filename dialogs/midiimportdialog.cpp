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

#include "midiimportdialog.h"
#include "localization.h"
#include "resource.h"
#include "mainwindow.h"

#define MIDIIMPORT_DIALOG_WIDTH 500
#define MIDIIMPORT_DIALOG_HEIGHT 450


NedMidiImportDialog::NedMidiImportDialog(GtkWindow *parent, char **folder, char **filename) : m_with_triplets(true),
	m_tempo_change_density(20), m_volume_change_density(20), m_last_folder(folder), m_filename(filename) {
	GtkWidget *dialog;
	GtkWidget *tempo_sensity_frame;
	GtkWidget *volume_sensity_frame;

	GtkWidget *main_vbox;

	GtkWidget *main_hbox;
	GtkWidget *left_vbox;
	GtkWidget *right_vbox;

	dialog = gtk_dialog_new_with_buttons(_("MIDI import"), parent, (GtkDialogFlags) (GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT),
		GTK_STOCK_OK, GTK_RESPONSE_ACCEPT, GTK_STOCK_CANCEL, GTK_RESPONSE_REJECT, NULL);

	g_signal_connect(dialog, "response", G_CALLBACK (OnClose), (void *) this);

	m_file_chooser = gtk_file_chooser_widget_new(GTK_FILE_CHOOSER_ACTION_OPEN);
	if (*m_last_folder != NULL) {
		gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(m_file_chooser), *m_last_folder);
	}
	if (*m_filename != NULL) {
		gtk_file_chooser_set_filename(GTK_FILE_CHOOSER(m_file_chooser), *m_filename);
	}
	GtkFileFilter *file_filter1 = gtk_file_filter_new();
	gtk_file_filter_set_name(file_filter1, ".mid,.midi (MIDI-Files)");
	gtk_file_filter_add_pattern(file_filter1, "*.mid");
	gtk_file_filter_add_pattern(file_filter1, "*.midi");

	GtkFileFilter *file_filter2 = gtk_file_filter_new();
	gtk_file_filter_set_name(file_filter2, "* (All Files)");
	gtk_file_filter_add_pattern(file_filter2, "*");

	gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(m_file_chooser), file_filter1);
	gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(m_file_chooser), file_filter2);

	left_vbox = gtk_vbox_new(FALSE, 2);
	right_vbox = gtk_vbox_new(FALSE, 2);

	m_with_triplet_button = gtk_toggle_button_new_with_label(_("triplet recognition"));
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(m_with_triplet_button), TRUE);
	m_sort_instruments_button = gtk_toggle_button_new_with_label(_("sort instruments"));
	m_force_piano_button = gtk_toggle_button_new_with_label(_("force piano"));
	m_dont_split_button = gtk_toggle_button_new_with_label(_("don't split tracks"));
	g_signal_connect(m_force_piano_button, "toggled", G_CALLBACK (OnPianoForcePressed), (void *) this);
	g_signal_connect(m_dont_split_button, "toggled", G_CALLBACK (OnDontSplitPressed), (void *) this);

	gtk_container_add (GTK_CONTAINER(left_vbox), m_with_triplet_button);
	gtk_container_add (GTK_CONTAINER(left_vbox), m_sort_instruments_button);
	gtk_container_add (GTK_CONTAINER(left_vbox), m_force_piano_button);
	gtk_container_add (GTK_CONTAINER(left_vbox), m_dont_split_button);

	tempo_sensity_frame = gtk_frame_new(_("tempo density"));

	m_tempo_change_density_scale = gtk_hscale_new_with_range(0.0, 100.0, 1.0);
	gtk_range_set_value(GTK_RANGE(m_tempo_change_density_scale), 20.0);
	gtk_container_add (GTK_CONTAINER(tempo_sensity_frame), m_tempo_change_density_scale);

	volume_sensity_frame = gtk_frame_new(_("volume density"));

	m_volume_change_density_scale = gtk_hscale_new_with_range(0.0, 100.0, 1.0);
	gtk_range_set_value(GTK_RANGE(m_volume_change_density_scale), 20.0);
	gtk_container_add (GTK_CONTAINER(volume_sensity_frame), m_volume_change_density_scale);


	gtk_box_pack_start(GTK_BOX(right_vbox), tempo_sensity_frame, FALSE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(right_vbox), volume_sensity_frame, FALSE, TRUE, 0);

	main_hbox = gtk_hbox_new(FALSE, 2);

	gtk_box_pack_start(GTK_BOX(main_hbox), left_vbox, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(main_hbox), right_vbox, TRUE, TRUE, 0);

	main_vbox = gtk_vbox_new(FALSE, 2);

	gtk_box_pack_start (GTK_BOX(main_vbox), main_hbox, FALSE, FALSE, 0);
	gtk_box_pack_start (GTK_BOX(main_vbox), m_file_chooser, TRUE, TRUE, 0);

	gtk_container_add (GTK_CONTAINER (GTK_DIALOG(dialog)->vbox), main_vbox);
	gtk_window_set_default_size (GTK_WINDOW (dialog), MIDIIMPORT_DIALOG_WIDTH, MIDIIMPORT_DIALOG_HEIGHT);
	gtk_widget_show_all (dialog);
	gtk_dialog_run(GTK_DIALOG(dialog));

}

void NedMidiImportDialog::OnClose(GtkDialog *dialog, gint result, gpointer data) {
	char *s;
	NedMidiImportDialog *midi_import_dialog = (NedMidiImportDialog *) data;
	/*
	printf("current folder = %s, filename = %s\n", gtk_file_chooser_get_current_folder(GTK_FILE_CHOOSER(midi_import_dialog->m_file_chooser)),
		gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(midi_import_dialog->m_file_chooser)));
	*/
	switch (result) {
		case GTK_RESPONSE_ACCEPT:
			midi_import_dialog->m_state = TRUE;
			if (*(midi_import_dialog->m_last_folder) != NULL) {
				free(*(midi_import_dialog->m_last_folder));
				*(midi_import_dialog->m_last_folder) = NULL;
			}
			if (*(midi_import_dialog->m_filename) != NULL) {
				free(*(midi_import_dialog->m_filename));
				*(midi_import_dialog->m_filename) = NULL;
			}
			s = gtk_file_chooser_get_current_folder(GTK_FILE_CHOOSER(midi_import_dialog->m_file_chooser));
			if (strlen(s) > 0) {
				*(midi_import_dialog->m_last_folder) = strdup(s);
			}
			s = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(midi_import_dialog->m_file_chooser));
			if (strlen(s) > 0) {
				*(midi_import_dialog->m_filename) = strdup(s);
			}

		break;
		default:
			midi_import_dialog->m_state = FALSE;
		break;
	}
	midi_import_dialog->m_with_triplets = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(midi_import_dialog->m_with_triplet_button));
	midi_import_dialog->m_tempo_change_density = (int) gtk_range_get_value(GTK_RANGE(midi_import_dialog->m_tempo_change_density_scale));
	midi_import_dialog->m_volume_change_density = (int) gtk_range_get_value(GTK_RANGE(midi_import_dialog->m_volume_change_density_scale));
	midi_import_dialog->m_force_piano = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(midi_import_dialog->m_force_piano_button));
	midi_import_dialog->m_sort_instruments = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(midi_import_dialog->m_sort_instruments_button));
	midi_import_dialog->m_dont_split = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(midi_import_dialog->m_dont_split_button));
	gtk_widget_destroy (GTK_WIDGET(dialog));
}


void NedMidiImportDialog::getValues(bool *state, bool *with_triplets, int *tempo_change_density, int *volume_change_density,
		bool *force_piano, bool *sort_instruments, bool *dont_split) {
	*state = m_state;
	*with_triplets = m_with_triplets;
	*force_piano = m_force_piano;
	*sort_instruments = m_sort_instruments;
	*dont_split = m_dont_split;
	*tempo_change_density = (int)(100.0 - m_tempo_change_density);
	*volume_change_density = (int)(100.0 - m_volume_change_density);
	if (*tempo_change_density < 0) *tempo_change_density = 0;
	if (*tempo_change_density > 100) *tempo_change_density = 100;
	if (*volume_change_density < 0) *volume_change_density = 0;
	if (*volume_change_density > 100) *volume_change_density = 100;
}


void NedMidiImportDialog::OnPianoForcePressed(GtkButton *button, gpointer data) {
	NedMidiImportDialog *the_dialog = (NedMidiImportDialog *) data;
	if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(the_dialog->m_force_piano_button))) {
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(the_dialog->m_dont_split_button), FALSE);
		gtk_widget_set_sensitive(the_dialog->m_dont_split_button, FALSE);
	}
	else {
		gtk_widget_set_sensitive(the_dialog->m_dont_split_button, TRUE);
	}
}

void NedMidiImportDialog::OnDontSplitPressed(GtkButton *button, gpointer data) {
	NedMidiImportDialog *the_dialog = (NedMidiImportDialog *) data;

	if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(the_dialog->m_dont_split_button))) {
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(the_dialog->m_force_piano_button), FALSE);
		gtk_widget_set_sensitive(the_dialog->m_force_piano_button, FALSE);
	}
	else {
		 gtk_widget_set_sensitive(the_dialog->m_force_piano_button, TRUE);
	}
}

