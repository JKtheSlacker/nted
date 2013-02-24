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
#include "freetext.h"
#include "chordorrest.h"
#include "textdialog.h"
#include "pangocairotext.h"
#include "staff.h"
#include "system.h"
#include "page.h"

#define X_POS_PAGE_REL(p) ((m_element->getPage()->getContentXpos() + (p)) * zoom_factor - leftx)
#define Y_POS_PAGE_REL(p) ((p) * zoom_factor - topy)
#define X_POS_PAGE_REL_INVERSE(p) (((p) / current_scale + leftx) / zoom_factor - m_element->getPage()->getContentXpos()) 
#define Y_POS_PAGE_REL_INVERSE(p) (((p) / current_scale + topy - (TOP_BOTTOM_BORDER + DEFAULT_BORDER)) / zoom_factor)

NedFreeText::NedFreeText(char *text, GtkWidget *drawing_area, int anchor,  unsigned int tempo, bool enable_tempo, unsigned int volume, bool enable_volume,
			unsigned short segno_sign, int midi_pgm, unsigned int channel, bool enable_channel, const char *font_family, double font_size, PangoStyle font_slant, PangoWeight font_weight) : NedFreeReplaceable(),
		m_midi_volume(volume), m_enable_volume_change(enable_volume), m_enable_tempo_change(enable_tempo), m_segno_sign(segno_sign), 
			m_midi_pgm(midi_pgm), m_channel(channel), m_enable_channel(enable_channel),
			m_font_family(font_family), m_font_size(font_size), m_font_slant(font_slant), m_font_weight(font_weight), m_anchor(anchor), m_tempo(tempo),
			m_text(NULL), m_drawing_area(drawing_area) {
		if (text != NULL) {
			m_text = new NedPangoCairoText(drawing_area->window, text, font_family, font_slant, font_weight, font_size, NedResource::getStartZoomScale(), 1.0, false);
		}
}

NedFreeText::NedFreeText(char *text, GtkWidget *drawing_area, int anchor,  unsigned int tempo, bool enable_tempo, unsigned int volume, bool enable_volume,
		unsigned short segno_sign, int midi_pgm, unsigned int channel, bool enable_channel, const char *font_family, double font_size, PangoStyle font_slant, PangoWeight font_weight, 
		NedChordOrRest *element) : NedFreeReplaceable(element), 
			m_midi_volume(volume), m_enable_volume_change(enable_volume), m_enable_tempo_change(enable_tempo),
			m_segno_sign(segno_sign), m_midi_pgm(midi_pgm), m_channel(channel), m_enable_channel(enable_channel), m_font_family(font_family),
			m_font_size(font_size), m_font_slant(font_slant), m_font_weight(font_weight), m_anchor(anchor), m_tempo(tempo),
			m_text(NULL), m_drawing_area(drawing_area) {
		if (text != NULL) {
			m_text = new NedPangoCairoText(drawing_area->window, text, font_family, font_slant, font_weight, font_size, NedResource::getStartZoomScale(), 1.0, false);
		}
}

NedFreeText::NedFreeText(char *text, GtkWidget *drawing_area, int anchor,  unsigned int tempo, bool enable_tempo, unsigned int volume, bool enable_volume,
		unsigned short segno_sign, int midi_pgm, unsigned int channel, bool enable_channel, const char *font_family, double font_size, PangoStyle font_slant, PangoWeight font_weight, 
		NedChordOrRest *element, double x, double y, bool relative) :
			NedFreeReplaceable(element, x, y, relative), 
		m_midi_volume(volume), m_enable_volume_change(enable_volume), m_enable_tempo_change(enable_tempo), m_segno_sign(segno_sign), m_midi_pgm(midi_pgm),
			 m_channel(channel), m_enable_channel(enable_channel),
			m_font_family(font_family), m_font_size(font_size),
			m_font_slant(font_slant), m_font_weight(font_weight), m_anchor(anchor), m_tempo(tempo),
			m_text(NULL), m_drawing_area(drawing_area) {
		if (text != NULL) {
			m_text = new NedPangoCairoText(drawing_area->window, text, font_family, font_slant, font_weight, font_size, NedResource::getStartZoomScale(), 1.0, false);
		}
}


NedFreeReplaceable *NedFreeText::clone(struct addr_ref_str **slurlist, NedChordOrRest *newelement, GtkWidget *drawing_area /* = NULL */,
		NedMainWindow *main_window /* = NULL */) {
	const char *nfamily;

	if (drawing_area == NULL) {
		NedResource::Abort("NedFreeText::clone(1)");
	}
	if ((nfamily = (char *) g_try_malloc(strlen(m_font_family) + 1)) == NULL) {
		NedResource::Abort("NedFreeText::clone(2)");
	}
	strcpy((char *) nfamily, m_font_family);
	NedFreeText *freetext = new NedFreeText(m_text->getText(), drawing_area, m_anchor, m_tempo, m_enable_tempo_change, m_midi_volume, m_enable_volume_change,
		m_segno_sign, m_midi_pgm, m_enable_channel, m_channel, nfamily, m_font_size, m_font_slant, m_font_weight, newelement, m_x, m_y, true);
	return freetext;
}

void NedFreeText::adjust_pointers(struct addr_ref_str *slurlist) {
}

void NedFreeText::startContextDialog(GtkWidget *ref) {
	bool state;
	char *newtext;
	const char *newfont_family;
	PangoStyle newfont_slant;
	PangoWeight newfont_weight;
	bool newenable_volume;
	bool newenable_tempo;
	unsigned short newsegno_sign;
	int new_anchor;
	unsigned int newvolume;
	unsigned int newtempo;
	unsigned int newchannel;
	bool newenable_channel;
	int new_midi_pgm;
	double newfont_size;

	NedTextDialog *dialog = new NedTextDialog (GTK_WINDOW(ref), m_text->getText(), m_anchor, m_tempo, m_enable_tempo_change, m_midi_volume, m_enable_volume_change,
		m_segno_sign, m_midi_pgm, m_channel, m_enable_channel, m_font_family, m_font_size, m_font_slant, m_font_weight);
	dialog->getValues(&state, &newtext, &new_anchor, &newtempo, &newenable_tempo, &newvolume, &newenable_volume, &newsegno_sign, &new_midi_pgm,
		&newchannel, &newenable_channel, &newfont_family, &newfont_size, &newfont_slant, &newfont_weight);
	if (state) {
		if (m_text != NULL) {
			delete m_text;
			m_text = NULL;
		}
		if (newtext != NULL) {
			if (strlen(newtext) > 0) {
				m_anchor = new_anchor;
				m_tempo = newtempo;
				m_enable_tempo_change = newenable_tempo;
				m_midi_volume = newvolume;
				m_enable_volume_change = newenable_volume;
				m_segno_sign = newsegno_sign;
				m_midi_pgm = new_midi_pgm;
				m_channel = newchannel;
				m_enable_channel = newenable_channel;
				m_font_family = newfont_family;
				m_font_size = newfont_size;
				m_font_slant = newfont_slant;
				m_font_weight = newfont_weight;  
				m_text = new NedPangoCairoText(ref->window, newtext, m_font_family, m_font_slant, m_font_weight, m_font_size, NedResource::getStartZoomScale(), 1.0, false);
			}
		}
	
	}
}


void NedFreeText::draw(cairo_t *cr, double leftx, double topy, double zoom_factor, int zoom_level, double scale) {
	if (m_element == NULL) return;

	double xpos, ypos;
	double xx, yy;
	if (m_text == NULL) return;

	getXYPos(&xpos, &ypos);

	cairo_new_path(cr);
	if (isActive()) {
		cairo_set_source_rgb (cr, 1.0, 0.0, 0.0);
	}
	xx = scale * X_POS_PAGE_REL(xpos);
	yy = scale * Y_POS_PAGE_REL(ypos) - m_text->getHeight() / 2.0;
	switch (m_anchor) {
		case ANCHOR_MID: xx = scale * X_POS_PAGE_REL(xpos) - m_text->getWidth() / 2.0; yy = scale * Y_POS_PAGE_REL(ypos) - m_text->getHeight() / 2.0; break;
		case ANCHOR_RIGHT: xx = scale * X_POS_PAGE_REL(xpos) - m_text->getWidth(); yy = scale * Y_POS_PAGE_REL(ypos) - m_text->getHeight() / 2.0; break;
	}
	m_text->draw(cr, xx, yy, zoom_factor, scale);
	if (isActive()) {
		cairo_set_source_rgb (cr, 0.0, 0.0, 0.0);
	}
	if (m_is_abs) {
		drawConnectionInFreeText(cr, xpos, ypos, scale, leftx, topy, zoom_factor);
	}
}

bool NedFreeText::trySelect(double xl, double yl, double zoom_factor, double current_scale, double leftx, double topy) {
	if (m_element == NULL) return false;
	double xpos, ypos;
	double x = X_POS_PAGE_REL_INVERSE(xl);
	double y = Y_POS_PAGE_REL_INVERSE(yl);

	if (m_text == NULL) return false;

	getXYPos(&xpos, &ypos);
	switch (m_anchor) {
		case ANCHOR_MID: xpos -= m_text->getWidth() / zoom_factor / 2.0; break;
		case ANCHOR_RIGHT: xpos -= m_text->getWidth() / zoom_factor; break;
	}
	if (x < xpos || x > xpos + m_text->getWidth() / zoom_factor || y < ypos - m_text->getHeight() / zoom_factor || y > ypos) return false;
	setThis();
	return true;
}

double NedFreeText::getTempoInverse() {
	return 60000.0 / (double) m_tempo;
}

void NedFreeText::setText(char *text, double zoom) {
	if (m_text != NULL) {
		delete m_text;
		m_text = NULL;
	}
	if (text != NULL && strlen(text) > 0) {
		m_text->setText(text, zoom, false);
	}
}

void NedFreeText::setZoom(double zoom, double scale) {
	if (m_text == NULL) return;
	m_text->setZoom(zoom, scale);
}

char *NedFreeText::getText() {
	if (m_text == NULL) return NULL;

	return m_text->getText();
}

void NedFreeText::getLilyText(char *s) {
	char *cptr;
#define LILYSEGNO "\\musicglyph #\"scripts.segno\""
#define LILYCODA "\\musicglyph #\"scripts.coda\""
	if (m_text == NULL) {
		s[0] = '\0';
		return;
	}

	for (cptr = m_text->getText(); *cptr != '\0'; cptr++, s++) {
		if (*cptr == '\\') {
			if (*(cptr + 1) == 'S') {
				strcpy(s, LILYSEGNO);
				s += strlen(LILYSEGNO) - 1;
				cptr++;
				continue;
			}
			else if (*(cptr + 1) == 'O') {
				strcpy(s, LILYCODA);
				s += strlen(LILYCODA) - 1;
				cptr++;
				continue;
			}
			else {
				*s = *cptr;
			}
		}
		else {
			*s = *cptr;
		}
	}
	*s = '\0';
}
