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

PYV1=`python -c 'import sys; print(sys.version_info[0])'`
PYV2=`python -c 'import sys; print(sys.version_info[1])'`

if [ $PYV1 -eq 2 ]
then
	PYLIB="python2."$PYV2
elif [ $PYV1 -eq 3 ]
then
	PYLIB="python3."$PYV2"mu"
fi

sed -i "s/AM_PYV1=.*/AM_PYV1=$PYV1/g" src/Makefile.am

sed -i "s/AC_CHECK_PROGS(\[PYTHONCNF.*/AC_CHECK_PROGS([PYTHONCNF], [python$PYV1-config])/g" configure.ac
sed -i "s/AC_CHECK_LIB(python.*/AC_CHECK_LIB($PYLIB,main,,AC_MSG_ERROR(Cannot find required library $PYLIB.))/g" configure.ac

autoreconf -fi && ./configure
