#!/bin/bash
# remove script automation
# kb3gtn

# check to see if there is an install..
if [! -d "/opt/bladeRf" ]; then
  echo "Don't detect anything in /opt/bladeRF was it installed?"
fi

echo "************************************************************"
echo "* This script will remove bladeRF install from your system *"
echo "************************************************************"

read -n1 -p "Do you wish to continue (Y/n) " runproc
echo

# if not yes, exit..
if [[ ! $runproc =~ ^[Yy]$ ]]
then
  echo "Exit..."
  exit 1
fi

# check to see if root.
if [[ $EUID -ne 0 ]]; then
  echo "This script requires root to execute..   try sudo.."
  exit 1
fi

echo "Removing files.."
echo "Removing /etc/profile.d/bladeRF_paths.sh"
rm /etc/profile.d/bladeRF_paths.sh
echo "Removing /etc/ld.so.conf.d/bladeRF.conf"
rm /etc/ld.so.conf.d/bladeRF.conf
echo "Removing udev rules..."
rm /etc/udev/rules.d/88-nuand-bladerf.rules
echo "Removing bladeRF directory /opt/bladeRF"
rm /opt/bladeRF -Rf
udevadm control --reload-rules
echo "Removing bladeRF group.."
groupdel bladerf
echo "Job Complete.."
