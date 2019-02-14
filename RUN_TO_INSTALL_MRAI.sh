#!/bin/bash
# -*- coding: utf-8 -*-
#
#  RUN_TO_INSTALL_MRAI.sh
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
if $(echo "$(lsb_release -i)" | grep -q "Solus"); then
    echo -e "\nWe're sorry. mrai does not support Solus at current time due to it using\n'eopkg' and not 'apt' for it's package manager.\n"
    exit 2
fi
if [[ "$EUID" != "0" ]]; then
    echo -e "\nPlease run RUN_TO_INSTALL_MRAI.sh with root privleges.\n"
    exit 2
fi
cache="/etc/mrai"
if [ "$1" == "remove" ]; then
    rm /bin/mrai /bin/aptupdate /bin/snapupdate /usr/share/man/man1/mrai.1.gz
    rm -rf "$cache"
    echo -e "\nmrai has been uninstalled.\n"
    exit 0
fi
x=0
while [ "$x" == "0" ]; do
  if $(ls | grep -q 'mrai') && $(ls | grep -q 'aptupdate'); then
    echo -e "\nInstalling/Updating mrai . . . \n"
    if  [ -d .git ]; then
        su $(users) -c 'git pull https://github.com/Batcastle/mrai'
    fi
    echo -e "\nUpdating the system . . .\n"
    ( apt update && apt-get -y upgrade ) || ( echo -e "\nWe encountered an error updating your system. Are you hooked up to the interent?\n" && exit 1 )
    echo -e "\nInstalling dependencies . . .\n"
    apt install git make
    echo -e "\nInstalling mrai . . .\n"
    if [[ ! -d /etc/mrai ]]; then
        mkdir /etc/mrai
    fi
    if [[ ! -d /etc/mrai/gitauto ]]; then
        mkdir /etc/mrai/gitauto
    fi
    if [[ ! -d /etc/mrai/gitman ]]; then
        mkdir /etc/mrai/gitman
    fi
    if [[ ! -d /etc/mrai/bin ]]; then
        mkdir /etc/mrai/bin
    fi
    cp mrai /bin/mrai
    cp aptupdate /bin/aptupdate
    cp snapupdate /bin/snapupdate
    cp gitautoinst.sh /etc/mrai/bin/gitautoinst.sh
    cp mrai.1.gz /usr/share/man/man1/mrai.1.gz
    cp clean.sh /etc/mrai/bin/clean.sh
    chmod +x /etc/mrai/bin/clean.sh
    chmod +x /bin/snapupdate
    chmod +x /bin/mrai
    chmod +x /bin/aptupdate
    chmod +x /etc/mrai/bin/gitautoinst.sh
    if $(dpkg -l | grep -q "ii  flatpak"); then
        echo "" >> /etc/mrai/flatpak.flag
    fi
    if $(dpkg -l | grep -q "ii  snapd"); then
        echo "" >> /etc/mrai/snapd.flag
    fi
    if $(dpkg -l | grep -q "ii  apt-fast"); then
        echo "" >> /etc/mrai/apt-fast.flag
    fi
    chown -R $(users):$(users) /etc/mrai
    if [[ -f "$cache"/wsl-yes.flag ]]; then
        sed -i 's/pkexec/sudo/g' /bin/mrai
        sed -i 's/pkexec/sudo/g' /bin/aptupdate
    elif [[ -f "$cache"/wsl-no.flag ]]; then
        continue
    else
        read -p "Are you running a Windows Subsystem for Linux Environment? (If unsure, select no) [y/N]: " ans
        if [[ "$ans" == "y" ]] || [[ "$ans" == "Y" ]]; then
            touch "$cache"/wsl-yes.flag
            sed -i 's/pkexec/sudo/g' /bin/mrai
            sed -i 's/pkexec/sudo/g' /bin/aptupdate
        else
            touch "$cache"/wsl-no.flag
        fi
    fi
    echo -e "\nRemoving old dependencies . . .\n"
    apt -y autoremove
    echo -e "\nCleaning up . . .\n"
    apt clean && apt -y purge $(dpkg -l | grep '^rc' | awk '{print $2}')
    echo -e "\nIntallation is complete!\n"
    x=1
  else
    su $(users) -c 'git clone https://github.com/Batcastle/mrai'
    cd mrai
    continue
  fi
done
