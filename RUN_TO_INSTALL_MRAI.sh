#!/bin/bash
if [[ "$EUID" != "0" ]]; then
    echo -e "\nPlease run RUN_TO_INSTALL_MRAI.sh with root privleges.\n" && exit 2
fi
x=0
while ["$x" == "0" ]; do
  if $(ls | grep -q 'mrai') && $(ls | grep -q 'aptupdate'); then
    echo -e "\nInstalling/Updating mrai . . . \n"
    if  [ -d .git ]; then
      git pull https://github.com/Batcastle/mrai
    fi
    cp mrai /bin/mrai
    cp aptupdate /bin/aptupdate
    chmod +x /bin/mrai
    chmod +x /bin/aptupdate
    mkdir /etc/mrai
    mkdir /etc/mrai/gitauto
    mkdir /etc/mrai/gitman
    x=1
  else
    git pull https://github.com/Batcastle/mrai
    cd mrai
    continue
  fi
done
    
  
