# Nuand BladeRF2 Setup Instructions for Ubuntu 20.04 

## Description of setup
These instructions assume ubuntu 20.04 (but will work on others distros) and will walk through how to install the bladeRF host software confined to /opt/bladeRf with user group bladerf allowed to access the devices with out root permissions.

## Install the dependencies
Ubuntu packages to install as prerequisites:   
> sudo apt install python3-six python-six python-mako python3-lxml python-lxml python-numpy python3-numpy libusb-1.0-0-dev libusb-1.0-0 build-essential cmake libncurses5-dev libtecla1 libtecla-dev pkg-config git wget

## Get bladeRF sources
pull the bladeRF sources to build host libraries and applications.     
I do this in a directory call sandbox I create in my home folder to doing builds.     
Do a git checkout of the bladeRF repository into your "sandbox" directory.     

>cd /home/user/sandbox    
>git clone https://github.com/Nuand/bladeRF  

This will create a new directory called bladeRF will all the files you will need.   

## Compile sources
The bladeRF software uses cmake to manage the compile and install activities.   

> cd bladeRF/host/  
> mkdir build   
> cd build   
> cmake ../ -CMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/opt/bladeRF -DINSTALL_UDEV_RULES=ON -DBLADERF_GROUP=bladerf   
> make -j4   
> sudo mkdir /opt/bladeRF   
> sudo chown <user> /opt/bladeRF  (optional step)   
  
## Add bladeRF to system path
create a new file in /etc/profile.d/bladeRF_paths.sh

> sudo vim /etc/profile.d/bladeRF_paths.sh

Put the following into the file.
> export PATH=$PATH:/opt/bladeRF/bin  
> export BLADERF_SEARCH_DIR=/opt/bladeRF/etc

