//////////////////////////////////////
// BladeRF C++ example modulator
// this generates a FM modulated tone
// or sweeping tone at 446.5 MHz.
//
// You can use a UHF receiver to demod.
//
// Peter Fetterer (kb3gtn@gmail.com)
//
//////////////////////////////////////

#include <iostream>
#include <string>
#include <cmath>
#include <atomic>
#include <thread>
#include <chrono>
#include <fstream>
#include <csignal>
#include <complex>

#include <fcntl.h>
#include <unistd.h>

#include "bladeRF.hpp"

std::atomic<bool> shutdown_app(false);

void signal_handler( int signum ) {
    // signal application to shutdown
    shutdown_app = true;
    std::cout << "\nOS Signal Received, shutting down initiated.." << std::endl;
}

constexpr double m_pi = 3.14159265359;
constexpr double m_2pi = 2*m_pi;
constexpr double m_pi_2 = m_pi/2;
constexpr double m_pi_4 = m_pi/4;

using cplx_double = std::complex<double>;
using std::string;
using std::vector;
using std::cout;
using std::cerr;
using std::endl;

class ToneGen {
    // radians per sample..
    double m_rad_rate;
    // current phase angle of generated sample
    double m_phase;
    // enable frequency sweeping of tone.
    bool m_do_sweep;

    // start sweep freq
    double m_sweep_start;
    // stop sweep freq
    double m_sweep_stop;
    // rate of sweep.
    double m_sweep_rate;

    double wrap_phase(double rads) {
        double r = rads;
        while (r > m_pi ) {
            r -= m_2pi;
        }
        while (r < -m_pi) {
            r += m_2pi;
        }
        return r;
    }
  public:
    ToneGen() {
        m_rad_rate = 0;
        m_phase = 0;
        m_do_sweep = false;
        m_sweep_start = 0;
        m_sweep_stop = 0;
        m_sweep_rate = 0;
    }
    void enable_sweep_hz(double sample_rate, double start, double stop, double rate) {
        double sweep_start = m_2pi*start / sample_rate;
        double sweep_stop = m_2pi*stop / sample_rate;
        double sweep_rate = m_2pi*rate / sample_rate;
        enable_sweep(sweep_start, sweep_stop, sweep_rate);
    }
    void enable_sweep(double sweep_start, double sweep_stop, double sweep_step ) {
        // if given in negitive, make it postive
        if ( sweep_step < 0 ) sweep_step = -1*sweep_step;
        m_do_sweep = true;
        m_sweep_start = sweep_start;
        m_sweep_stop = sweep_stop;
        if ( sweep_stop < sweep_start ) {
            m_sweep_rate = -1*sweep_step;
        } else {
            m_sweep_rate = sweep_step;
        }
        m_rad_rate = m_sweep_start;
        cout << "Sweep Info: \n";
        cout << "Sweep Start: " << m_sweep_start << " rad/sample \n";
        cout << "Sweep Stop: " << m_sweep_stop << " rad/sample \n";
        cout << "Sweep Rate: " << m_sweep_rate << " rad/sample \n";
    }
    void disable_sweep() {
        m_do_sweep = false;
    }
    bool sweep_enabled() {
        return m_do_sweep;
    }
    double get_rad_rate() { return m_rad_rate; }
    double get_phase() { return m_phase; }
    // set phase in rads
    void set_phase( double phase) {
        m_phase = wrap_phase(phase);
    }
    // radian rate in rads per sample
    void set_rad_rate(double rad_rate) {
        m_rad_rate = rad_rate;
    }
    // compute radian rate from hz and sample rate
    void set_freq_hz(double sample_rate, double freq_hz) {
        m_rad_rate = (m_2pi*freq_hz) / sample_rate;
    }
    // compute buffer of samples, continued from last call..
    void generate( vector<double> *buffer ) {
        // get number of samples to generate based on size of buffer
        // provided
        unsigned int sample_cnt = buffer->size();
        // make rads in correct range..
        m_phase = wrap_phase(m_phase); 
        // generate samples in loop.
        //cout << "Rad Rate: " << m_rad_rate << '\n';
        for (int idx=0; idx < sample_cnt; ++idx) {
            (*buffer)[idx] = std::cos(m_phase);
            m_phase += m_rad_rate; // update phase for next sample.
            if (m_do_sweep) {
                m_rad_rate += m_sweep_rate;
                if (m_sweep_rate >= 0 ) {
                    if (m_rad_rate >= m_sweep_stop)
                        m_rad_rate = m_sweep_start;
                } else {
                    if (m_rad_rate <= m_sweep_stop)
                        m_rad_rate = m_sweep_start;
                }
            }
        }
    }
};

class FmModulator {
    double m_carrier_phase;
    double m_mod_index; // change in put create change in radian rate.
    double wrap_phase(double rads) {
        double r = rads;
        while (r > m_pi ) {
            r -= m_2pi;
        }
        while (r < -m_pi) {
            r += m_2pi;
        }
        return r;
    }
    cplx_double complex_exp( double jwt ) {
        cplx_double s = std::polar<double>(0.9, wrap_phase(jwt));
        return s;
    }
  public:
    FmModulator() {
        m_carrier_phase = 0;
        m_mod_index = 1;
    }
    FmModulator( double initial_phase, double mod_index ) {
        m_carrier_phase = wrap_phase(initial_phase);
        m_mod_index = m_mod_index;
    }
    void set_phase( double phase ) {
        m_carrier_phase = wrap_phase( phase );
    }
    // radian rate change for 1.0 input.
    void set_mod_idx( double mod_index ) {
        m_mod_index = mod_index;
    }
    // compute radian rate change for 1.0 from dev in hz
    void set_mod_dev_hz( double samplerate, double mod_dev_hz ) {
        m_mod_index = m_2pi*mod_dev_hz / samplerate;
    }
    // generate output_sample for each input sample provided.
    // output is resized to match mod_in in length.
    void generate( vector<double> *mod_in, vector<cplx_double> *output ) {
        if ( mod_in->size() != output->size() ) {
            output->resize(mod_in->size());
        }
        unsigned int sample_cnt = mod_in->size();
        for (int idx=0; idx < sample_cnt; ++idx) {
            // compute rad step for this sample based on mod input
            double rad_step = (*mod_in)[idx] * m_mod_index;
            // compute carrier phase for this sample based on computed rad step
            m_carrier_phase = wrap_phase( m_carrier_phase - rad_step);
            // compute tx sample
            (*output)[idx] = complex_exp( m_carrier_phase );
        }
    }
};

int main() {
    std::cout << "FM Modulator Test.." << std::endl;
    
    bladeRF::bladeRF myRadio;

    // attempt to open bladeRF radio device with args provided.
    string devargs("*");
    if ( myRadio.open(devargs.c_str()) != bladeRF::result_t::ok ) {
        cerr << "Failed to open radio..\n";
        return -1;
    }

    constexpr unsigned int samplerate = 1e6;

    bladeRF::channel_cfg_t radioConfig;
    radioConfig.ch_num(0); // 0 - TX1/RX1
    radioConfig.tx_freq(446500000); // tx freq
    radioConfig.tx_samplerate(samplerate); // desired samplerate
    radioConfig.tx_bandwidth(200000); // baseband filter bandwidth
    radioConfig.tx_gain(30);
    radioConfig.tx_enabled(true);

    // install signal handle
    signal(SIGINT, signal_handler );

    cout << "Radio configuration requested: \n";
    cout << toString(radioConfig) << "\n";
    cout << "-------------------------------\n\n";
    cout << "Performing radio configuration..\n";
    // configure radio
    if ( myRadio.configure( radioConfig ) != bladeRF::result_t::ok ) {
      cout << "Failed to configure radio.. exit..\n";
      return -1;
    }
    cout << "Done.\n\n";
    cout << "Radio configuration result: \n";  
    cout << toString(radioConfig) << "\n";
    cout << "-------------------------------\n";

    // enable streaming of samples.
    if ( myRadio.configure_streaming() != bladeRF::result_t::ok ) {
      cout << "Failed to configure streaming on the radio..\n";
      return -1;
    }

    int loopcnt = 0;

    // setup FM Modulator blocks and buffers

    vector<double> tone_samples(myRadio.get_stream_buffer_size() );
    vector<cplx_double> fm_mod_samples(myRadio.get_stream_buffer_size() );

    ToneGen tone_source;
    // generate 800 Hz tone..
    //tone_source.set_freq_hz(radioConfig.get_tx_samplerate(), 800);
    // sweeping tone...
    tone_source.enable_sweep_hz(
             radioConfig.get_tx_samplerate(),
             600,
             4000,
             0.001 );
    
    FmModulator fm_mod;
    fm_mod.set_phase( 0 );
    fm_mod.set_mod_dev_hz( radioConfig.get_tx_samplerate(), 4000 );

    cout << "/////////////////////////////////\n";
    cout << "       Starting Main Loop        \n";
    cout << "    Ctrl-C to terminate App      \n";
    cout << "/////////////////////////////////\n";

    // output file used for verification
    //int fh = open("output_full.c64", O_RDWR | O_CREAT, S_IRUSR | S_IWUSR | S_IROTH );

    ///////////////////////////////////////////
    // Main Loop
    while ( shutdown_app == false ) {

        // generate next set of tone samples to modulate
        tone_source.generate( &tone_samples );
        // modulate tone samples into FM mod carrier
        fm_mod.generate( &tone_samples, &fm_mod_samples);
        // send samples to output
        myRadio.transmit( &fm_mod_samples );
        
        // write samples to output file for verification.
        //write( fh, fm_mod_samples.data(), sizeof(cplx_double)*fm_mod_samples.size() );

        // print status indicator every 100 buffers.
        if ( loopcnt == 100 ) {
            cout << ".";
            loopcnt = 0;
        } else {
            ++loopcnt;
        }
    }
    std::cout << "Sample Generation Loop Exit.." << std::endl;
    // disable channels if active..
    std::cout << std::endl << "Cleaning up BladeRF..    " << std::flush;
    myRadio.close();
    std::cout << "Application Exit.." << std::endl;
    return 0;
}


