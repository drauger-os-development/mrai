#!/bin/bash
# -*- coding: utf-8 -*-
#
#  clean.sh
#  
#  Copyright 2018 Thomas Castleman <draugeros@gmail.com>
#  
#  This program is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation; either version 2 of the License, or
#  (at your option) any later version.
#  
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#  
#  You should have received a copy of the GNU General Public License
#  along with this program; if not, write to the Free Software
#  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
#  MA 02110-1301, USA.
#
#VERSION: 0.0.1
set -e
set -o pipefail
user="$1"
echo -e "\nDeleting old *.deb packages . . . \n"
apt clean
echo -e "\nDeleting old config files . . . \n"
apt purge $(dpkg -l | grep '^rc' | awk '{print $2}') 
echo -e "\nCleaning up old GitHub files . . .\n"
rm -rf /home/$user/.mrai
mkdir /home/$user/.mrai
chown $user:$user /home/$user/.mrai
echo -e "\nClean up complete.\n"
