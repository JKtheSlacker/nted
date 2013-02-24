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

#include "lilypondexportdialog.h"
#include "localization.h"
#include "resource.h"
#include "mainwindow.h"
#include "staffselectdialog.h"

#define LILYPOND_EXPORT_DIALOG_WIDTH 500
#define LILYPOND_EXPORT_DIALOG_HEIGHT 450


NedLilyPondExportDialog::NedLilyPondExportDialog(GtkWindow *parent, NedMainWindow *main_window, char *filename, bool *selected_staves, bool *keep_beams) : m_main_window(main_window), m_with_breaks(true),
	m_filename(filename), m_selected_staves(selected_staves), m_keep_beams(keep_beams) {
	GtkWidget *select_button;
	GtkWidget *beam_button;
	GtkWidget *hbox;
	GtkWidget *param_frame;
	char *cptr1, *cptr2;
	bool full = false;
	char fnam[4096], foldername[4096];

	GtkWidget *main_vbox;

	m_dialog = gtk_dialog_new_with_buttons(_("LilyPond export"), parent, (GtkDialogFlags) (GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT),
		GTK_STOCK_OK, GTK_RESPONSE_ACCEPT, GTK_STOCK_CANCEL, GTK_RESPONSE_REJECT, NULL);

	g_signal_connect(m_dialog, "response", G_CALLBACK (OnClose), (void *) this);

	m_file_chooser = gtk_file_chooser_widget_new(GTK_FILE_CHOOSER_ACTION_SAVE);
	GtkFileFilter *file_filter1 = gtk_file_filter_new();
	gtk_file_filter_set_name(file_filter1, "*.ly (LilyPond-Files)");
	gtk_file_filter_add_pattern(file_filter1, "*.ly");

	GtkFileFilter *file_filter2 = gtk_file_filter_new();
	gtk_file_filter_set_name(file_filter2, "* (All Files)");
	gtk_file_filter_add_pattern(file_filter2, "*");

	gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(m_file_chooser), file_filter1);
	gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(m_file_chooser), file_filter2);

	main_vbox = gtk_vbox_new(FALSE, 2);

	param_frame = gtk_frame_new(_("export params"));
	hbox = gtk_hbox_new(FALSE, 2);

	m_with_break_checkbox = gtk_check_button_new_with_label(_("create line breaks"));
	gtk_box_pack_start (GTK_BOX(hbox), m_with_break_checkbox, TRUE, TRUE, 0);

	if (main_window->getStaffCount() > 1) {
		select_button = gtk_button_new_with_label(_("select staves"));
		g_signal_connect(select_button, "pressed", G_CALLBACK (OnStaffSelection), (void *) this);
		gtk_box_pack_start (GTK_BOX(hbox), select_button, TRUE, TRUE, 0);
	}
	beam_button = gtk_button_new_with_label(_("keep beams"));
	g_signal_connect(beam_button, "pressed", G_CALLBACK (OnBeamSelection), (void *) this);
	gtk_box_pack_start (GTK_BOX(hbox), beam_button, TRUE, TRUE, 0);

	gtk_container_add (GTK_CONTAINER(param_frame), hbox);

	gtk_box_pack_start (GTK_BOX(main_vbox), param_frame, FALSE, FALSE, 5);
	gtk_box_pack_start (GTK_BOX(main_vbox), m_file_chooser, TRUE, TRUE, 0);

	gtk_container_add (GTK_CONTAINER (GTK_DIALOG(m_dialog)->vbox), main_vbox);
	gtk_window_set_default_size (GTK_WINDOW (m_dialog), LILYPOND_EXPORT_DIALOG_WIDTH, LILYPOND_EXPORT_DIALOG_HEIGHT);
	gtk_widget_show_all (m_dialog);
	if (strlen(filename) > 0) {
		for (cptr1 = filename; *cptr1 != '\0'; cptr1++) {
			if ((*(unsigned char *) cptr1) <= ' ') continue;
			if (*cptr1 == '/') {
				full = true;
				break;
			}
		}
		if (full) {
			if ((cptr2 = strrchr(filename, '/')) != NULL) {
				strncpy(foldername, cptr1, cptr2 - cptr1);
				foldername[cptr2 - cptr1] = '\0';
				gtk_file_chooser_set_current_folder (GTK_FILE_CHOOSER(m_file_chooser), foldername);
				strcpy (fnam, cptr2 + 1);
				gtk_file_chooser_set_current_folder (GTK_FILE_CHOOSER(m_file_chooser), foldername);
				gtk_file_chooser_set_current_name(GTK_FILE_CHOOSER(m_file_chooser), fnam);
			}
			else {
				gtk_file_chooser_set_current_folder (GTK_FILE_CHOOSER(m_file_chooser), "/tmp");
				gtk_file_chooser_set_current_name(GTK_FILE_CHOOSER(m_file_chooser), m_filename);
			}
		}
		else {
			gtk_file_chooser_set_current_folder (GTK_FILE_CHOOSER(m_file_chooser), "/tmp");
			gtk_file_chooser_set_current_name(GTK_FILE_CHOOSER(m_file_chooser), m_filename);
		}
	}
	gtk_dialog_run(GTK_DIALOG(m_dialog));

}

void NedLilyPondExportDialog::OnClose(GtkDialog *dialog, gint result, gpointer data) {
	char *s;
	char *cptr;
	bool empty;
	NedLilyPondExportDialog *lilyexport_dialog = (NedLilyPondExportDialog *) data;
	lilyexport_dialog->m_with_breaks = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(lilyexport_dialog->m_with_break_checkbox));
	switch (result) {
		case GTK_RESPONSE_ACCEPT:
			lilyexport_dialog->m_state = TRUE;
			s = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(lilyexport_dialog->m_file_chooser));
			if (s == NULL) {
				lilyexport_dialog->m_state = FALSE;
				break;
			}
			empty = true;
			for (cptr = s; *cptr != '\0'; cptr++) {
				if ((*(unsigned char *) cptr) > ' ') {
					empty = false;
					break;
				}
			}
			if (empty) {
				lilyexport_dialog->m_state = FALSE;
				break;
			}
			empty = true;
			for (int i = 0; i < lilyexport_dialog->m_main_window->getStaffCount(); i++) {
				if (lilyexport_dialog->m_selected_staves[i]) {
					empty = false;
					break;
				}
			}
			if (empty) {
				lilyexport_dialog->m_state = FALSE;
				break;
			}
			strcpy(lilyexport_dialog->m_filename, s);
		break;
		default:
			lilyexport_dialog->m_state = FALSE;
		break;
	}
	gtk_widget_destroy (GTK_WIDGET(dialog));
}


void NedLilyPondExportDialog::getValues(bool *state, bool *with_break) {
	*state = m_state;
	*with_break = m_with_breaks;
}

void NedLilyPondExportDialog::OnStaffSelection  (GtkButton *button, gpointer data) {
	bool state;
	NedLilyPondExportDialog *lilyexport_dialog = (NedLilyPondExportDialog *) data;
	for (int i = 0; i < lilyexport_dialog->m_main_window->getStaffCount(); lilyexport_dialog->m_selected_staves[i++] = false);
	NedStaffSelectDialog *staff_select_dialog = new NedStaffSelectDialog(GTK_WINDOW(lilyexport_dialog->m_dialog), _("Staff selection"), 
		lilyexport_dialog->m_main_window, lilyexport_dialog->m_selected_staves);
	staff_select_dialog->getValues(&state);
}

void NedLilyPondExportDialog::OnBeamSelection  (GtkButton *button, gpointer data) {
	bool state;
	NedLilyPondExportDialog *lilyexport_dialog = (NedLilyPondExportDialog *) data;
	for (int i = 0; i < lilyexport_dialog->m_main_window->getStaffCount(); lilyexport_dialog->m_keep_beams[i++] = false);
	NedStaffSelectDialog *staff_select_dialog = new NedStaffSelectDialog(GTK_WINDOW(lilyexport_dialog->m_dialog), _("keep beams selection"), 
		lilyexport_dialog->m_main_window, lilyexport_dialog->m_keep_beams);
	staff_select_dialog->getValues(&state);
}

