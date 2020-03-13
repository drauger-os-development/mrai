## **mrai** ##
#### By: Thomas Castleman 
#### Lead Dev, Drauger OS
#### <contact@draugeros.org>
#### https://draugeros.org
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
As of yet, this is still pre-production software. There are definitely bugs that need to be worked out. `mrai` does work, but please use it with caution.
If you want a more up-to-date version, consider looking at the `dev-branch` branch. 

# Installation
Right now, the preferred method of installation is to either install `mrai` from this GitHub repo so you can have the latest, most up-to-date version, or install it from the \*.deb package in the releases page so that apt can uninstall it later. 

If installing it from the GitHub repo, we suggest running:

`git clone https://github.com/Batcastle/mrai`

`cd mrai`

`./build.sh`

Set the Architecture field in `DEBIAN/control` to build a *.deb package in that CPU architecture. Currently, `amd64` and `arm64` are supported.

This will generate a \*.deb file in the parent directory which you can then install.

Drauger OS users can install it directly from the repos by running `sudo apt install mrai`

# Uninstalling
To uninstall, run:

    `sudo apt purge mrai`

Or, you can use the stones to destroy the stones:
    `mrai -r mrai`

# Usage
A near-full set of usage instructions can be obtained with:
    `mrai -h`
or
    `mrai --help`
