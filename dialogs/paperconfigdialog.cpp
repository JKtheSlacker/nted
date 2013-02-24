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

#include "paperconfigdialog.h"
#include "localization.h"
#include "resource.h"
#include "config.h"

NedPaperConfigDialog::NedPaperConfigDialog(GtkWindow *parent,
 const char *current_paper, bool portrait, bool with_cancel) : m_paper(current_paper), m_portrait(portrait) {
	GtkWidget *dialog;
	GtkWidget *paper_type_frame;
	GtkWidget *paper_type_vbox;
	GtkWidget *orientation_frame;
	GtkWidget *orientation_vbox;
	GtkWidget *hbox;

	if (with_cancel) {
		dialog = gtk_dialog_new_with_buttons(_("Paper"), parent, (GtkDialogFlags) (GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT),
			GTK_STOCK_OK, GTK_RESPONSE_ACCEPT, GTK_STOCK_CANCEL, GTK_RESPONSE_REJECT, NULL);
	}
	else {
		dialog = gtk_dialog_new_with_buttons(_("Paper"), parent, (GtkDialogFlags) (GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT),
			GTK_STOCK_OK, GTK_RESPONSE_ACCEPT, NULL);
	}
	g_signal_connect(dialog, "response", G_CALLBACK (CloseCallBack), (void *) this);

	paper_type_frame = gtk_frame_new(_("paper type"));
	m_radio_box_A5 = gtk_radio_button_new_with_label(NULL, "A5");
	m_radio_box_A4 = gtk_radio_button_new_with_label_from_widget (GTK_RADIO_BUTTON (m_radio_box_A5), "A4");
	m_radio_box_A3 = gtk_radio_button_new_with_label_from_widget (GTK_RADIO_BUTTON (m_radio_box_A5), "A3");
	m_radio_box_B4 = gtk_radio_button_new_with_label_from_widget (GTK_RADIO_BUTTON (m_radio_box_A5), "B4");
	m_radio_box_Letter = gtk_radio_button_new_with_label_from_widget (GTK_RADIO_BUTTON (m_radio_box_A5), "Letter");
	m_radio_box_Legal = gtk_radio_button_new_with_label_from_widget (GTK_RADIO_BUTTON (m_radio_box_A5), "Legal");
	m_radio_box_230x293 = gtk_radio_button_new_with_label_from_widget (GTK_RADIO_BUTTON (m_radio_box_A5), "230x293");
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (m_radio_box_A4), TRUE);
	if (!strcmp(current_paper, "A3")) {
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (m_radio_box_A3), TRUE);
	}
	else if (!strcmp(current_paper, "B4")) {
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (m_radio_box_B4), TRUE);
	}
	else if (!strcmp(current_paper, "Letter")) {
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (m_radio_box_Letter), TRUE);
	}
	else if (!strcmp(current_paper, "Legal")) {
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (m_radio_box_Legal), TRUE);
	}
	else if (!strcmp(current_paper, "230x293")) {
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (m_radio_box_230x293), TRUE);
	}

	paper_type_vbox = gtk_vbox_new(FALSE, 5);
	gtk_box_pack_start (GTK_BOX (paper_type_vbox), m_radio_box_A5, FALSE, FALSE, 0);
	gtk_box_pack_start (GTK_BOX (paper_type_vbox), m_radio_box_A4, FALSE, FALSE, 0);
	gtk_box_pack_start (GTK_BOX (paper_type_vbox), m_radio_box_A3, FALSE, FALSE, 0);
	gtk_box_pack_start (GTK_BOX (paper_type_vbox), m_radio_box_B4, FALSE, FALSE, 0);
	gtk_box_pack_start (GTK_BOX (paper_type_vbox), m_radio_box_Letter, FALSE, FALSE, 0);
	gtk_box_pack_start (GTK_BOX (paper_type_vbox), m_radio_box_Legal, FALSE, FALSE, 0);
	gtk_box_pack_start (GTK_BOX (paper_type_vbox), m_radio_box_230x293, FALSE, FALSE, 0);
	gtk_container_add (GTK_CONTAINER(paper_type_frame), paper_type_vbox);

	orientation_frame = gtk_frame_new(_("orientation"));
	m_radio_box_portrait = gtk_radio_button_new_with_label(NULL, _(_("portrait")));
	m_radio_box_landspace = gtk_radio_button_new_with_label_from_widget (GTK_RADIO_BUTTON (m_radio_box_portrait), _("landscape"));

	if (portrait) {
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (m_radio_box_portrait), TRUE);
	}
	else {
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (m_radio_box_landspace), TRUE);
	}

	orientation_vbox = gtk_vbox_new(FALSE, 5);
	gtk_box_pack_start (GTK_BOX (orientation_vbox), m_radio_box_portrait, FALSE, FALSE, 0);
	gtk_box_pack_start (GTK_BOX (orientation_vbox), m_radio_box_landspace, FALSE, FALSE, 0);
	gtk_container_add (GTK_CONTAINER(orientation_frame), orientation_vbox);

	hbox = gtk_hbox_new(FALSE, 5);
	gtk_box_pack_start (GTK_BOX (hbox), paper_type_frame, FALSE, FALSE, 0);
	gtk_box_pack_start (GTK_BOX (hbox), orientation_frame, FALSE, FALSE, 0);

	gtk_container_add (GTK_CONTAINER (GTK_DIALOG(dialog)->vbox), hbox);
	gtk_widget_show_all (dialog);
	gtk_dialog_run(GTK_DIALOG(dialog));

}

void NedPaperConfigDialog::CloseCallBack(GtkDialog *dialog, gint result, gpointer data) {
	NedPaperConfigDialog *paper_dialog = (NedPaperConfigDialog *) data;

	switch (result) {
		case GTK_RESPONSE_ACCEPT:
			paper_dialog->m_state = TRUE;
		break;
		default:
			paper_dialog->m_state = FALSE;
		break;
	}
	paper_dialog->m_paper = "A4";
	paper_dialog->m_portrait = TRUE;
	if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (paper_dialog->m_radio_box_A3))) {
		paper_dialog->m_paper = "A3";
	}
	else if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (paper_dialog->m_radio_box_A5))) {
		paper_dialog->m_paper = "A5";
	}
	else if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (paper_dialog->m_radio_box_B4))) {
		paper_dialog->m_paper = "B4";
	}
	else if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (paper_dialog->m_radio_box_Letter))) {
		paper_dialog->m_paper = "Letter";
	}
	else if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (paper_dialog->m_radio_box_Legal))) {
		paper_dialog->m_paper = "Legal";
	}
	else if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (paper_dialog->m_radio_box_230x293))) {
		paper_dialog->m_paper = "230x293";
	}
	if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (paper_dialog->m_radio_box_landspace))) {
		paper_dialog->m_portrait = FALSE;
	}
	gtk_widget_destroy (GTK_WIDGET(dialog));
	gtk_widget_hide (GTK_WIDGET(dialog));
}

void NedPaperConfigDialog::getValues(bool *state, const char **paper, bool *portrait) {
	*state = m_state;
	*paper = m_paper;
	*portrait = m_portrait;
}
