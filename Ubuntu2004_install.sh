#!/bin/bash
# Script to automated install process for install on ubuntu 20.40
# KB3GTN

echo "#######################################################################"
echo "# BladeRF build / install / setup automation script                   #"
echo "#                                                                     #"
echo "#  * This script will make a directory called ~/bladeRF_Install       #"
echo "#  * Apt install build depencencies for bladeRF build..               #"
echo "#  * Will clone bladeRF repo into this directory                      #"
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
