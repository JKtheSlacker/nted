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

#ifndef VOLUME_SIGN_H

#define VOLUME_SIGN_H
#include "freereplaceable.h"

#define VOL_PPP   1
#define VOL_PP    2
#define VOL_P     3
#define VOL_MF    4
#define VOL_SF    5
#define VOL_MP    6
#define VOL_SP    7
#define VOL_F     8
#define VOL_FF    9
#define VOL_FFF  10


class NedVolumeSign : public NedFreeReplaceable {
	public:
		NedVolumeSign(int kind);
		NedVolumeSign(int kind, NedChordOrRest *element);
		NedVolumeSign(int kind, NedChordOrRest *element, double x, double y, bool relative);
		virtual NedFreeReplaceable *clone(struct addr_ref_str **slurlist, NedChordOrRest *newelement, GtkWidget *drawing_area = NULL, NedMainWindow *main_window = NULL );
		virtual void adjust_pointers(struct addr_ref_str *slurlist);
		virtual void draw(cairo_t *cr, double leftx, double topy, double zoom_factor, int zoom_level, double scale);
		virtual int getType() {return FREE_PLACEABLE_VOLSIGN;}
		static int determineKind(int volume);
		int getKind() {return m_kind;}
		virtual void startContextDialog(GtkWidget *ref);
		int m_midi_volume;
	private: 
		void setMidiVolume();
		int m_kind;
};

#endif /* VOLUME_SIGN_H */
