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

#include "scaledialog.h"
#include "localization.h"
#include "resource.h"
#include "mainwindow.h"

#define SCALE_STEPS (1.0 / (double) SCALE_GRANULARITY)



NedScaleDialog::NedScaleDialog(GtkWindow *parent, int scale) : m_scale(scale) {
	GtkWidget *dialog;
	GtkWidget *scale_frame;

	dialog = gtk_dialog_new_with_buttons(_("Scale"), parent, (GtkDialogFlags) (GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT),
		GTK_STOCK_OK, GTK_RESPONSE_ACCEPT, GTK_STOCK_CANCEL, GTK_RESPONSE_REJECT, NULL);

	g_signal_connect(dialog, "response", G_CALLBACK (OnClose), (void *) this);

	scale_frame = gtk_frame_new(_("Scale"));

	m_scale_slider = gtk_hscale_new_with_range(SCALE_MIN, SCALE_MAX, SCALE_STEPS);
	gtk_range_set_value(GTK_RANGE(m_scale_slider), (double) scale / (double) SCALE_GRANULARITY);

	gtk_container_add (GTK_CONTAINER(scale_frame), m_scale_slider);

	gtk_container_add (GTK_CONTAINER (GTK_DIALOG(dialog)->vbox), scale_frame);
	gtk_widget_show_all (dialog);
	gtk_dialog_run(GTK_DIALOG(dialog));

}

void NedScaleDialog::OnClose(GtkDialog *dialog, gint result, gpointer data) {
	NedScaleDialog *scale_dialog = (NedScaleDialog *) data;
	double val;
	switch (result) {
		case GTK_RESPONSE_ACCEPT:
			scale_dialog->m_state = TRUE;
		break;
		default:
			scale_dialog->m_state = FALSE;
		break;
	}
	val = gtk_range_get_value(GTK_RANGE(scale_dialog->m_scale_slider));
	scale_dialog->m_scale  = (int) (val * (double) SCALE_GRANULARITY + 0.5);
	gtk_widget_destroy (GTK_WIDGET(dialog));
}


void NedScaleDialog::getValues(bool *state, int *scale) {
	*state = m_state;
	*scale = m_scale;
}

