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

#include "scoreinfodialog.h"
#include "localization.h"
#include "resource.h"
#include "pangocairotext.h"
#include "mainwindow.h"
#include "config.h"

#define DIALOG_WIDTH 300
#define DIALOG_HEIGHT 10 // dummy

ScoreInfo::ScoreInfo() : title(NULL), subject(NULL), composer(NULL), arranger(NULL), copyright(NULL) {}

ScoreInfo::~ScoreInfo() {
	if (title != NULL) {
		delete title;
		title = NULL;
	}
	if (subject != NULL) {
		delete subject;
		subject = NULL;
	}

	if (composer != NULL) {
		delete composer;
		composer = NULL;
	}

	if (arranger != NULL) {
		delete arranger;
		arranger = NULL;
	}

	if (copyright != NULL) {
		delete copyright;
		copyright = NULL;
	}
}

#define TITLELENGTH 100


NedScoreInfoConfigDialog::NedScoreInfoConfigDialog(GtkWindow *parent, NedMainWindow *main_window, ScoreInfo *scoreinfo) :
m_scoreinfo(scoreinfo), m_main_window(main_window) {
	GtkWidget *dialog;
	GtkWidget *label;
	GtkWidget *frame;
	GtkWidget *grid;


	dialog = gtk_dialog_new_with_buttons(_("Score info"), parent, (GtkDialogFlags) (GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT),
		GTK_STOCK_OK, GTK_RESPONSE_ACCEPT, GTK_STOCK_CANCEL, GTK_RESPONSE_REJECT, NULL);
	g_signal_connect(dialog, "response", G_CALLBACK (CloseCallBack), (void *) this);

	frame = gtk_frame_new(_("Score info"));

	grid = gtk_table_new(2, 5, FALSE);

	label = gtk_label_new(_("Title:"));
	m_title = gtk_entry_new_with_max_length(TITLELENGTH);
	if (m_scoreinfo->title != NULL) {
		gtk_entry_set_text(GTK_ENTRY(m_title), m_scoreinfo->title->getText());
	}
	gtk_table_attach(GTK_TABLE(grid), label, 0, 1, 0, 1, GTK_SHRINK, (GtkAttachOptions) 0,  0, 0);
	gtk_table_attach(GTK_TABLE(grid), m_title, 1, 2, 0, 1, (GtkAttachOptions) (GTK_EXPAND | GTK_FILL) , (GtkAttachOptions) 0,  0, 0);

	label = gtk_label_new(_("Subject:"));
	m_subject = gtk_entry_new_with_max_length(TITLELENGTH);
	if (m_scoreinfo->subject != NULL) {
		gtk_entry_set_text(GTK_ENTRY(m_subject), m_scoreinfo->subject->getText());
	}
	gtk_table_attach(GTK_TABLE(grid), label, 0, 1, 1, 2, GTK_SHRINK, (GtkAttachOptions) 0,  0, 0);
	gtk_table_attach(GTK_TABLE(grid), m_subject, 1, 2, 1, 2, (GtkAttachOptions) (GTK_EXPAND | GTK_FILL) , (GtkAttachOptions) 0,  0, 0);

	label = gtk_label_new(_("Composer:"));
	m_composer = gtk_entry_new_with_max_length(TITLELENGTH);
	if (m_scoreinfo->composer != NULL) {
		gtk_entry_set_text(GTK_ENTRY(m_composer), m_scoreinfo->composer->getText());
	}
	gtk_table_attach(GTK_TABLE(grid), label, 0, 1, 2, 3, GTK_SHRINK, (GtkAttachOptions) 0,  0, 0);
	gtk_table_attach(GTK_TABLE(grid), m_composer, 1, 2, 2, 3, (GtkAttachOptions) (GTK_EXPAND | GTK_FILL) , (GtkAttachOptions) 0,  0, 0);

	label = gtk_label_new(_("Arranger:"));
	m_arranger = gtk_entry_new_with_max_length(TITLELENGTH);
	if (m_scoreinfo->arranger != NULL) {
		gtk_entry_set_text(GTK_ENTRY(m_arranger), m_scoreinfo->arranger->getText());
	}
	gtk_table_attach(GTK_TABLE(grid), label, 0, 1, 3, 4, GTK_SHRINK, (GtkAttachOptions) 0,  0, 0);
	gtk_table_attach(GTK_TABLE(grid), m_arranger, 1, 2, 3, 4, (GtkAttachOptions) (GTK_EXPAND | GTK_FILL) , (GtkAttachOptions) 0,  0, 0);

	label = gtk_label_new(_("Copyright:"));
	m_copyright = gtk_entry_new_with_max_length(TITLELENGTH);
	if (m_scoreinfo->copyright != NULL) {
		gtk_entry_set_text(GTK_ENTRY(m_copyright), m_scoreinfo->copyright->getText());
	}
	gtk_table_attach(GTK_TABLE(grid), label, 0, 1, 4, 5, GTK_SHRINK, (GtkAttachOptions) 0, 0, 0);
	gtk_table_attach(GTK_TABLE(grid), m_copyright, 1, 2, 4, 5, (GtkAttachOptions) (GTK_EXPAND | GTK_FILL) , (GtkAttachOptions) 0, 0, 0);

	gtk_container_add (GTK_CONTAINER(frame), grid);

	gtk_container_add (GTK_CONTAINER (GTK_DIALOG(dialog)->vbox), frame);
	gtk_window_set_default_size (GTK_WINDOW (dialog), DIALOG_WIDTH, DIALOG_HEIGHT);
	gtk_widget_show_all (dialog);
	gtk_dialog_run(GTK_DIALOG(dialog));

}

void NedScoreInfoConfigDialog::CloseCallBack(GtkDialog *dialog, gint result, gpointer data) {
	NedScoreInfoConfigDialog *score_config_dialog = (NedScoreInfoConfigDialog *) data;
	bool diff;
	const char *cptr;

	switch (result) {
		case GTK_RESPONSE_ACCEPT:
			score_config_dialog->m_state = TRUE;
		break;
		default:
			score_config_dialog->m_state = FALSE;
			gtk_widget_destroy (GTK_WIDGET(dialog));
			return;
		break;
	}

	cptr = gtk_entry_get_text(GTK_ENTRY(score_config_dialog->m_title));
	if (score_config_dialog->m_scoreinfo->title) {
		diff = score_config_dialog->m_scoreinfo->title->textDiffers((char *) cptr);
	}
	else {
		diff = (cptr != NULL && strlen(cptr) > 0);
	}
	if (diff) {
		if (score_config_dialog->m_scoreinfo->title != NULL) {
			delete score_config_dialog->m_scoreinfo->title;
			score_config_dialog->m_scoreinfo->title = NULL;
		}
		if (cptr != NULL && strlen(cptr) > 0) {
			score_config_dialog->m_scoreinfo->title = new NedPangoCairoText(score_config_dialog->m_main_window->getDrawingArea()->window,
				(char *) cptr, TITLE_FONT, TITLE_FONT_SLANT, TITLE_FONT_WEIGHT, TITLE_FONT_SIZE, score_config_dialog->m_main_window->getCurrentZoomFactor(), 1.0, false);
		}
	}

	cptr = gtk_entry_get_text(GTK_ENTRY(score_config_dialog->m_subject));
	if (score_config_dialog->m_scoreinfo->subject) {
		diff = score_config_dialog->m_scoreinfo->subject->textDiffers((char *) cptr);
	}
	else {
		diff = (cptr != NULL && strlen(cptr) > 0);
	}
	if (diff) {
		if (score_config_dialog->m_scoreinfo->subject != NULL) {
			delete score_config_dialog->m_scoreinfo->subject;
			score_config_dialog->m_scoreinfo->subject = NULL;
		}
		if (cptr != NULL && strlen(cptr) > 0) {
			score_config_dialog->m_scoreinfo->subject = new NedPangoCairoText(score_config_dialog->m_main_window->getDrawingArea()->window,
				(char *) cptr, SUBJECT_FONT, SUBJECT_FONT_SLANT, SUBJECT_FONT_WEIGHT, SUBJECT_FONT_SIZE, score_config_dialog->m_main_window->getCurrentZoomFactor(), 1.0, false);
		}
	}

	cptr = gtk_entry_get_text(GTK_ENTRY(score_config_dialog->m_composer));
	if (score_config_dialog->m_scoreinfo->composer) {
		diff = score_config_dialog->m_scoreinfo->composer->textDiffers((char *) cptr);
	}
	else {
		diff = (cptr != NULL && strlen(cptr) > 0);
	}
	if (diff) {
		if (score_config_dialog->m_scoreinfo->composer != NULL) {
			delete score_config_dialog->m_scoreinfo->composer;
			score_config_dialog->m_scoreinfo->composer = NULL;
		}
		if (cptr != NULL && strlen(cptr) > 0) {
			score_config_dialog->m_scoreinfo->composer = new NedPangoCairoText(score_config_dialog->m_main_window->getDrawingArea()->window,
				(char *) cptr, COMPOSER_FONT, COMPOSER_FONT_SLANT, COMPOSER_FONT_WEIGHT, COMPOSER_FONT_SIZE, score_config_dialog->m_main_window->getCurrentZoomFactor(), 1.0, false);
		}
	}
	cptr = gtk_entry_get_text(GTK_ENTRY(score_config_dialog->m_arranger));
	if (score_config_dialog->m_scoreinfo->arranger) {
		diff = score_config_dialog->m_scoreinfo->arranger->textDiffers((char *) cptr);
	}
	else {
		diff = (cptr != NULL && strlen(cptr) > 0);
	}
	if (diff) {
		if (score_config_dialog->m_scoreinfo->arranger != NULL) {
			delete score_config_dialog->m_scoreinfo->arranger;
			score_config_dialog->m_scoreinfo->arranger = NULL;
		}
		if (cptr != NULL && strlen(cptr) > 0) {
			score_config_dialog->m_scoreinfo->arranger = new NedPangoCairoText(score_config_dialog->m_main_window->getDrawingArea()->window,
				(char *) cptr, ARRANGER_FONT, ARRANGER_FONT_SLANT, ARRANGER_FONT_WEIGHT, ARRANGER_FONT_SIZE, score_config_dialog->m_main_window->getCurrentZoomFactor(), 1.0, false);
		}
	}

	cptr = gtk_entry_get_text(GTK_ENTRY(score_config_dialog->m_copyright));
	if (score_config_dialog->m_scoreinfo->copyright) {
		diff = score_config_dialog->m_scoreinfo->copyright->textDiffers((char *) cptr);
	}
	else {
		diff = (cptr != NULL && strlen(cptr) > 0);
	}
	if (diff) {
		if (score_config_dialog->m_scoreinfo->copyright != NULL) {
			delete score_config_dialog->m_scoreinfo->copyright;
			score_config_dialog->m_scoreinfo->copyright = NULL;
		}
		if (cptr != NULL && strlen(cptr) > 0) {
			score_config_dialog->m_scoreinfo->copyright = new NedPangoCairoText(score_config_dialog->m_main_window->getDrawingArea()->window,
				(char *) cptr, COPYRIGHT_FONT, COPYRIGHT_FONT_SLANT, COPYRIGHT_FONT_WEIGHT, COPYRIGHT_FONT_SIZE, score_config_dialog->m_main_window->getCurrentZoomFactor(), 1.0, false);
		}
	}

	gtk_widget_destroy (GTK_WIDGET(dialog));
}

void NedScoreInfoConfigDialog::getValues(bool *state) {
	*state = m_state;
}
