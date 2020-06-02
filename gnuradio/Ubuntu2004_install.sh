#!/bin/bash

# check to see if root
if [[ $EUID -ne 0 ]]; then
    echo "This script requires to be run as root.."
    exit 1
fi


#check if this is a uninstall
if [ "$1" = "uninstall" ]; then
    echo "Uninstalling volk and gnuradio"
    rm /opt/volk -rf
    rm /etc/profile.d/volk_paths.sh -f
    rm /etc/ld.so.conf.d/volk.conf -f
    rm /opt/gnuradio -rf
    rm /etc/profile.d/gnuradio_paths.sh -f
    rm /etc/ld.so.conf.d/gnuradio.conf -f
    exit 1;
fi


echo "***************************************************"
echo "* This script will attempt to compile and install *"
echo "* Gnuradio from source on your system..           *"
echo "***************************************************"
echo
read -n1 -p "Run this script (Y/n): " runproc
echo
 
# if not Yes, then exit script
if [[ ! $runproc =~ ^[Yy]$ ]]
then
    echo "Exit..."
    exit 1
fi

# Test script to build gnuradio and install in /opt
apt install git cmake g++ libboost-all-dev libgmp-dev swig python3-numpy \
python3-mako python3-sphinx python3-lxml doxygen libfftw3-dev \
libsdl1.2-dev libgsl-dev libqwt-qt5-dev libqt5opengl5-dev python3-pyqt5 \
liblog4cpp5-dev libzmq3-dev python3-yaml python3-click python3-click-plugins \
python3-zmq python3-scipy python3-gi python3-gi-cairo gobject-introspection gir1.2-gtk-3.0 python3-matplotlib libqt5svg-dev

mkdir build_space
cd build_space

# build volk
if [ -d "volk" ]
then
  rm volk -Rf
fi

git clone "https://github.com/gnuradio/volk.git"
cd volk
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release -DPYTHON_EXECUTABLE=/usr/bin/python3 -DCMAKE_INSTALL_PREFIX=/opt/volk ../
make -j4
make install

# add path and library paths to system
cat > /etc/profile.d/volk_paths.sh << EOF
export PATH=\$PATH:/opt/volk/bin
export PKG_CONFIG_PATH=\$PKG_CONFIG_PATH:/opt/volk/lib/pkgconfig
export PYTHONPATH=\$PYTHONPATH:/opt/volk/lib/python3/dist-packages
EOF

# update local environment
source /etc/profile.d/volk_paths.sh

# add library path
cat > /etc/ld.so.conf.d/volk.conf << EOF
/opt/volk/lib
EOF

# update library cache
ldconfig

cd ..
cd ..

# pull gnuradio and build..

git clone https://github.com/gnuradio/gnuradio.git
cd gnuradio
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/opt/gnuradio -DPYTHON_EXECUTABLE=/usr/bin/python3 -DUHD_DIR=/opt/uhd/ -DUHD_INCLUDE_DIRS=/opt/uhd/include/ -DUHD_LIBRARIES=/opt/uhd/lib/libuhd.so ../
make -j4
make install

echo "----------------------------------"
echo " Setting up enviroment...
echo "----------------------------------"

cat > /etc/profile.d/gnuradio_paths.sh << EOF
export PATH=\$PATH:/opt/gnuradio/bin
export PKG_CONFIG_PATH=\$PKG_CONFIG_PATH:/opt/gnuradio/lib/pkgconfig
export PYTHONPATH=\$PYTHONPATH:/opt/gnuradio/lib/python3/dist-packages
EOF

cat > /etc/ld.so.conf.d/gnuradio.conf << EOF
/opt/gnuradio/lib
EOF

# Build gr-osmosdr for bladeRF I/O
echo "Building gr-osmosdr for bladeRF access.."
cd ..
cd ..
git clone git://git.osmocom.org/gr-osmosdr
cd gr-osmosdr
mkdir build
cd build
cmake ../
make -j4
make install

# update library cache
ldconfig

echo "DONE.."

