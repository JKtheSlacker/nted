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
#include <exception>
#include "selectordialog.h"
#include "localization.h"
#include "resource.h"
#include "mainwindow.h"
#include "pangocairotext.h"


NedSelectorDialog::NedSelectorDialog(GtkWindow *parent, bool with_transpose, bool *staff_list, bool selected, struct staff_context_str *staff_contexts, int numstaffs) :
		m_with_transpose(with_transpose), m_staff_list(staff_list), m_selected(selected), m_numstaffs(numstaffs) {
	GtkWidget *dialog;
	int i;
	char Str[128];
	if (with_transpose) {
		dialog = gtk_dialog_new_with_buttons(_("Transpose"), parent, (GtkDialogFlags) (GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT),
			GTK_STOCK_OK, GTK_RESPONSE_ACCEPT, GTK_STOCK_CANCEL, GTK_RESPONSE_REJECT, NULL);
	}
	else {
		dialog = gtk_dialog_new_with_buttons(_("Select"), parent, (GtkDialogFlags) (GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT),
			GTK_STOCK_OK, GTK_RESPONSE_ACCEPT, GTK_STOCK_CANCEL, GTK_RESPONSE_REJECT, NULL);
	}

	g_signal_connect(dialog, "response", G_CALLBACK (OnClose), (void *) this);

	GtkWidget *transpose_frame;
	GtkWidget *transpose_hbox;
	GtkWidget *buttonbox;
	GtkWidget *hbox;
	GtkWidget *selection_frame;
	buttonbox = gtk_vbox_new(FALSE, 0);

	try {
	  m_checkboxes= new GtkWidget*[numstaffs];
	}
	catch(std::exception& e){
	  NedResource::Abort("NedSelectorDialog: allocation error");
	}

	for (i = 0; i < numstaffs; i++) {
		if (staff_contexts[i].m_staff_name == NULL) {
			sprintf(Str, _("Staff %d"), i);
			m_checkboxes[i] = gtk_check_button_new_with_label(Str);
		}
		else {
			m_checkboxes[i] = gtk_check_button_new_with_label(staff_contexts[i].m_staff_name->getText());
		}
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(m_checkboxes[i]),
			staff_contexts[i].m_midi_channel != 9);
		gtk_box_pack_start(GTK_BOX(buttonbox), m_checkboxes[i], FALSE, FALSE, 0);
	}

	if (with_transpose) {
		transpose_frame = gtk_frame_new(_("half-tones"));
	}
	else {
		transpose_frame = gtk_frame_new(_("selection"));
	}
	transpose_hbox = gtk_hbox_new(FALSE, 2);
	if (with_transpose) {


		m_transpose_spin_box = gtk_spin_button_new_with_range(-12.0, 12.0, 1.0);
		gtk_spin_button_set_value(GTK_SPIN_BUTTON(m_transpose_spin_box), 0.0);
		gtk_box_pack_start(GTK_BOX(transpose_hbox), m_transpose_spin_box, FALSE, FALSE, 0);
	}

	m_selection_checkbox = gtk_check_button_new_with_label(_("use selection"));

	gtk_box_pack_end(GTK_BOX(transpose_hbox), m_selection_checkbox, FALSE, FALSE, 0);

	g_signal_connect(m_selection_checkbox, "toggled", G_CALLBACK (OnUseSelection), (void *) this);

	gtk_container_add (GTK_CONTAINER(transpose_frame), transpose_hbox);

	selection_frame = gtk_frame_new(_("selection"));

	hbox = gtk_hbox_new(TRUE, 0);
	m_allbutton = gtk_button_new_with_label(_("all"));
	g_signal_connect(m_allbutton, "pressed", G_CALLBACK (OnAll), (void *) this);
	m_nonebutton = gtk_button_new_with_label(_("none"));
	g_signal_connect(m_nonebutton, "pressed", G_CALLBACK (OnNone), (void *) this);
	gtk_box_pack_start(GTK_BOX(hbox), m_allbutton, TRUE, TRUE, 5);
	gtk_box_pack_start(GTK_BOX(hbox), m_nonebutton, TRUE, TRUE, 5);
	gtk_box_pack_start(GTK_BOX(buttonbox), hbox, TRUE, TRUE, 5);

	gtk_container_add (GTK_CONTAINER(selection_frame), buttonbox);

	if (m_selected) {
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(m_selection_checkbox), TRUE);
		gtk_widget_set_sensitive(m_allbutton, FALSE);
		gtk_widget_set_sensitive(m_nonebutton, FALSE);
		for (i = 0; i < numstaffs; i++) {
			if (staff_contexts[i].m_staff_name == NULL) {
				gtk_widget_set_sensitive(m_checkboxes[i], FALSE);
			}
		}
	}
	else {
		gtk_widget_set_sensitive(m_selection_checkbox, FALSE);
	}

	gtk_container_add (GTK_CONTAINER (GTK_DIALOG(dialog)->vbox), transpose_frame);
	gtk_container_add (GTK_CONTAINER (GTK_DIALOG(dialog)->vbox), selection_frame);
	gtk_widget_show_all (dialog);
	gtk_dialog_run(GTK_DIALOG(dialog));

}

void NedSelectorDialog::OnClose(GtkDialog *dialog, gint result, gpointer data) {
	int i;
	double val;
	bool one_checked = false;
	NedSelectorDialog *selector_dialog = (NedSelectorDialog *) data;
	for (i = 0; i < selector_dialog->m_numstaffs; i++) {
		selector_dialog->m_staff_list[i] = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(selector_dialog->m_checkboxes[i]));
		if (selector_dialog->m_staff_list[i]) one_checked = true;
	}
	switch (result) {
		case GTK_RESPONSE_ACCEPT:
			if (!one_checked) {
				selector_dialog->m_state = FALSE;
				break;
			}
			selector_dialog->m_state = TRUE;
		break;
		default:
			selector_dialog->m_state = FALSE;
		break;
	}
	if (selector_dialog->m_with_transpose) {
		val = gtk_spin_button_get_value(GTK_SPIN_BUTTON(selector_dialog->m_transpose_spin_box));
		selector_dialog->m_pitch_dist = (int) ((val > 0.0) ? (val + 0.5) : (val - 0.5));
	}
	gtk_widget_destroy (GTK_WIDGET(dialog));
	delete[] selector_dialog->m_checkboxes;
}

void NedSelectorDialog::getValues(bool *state, int *pitchdist, bool *selected) {
	*state = m_state;
	if (m_with_transpose) {
		*state = *state && m_pitch_dist != 0;
		*pitchdist = m_pitch_dist;
	}
	*selected = m_selected;
}


void NedSelectorDialog::OnAll(GtkButton *button, gpointer data) {
	int i;
	NedSelectorDialog *selector_dialog = (NedSelectorDialog *) data;
	for (i = 0; i < selector_dialog->m_numstaffs; i++) {
		 gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(selector_dialog->m_checkboxes[i]), TRUE);
	}
}

void NedSelectorDialog::OnNone(GtkButton *button, gpointer data) {
	int i;
	NedSelectorDialog *selector_dialog = (NedSelectorDialog *) data;
	for (i = 0; i < selector_dialog->m_numstaffs; i++) {
		 gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(selector_dialog->m_checkboxes[i]), FALSE);
	}
}

void NedSelectorDialog::OnUseSelection(GtkToggleButton *togglebutton, gpointer data) {
	int i;
	NedSelectorDialog *selector_dialog = (NedSelectorDialog *) data;

	if (gtk_toggle_button_get_active(togglebutton)) {
		gtk_widget_set_sensitive(selector_dialog->m_allbutton, FALSE);
		gtk_widget_set_sensitive(selector_dialog->m_nonebutton, FALSE);
		for (i = 0; i < selector_dialog->m_numstaffs; i++) {
		 	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(selector_dialog->m_checkboxes[i]), FALSE);
		 	gtk_widget_set_sensitive(selector_dialog->m_checkboxes[i], FALSE);
		}
	}
	else {
		gtk_widget_set_sensitive(selector_dialog->m_allbutton, TRUE);
		gtk_widget_set_sensitive(selector_dialog->m_nonebutton, TRUE);
		for (i = 0; i < selector_dialog->m_numstaffs; i++) {
		 	gtk_widget_set_sensitive(selector_dialog->m_checkboxes[i], TRUE);
		}
	}
	for (i = 0; i < selector_dialog->m_numstaffs; i++) {
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(selector_dialog->m_checkboxes[i]), TRUE);
	}
}
