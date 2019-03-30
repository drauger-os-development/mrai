#!/bin/bash
# -*- coding: utf-8 -*-
#
#  edit-apt-sources.sh
#  
#  Copyright 2019 Thomas Castleman <contact@draugeros.org>
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
#VERSION 0.1.3-alpha2
set -e
set -o pipefail
file="$1"
usr=$(/usr/bin/users)
cache="/etc/mrai"
int=0
while [ "$int" == "0" ]; do
    if [ -f /home/$usr/.selected_editor ] || [ -f $cache/selected_editor.conf ]; then
        if [ ! -f /home/$usr/.selected_editor ] && [ -f $cache/selected_editor.conf ]; then
            editor=$(cat $cache/selected_editor.conf | /bin/grep 'SELECTED_EDITOR=' | /bin/sed 's/SELECTED_EDITOR=//g')
            int="1"
        elif [ -f /home/$usr/.selected_editor ] && [ ! -f $cache/selected_editor.conf ]; then
            editor=$(cat /home/$usr/.selected_editor | /bin/grep 'SELECTED_EDITOR=' | /bin/sed 's/SELECTED_EDITOR=//g')
            /bin/cp /home/$usr/.selected_editor $cache/selected_editor.conf
            /bin/chown "$usr":"$usr" $cache/selected_editor.conf
            int="1"
        elif [ -f /home/$usr/.selected_editor ] && [ -f $cache/selected_editor.conf ]; then
            editor=$(cat $cache/selected_editor.conf | /bin/grep 'SELECTED_EDITOR=' | /bin/sed 's/SELECTED_EDITOR=//g')
            int="1"
        else
            exit 2
        fi
    else
        /usr/bin/select-editor
    fi
done
{
    /bin/cp "$file" "$file.bak"
    eval "$editor $1"
} && {
    /bin/echo -e "\nUpdating Apt Repository Cache . . .\n"
    /usr/bin/apt update
} && {
    /bin/echo -e "\nConfiguring Apt Repos Complete!\n"
}
