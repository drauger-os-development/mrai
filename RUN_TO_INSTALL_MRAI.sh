#!/bin/bash
# -*- coding: utf-8 -*-
#
#  RUN_TO_INSTALL_MRAI.sh
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
if $(/bin/echo "$(/usr/bin/lsb_release -i)" | /bin/grep -q "Solus"); then
    /bin/echo -e "\nWe're sorry. mrai does not support Solus at current time due to it using\n'eopkg' and not 'apt' for it's package manager.\n"
    exit 2
fi
if [[ "$EUID" != "0" ]]; then
    /bin/echo -e "\nPlease run RUN_TO_INSTALL_MRAI.sh with root privleges.\n"
    exit 2
fi
cache="/etc/mrai"
if [ "$1" == "remove" ]; then
    /bin/rm /bin/mrai /bin/aptupdate /bin/snapupdate /usr/share/man/man1/mrai.1.gz
    /bin/rm -rf "$cache"
    /bin/echo -e "\nmrai has been uninstalled.\n"
    exit 0
fi
x=0
while [ "$x" == "0" ]; do
  if $(/bin/ls | /bin/grep -q 'mrai') && $(/bin/ls | /bin/grep -q 'aptupdate'); then
    /bin/echo -e "\nInstalling/Updating mrai . . . \n"
    if  [ -d .git ]; then
        /bin/su $(/usr/bin/users) -c '/usr/bin/git pull https://github.com/Batcastle/mrai'
    fi
    /bin/echo -e "\nUpdating the system . . .\n"
    ( /usr/bin/apt update && /usr/bin/apt-get -y upgrade ) || ( /bin/echo -e "\nWe encountered an error updating your system. Are you hooked up to the interent?\n" && exit 1 )
    /bin/echo -e "\nInstalling dependencies . . .\n"
    /usr/bin/apt install git make
    /bin/echo -e "\nInstalling mrai . . .\n"
    if [[ ! -d /etc/mrai ]]; then
        /bin/mkdir /etc/mrai
    fi
    if [[ ! -d /etc/mrai/gitauto ]]; then
        /bin/mkdir /etc/mrai/gitauto
    fi
    if [[ ! -d /etc/mrai/gitman ]]; then
        /bin/mkdir /etc/mrai/gitman
    fi
    if [[ ! -d /etc/mrai/bin ]]; then
        /bin/mkdir /etc/mrai/bin
    fi
    /bin/cp mrai /bin/mrai
    /bin/cp aptupdate /bin/aptupdate
    /bin/cp snapupdate /bin/snapupdate
    /bin/cp gitautoinst.sh /etc/mrai/bin/gitautoinst.sh
    /bin/cp mrai.1.gz /usr/share/man/man1/mrai.1.gz
    /bin/cp clean.sh /etc/mrai/bin/clean.sh
    /bin/chmod +x /etc/mrai/bin/clean.sh
    /bin/chmod +x /bin/snapupdate
    /bin/chmod +x /bin/mrai
    /bin/chmod +x /bin/aptupdate
    /bin/chmod +x /etc/mrai/bin/gitautoinst.sh
    if $(/usr/bin/dpkg -l | /bin/grep -q "ii  flatpak"); then
        /bin/echo "" >> /etc/mrai/flatpak.flag
    fi
    if $(/usr/bin/dpkg -l | /bin/grep -q "ii  snapd"); then
        /bin/echo "" >> /etc/mrai/snapd.flag
    fi
    if $(/usr/bin/dpkg -l | /bin/grep -q "ii  /usr/bin/apt-fast"); then
        /bin/echo "" >> /etc/mrai//usr/bin/apt-fast.flag
    fi
    /bin/chown -R $(/usr/bin/users):$(/usr/bin/users) /etc/mrai
    if [[ -f "$cache"/wsl-yes.flag ]]; then
        /bin/sed -i 's/pkexec/sudo/g' /bin/mrai
        /bin/sed -i 's/pkexec/sudo/g' /bin/aptupdate
    elif [[ -f "$cache"/wsl-no.flag ]]; then
        continue
    else
        read -p "Are you running a Windows Subsystem for Linux Environment? (If unsure, select no) [y/N]: " ans
        if [[ "$ans" == "y" ]] || [[ "$ans" == "Y" ]]; then
            touch "$cache"/wsl-yes.flag
            /bin/sed -i 's/pkexec/sudo/g' /bin/mrai
            /bin/sed -i 's/pkexec/sudo/g' /bin/aptupdate
        else
            touch "$cache"/wsl-no.flag
        fi
    fi
    /bin/echo -e "\nRemoving old dependencies . . .\n"
    /usr/bin/apt -y autoremove
    /bin/echo -e "\nCleaning up . . .\n"
    /usr/bin/apt clean && /usr/bin/apt -y purge $(/usr/bin/dpkg -l | /bin/grep '^rc' | /usr/bin/awk '{print $2}')
    /bin/echo -e "\nIntallation is complete!\n"
    x=1
  else
    /bin/su $(/usr/bin/users) -c '/usr/bin/git clone https://github.com/Batcastle/mrai'
    cd mrai
    continue
  fi
done
