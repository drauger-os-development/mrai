#!/bin/bash
if [[ "$EUID" != "0" ]]; then
    echo -e "\nPlease run RUN_TO_INSTALL_MRAI.sh with root privleges.\n" && exit 2
fi
if [ "$1" == "remove" ]; then
    rm /bin/mrai /bin/aptupdate
    rm -rf /etc/mrai
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
    ( apt-get update && apt-get -y upgrade ) || ( echo -e "\nWe encountered an error updating your system. Are you hooked up to the interent?\n" && exit 1 )
    echo -e "\nInstalling dependencies . . .\n"
    apt-get install checkinstall git make
    echo -e "\nInstalling mrai . . .\n"
    mkdir /etc/mrai
    mkdir /etc/mrai/gitauto
    mkdir /etc/mrai/gitman
    mkdir /etc/mrai/bin
    cp mrai /bin/mrai
    cp aptupdate /bin/aptupdate
    cp snapupdate /bin/snapupdate
    cp gitautoinst.sh /etc/mrai/bin/gitautoinst.sh
    cp mrai.1.gz /usr/share/man/man1/mrai.1.gz
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
    echo -e "\nRemoving old dependencies . . .\n"
    apt-get -y autoremove
    echo -e "\nCleaning up . . .\n"
    apt-get clean && apt-get -y purge $(dpkg -l | grep '^rc' | awk '{print $2}')
    echo -e "\nIntallation is complete!\n"
    x=1
  else
    su $(users) -c 'git clone https://github.com/Batcastle/mrai'
    cd mrai
    continue
  fi
done
