#!/bin/bash
# -*- coding: utf-8 -*-
#
#  gitautoinst.sh
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
#VERSION 0.0.5-beta1
home="$1"
user="$2"
pass="$3"
gitautocache="/etc/mrai/gitauto"
scripts="/usr/share/mrai"
called_as="$0"
R='\033[0;31m'
NC='\033[0m'
#report errors
error_report () {
	/bin/echo -e "\n$R \bERROR:$NC $2\n"
	"$scripts"/log-out.sh "$1" "/usr/share/mrai/gitautoinst.sh" "$2" "$called_as" "$(/bin/pwd)"
	if [[ "$1" != "1" ]]; then
		exit "$1"
	fi
}

cd "$home" || exit 1
#make directory .mrai if it exists
if [[ ! -d .mrai ]]; then
	/bin/su "$user" -c 'mkdir .mrai'
fi
cd .mrai || exit 1 
#check to see how the URL to github is formatted
#check for a VERY specific location
{
	if $(/bin/echo "$pass" | /bin/grep -q 'https://github.com/') || $(/bin/echo "$pass" | /bin/grep -q 'http://github.com/'); then
		/bin/su "$user" -c "/usr/bin/git clone $pass"
	#check for a slightly less specific location
	elif $(/bin/echo "$pass" | /bin/grep -q 'github.com/'); then
		/bin/su "$user" -c "/usr/bin/git clone https://$pass"
	#if they did not specifiy "GitHub.com", assume it.
	else 
		/bin/su "$user" -c "/usr/bin/git clone https://github.com/$pass"
	fi
} || {
	error_report "$?" "git clone failed. Either because the provided URL is not GitHub, or because the cloning is being done as root, or there is no internet."
}
#cd into the new directory git just make from the GitHub repo
pass1=$(/bin/echo "$pass" | /bin/sed 's/.*\///')
cd "$pass1" || exit 1
#Check for a Makefile
if [ -f Makefile ] || [ -f makefile ] || [ -f MAKEFILE ]; then
	#ask if we need to run a specific file in order to get set up
	/bin/echo -e "\nA Makefile has been detected. Some apps require you to run a seperate file before the Makefile."
	read -pr "Would you like to run such a file? [y/N]: " ans
	if [ "$ans" == "Y" ] || [ "$ans" == "y" ]; then
		#show a list of files for the user to chose from
		/bin/ls --color=auto
		read -pr "Which of the above files would you like to run? (Press enter with no input to cancel and continue with running the Makefile. Type 'exit' to exit.): " ans
		#check to see if $ans is set and NOT to exit, if it is it should be set to a file name
		if [[ ! -z $ans ]] && [ "$ans" != "exit" ]; then
			read -pr "Would you like to run this file as root? [y/N]: " ans1
			#make the designated file executable and run it
			/bin/chmod +x "$ans"
			if [ "$ans1" == "Y" ] || [ "$ans1" == "y" ]; then
				./"$ans" || error_report "2" "$ans for GitHub app $pass1 has failed. Please contact the developer."
			else
				/bin/su "$user" -c "./$ans" || error_report "2" "$ans for GitHub app $pass1 has failed. Please contact the developer."
			fi
		#check to see if $ans is set to exit
		elif [ "$ans" == "exit" ]; then
			#exit
			/bin/echo -e "\n$R \bAborting . . .\n"
			exit 1
		fi
	fi
	#if a Makefile exists, run make depend to ensure we have all the dependencies we need
	/usr/bin/make depend 2>/dev/null || error_report "1" "make depend not recognized inside Makefile. Continuing . . ."
	#run make && make install to install the new software, if make fails don't bother running make install since
	#it would most likely fail
	/usr/bin/make || error_report "2" "Make has failed. Please ensure all dependencies are satisfied."
	/usr/bin/make install || error_report "2" "Make has failed. Please ensure all dependencies are satisfied."
	#Back up the Makefile since it might be useful in uninstalling the software
	{
		/bin/su "$user" -c "( /bin/mkdir $gitautocache/$pass1 && /bin/cp Makefile $gitautocache/$pass1 ) || ( cd .. && /bin/rm -rf $pass1 && exit 1 ) || exit 2"
		/bin/echo "ADDRESS=$pass" >> "$gitautocache"/"$pass1"/auto.flag
	} || {
		error_report "2" "Could not modify Makefile. Incorrect file system permissions."
	}
	#copy Makefile to a directory in a good place in the file system. Name the directory it's in the value of $z
else
	#remove the folder to save space
	cd ..
	/bin/rm -rf "$pass1" || error_report "2" "Could not remove directory. Incorrect file system permissions."
fi