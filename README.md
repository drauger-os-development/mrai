## **mrai** ##
#### By: Thomas Castleman 
#### Lead Dev, Drauger OS
#### <draugeros@gmail.com>
#### https://draugeros.ml
---
mrai (meer-eye) 

mrai: Multiple Repository App Installer
---

`mrai` is an AUR-helper-like script for Debian-based Linux Operating Systems. `mrai` works with apt, snaps, flatpaks, and can install apps from GitHub.

# Dependencies
  `apt`
  
  `git`
  
  `make`
  
# Recommends
  `snapd`
  
  `flatpak`
  
# Suggests
  `apt-fast`
  
  
# Notes
As of yet, this is still pre-production software. There are definitly bugs that need to be worked out. `mrai` does work, but please use it with caution.

# Installation
Right now, the prefered method of installtion is to either install `mrai` from this GitHub repo so you can have the latest, most up-to-date version, or install it from the \*.deb package in the releases page so that apt can uninstall it later. `mrai` currently is not on any apt repo but is planned to have a PPA once a production ready release is reached.

If installing it from the GitHub repo, we suggest running:
`git clone https://github.com/Batcastle/mrai`
`cd mrai`
`sudo ./RUN_TO_INSTALL_MRAI.sh`

If installing the \*.deb, after downloading it:
`sudo apt install /path/to/deb/file/mrai_x.x.x_all.deb`

# Uninstalling
To uninstall, run the following code:

  If installed from GitHub repo:
  `sudo rm /bin/mrai;
  sudo rm /bin/aptupdate;
  sudo rm /bin/snapupdate;
  sudo rm /usr/share/man/man1/mrai.1.gz;
  sudo rm -rf /etc/mrai`
  
  or run
  `sudo ./RUN_TO_INSTALL_MRAI.sh remove`
  
  If installed from the .deb package:
  `sudo apt purge mrai`
