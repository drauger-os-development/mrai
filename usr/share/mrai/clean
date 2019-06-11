#!/bin/bash
# -*- coding: utf-8 -*-
#
#  clean.sh
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
#VERSION: 0.0.8-beta1
set -e
set -o pipefail
y=0
scripts="/usr/share/mrai"
called_as="$0"
R='\033[0;31m'
G='\033[0;32m'
#Y='\033[1;33m'
NC='\033[0m'
#report errors
error_report () {
	/bin/echo -e "\n$R \bERROR:$NC $2\n"
	"$scripts"/log-out.sh "$1" "/usr/share/mrai/clean.sh" "$2" "$called_as" "$(/bin/pwd)"
	if [[ "$1" != "1" ]]; then
		exit "$1"
	fi
}

while getopts 'y' flag; do
	case "${flag}" in
		y) y=1 ;;
		*) exit 1 ;;
	esac
done
if [ "$y" == "1" ]; then
	user="$2"
else
	user="$1"
fi
echo -e "\n$G \bDeleting old *.deb packages . . . $NC\n"
{
        /usr/bin/apt clean
        
} || {
        error_report "$?" "apt clean has failed. Most likly due to file system permission issues."
}
if [ "$y" == "1" ]; then
	/bin/echo -e "\n$G \bRemoving old and out dated dependencies . . . $NC\n"
	{
		/usr/bin/apt -y autoremove
	} || {
		error_report "$?" "apt autoremove has failed. Most likly due to app configuration issues."
	}
	/echo/echo -e "\n$G \bDeleting old config files . . . $NC\n" 
	{
		/usr/bin/apt -y purge $(/usr/bin/dpkg -l | /bin/grep '^rc' | /usr/bin/awk '{print $2}')
	} || {
		error_report "$?" "Config file clean up has failed. Most likely due to app config issues."
	}
elif [ "$y" == "0" ]; then
	/bin/echo -e "\n$G \bRemoving old and out dated dependencies . . . $NC\n"
	{
		/usr/bin/apt autoremove
	} || {
		error_report "$?" "apt autoremove has failed. Most likly due to app configuration issues."
	}
	/bin/echo -e "\n$G \bDeleting old config files . . . $NC\n" 
	{
		/usr/bin/apt purge $(/usr/bin/dpkg -l | /bin/grep '^rc' | /usr/bin/awk '{print $2}')
	} || {
		error_report "$?" "Config file clean up has failed. Most likely due to app config issues."
	}
else
	exit 2
fi
/bin/echo -e "\n$G \bCleaning up old GitHub files . . .$NC\n"
{
	/bin/rm -rf /home/"$user"/.mrai
	/bin/mkdir /home/"$user"/.mrai
	/bin/chown "$user:$user" /home/"$user"/.mrai
	/bin/echo -e "\n$G \bClean up complete.\n"
} || {
	error_report "2" "Github clean up failed. Most likly due to incorrect file system permissions."
}
