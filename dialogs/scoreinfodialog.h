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

#ifndef SCORE_INFO_BLOCK_DIALOG_H

#define SCORE_INFO_BLOCK_DIALOG_H
#include "config.h"
#include <gtk/gtk.h>
#include <gtk/gtk.h>
class NedPangoCairoText;
class NedMainWindow;

class ScoreInfo {
	public:
		ScoreInfo();
		~ScoreInfo();
		NedPangoCairoText *title, *subject, *composer, *arranger, *copyright;

		cairo_text_extents_t title_extends, subject_extends, composer_extends, arranger_extends, copyright_extends;
};


class NedScoreInfoConfigDialog {
	public:
		NedScoreInfoConfigDialog(GtkWindow *parent, NedMainWindow *main_window, ScoreInfo *scoreinfo);
		void getValues(bool *state);
	private:
		static void CloseCallBack(GtkDialog *dialog, gint result, gpointer data);
		bool m_state;
		ScoreInfo *m_scoreinfo;
		GtkWidget *m_title;
		GtkWidget *m_subject;
		GtkWidget *m_composer;
		GtkWidget *m_arranger;
		GtkWidget *m_copyright;
		NedMainWindow *m_main_window;
};

#endif /*  SCORE_INFO_BLOCK_DIALOG_H */
