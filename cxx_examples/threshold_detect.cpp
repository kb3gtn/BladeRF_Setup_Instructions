////////////////////////////////////////////
// Threshold Detect
//
// This is a simple demodulator that just tells
// if the received energy is above a threshold
// or not.
//
// You can use a UHF transmitter to transmit
// a CW or modulated signal at 446.5 MHz.
//
// This will indicate if it see the start of the signal
// and when the signal goes away..
//
// Peter Fetterer (kb3gtn@gmail.com)
//
////////////////////////////////////////////

#include <vector>
#include <string>
#include <cstdint>
#include <complex>
#include <atomic>
#include <csignal>

#include "bladeRF.hpp"

using std::atomic;
using std::cout;
using std::vector;
using std::complex;
using cplx_double = complex<double>;
using std::string;

atomic<bool> g_app_running;

void signal_handler( int signum ) {
  cout << "\nReceived signal: " << signum << " from os. \n";
  g_app_running = false;
}

double compute_avg_magnatude( vector<cplx_double> *samples );

int main(int argc, char **argv) {
  g_app_running = true;
  bladeRF::bladeRF myRadio;

  bladeRF::channel_cfg_t radioConfig;

  radioConfig.ch_num(0);
  radioConfig.rx_freq(446500000);
  radioConfig.rx_samplerate(521000);
  radioConfig.rx_bandwidth(200000);
  radioConfig.rx_gain(60);
  radioConfig.rx_enabled(true);

  string argstring("*");

  if ( myRadio.open( argstring.c_str() ) != bladeRF::result_t::ok ) {
    cout << "Failed to open radio..\n";
    return -1;
  }

  if ( myRadio.configure( radioConfig ) != bladeRF::result_t::ok ) {
    cout << "Failed to configure radio..\n";
    return -1;
  }

  if ( myRadio.configure_streaming() != bladeRF::result_t::ok ) {
    cout << "Faile to configure streaming on the radio..\n";
    return -1;
  }

  // create a buffer to hold samples of the correct size for streaming to use.
  vector<cplx_double> sample_buffer(myRadio.get_stream_buffer_size() );

  bool above_threshold = false;
  double threshold = 0.05;
  double measured = 0;
  int loopcnt=0;

  // install signal handler
  signal(SIGINT, signal_handler);

  cout << "Starting Main Loop.\n";

  while( g_app_running == true ) {
    myRadio.receive( &sample_buffer );
    measured = compute_avg_magnatude( &sample_buffer );
    if ( measured > threshold ) {
      if ( above_threshold == false ) {
        cout << "Signal Detected..\n";
        above_threshold = true;
      }
    } else {
      // less than = threshold
      if ( above_threshold == true ) {
        cout << "Signal Lost..\n";
      }
      above_threshold = false;
    }

    if ( loopcnt == 30 ) {
      cout << "measured: " << measured << '\n';
      loopcnt = 0;
    } else {
      ++loopcnt;
    }

  }

  cout << "Main Loop Exit.. clean-up..\n";

  myRadio.close(); 

  return 0;
}


double compute_avg_magnatude( vector<cplx_double> *samples ) {
  double avg = 0;
  for (int idx=0; idx < samples->size(); ++idx) {
    avg += std::abs( (*samples)[idx] );
  }
  avg = avg / samples->size();
  return avg;
}

