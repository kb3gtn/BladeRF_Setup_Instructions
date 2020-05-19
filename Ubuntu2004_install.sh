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
echo " This script requires to be run as root since "
echo " we need to modify system files."
echo ""
read -n1 -p "Run this script (Y/n): " runproc
echo

# if not Yes, then exit script
if [[ ! $runproc =~ ^[Yy]$ ]]
then
    echo "Exit..."
    exit 1
fi

# check to see if root
if [[ $EUID -ne 0 ]]; then
    echo "This script requires to be run as root.."
    exit 1
fi


echo "Installer Starting.."

###############################
# Run install process
###############################

# Build libbladeRF and install it
echo "* Making bladeRF build directory."
if [ -d "bladeRF_Install" ]; then
  echo "Cleaning up old install space.."
  rm bladeRF_Install -rf
fi
mkdir -p bladeRF_Install
cd bladeRF_Install
mkdir -p /opt/bladeRF/lib
echo "* Clone bladeRF repository"
git clone https://github.com/Nuand/bladeRF
echo "* Apt install needed dependencies.."
 apt install python3-six python-six python-mako python3-lxml python-lxml python-numpy python3-numpy libusb-1.0-0-dev libusb-1.0-0 build-essential cmake libncurses5-dev libtecla1 libtecla-dev pkg-config git wget
echo "* Starting Build for BladeRF host software.."
echo " Change Directory to bladeRF/host..."
cd bladeRF/host
mkdir build
cd build
cmake ../ -DCMAKE_INSTALL_PREFIX=/opt/bladeRF
make -j4
echo "* Creating bladeRF install directories and perform installization of bladeRF software"
mkdir -p /opt/bladeRF/etc
make install
echo "* libbladeRF build complete...."
echo "------------------------------------------------"

# create bladerf_paths.sh for profile.d .. 
echo "* Create bladeRF_path.sh in /etc/profile.d/bladeRF_path.sh"
cat >/etc/profile.d/bladeRF_paths.sh <<EOF
export PATH=$PATH:/opt/bladeRF/bin
export BLADERF_SEARCH_DIR=/opt/bladeRF/etc
export PKG_CONFIG_PATH=$PKG_CONFIG_PATH:/opt/bladeRF/lib/pkgconfig
EOF

# Add bladeRF libraries to library search path
echo "* create library path in /etc/ld.so.conf.d/bladeRF.conf"
cat > /etc/ld.so.conf.d/bladeRF.conf <<EOF
/opt/bladeRF/lib
EOF
# update library cache
ldconfig

# Add udev rules
echo "* creating udev rules for bladeRF in /etc/udev/rules.d/88-nuand-bladerf.rules"
cat > /etc/udev/rules.d/88-nuand-bladerf.rules << EOF
# Nuand bladeRF 2.0 micro
ATTR{idVendor}=="2cf0", ATTR{idProduct}=="5250", MODE="660", GROUP="bladerf"
# Nuand bladeRF, legacy VID/PID
ATTR{idVendor}=="1d50", ATTR{idProduct}=="6066", MODE="660", GROUP="bladerf"
EOF
# reload udev rules
udevadm control --reload-rules

# grab fpga images from nuand
echo "------------------------------------------------------"
echo "* Grabbing FPGA and firmware from NUAND..."
cd /opt/bladeRF/etc
echo -n "Downloading hostedxA4 image.."
wget "https://www.nuand.com/fpga/hostedxA4-latest.rbf" -q
mv hostedxA4-latest.rbf hostedxA4.rbf
echo "  OK"
echo -n "DOwnloading hostedxA9 image.."
wget "https://www.nuand.com/fpga/hostedxA9-latest.rbf" -q
mv hostedxA9-latest.rbf hostedxA9.rbf
echo "  OK"
echo -n "Downloading fx3 firmware.."
wget "https://www.nuand.com/fx3/bladeRF_fw_latest.img" -q
mv bladeRF_fw_latest.img bladeRF.img
echo "  OK"

# create bladerf group
groupadd bladerf
# todo add a user to be bladerf group.
echo "Note:"
echo "you will need to manualy add users to the bladerf group"
echo "use: sudo usermod -a -G bladerf <username>"
echo
echo "You will probably need reboot to get everything up and working again."
echo
echo "Done.."


