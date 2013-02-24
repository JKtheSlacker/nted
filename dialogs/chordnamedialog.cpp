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

#include "chordnamedialog.h"
#include "mainwindow.h"
#include "localization.h"
#include "resource.h"
#include "config.h"

double NedChordNameDialog::m_font_size = 0.4;
bool NedChordNameDialog::m_chord_name_dialog_active = false;
double NedChordNameDialog::m_chord_name_y_dist = 0.4;

NedChordNameDialog::NedChordNameDialog(NedMainWindow *main_window, GtkWindow *parent, char *roottext, char *uptext, char *downtext, double fontsize,
		 double ydist, bool ydist_relenvant) :
m_main_window(main_window) {
	if (fontsize > 0.0) {
		m_font_size = fontsize;
	}
	if (ydist > 0.0) {
		m_chord_name_y_dist = ydist;
	}
	GtkWidget *text_frame;
	GtkWidget *size_frame;
	GtkWidget *ydist_frame;
	GtkWidget *main_h_box;
	GtkWidget *text_h_box;
	GtkWidget *spin_button_vbox;
	GtkWidget *vbox;

	if (m_main_window) {
		m_dialog = gtk_dialog_new_with_buttons(_("Chordname"), NULL, (GtkDialogFlags) 0,
			GTK_STOCK_OK, GTK_RESPONSE_ACCEPT, GTK_STOCK_CANCEL, GTK_RESPONSE_REJECT, NULL);
	}
	else {
		m_dialog = gtk_dialog_new_with_buttons(_("Chordname"), parent, (GtkDialogFlags) (GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT),
			GTK_STOCK_OK, GTK_RESPONSE_ACCEPT, GTK_STOCK_CANCEL, GTK_RESPONSE_REJECT, NULL);
	}
	gtk_window_set_transient_for(GTK_WINDOW (m_dialog), parent);
	g_signal_connect(m_dialog, "response", G_CALLBACK (ResponseCallBack), (void *) this);


	text_frame = gtk_frame_new(_("chordname"));
	text_h_box = gtk_hbox_new(FALSE, 2);
	vbox = gtk_vbox_new(FALSE, 2);

	m_root_text_widget = gtk_entry_new_with_max_length(10);
	if (roottext != NULL) {
		gtk_entry_set_text(GTK_ENTRY(m_root_text_widget), roottext);
	}

	m_up_text_widget = gtk_entry_new_with_max_length(10);
	if (uptext != NULL) {
		gtk_entry_set_text(GTK_ENTRY(m_up_text_widget), uptext);
	}

	m_down_text_widget = gtk_entry_new_with_max_length(10);
	if (downtext != NULL) {
		gtk_entry_set_text(GTK_ENTRY(m_down_text_widget), downtext);
	}

	size_frame = gtk_frame_new(_("size"));
	m_font_size_spin_box = gtk_spin_button_new_with_range (0.3, 0.5, 0.1);
        gtk_spin_button_set_value(GTK_SPIN_BUTTON(m_font_size_spin_box), m_font_size);

	ydist_frame = gtk_frame_new(_("position"));
	m_ydist_spin_box = gtk_spin_button_new_with_range (0.01, 2.0, 0.05);
        gtk_spin_button_set_value(GTK_SPIN_BUTTON(m_ydist_spin_box), m_chord_name_y_dist);
	if (!ydist_relenvant) {
		gtk_widget_set_sensitive(m_ydist_spin_box, FALSE);
	}

	spin_button_vbox = gtk_hbox_new(FALSE, 2);
	main_h_box = gtk_hbox_new(FALSE, 2);

	gtk_box_pack_start(GTK_BOX(text_h_box), m_root_text_widget, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(vbox), m_up_text_widget, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(vbox), m_down_text_widget, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(text_h_box), vbox, FALSE, FALSE, 0);
	gtk_container_add (GTK_CONTAINER(text_frame), text_h_box);
	gtk_container_add (GTK_CONTAINER(size_frame), m_font_size_spin_box);
	gtk_container_add (GTK_CONTAINER(ydist_frame), m_ydist_spin_box);
	gtk_box_pack_start(GTK_BOX(main_h_box), text_frame, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(spin_button_vbox), size_frame, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(spin_button_vbox), ydist_frame, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(main_h_box), spin_button_vbox, FALSE, FALSE, 0);
	m_chord_name_dialog_active = true;
	if (m_main_window) {
		g_signal_connect(m_root_text_widget, "key-release-event", G_CALLBACK (OnKeyRelease), (void *) this);
		g_signal_connect(m_up_text_widget, "key-release-event", G_CALLBACK (OnKeyRelease), (void *) this);
		g_signal_connect(m_down_text_widget, "key-release-event", G_CALLBACK (OnKeyRelease), (void *) this);
		g_signal_connect(m_font_size_spin_box, "value-changed", G_CALLBACK (OnValueChanged), (void *) this);
		g_signal_connect(m_ydist_spin_box, "value-changed", G_CALLBACK (OnValueChanged), (void *) this);
	}

	gtk_container_add (GTK_CONTAINER (GTK_DIALOG(m_dialog)->vbox), main_h_box);
	gtk_widget_show_all (m_dialog);
	gtk_dialog_run(GTK_DIALOG(m_dialog));
}

bool NedChordNameDialog::updateMainSpecial() {
	const gchar *newtext;
	char *newroottext, *newuptext, *newdowntext;

	newtext = gtk_entry_get_text(GTK_ENTRY(m_root_text_widget));
	if (strlen(newtext) > 0) {
		newroottext = strdup(newtext);
		newtext = gtk_entry_get_text(GTK_ENTRY(m_up_text_widget));
		newuptext = NULL;
		if (strlen(newtext) > 0) {
			newuptext = strdup(newtext);
		}
		newdowntext = NULL;
		newtext = gtk_entry_get_text(GTK_ENTRY(m_down_text_widget));
		if (strlen(newtext) > 0) {
			newdowntext = strdup(newtext);
		}
		m_font_size = gtk_spin_button_get_value(GTK_SPIN_BUTTON(m_font_size_spin_box));
		m_chord_name_y_dist = gtk_spin_button_get_value(GTK_SPIN_BUTTON(m_ydist_spin_box));
		m_main_window->setChorNameInfo(newroottext, newuptext, newdowntext, m_font_size, m_chord_name_y_dist);
		return true;
	}
	return false;
}

void NedChordNameDialog::ResponseCallBack(GtkDialog *dialog, gint result, gpointer data) {
	NedChordNameDialog *font_name_dialog = (NedChordNameDialog *) data;
	const gchar *newtext;

	if (font_name_dialog->m_main_window) {
		switch (result) {
			case GTK_RESPONSE_ACCEPT:
				if (font_name_dialog->updateMainSpecial()) {
					break;
				} // no break is correct !!
			default:
				font_name_dialog->m_main_window->resetSpecialType();
				delete font_name_dialog;
			break;
		}
		return;
	}
	switch (result) {
		case GTK_RESPONSE_ACCEPT:
			font_name_dialog->m_state = TRUE;
		break;
		default:
			font_name_dialog->m_state = FALSE;
		break;
	}
	if (font_name_dialog->m_state) {
		font_name_dialog->m_root_text = NULL;
		newtext = gtk_entry_get_text(GTK_ENTRY(font_name_dialog->m_root_text_widget));
		NedResource::removeNonAsciiFromUTF8String(newtext);
		if (strlen(newtext) > 0) {
			font_name_dialog->m_root_text = strdup(newtext);
		}
		else {
			font_name_dialog->m_state = false;
		}
		font_name_dialog->m_up_text = NULL;
		newtext = gtk_entry_get_text(GTK_ENTRY(font_name_dialog->m_up_text_widget));
		if (strlen(newtext) > 0) {
			NedResource::removeNonAsciiFromUTF8String(newtext);
			if (strlen(newtext) > 0) {
				font_name_dialog->m_up_text = strdup(newtext);
			}
		}
		font_name_dialog->m_down_text = NULL;
		newtext = gtk_entry_get_text(GTK_ENTRY(font_name_dialog->m_down_text_widget));
		if (strlen(newtext) > 0) {
			NedResource::removeNonAsciiFromUTF8String(newtext);
			if (strlen(newtext) > 0) {
				font_name_dialog->m_down_text = strdup(newtext);
			}
		}
		font_name_dialog->m_font_size = gtk_spin_button_get_value(GTK_SPIN_BUTTON(font_name_dialog->m_font_size_spin_box));
		font_name_dialog->m_chord_name_y_dist = gtk_spin_button_get_value(GTK_SPIN_BUTTON(font_name_dialog->m_ydist_spin_box));
	}
	delete font_name_dialog;
}

bool NedChordNameDialog::OnKeyRelease(GtkWidget *widget, GdkEventKey *event, gpointer data) {
	NedChordNameDialog *font_name_dialog = (NedChordNameDialog *) data;

	font_name_dialog->updateMainSpecial();
	return FALSE;
}

void NedChordNameDialog::OnValueChanged(GtkSpinButton *spinbutton, gpointer data) {
	NedChordNameDialog *font_name_dialog = (NedChordNameDialog *) data;

	font_name_dialog->updateMainSpecial();
}

NedChordNameDialog::~NedChordNameDialog() {
	gtk_widget_destroy (GTK_WIDGET(m_dialog));
	m_chord_name_dialog_active = false;
}

void NedChordNameDialog::getValues(bool *state, char **roottext, char **uptext, char **downtext, double *fontsize, double *ydist) {
	*state = m_state;
	*roottext = m_root_text;
	*uptext = m_up_text;
	*downtext = m_down_text;
	*fontsize = m_font_size;
	*ydist = m_chord_name_y_dist;
}
