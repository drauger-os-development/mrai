#!/bin/bash
# -*- coding: utf-8 -*-
#
#  aptremove.sh
#  
#  Copyright 2019 Thomas Castleman <contact@draugeros.ml>
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
#VERSION 0.0.3-alpha8
#make sure we have the right permissions, exit if we don't
#it won't work anyways if we don't have the right permissions
if [[ "$EUID" != "0" ]]; then
    /bin/echo -e "\naptremove.sh has failed with fatal error: Not running with correct permission set.\nPlease file a bug report at https://github.com/drauger-os-development/mrai/issues\n" && exit 2
fi
#remove the software
cache="/etc/mrai"
if [ -f "$cache"/apt-fast.flag ]; then
	if [ "$1" == "1" ]; then
		{
			/usr/sbin/apt-fast -y purge "$2"
		} && {
			/usr/sbin/apt-fast -y autoremove
		}
	else
		{ 
			/usr/sbin/apt-fast purge "$2"
		} && {
			read -p "Would you like mrai to clean up left-over, unused dependencies? [y/N]: " ans
			if [ "$ans" == "Y" ] || [ "$ans" == "y" ]; then
				/usr/sbin/apt-fast autoremove
			fi
		}
	fi
else
	if [ "$1" == "1" ]; then
		{
			/usr/bin/apt -y purge "$2"
		} && {
			/usr/bin/apt -y autoremove
		}
	else
		{
			/usr/bin/apt purge "$2"
		} && (
			read -p "Would you like mrai to clean up left-over, unused dependencies? [y/N]: " ans
			if [ "$ans" == "Y" ] || [ "$ans" == "y" ]; then
				/usr/bin/apt autoremove
			fi
		)
	fi
fi
#check if the software removed was apt-fast, snapd, or flatpak
if [ "$2" == "apt-fast" ]; then
	#remove apt-fast.flag
	/bin/rm "$cache"/apt-fast.flag
elif [ "$2" == "snapd" ]; then
	#remove snapd.flag
	/bin/rm "$cache"/snapd.flag
elif [ "$2" == "flatpak" ]; then
	#remove flatpak.flag
	/bin/rm "$cache"/flatpak.flag
fi
#clean up after ourselves
{
	if [ "$1" == "1" ]; then
		"$cache"/bin/clean.sh -y $3
	else
		"$cache"/bin/clean.sh $3
	fi
} || {
	/bin/echo -e "\nWe're sorry. clean.sh has failed with exit code $?. Please fill out an issue report at\nhttps://github.com/drauger-os-development/mrai/issues\n"
	exit $?
}
