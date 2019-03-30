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
#VERSION 0.0.5-alpha1
set -e
set -o pipefail
file="$1"
usr=$(/usr/bin/users)
cache="/etc/mrai"
int=0
while [ "$int" == "0" ]; do
    if [ -f /home/$usr/.selected_editor ] || [ -f $cache/selected_editor.conf ]; then
        if [ ! -f /home/$usr/.selected_editor ] && [ -f $cache/selected_editor.conf ]; then
            editor=$(/bin/grep $cache/selected_editor.conf | /bin/sed 's/SELECTED_EDITOR=//g')
            #eval $cache/selected_editor.conf
            int="1"
        elif [ -f /home/$usr/.selected_editor ] && [ ! -f $cache/selected_editor.conf ]; then
            editor=$(/bin/grep /home/$usr/.selected_editor | /bin/sed 's/SELECTED_EDITOR=//g')
            #eval $cache/selected_editor.conf
            /bin/cp /home/$usr/.selected_editor $cache/selected_editor.conf
            /bin/chown "$usr":"$usr" $cache/selected_editor.conf
            int="1"
        elif [ -f /home/$usr/.selected_editor ] && [ -f $cache/selected_editor.conf ]; then
            editor=$(/bin/grep $cache/selected_editor.conf | /bin/sed 's/SELECTED_EDITOR=//g')
            int="1"
        else
            exit 2
        fi
    else
        /usr/bin/select-editor
    fi
done
{
    /bin/cp $file "$file.back_up_mrai"
    $editor $file
} && {
    /bin/echo -e "\nUpdating Apt Repository Cache . . .\n"
    /usr/bin/apt update
} && {
    /bin/echo -e "\nConfiguring Apt Repos Complete!\n"
}