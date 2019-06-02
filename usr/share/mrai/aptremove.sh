#!/bin/bash
# -*- coding: utf-8 -*-
#
#  aptremove.sh
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
#VERSION 0.0.6-beta1
#make sure we have the right permissions, exit if we don't
#this entire script won't work anyways if we don't have the right permissions
scripts="/usr/share/mrai"
called_as="$0"
R='\033[0;31m'
G='\033[0;32m'
Y='\033[1;33m'
NC='\033[0m'
#report errors
error_report () {
	/bin/echo -e "\n$R \bERROR:$NC $2\n"
	"$scripts"/log-out.sh $1 "/usr/share/mrai/aptremove.sh" $2 "$called_as" $(/bin/pwd)
	if [[ "$1" != "1" ]]; then
		exit "$1"
	fi
}
if [[ "$EUID" != "0" ]]; then
    error_report "2" "aptremove.sh has failed with fatal error: Not running with correct permission set.\n"
fi
#remove the software
cache="/etc/mrai"
{
	if [ -f "$cache"/apt-fast.flag ]; then
		if [ "$1" == "1" ]; then
			/usr/sbin/apt-fast -y purge "$2"
		else
			/usr/sbin/apt-fast purge "$1"
		fi
	else
		if [ "$1" == "1" ]; then
			/usr/bin/apt -y purge "$2"
		else
			/usr/bin/apt purge "$1"
		fi
	fi
} || {
	error_report "$?" "apt purge has failed. Most likely a configuration bug with an app."
}
{
	#check if the software removed was apt-fast, snapd, or flatpak
	if [ "$2" == "apt-fast" ] || [ "$1" == "apt-fast" ]; then
		#remove apt-fast.flag
		/bin/rm "$cache"/apt-fast.flag
	elif [ "$2" == "snapd" ] || [ "$1" == "snapd" ]; then
		#remove snapd.flag
		/bin/rm "$cache"/snapd.flag
	elif [ "$2" == "flatpak" ] || [ "$1" == "flatpak" ]; then
		#remove flatpak.flag
		/bin/rm "$cache"/flatpak.flag
	fi
} || {
	error_report "1" "Cannot modify files in $cache"
}
#clean up after ourselves
if [ "$1" == "mrai" ] || [ "$2" == "mrai" ]; then
	exit
fi
{
	if [ "$1" == "1" ]; then
		"$scripts"/clean.sh -y $3
	else
		read -p "Would you like mrai to clean up left-over, unused dependencies? [y/N]: " ans
		if [ "$ans" == "Y" ] || [ "$ans" == "y" ]; then
			"$scripts"/clean.sh $2
		fi
	fi
} || {
	error_report "$?" "clean.sh has failed with exit code $?."
}
