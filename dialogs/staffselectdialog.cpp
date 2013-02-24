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

#include "staffselectdialog.h"
#include "mainwindow.h"
#include "localization.h"
#include "pangocairotext.h"
#include "resource.h"


NedStaffSelectDialog::NedStaffSelectDialog(GtkWindow *parent, const char *title, NedMainWindow *main_window, bool *selected_staves) : m_main_window(main_window), m_selected_staves(selected_staves) {
	GtkWidget *dialog;
	GtkTreeIter iter;
	GtkListStore *position_list_store;
	GtkCellRenderer *position_renderer;
	GtkTreeViewColumn *position_column;
	GtkTreeViewColumn *position_num_column;
	GtkObject *position_adjustment;
	GtkWidget *position_scroll = NULL;
	GtkWidget *hbox;
	GtkWidget *button_h_box;
	GtkWidget *main_vbox;
	GtkWidget *all_button;
	GtkWidget *none_button;
	int i;

	dialog = gtk_dialog_new_with_buttons(title, parent, (GtkDialogFlags) (GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT),
		GTK_STOCK_OK, GTK_RESPONSE_ACCEPT, GTK_STOCK_CANCEL, GTK_RESPONSE_REJECT, NULL);

	g_signal_connect(dialog, "response", G_CALLBACK (OnClose), (void *) this);

	position_list_store = gtk_list_store_new (2, G_TYPE_STRING, G_TYPE_INT);

	for (i = 0; i < main_window->getStaffCount(); i++) {
		gtk_list_store_append (position_list_store, &iter);
		if (main_window->m_staff_contexts[i].m_staff_name != NULL && strlen(main_window->m_staff_contexts[i].m_staff_name->getText()) > 0) {
			gtk_list_store_set (position_list_store, &iter, 0, main_window->m_staff_contexts[i].m_staff_name->getText() , 1, i, -1);
		}
		else {
			gtk_list_store_set (position_list_store, &iter, 0, _("staff") , 1, i, -1);
		}
	}

	
	m_position_list = gtk_tree_view_new_with_model(GTK_TREE_MODEL(position_list_store));
	m_tree_selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(m_position_list));
	gtk_tree_selection_set_mode(m_tree_selection, GTK_SELECTION_MULTIPLE);
	position_renderer = gtk_cell_renderer_text_new ();
	position_column = gtk_tree_view_column_new_with_attributes (_("item"), position_renderer, "text", 0, NULL);
	gtk_tree_view_append_column (GTK_TREE_VIEW (m_position_list), position_column);
	position_num_column = gtk_tree_view_column_new_with_attributes (_("no"), position_renderer, "text", 1, NULL);
	gtk_tree_view_append_column (GTK_TREE_VIEW (m_position_list), position_num_column);
	position_adjustment = gtk_adjustment_new (0.0, 0.0, 4, 1.0,  10.0, 10.0);
	position_scroll = gtk_vscrollbar_new(GTK_ADJUSTMENT(position_adjustment));
	gtk_tree_view_set_vadjustment (GTK_TREE_VIEW (m_position_list), GTK_ADJUSTMENT(position_adjustment));
	gtk_widget_set_size_request(m_position_list, 150, 200);

	all_button = gtk_button_new_with_label(_("All"));
	g_signal_connect(all_button, "pressed",  G_CALLBACK(OnAll), (void *) this);

	none_button = gtk_button_new_with_label(_("None"));
	g_signal_connect(none_button, "pressed",  G_CALLBACK(OnNone), (void *) this);

	button_h_box = gtk_hbox_new(FALSE, 2);
	gtk_box_pack_start(GTK_BOX(button_h_box), all_button, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(button_h_box), none_button, TRUE, TRUE, 0);

	hbox = gtk_hbox_new(FALSE, 2);
	gtk_box_pack_start(GTK_BOX(hbox), m_position_list, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(hbox), position_scroll, FALSE, TRUE, 0);

	main_vbox = gtk_vbox_new(FALSE, 2);
	gtk_box_pack_start(GTK_BOX(main_vbox), hbox, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(main_vbox), button_h_box, FALSE, TRUE, 0);

	gtk_container_add (GTK_CONTAINER (GTK_DIALOG(dialog)->vbox), main_vbox);
	gtk_widget_show_all (dialog);
	gtk_dialog_run(GTK_DIALOG(dialog));

}

void NedStaffSelectDialog::OnClose(GtkDialog *dialog, gint result, gpointer data) {
	GList *selected_rows;
	int i;

	NedStaffSelectDialog *selection_dialog = (NedStaffSelectDialog *) data;
	switch (result) {
		case GTK_RESPONSE_ACCEPT:
			selection_dialog->m_state = TRUE;
			selected_rows = gtk_tree_selection_get_selected_rows (selection_dialog->m_tree_selection, NULL);
			if (g_list_length(selected_rows) > 0) {
				for (i = 0; i < selection_dialog->m_main_window->getStaffCount(); selection_dialog->m_selected_staves[i++] = false);
				gtk_tree_selection_selected_foreach (selection_dialog->m_tree_selection, add_staff, data);
			}
			else {
				selection_dialog->m_state = FALSE;
			}
			break;
		default:
			selection_dialog->m_state = FALSE;
		break;
	}
	gtk_widget_destroy (GTK_WIDGET(dialog));
}

void NedStaffSelectDialog::getValues(bool *state) {
	*state = m_state;
}

void NedStaffSelectDialog::add_staff(GtkTreeModel *model, GtkTreePath *path, GtkTreeIter *iter, gpointer data) {
	NedStaffSelectDialog *selection_dialog = (NedStaffSelectDialog *) data;
	int sel_num;


	char *tp = gtk_tree_path_to_string(path);
	if (sscanf(tp, "%d", &sel_num) != 1) {
		NedResource::Abort("NedStaffSelectDialog::add_staff: error reading tree path(1)");
	}
	if (sel_num < 0 || sel_num >= selection_dialog->m_main_window->getStaffCount()) {
		NedResource::Abort("NedStaffSelectDialog::add_staff(2)");
	}

	selection_dialog->m_selected_staves[sel_num] = true;
}

void NedStaffSelectDialog::OnAll(GtkButton *button, gpointer data) {
	NedStaffSelectDialog *selection_dialog = (NedStaffSelectDialog *) data;
	int i;

	gtk_tree_selection_select_all(selection_dialog->m_tree_selection);
	for (i = 0; i < selection_dialog->m_main_window->getStaffCount(); selection_dialog->m_selected_staves[i++] = true);
}

void NedStaffSelectDialog::OnNone(GtkButton *button, gpointer data) {
	NedStaffSelectDialog *selection_dialog = (NedStaffSelectDialog *) data;
	int i;

	gtk_tree_selection_unselect_all(selection_dialog->m_tree_selection);
	for (i = 0; i < selection_dialog->m_main_window->getStaffCount(); selection_dialog->m_selected_staves[i++] = false);
}
