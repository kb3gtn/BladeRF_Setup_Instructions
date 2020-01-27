![NUAND LOGO](http://nuand.com/bladeRF-doc/libbladeRF/v2.2.1/logo.png)

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

> $ cd /home/user/sandbox    
> $ git clone https://github.com/Nuand/bladeRF  

This will create a new directory called bladeRF will all the files you will need.   

## Compile sources
The bladeRF software uses cmake to manage the compile and install activities.   

> $ cd bladeRF/host/  
> $ mkdir build   
> $ cd build   
> $ cmake ../ -CMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/opt/bladeRF -DINSTALL_UDEV_RULES=ON -DBLADERF_GROUP=bladerf   
> $ make -j4   
> $ sudo mkdir /opt/bladeRF   
> $ sudo make install  
> $ sudo chown \<user\> /opt/bladeRF -R (optional step)  
 
## create bladerf group
create a bladerf group and add users to it so they can access the bladeRF.

> $ groupadd bladerf
> $ usermod -a -G bladerf username
  
## Add bladeRF to system path
create a new file in /etc/profile.d/bladeRF_paths.sh
> $ sudo vim /etc/profile.d/bladeRF_paths.sh

Put the following into the file.
> $ export PATH=$PATH:/opt/bladeRF/bin  
> $ export BLADERF_SEARCH_DIR=/opt/bladeRF/etc   
> $ export PKG_CONFIG_PATH=$PKG_CONFIG_PATH:/opt/bladeRF/lib/pkgconfig   


## add bladeRF libraries to library search path
create/edit /etc/ld.so.conf.d/bladeRF.conf  
> $ sudo vim /etc/ld.so.conf.d/bladeRF.conf   

And insert the following text:  
> /opt/bladeRF/lib   

## BladeRF firmware auto-loading
When the bladeRF host software is started it will load a FPGA image into the bladeRF to initialize it if it is not already initialized.  The bladeRF.conf file contains bladeRF-CLI commands.

make a directory to store these images.  I use /opt/bladeRF/etc.
> mkdir /opt/bladeRF/etc   
> cd /opt/bladeRF/etc   

The fpga and fx3 firmware images can be downloaded at: https://www.nuand.com/fpga_images/  
Download the hosted hostedxA4-latest.rbf (hostedxA9-latest.rbf for the bigger FPGA version)

> wget "https://www.nuand.com/fpga/hostedxA4-latest.rbf"   
> mv hostedxA4-latest.rbf hostedxA4.rbf   
> wget "https://www.nuand.com/fx3/bladeRF_fw_latest.img"   
> mv bladeRF_fw_lastest.img bladeRF.img   

## test it out
Before things will work, you will probably need to logout and log back in.   
when you log back in, connect up the BladeRF and try running "bladeRF-CLI -i".   
It should load an FPGA image and give you a prompt.
At the prompt type 'print' to get the SDR's configuration.


End.





