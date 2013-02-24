#########################################################################################
#											#
# This program is free software; you can redistribute it and#or modify it under the	#
# terms of the GNU General Public License as published by the Free Software		#
# Foundation; either version 2 of the License, or (at your option) any later version.	#
#											#
# This program is distributed in the hope that it will be useful, but WITHOUT ANY	#
# WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A	#
# PARTICULAR PURPOSE. See the GNU General Public License for more details.		#
#											#
# You should have received a copy of the GNU General Public License along with this	#
# program; (See "COPYING"). If not, If not, see <http://www.gnu.org/licenses/>.         #
#											#
#---------------------------------------------------------------------------------------#
#											#
# Copyright	Joerg Anders, TU Chemnitz, Fakultaet fuer Informatik, GERMANY		#
#		ja@informatik.tu-chemnitz.de						#
#											#
#											#
#########################################################################################

LANGUAGE = en

all-local: index.html

EXTRA_DIST = index.docbook

index.html: index.docbook
	$(XMLTO) html $(srcdir)/index.docbook

install-data-local: index.docbook index.html
	$(INSTALL) -D -m "u=rw,og=r" $(srcdir)/index.docbook $(DESTDIR)$(htmldir)/$(LANGUAGE)/index.docbook
	@-for i in *.html; \
	do \
		$(INSTALL) -D -m "u=rw,og=r" $$i $(DESTDIR)$(htmldir)/$(LANGUAGE)/$$i; \
	done

uninstall-local:
	rm -f $(DESTDIR)$(htmldir)/$(LANGUAGE)/*

clean-local:
	rm -f *.html
