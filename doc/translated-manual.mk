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

EXTRA_DIST = $(LANGUAGE).po index.docbook

if MAKE_DOCS

if MAKE_DOCBOOKS

all-local: index.docbook index.html

index.docbook: ../en/index.docbook $(LANGUAGE).po
	$(PO2XML) $(srcdir)/../en/index.docbook $(srcdir)/$(LANGUAGE).po >index.docbook

index.html: index.docbook
	$(XMLTO) html index.docbook

../templates/nted.pot:
	cd ../templates && $(MAKE) nted.pot

$(srcdir)/$(LANGUAGE).po: ../templates/nted.pot ../en/index.docbook
	$(MSGMERGE) $(srcdir)/$(LANGUAGE).po ../templates/nted.pot > xx.po
	@if cmp xx.po $(srcdir)/$(LANGUAGE).po > /dev/null; then \
		rm -f xx.po; \
	else \
		rm -f $(srcdir)/$(LANGUAGE).po; \
		echo mv -f xx.po $(srcdir)/$(LANGUAGE).po; \
		mv -f xx.po $(srcdir)/$(LANGUAGE).po; \
	fi

install-data-local: index.docbook index.html
	$(INSTALL) -D -m "u=rw,og=r" index.docbook $(DESTDIR)$(htmldir)/$(LANGUAGE)/index.docbook
	@-for i in *.html; \
	do \
		$(INSTALL) -D -m "u=rw,og=r" $$i $(DESTDIR)$(htmldir)/$(LANGUAGE)/$$i; \
	done

uninstall-local:
	rm -f $(DESTDIR)$(htmldir)/$(LANGUAGE)/*

clean-local:
	rm -f *.html index.docbook

else 

all-local: index.html

index.html:
	$(XMLTO) html index.docbook

$(srcdir)/$(LANGUAGE).po: ../templates/nted.pot ../en/index.docbook
	$(MSGMERGE) $(srcdir)/$(LANGUAGE).po ../templates/nted.pot > xx.po
	@if cmp xx.po $(srcdir)/$(LANGUAGE).po > /dev/null; then \
		rm -f xx.po; \
	else \
		rm -f $(srcdir)/$(LANGUAGE).po; \
		echo mv -f xx.po $(srcdir)/$(LANGUAGE).po; \
		mv -f xx.po $(srcdir)/$(LANGUAGE).po; \
	fi

install-data-local: index.docbook index.html
	$(INSTALL) -D -m "u=rw,og=r" index.docbook $(DESTDIR)$(htmldir)/$(LANGUAGE)/index.docbook
	@-for i in *.html; \
	do \
		$(INSTALL) -D -m "u=rw,og=r" $$i $(DESTDIR)$(htmldir)/$(LANGUAGE)/$$i; \
	done

uninstall-local:
	rm -f $(DESTDIR)$(htmldir)/$(LANGUAGE)/*

clean-local:
	rm -f *.html

endif

endif