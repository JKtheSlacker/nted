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
#include "resource.h"
#include "freechordname.h"
#include "chordorrest.h"
#include "textdialog.h"
#include "pangocairotext.h"
#include "chordnamedialog.h"
#include "staff.h"
#include "system.h"
#include "page.h"

#define X_POS_PAGE_REL(p) ((m_element->getPage()->getContentXpos() + (p)) * zoom_factor - leftx)
#define Y_POS_PAGE_REL(p) ((p) * zoom_factor - topy)
#define X_POS_PAGE_REL_INVERSE(p) (((p) / current_scale + leftx) / zoom_factor - m_element->getPage()->getContentXpos()) 
#define Y_POS_PAGE_REL_INVERSE(p) (((p) / current_scale + topy - (TOP_BOTTOM_BORDER + DEFAULT_BORDER)) / zoom_factor)

#define ROOT_FONT "Sans"
#define ROOT_FONT_SLANT PANGO_STYLE_NORMAL
#define ROOT_FONT_WEIGHT PANGO_WEIGHT_NORMAL

#define UPDOWN_FONT "Sans"
#define UPDOWN_FONT_SLANT PANGO_STYLE_NORMAL
#define UPDOWN_FONT_WEIGHT PANGO_WEIGHT_NORMAL

NedChordName::NedChordName(GtkWidget *drawing_area,  char *roottext, char *uptext, char *downtext, double fontsize) :
NedFreeReplaceable(), m_root_text(NULL), m_up_text(NULL), m_down_text(NULL), m_fontsize(fontsize), m_drawing_area(drawing_area)  {
	if (roottext != NULL && strlen(roottext) > 0) {
		m_root_text = new NedPangoCairoText(drawing_area->window, roottext, ROOT_FONT, ROOT_FONT_SLANT, ROOT_FONT_WEIGHT, m_fontsize, NedResource::getStartZoomScale(), 1.0, false);
	}
	if (uptext != NULL && strlen(uptext) > 0) {
		m_up_text = new NedPangoCairoText(drawing_area->window, uptext, UPDOWN_FONT, UPDOWN_FONT_SLANT, UPDOWN_FONT_WEIGHT, m_fontsize * 4.0/5.0, NedResource::getStartZoomScale(), 1.0, false);
	}
	if (downtext != NULL && strlen(downtext) > 0) {
		m_down_text = new NedPangoCairoText(drawing_area->window, downtext, UPDOWN_FONT, UPDOWN_FONT_SLANT, UPDOWN_FONT_WEIGHT, m_fontsize * 4.0/5.0, NedResource::getStartZoomScale(), 1.0, false);
	}
}

NedChordName::NedChordName(GtkWidget *drawing_area, char *roottext, char *uptext, char *downtext, double fontsize,
		NedChordOrRest *element) : NedFreeReplaceable(element), m_root_text(NULL),
			 m_up_text(NULL), m_down_text(NULL), m_fontsize(fontsize), m_drawing_area(drawing_area) {
	if (roottext != NULL && strlen(roottext) > 0) {
		m_root_text = new NedPangoCairoText(drawing_area->window, roottext, ROOT_FONT, ROOT_FONT_SLANT, ROOT_FONT_WEIGHT, m_fontsize, NedResource::getStartZoomScale(), 1.0, false);
	}
	if (uptext != NULL && strlen(uptext) > 0) {
		m_up_text = new NedPangoCairoText(drawing_area->window, uptext, UPDOWN_FONT, UPDOWN_FONT_SLANT, UPDOWN_FONT_WEIGHT, m_fontsize * 4.0/5.0, NedResource::getStartZoomScale(), 1.0, false);
	}
	if (downtext != NULL && strlen(downtext) > 0) {
		m_down_text = new NedPangoCairoText(drawing_area->window, downtext, UPDOWN_FONT, UPDOWN_FONT_SLANT, UPDOWN_FONT_WEIGHT, m_fontsize * 4.0/5.0, NedResource::getStartZoomScale(), 1.0, false);
	}
}

NedChordName::NedChordName(GtkWidget *drawing_area, char *roottext, char *uptext, char *downtext, double fontsize,
		NedChordOrRest *element, double x, double y, bool relative) :
			NedFreeReplaceable(element, x, y, relative), 
			m_root_text(NULL), m_up_text(NULL), m_down_text(NULL), m_fontsize(fontsize), m_drawing_area(drawing_area) {
	if (roottext != NULL && strlen(roottext) > 0) {
		m_root_text = new NedPangoCairoText(drawing_area->window, roottext, ROOT_FONT, ROOT_FONT_SLANT, ROOT_FONT_WEIGHT, m_fontsize, NedResource::getStartZoomScale(), 1.0, false);
	}
	if (uptext != NULL && strlen(uptext) > 0) {
		m_up_text = new NedPangoCairoText(drawing_area->window, uptext, UPDOWN_FONT, UPDOWN_FONT_SLANT, UPDOWN_FONT_WEIGHT, m_fontsize * 4.0/5.0, NedResource::getStartZoomScale(), 1.0, false);
	}
	if (downtext != NULL && strlen(downtext) > 0) {
		m_down_text = new NedPangoCairoText(drawing_area->window, downtext, UPDOWN_FONT, UPDOWN_FONT_SLANT, UPDOWN_FONT_WEIGHT, m_fontsize * 4.0/5.0, NedResource::getStartZoomScale(), 1.0, false);
	}
}


NedFreeReplaceable *NedChordName::clone(struct addr_ref_str **slurlist, NedChordOrRest *newelement, GtkWidget *drawing_area /* = NULL */,
		NedMainWindow *main_window /* = NULL */) {
	if (drawing_area == NULL) {
		NedResource::Abort("NedChordName::clone(1)");
	}
	NedChordName *chordname = new NedChordName(drawing_area, m_root_text->getText(), m_up_text->getText(),
				m_down_text->getText(), m_fontsize, newelement, m_x, m_y, true);
	return chordname;
}

NedChordName::~NedChordName() {
	if (m_root_text != NULL) {
		delete m_root_text;
		m_root_text = NULL;
	}
	if (m_up_text != NULL) {
		delete m_up_text;
		m_up_text = NULL;
	}
	if (m_down_text != NULL) {
		delete m_down_text;
		m_down_text = NULL;
	}
}

void NedChordName::adjust_pointers(struct addr_ref_str *slurlist) {
}

void NedChordName::startContextDialog(GtkWidget *ref) {
	bool state;
	char *newroottext;
	char *newuptext;
	char *newdowntext;
	double dummy;

	NedChordNameDialog *dialog = new NedChordNameDialog (NULL, GTK_WINDOW(ref),
			m_root_text != NULL ? m_root_text->getText() : NULL,
			m_up_text != NULL ? m_up_text->getText() : NULL,
			m_down_text != NULL ? m_down_text->getText() : NULL, m_fontsize, -1.5, false);
	dialog->getValues(&state, &newroottext, &newuptext, &newdowntext, &m_fontsize, &dummy);
	if (state) {
		if (m_root_text != NULL) {
			delete m_root_text;
			m_root_text = NULL;
		}
		if (m_up_text != NULL) {
			delete m_up_text;
			m_up_text = NULL;
		}
		if (m_down_text != NULL) {
			delete m_down_text;
			m_down_text = NULL;
		}
		if (newroottext != NULL) {
			if (strlen(newroottext) > 0) {
				m_root_text = new NedPangoCairoText(ref->window, newroottext, ROOT_FONT, ROOT_FONT_SLANT, ROOT_FONT_WEIGHT, m_fontsize, NedResource::getStartZoomScale(), 1.0, false);
			}
		}
		if (newuptext != NULL) {
			if (strlen(newuptext) > 0) {
				m_up_text = new NedPangoCairoText(ref->window, newuptext, UPDOWN_FONT, UPDOWN_FONT_SLANT, UPDOWN_FONT_WEIGHT, m_fontsize * 4.0/5.0, NedResource::getStartZoomScale(), 1.0, false);
			}
		}
		if (newdowntext != NULL) {
			if (strlen(newdowntext) > 0) {
				m_down_text = new NedPangoCairoText(ref->window, newdowntext, UPDOWN_FONT, UPDOWN_FONT_SLANT, UPDOWN_FONT_WEIGHT, m_fontsize * 4.0/5.0, NedResource::getStartZoomScale(), 1.0, false);
			}
		}
	
	}
}

#define UP_X_OFFS 0.1
#define UP_Y_OFFS -0.15
#define DOWN_X_OFFS 0.1
#define DOWN_Y_OFFS 0.2

void NedChordName::draw(cairo_t *cr, double leftx, double topy, double zoom_factor, int zoom_level, double scale) {
	if (m_element == NULL) return;

	double xpos, ypos;
	double xx, yy;

	getXYPos(&xpos, &ypos);

	cairo_new_path(cr);
	if (isActive()) {
		cairo_set_source_rgb (cr, 1.0, 0.0, 0.0);
	}
	if (m_root_text != NULL) {
		xx = scale * X_POS_PAGE_REL(xpos);
		yy = scale * Y_POS_PAGE_REL(ypos) - m_root_text->getHeight() / 2.0;
		m_root_text->draw(cr, xx, yy, zoom_factor, scale);
	}
	if (m_up_text != NULL) {
		xx = scale * X_POS_PAGE_REL(xpos + UP_X_OFFS * m_fontsize / 0.5 * 4.0/5.0) + m_root_text->getInternalWidth();
		yy = scale * Y_POS_PAGE_REL(ypos + UP_Y_OFFS * m_fontsize / 0.5 * 4.0/5.0) - m_root_text->getHeight() / 2.0;
		m_up_text->draw(cr, xx, yy, zoom_factor, scale);
	}
	if (m_down_text != NULL) {
		xx = scale * X_POS_PAGE_REL(xpos + DOWN_X_OFFS * m_fontsize / 0.5 * 4.0/5.0) + m_root_text->getInternalWidth();
		yy = scale * Y_POS_PAGE_REL(ypos + DOWN_Y_OFFS * m_fontsize / 0.5 * 4.0/5.0) - m_root_text->getHeight() / 2.0;
		m_down_text->draw(cr, xx, yy, zoom_factor, scale);
	}
	if (isActive()) {
		cairo_set_source_rgb (cr, 0.0, 0.0, 0.0);
	}
	if (m_is_abs) {
		drawConnectionInFreeText(cr, xpos, ypos, scale, leftx, topy, zoom_factor);
	}
}

bool NedChordName::trySelect(double xl, double yl, double zoom_factor, double current_scale, double leftx, double topy) {
	if (m_element == NULL) return false;
	double xpos, ypos;
	double x = X_POS_PAGE_REL_INVERSE(xl);
	double y = Y_POS_PAGE_REL_INVERSE(yl);

	if (m_root_text == NULL) return false;

	getXYPos(&xpos, &ypos);
	if (x < xpos || x > xpos + m_root_text->getWidth() / zoom_factor || y < ypos - m_root_text->getHeight() / zoom_factor || y > ypos) return false;
	setThis();
	return true;
}


void NedChordName::setTexts(char *roottext, char *uptext, char *downtext, double zoom) {
	if (m_root_text != NULL) {
		delete m_root_text;
		m_root_text = NULL;
	}
	if (roottext != NULL && strlen(roottext) > 0) {
		m_root_text->setText(roottext, zoom, false);
	}
	if (m_up_text != NULL) {
		delete m_up_text;
		m_up_text = NULL;
	}
	if (uptext != NULL && strlen(uptext) > 0) {
		m_up_text->setText(uptext, zoom, false);
	}
	if (m_down_text != NULL) {
		delete m_down_text;
		m_down_text = NULL;
	}
	if (downtext != NULL && strlen(downtext) > 0) {
		m_down_text->setText(downtext, zoom, false);
	}
}

void NedChordName::setZoom(double zoom, double scale) {
	if (m_root_text != NULL) {
		m_root_text->setZoom( zoom, scale);
	}
	if (m_up_text != NULL) {
		m_up_text->setZoom( zoom, scale);
	}
	if (m_down_text != NULL) {
		m_down_text->setZoom(zoom, scale);
	}
}

char *NedChordName::getRootText() {
	if (m_root_text == NULL) return NULL;

	return m_root_text->getText();
}

char *NedChordName::getUpText() {
	if (m_up_text == NULL) return NULL;

	return m_up_text->getText();
}

char *NedChordName::getDownText() {
	if (m_down_text == NULL) return NULL;

	return m_down_text->getText();
}
