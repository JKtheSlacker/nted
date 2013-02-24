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
#include "mutedialog.h"
#include "localization.h"
#include "resource.h"
#include "mainwindow.h"
#include "pangocairotext.h"


NedMuteDialog::NedMuteDialog(GtkWindow *parent, bool *mutes, struct staff_context_str *staff_contexts, int numstaffs) : m_mutes(mutes), m_numstaffs(numstaffs) {
	GtkWidget *dialog;
	bool is_muted;
	int i, j;
	char Str[128];
	dialog = gtk_dialog_new_with_buttons(_("Muting"), parent, (GtkDialogFlags) (GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT),
		GTK_STOCK_OK, GTK_RESPONSE_ACCEPT, GTK_STOCK_CANCEL, GTK_RESPONSE_REJECT, NULL);

	g_signal_connect(dialog, "response", G_CALLBACK (OnClose), (void *) this);

	GtkWidget *buttonbox;
	GtkWidget *allbutton;
	GtkWidget *nonebutton;
	GtkWidget *hbox;
	GtkWidget *mute_frame;
	buttonbox = gtk_vbox_new(FALSE, 0);

	try {
	  m_checkboxes= new GtkWidget*[numstaffs];
	}
	catch(std::exception& e){
	  NedResource::Abort("NedMuteDialog: allocation error1");
	}

	for (i = 0; i < numstaffs; i++) {
		if (staff_contexts[i].m_staff_name == NULL) {
			sprintf(Str, _("Staff %d"), i);
			m_checkboxes[i] = gtk_check_button_new_with_label(Str);
		}
		else {
			m_checkboxes[i] = gtk_check_button_new_with_label(staff_contexts[i].m_staff_name->getText());
		}
		is_muted = true;
		for (j = 0; j < VOICE_COUNT; j++) {
			if (!staff_contexts[i].m_muted[j]) is_muted = false;
		}
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(m_checkboxes[i]), is_muted);
		gtk_box_pack_start(GTK_BOX(buttonbox), m_checkboxes[i], FALSE, FALSE, 0);
	}

	mute_frame = gtk_frame_new(_("muting"));

	hbox = gtk_hbox_new(TRUE, 0);
	allbutton = gtk_button_new_with_label(_("all"));
	g_signal_connect(allbutton, "pressed", G_CALLBACK (OnAll), (void *) this);
	nonebutton = gtk_button_new_with_label(_("none"));
	g_signal_connect(nonebutton, "pressed", G_CALLBACK (OnNone), (void *) this);
	gtk_box_pack_start(GTK_BOX(hbox), allbutton, TRUE, TRUE, 5);
	gtk_box_pack_start(GTK_BOX(hbox), nonebutton, TRUE, TRUE, 5);
	gtk_box_pack_start(GTK_BOX(buttonbox), hbox, TRUE, TRUE, 5);

	gtk_container_add (GTK_CONTAINER(mute_frame), buttonbox);

	gtk_container_add (GTK_CONTAINER (GTK_DIALOG(dialog)->vbox), mute_frame);
	gtk_widget_show_all (dialog);
	gtk_dialog_run(GTK_DIALOG(dialog));

}

void NedMuteDialog::OnClose(GtkDialog *dialog, gint result, gpointer data) {
	int i;
	NedMuteDialog *mute_dialog = (NedMuteDialog *) data;
	switch (result) {
		case GTK_RESPONSE_ACCEPT:
			mute_dialog->m_state = TRUE;
		break;
		default:
			mute_dialog->m_state = FALSE;
		break;
	}
	for (i = 0; i < mute_dialog->m_numstaffs; i++) {
		mute_dialog->m_mutes[i] = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(mute_dialog->m_checkboxes[i]));
	}
	gtk_widget_destroy (GTK_WIDGET(dialog));
	delete[] mute_dialog->m_checkboxes;
}

void NedMuteDialog::getValues(bool *state) {
	*state = m_state;
}


void NedMuteDialog::OnAll(GtkButton *button, gpointer data) {
	int i;
	NedMuteDialog *mute_dialog = (NedMuteDialog *) data;
	for (i = 0; i < mute_dialog->m_numstaffs; i++) {
		 gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(mute_dialog->m_checkboxes[i]), TRUE);
	}
}

void NedMuteDialog::OnNone(GtkButton *button, gpointer data) {
	int i;
	NedMuteDialog *mute_dialog = (NedMuteDialog *) data;
	for (i = 0; i < mute_dialog->m_numstaffs; i++) {
		 gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(mute_dialog->m_checkboxes[i]), FALSE);
	}
}
