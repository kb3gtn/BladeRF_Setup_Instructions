#!/bin/bash
# Script to automated install process for install on ubuntu 20.40
# KB3GTN

# cause bash script to exit on error response code from a command.
set -e

echo "#######################################################################"
echo "# BladeRF build / install / setup automation script                   #"
echo "#                                                                     #"
echo "#  * This script will make a directory called ~/bladeRF_Install       #"
echo "#  * Apt install build depencencies for bladeRF build..               #"
echo "#  * Will clone bladeRF repo into this directory                      #"
echo "#  * perform build/install of bladeRF host software                   #"
echo "#  * create /etc/profile.d/bladeRF_paths.sh                           #"
echo "#  * create /etc/ld.so.conf.d/bladeRF.conf                            #"
echo "#  * pull latest firmware from nuand's website into autoload path     #"
echo "#  * create a group called bladeRF                                    #"
echo "#  * install bladeRF udev rules and restart udevadmin                 #"
echo "#                                                                     #"
echo "#######################################################################"
read 'Run this script (Y/n)' $runproc

# if not Yes, then exit script
if [[ ! $runproc =~ ^[Yy]$ ]]
then
    echo "Exit..."
    exit 1
fi

# Run install process
echo "* Making bladeRF build directory.
mkdir ~/bladeRF_Install
echo "* Clone bladeRF repository"
git clone https://github.com/Nuand/bladeRF
echo "* Apt install needed dependencies.."
sudo apt install python3-six python-six python-mako python3-lxml python-lxml python-numpy python3-numpy libusb-1.0-0-dev libusb-1.0-0 build-essential cmake libncurses5-dev libtecla1 libtecla-dev pkg-config git wget
echo "* Starting Build for BladeRF host software.."
cd bladeRF
mkdir build
cd build
cmake ../ -CMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/opt/bladeRF -DINSTALL_UDEV_RULES=ON -DBLADERF_GROUP=bladerf
make -j4
echo "* Creating bladeRF install directories and perform installization of bladeRF software"
sudo mkdir -p /opt/bladeRF/etc
sudo make install
echo "* Create bladeRF_path.sh in /etc/profile.d/bladeRF.conf"



