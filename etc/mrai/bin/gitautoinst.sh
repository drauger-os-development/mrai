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
#VERSION 0.0.3
home="$1"
user="$2"
pass="$3"
cache="/etc/mrai"
gitautocache="/etc/mrai/gitauto"
cd $home
#make directory .mrai if it exists
if [[ ! -d .mrai ]]; then
	su $user -c 'mkdir .mrai'
fi
cd .mrai
#check to see how the URL to github is formatted
#check for a VERY specific location
if $(echo "$pass" | grep -q 'https://github.com/') || $(echo "$pass" | grep -q 'http://github.com/'); then
        su $user -c "git clone $pass"
#check for a slightly less specific location
elif $(echo "$pass" | grep -q 'github.com/'); then
        su $user -c "git clone https://$pass"
#if they did not specifiy "GitHub.com", assume it.
else 
        su $user -c "git clone https://github.com/$pass"
fi
#cd into the new directory git just make from the GitHub repo
pass1=$(echo $pass | sed 's/.*\///')
cd $pass1
#Check for a Makefile
if [ -f Makefile ] || [ -f makefile ]; then
	#ask if we need to run a specific file in order to get set up
	echo -e "\nA Makefile has been detected. Some apps require you to run a seperate file before the Makefile."
	read -p "Would you like to run such a file? [y/N]: " ans
	if [ "$ans" == "Y" ] || [ "$ans" == "y" ]; then
		#show a list of files for the user to chose from
		ls --color=auto
		read -p "Which of the above files would you like to run? (Press enter to cancel and continue with running the Makefile. Type 'exit' to exit.): " ans		#check to see if $ans is set and NOT to exit, if it is it should be set to a file name
		if [[ ! -z $ans ]] && [ "$ans" != "exit" ]; then
			read -p "Would you like to run this file as root? [y/N]: " ans1
			#make the designated file executable and run it
			chmod +x "$ans"
			if [ "$ans1" == "Y" ] || [ "$ans1" == "y" ]; then
				./$ans
			else
				su $user -c "./$ans"
			fi
		#check to see if $ans is set to exit
		elif [ "$ans" == "exit" ]; then
			#exit
			echo -e "\nAborting . . .\n"
			exit 1
		fi
	fi
	#if a Makefile exists, run make depend to ensure we have all the dependencies we need
	make depend
	#run make && make install to install the new software, if make fails don't bother running make install since
	#it would most likely fail
        make && make install
	#Back up the Makefile since it might be useful in uninstalling the software
	su $user -c "( mkdir $gitautocache/$pass1 && cp Makefile $gitautocache/$pass1 ) || ( cd .. && rm -rf $pass1 && exit 1 ) || exit 2"
	echo "ADDRESS=$pass" >> "$gitautocache"/"$pass1"/auto.flag
       #copy Makefile to a directory in a good place in the file system. Name the directory it's in the value of $z
else
        #remove the folder to save space
	cd ..
        rm -rf $pass1
fi
