### files
* bladeRF.hpp  
Header file containing a c++ wrapper implementation of the libbladeRF C functions.

* threshold_detect.cpp  
Example program that configures and receives samples from the bladeRF. This program measures the magnatude of the received samples over 1 buffer and compares the average to a threshold value.
If over the threshold value, will indicate a signal is present.  If under the threshold will indicate not present.

* fm_modulator.cpp  
Example program that generates a FM modulated tone or sweeping tone at 446.5 MHz.  This can be received by a UHF HAM FM demodulator to hear the tone generated and modulated.

### build instructions
Run the build.sh script to compile the 2 programs.   Assumes you have pkg-config to point out the bladeRF compile flags.

