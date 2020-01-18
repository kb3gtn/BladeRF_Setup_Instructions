# Nuand BladeRF2 Setup Instructions for Ubuntu 20.04 

## Description of setup
These instructions assume ubuntu 20.04 (but will work on others distros) and will walk through how to install the bladeRF host software confined to /opt/bladeRf with user group bladerf allowed to access the devices with out root permissions.

## Install the dependencies
Ubuntu packages to install as prerequisites:
> sudo apt install python3-six python-six python-mako python3-lxml python-lxml python-numpy python3-numpy libusb-1.0-0-dev libusb-1.0-0 build-essential cmake libncurses5-dev libtecla1 libtecla-dev pkg-config git wget

## Get bladeRF sources
next we need to pull the bladeRF sources to build.
I do this in a directory call sandbox I create in my home folder to doing builds.
Then do a git checkout of the bladeRF repository.

>cd /home/user/sandbox   
>git clone https://github.com/Nuand/bladeRF

https://github.com/Nuand/bladeRFhttps://github.com/Nuand/bladeRF
