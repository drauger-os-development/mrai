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
#VERSION: 0.0.4-alpha4
set -e
set -o pipefail
y=0
#while getopts 'y' flag; do
# case "${flag}" in
#   y) y=1
#   *) exit 1
# esac
#done
#if [ "$y" == "1" ]; then
# user="$2"
#else
user="$1"
#fi
echo -e "\nDeleting old *.deb packages . . . \n"
apt clean
echo -e "\nDeleting old config files . . . \n"
#if [ "$y" == "1" ]; then
# apt -y purge $(dpkg -l | grep '^rc' | awk '{print $2}')
#elif [ "$y" == "0" ]; then
apt purge $(dpkg -l | grep '^rc' | awk '{print $2}')
#else
# exit 2
#fi
echo -e "\nCleaning up old GitHub files . . .\n"
rm -rf /home/$user/.mrai
mkdir /home/$user/.mrai
chown $user:$user /home/$user/.mrai
echo -e "\nClean up complete.\n"
