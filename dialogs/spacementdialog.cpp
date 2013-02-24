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
#include "spacementdialog.h"
#include "localization.h"
#include "resource.h"
#include "mainwindow.h"



NedSpacementDialog::NedSpacementDialog(GtkWindow *parent, double spacement) : m_spacement(spacement) {
	GtkWidget *dialog;
	GtkWidget *spacement_frame;

	dialog = gtk_dialog_new_with_buttons(_("Horizontal Spacement"), parent, (GtkDialogFlags) (GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT),
		GTK_STOCK_OK, GTK_RESPONSE_ACCEPT, GTK_STOCK_CANCEL, GTK_RESPONSE_REJECT, NULL);

	g_signal_connect(dialog, "response", G_CALLBACK (OnClose), (void *) this);


	spacement_frame = gtk_frame_new(_("Spacement"));

	m_spacement_slider = gtk_hscale_new_with_range(MEASURE_MIN_SPREAD, MEASURE_MAX_SPREAD, MEASURE_SPREAD_STEP);
	gtk_range_set_value(GTK_RANGE(m_spacement_slider), spacement);

	gtk_container_add (GTK_CONTAINER(spacement_frame), m_spacement_slider);

	gtk_container_add (GTK_CONTAINER (GTK_DIALOG(dialog)->vbox), spacement_frame);
	gtk_widget_show_all (dialog);
	gtk_dialog_run(GTK_DIALOG(dialog));

}

void NedSpacementDialog::OnClose(GtkDialog *dialog, gint result, gpointer data) {
	NedSpacementDialog *space_dialog = (NedSpacementDialog *) data;
	switch (result) {
		case GTK_RESPONSE_ACCEPT:
			space_dialog->m_state = TRUE;
		break;
		default:
			space_dialog->m_state = FALSE;
		break;
	}
	space_dialog->m_spacement = gtk_range_get_value(GTK_RANGE(space_dialog->m_spacement_slider));
	gtk_widget_destroy (GTK_WIDGET(dialog));
}


void NedSpacementDialog::getValues(bool *state, double *spacement) {
	*state = m_state;
	*spacement = m_spacement;
}

