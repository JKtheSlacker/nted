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

#include "portchoosedialog.h"
#include "localization.h"
#include "resource.h"
#include "config.h"


NedPortChooseDialog::NedPortChooseDialog(GtkWindow *parent, GList *ports, unsigned int idx, bool midi_echo, bool show_echo_box) :
	m_idx(idx), m_midi_echo(midi_echo), m_with_midi_echo_box(show_echo_box) {
	GtkWidget *dialog;
	GtkWidget *port_list_hbox;
	GtkWidget *vbox;
	GtkObject *port_list_adjustment;
	GtkWidget *port_list_scroll;
	GtkListStore *port_list_store;
	GtkTreeIter iter;
	GtkCellRenderer *port_list_renderer;
	GtkTreeViewColumn *port_list_column;
	GtkTreeViewColumn *port_list_num_column;
	GtkTreePath* port_list_tree_path;
	GList *lptr;
	char Str[128];
	int i;

	if (m_idx >= g_list_length(ports)) m_idx = 0;
	dialog = gtk_dialog_new_with_buttons(_("MIDI ports"), parent, (GtkDialogFlags) (GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT),
		GTK_STOCK_OK, GTK_RESPONSE_ACCEPT, GTK_STOCK_CANCEL, GTK_RESPONSE_REJECT, NULL);
	g_signal_connect(dialog, "response", G_CALLBACK (OnClose), (void *) this);

	port_list_store = gtk_list_store_new (2, G_TYPE_INT, G_TYPE_STRING);

	for (i = 0, lptr = g_list_first(ports); lptr; lptr = g_list_next(lptr), i++) {
		gtk_list_store_append (port_list_store, &iter);
		sprintf(Str, "%d:%d %s", ((MidiPortStruct *) lptr->data)->client, ((MidiPortStruct *) lptr->data)->port,
				    ((MidiPortStruct *) lptr->data)->name);
		gtk_list_store_set (port_list_store, &iter, 0, i + 1, 1, Str, -1);
	}
	m_port_list_view = gtk_tree_view_new_with_model(GTK_TREE_MODEL(port_list_store));
	port_list_renderer = gtk_cell_renderer_text_new ();
	port_list_num_column = gtk_tree_view_column_new_with_attributes (_("no"), port_list_renderer, "text", 0, NULL);
	gtk_tree_view_append_column (GTK_TREE_VIEW (m_port_list_view), port_list_num_column);
	port_list_column = gtk_tree_view_column_new_with_attributes (_("port"), port_list_renderer, "text", 1, NULL);
	gtk_tree_view_append_column (GTK_TREE_VIEW (m_port_list_view), port_list_column);
	port_list_adjustment = gtk_adjustment_new (0.0, 0.0, g_list_length(ports), 1.0,  10.0, 10.0);
	port_list_scroll = gtk_vscrollbar_new(GTK_ADJUSTMENT(port_list_adjustment));
	gtk_tree_view_set_vadjustment (GTK_TREE_VIEW (m_port_list_view), GTK_ADJUSTMENT(port_list_adjustment));
	gtk_widget_set_size_request(m_port_list_view, 200, 200);
	sprintf(Str, "%d", m_idx);
	port_list_tree_path = gtk_tree_path_new_from_string(Str);
	gtk_tree_view_scroll_to_cell(GTK_TREE_VIEW (m_port_list_view), port_list_tree_path, NULL, FALSE, 0.0, 0.0);
	gtk_tree_view_set_cursor(GTK_TREE_VIEW (m_port_list_view), port_list_tree_path, NULL, FALSE);

	port_list_hbox = gtk_hbox_new(FALSE, 2);
	gtk_box_pack_start(GTK_BOX(port_list_hbox), m_port_list_view, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(port_list_hbox), port_list_scroll, FALSE, TRUE, 0);

	if (m_with_midi_echo_box) {
		m_midi_echo_check_box = gtk_check_button_new_with_label(_("midi echo"));
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(m_midi_echo_check_box), m_midi_echo);
	}

	vbox = gtk_vbox_new(FALSE, 2);
	gtk_box_pack_start(GTK_BOX(vbox), port_list_hbox, TRUE, TRUE, 0);
	if (m_with_midi_echo_box) {
		gtk_box_pack_start(GTK_BOX(vbox), m_midi_echo_check_box, TRUE, TRUE, 0);
	}

	gtk_container_add (GTK_CONTAINER (GTK_DIALOG(dialog)->vbox), vbox);
	gtk_widget_show_all (dialog);
	gtk_dialog_run(GTK_DIALOG(dialog));

}

void NedPortChooseDialog::OnClose(GtkDialog *dialog, gint result, gpointer data) {
	GtkTreeSelection* selection;
	GList *selected_rows;

	NedPortChooseDialog *choose_dialog = (NedPortChooseDialog *) data;
	if (result != GTK_RESPONSE_ACCEPT) {
		choose_dialog->m_idx = -1;
		gtk_widget_destroy (GTK_WIDGET(dialog));
		return;
	}
	selection =  gtk_tree_view_get_selection (GTK_TREE_VIEW(choose_dialog->m_port_list_view));
	selected_rows = gtk_tree_selection_get_selected_rows (selection, NULL);
	if (g_list_length(selected_rows) < 1) {
		choose_dialog->m_idx = -1;
		gtk_widget_destroy (GTK_WIDGET(dialog));
		return;
	}
	GtkTreePath *path = (GtkTreePath *) g_list_first(selected_rows)->data;
	char *tp = gtk_tree_path_to_string(path);
	if (sscanf(tp, "%d", &(choose_dialog->m_idx)) != 1) {
		NedResource::Abort("NedPortChooseDialog::OnClose: error reading tree path");
	}
	if (choose_dialog->m_with_midi_echo_box) {
		choose_dialog->m_midi_echo = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(choose_dialog->m_midi_echo_check_box));
	}
	else {
		choose_dialog->m_midi_echo = false;
	}
	gtk_widget_destroy (GTK_WIDGET(dialog));
}

