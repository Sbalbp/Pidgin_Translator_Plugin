#
# Pidgin Translator Plugin.
#
# Copyright (C) 2014 Sergio Balbuena <sbalbp@gmail.com>.
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License as
# published by the Free Software Foundation; either version 3 of the
# License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program. If not, see <http://www.gnu.org/licenses/>.
#

if HAVE_DOXYGEN

if HAVE_PDFLATEX

all-local:
	doxygen
	mkdir -p pdf && cd latex && make && mv refman.pdf ../pdf/Pidgin_Translator_Plugin.pdf && cd ..

clean-local:
	rm -rf $(top_srcdir)/doc/html
	rm -rf $(top_srcdir)/doc/latex
	rm -rf $(top_srcdir)/doc/pdf

else

all-local:
	doxygen

clean-local:
	rm -rf $(top_srcdir)/doc/html
	rm -rf $(top_srcdir)/doc/latex

endif

endif
