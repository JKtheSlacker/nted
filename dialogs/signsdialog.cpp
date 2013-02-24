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
#include "signsdialog.h"
#include "localization.h"
#include "resource.h"
#include "freesign.h"
#include "mainwindow.h"

#define SIGN_BOX_WIDTH  40
#define SIGN_BOX_HEIGHT 40
#define RECT_OFFS 4

#define MAX_XVAL 3
#define MAX_YVAL 0

NedSignsDialog::NedSignsDialog(GtkWindow *parent) : m_current_row(-1), m_current_column(-1) {
	GtkWidget *dialog;
	GtkWidget *sign_frame;
	GtkWidget *sign_image_widget;

	m_black.pixel = 1; m_black.red = m_black.green = m_black.blue = 0;
	m_red.pixel = 2; m_red.red = 65535; m_red.green = m_red.blue = 0;

	dialog = gtk_dialog_new_with_buttons(_("Signs"), parent, (GtkDialogFlags) (GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT),
		GTK_STOCK_OK, GTK_RESPONSE_ACCEPT, GTK_STOCK_CANCEL, GTK_RESPONSE_REJECT, NULL);

	g_signal_connect(dialog, "response", G_CALLBACK (OnClose), (void *) this);


	sign_frame = gtk_frame_new(_("signs"));
	sign_image_widget = gtk_drawing_area_new();
	gtk_drawing_area_size(GTK_DRAWING_AREA(sign_image_widget), NedResource::m_lines_buf_width, NedResource::m_lines_buf_height);

	g_signal_connect (sign_image_widget, "expose-event", G_CALLBACK (draw_menu), (void *) this);
	gtk_container_add (GTK_CONTAINER(sign_frame), sign_image_widget);

	gtk_container_add (GTK_CONTAINER (GTK_DIALOG(dialog)->vbox), sign_frame);
	gtk_widget_show_all (dialog);
	m_drawing_area_gc = gdk_gc_new(sign_image_widget->window);
	gtk_widget_add_events(sign_image_widget, GDK_BUTTON_PRESS_MASK);
	g_signal_connect (sign_image_widget, "button-press-event", G_CALLBACK (change_selection), (void *) this);
	gtk_dialog_run(GTK_DIALOG(dialog));

}

void NedSignsDialog::OnClose(GtkDialog *dialog, gint result, gpointer data) {
	NedSignsDialog *lines_dialog = (NedSignsDialog *) data;
	switch (result) {
		case GTK_RESPONSE_ACCEPT:
			lines_dialog->m_state = TRUE;
		break;
		default:
			lines_dialog->m_state = FALSE;
		break;
	}
	gtk_widget_destroy (GTK_WIDGET(dialog));
}


void NedSignsDialog::getValues(bool *state, int *signtype) {
	*state = m_state && m_current_row >= 0 && m_current_column >= 0;
	*signtype = 0;

	if (state) {
		switch (m_current_row) {
			case 0: switch(m_current_column) {
				case 0: *signtype = SIGN_CODA; break;
				case 1: *signtype = SIGN_SEGNO; break;
				case 2: *signtype = SIGN_BREATH1; break;
				case 3: *signtype = SIGN_BREATH2; break;
			}
			break;
		}
	}
}

gboolean NedSignsDialog::draw_menu(GtkWidget *widget, GdkEventExpose *event, gpointer data) {
	NedSignsDialog *lines_dialog = (NedSignsDialog *) data;

	gdk_gc_set_foreground(lines_dialog->m_drawing_area_gc, &(lines_dialog->m_black));
	gdk_gc_set_rgb_fg_color(lines_dialog->m_drawing_area_gc, &(lines_dialog->m_black));
	gdk_draw_pixbuf(widget->window, lines_dialog->m_drawing_area_gc, NedResource::m_signs,
		0, 0, 0, 0, NedResource::m_signs_buf_width, NedResource::m_signs_buf_height, GDK_RGB_DITHER_NONE, 0, 0);

	if (lines_dialog->m_current_row >= 0 && lines_dialog->m_current_column >= 0) {
		gdk_gc_set_foreground(lines_dialog->m_drawing_area_gc, &(lines_dialog->m_red));
		gdk_gc_set_rgb_fg_color(lines_dialog->m_drawing_area_gc, &(lines_dialog->m_red));
		gdk_draw_rectangle(widget->window, lines_dialog->m_drawing_area_gc, false,
			lines_dialog->m_current_column * SIGN_BOX_WIDTH + RECT_OFFS, lines_dialog->m_current_row * SIGN_BOX_HEIGHT + RECT_OFFS,
				SIGN_BOX_WIDTH - 2 * RECT_OFFS, SIGN_BOX_HEIGHT - 2 * RECT_OFFS);
	}
	return FALSE;
}

gboolean NedSignsDialog::change_selection (GtkWidget *widget, GdkEventButton *event, gpointer data) {
	GdkRectangle rect;
        int dummy;
	int newx, newy;

	NedSignsDialog *signs_dialog = (NedSignsDialog *) data;
	newx = (int) event->x / SIGN_BOX_WIDTH;
	newy = (int) event->y / SIGN_BOX_HEIGHT;

	if (newx > MAX_XVAL) return FALSE;
	if (newy > MAX_YVAL) return FALSE;
	signs_dialog->m_current_column = (int) event->x / SIGN_BOX_WIDTH;
	signs_dialog->m_current_row = (int) event->y / SIGN_BOX_HEIGHT;

        gdk_window_get_geometry(widget->window, &dummy, &dummy, &rect.width, &rect.height, &dummy);
        rect.x = rect.y = 0;
        gdk_window_invalidate_rect (widget->window, &rect, FALSE);
	return FALSE;
}
