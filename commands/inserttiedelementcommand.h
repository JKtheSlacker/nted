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

#ifndef INSERT_TIED_ELEMENT_COMMAND_H

#define INSERT_TIED_ELEMENT_COMMAND_H
#include "config.h"
#include "command.h"

class NedChordOrRest;
class NedVoice;

class NedInsertTiedElementCommand : public NedCommand {
	public:	
		NedInsertTiedElementCommand(NedVoice *voice, NedChordOrRest *chord_or_rest, unsigned int len, int dotcount);
		virtual void execute(bool adjust = false);
		virtual void unexecute(bool adjust = false);
	private:
		NedChordOrRest *m_chord_or_rest, *m_new_chord_or_rest;
		NedVoice *m_voice;
		unsigned int m_length;
};

#endif /* CINSERT_TIED_ELEMENT_COMMAND_HHANGE_CHORD_OR_REST_LENGTH_H */
