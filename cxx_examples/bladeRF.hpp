//////////////////////////////////
// BladeRF interface wrapper
//
// This is a a take at implementing a
// c++ wrap to simplify using the 
// bladeRF API in C++ programs.
//
// This is a header-only wrapper to
// the libbladeRF library provided by 
// NUAND.
//
// PRF (kb3gtn@gmail.com)
//
//////////////////////////////////////

#ifndef __bladeRF__hpp
#define __bladeRF__hpp

#include <iostream>
#include <string>
#include <vector>
#include <cstdint>
#include <complex>
#include <cstring>
#include <sstream>

#include <libbladeRF.h>

namespace bladeRF {

  using std::cout;
  using std::cerr;
  using std::endl;
  using std::flush;
  using std::vector;
  using std::string;
  using std::complex;
  using std::stringstream;

  using cplx_double = complex<double>;

  // print bladeRF error message
  int print_bladerf_error(int result) {
    if ( result < 0 ) {
      std::cout << "BladeRF Error: " << bladerf_strerror(result) << std::endl;
    }
    return result;
  }


  enum class result_t {
    ok,
    general_error,
    value_out_of_range,
    configuration_failed,
    failed_to_start_streaming,
    open_failed,
    not_open,
    not_configured,
    not_streaming,
    bad_buffer_size,
    stream_error
  };

  char* toString(result_t r) {
    switch(r) {
      case result_t::ok:
        return (char*)"ok";
      case result_t::general_error:
        return (char*)"general error";
      case result_t::value_out_of_range:
        return (char*)"value out of range";
      case result_t::configuration_failed:
        return (char*)"configuration failed";
      case result_t::failed_to_start_streaming:
        return (char*)"failed to start streaming";
      case result_t::open_failed:
        return (char*)"open failed";
      case result_t::not_open:
        return (char*)"not open";
      case result_t::bad_buffer_size:
        return (char*)"bad buffer size";
      case result_t::not_configured:
        return (char*)"not configured";
      case result_t::not_streaming:
        return (char*)"not streaming";
      case result_t::stream_error:
        return (char*)"stream error";
      default:
        return (char*)"unknown";
    }
  }

  class bladeRF;

  class channel_cfg_t {
    int m_ch_num;
    long unsigned int m_tx_freq;
    long unsigned int m_rx_freq;
    unsigned int m_tx_samplerate;
    unsigned int m_rx_samplerate;
    unsigned int m_tx_bandwidth;
    unsigned int m_rx_bandwidth;
    unsigned int m_rx_gain;
    unsigned int m_tx_gain;
    bladerf_gain_mode m_rx_gainmode;
    bladerf_gain_mode m_tx_gainmode;
    bool m_enable_tx;
    bool m_enable_rx;

    friend bladeRF;

  public:
    // initialization defaults.
    channel_cfg_t() {
      m_ch_num = 0; 
      m_tx_freq = 446.5e6;
      m_rx_freq = 446.5e6;
      m_tx_samplerate = 521000;
      m_rx_samplerate = 521000;
      m_tx_bandwidth = 200000;
      m_rx_bandwidth = 200000;
      m_rx_gain = 0;
      m_tx_gain = 0;
      m_rx_gainmode = bladerf_gain_mode::BLADERF_GAIN_DEFAULT;
      m_tx_gainmode = bladerf_gain_mode::BLADERF_GAIN_DEFAULT;
      m_enable_tx = false;
      m_enable_rx = false;
    }

    // getters
    int    get_ch_num() { return m_ch_num; }
    double get_tx_freq() { return m_tx_freq; }
    double get_rx_freq() { return m_rx_freq; }
    double get_tx_samplerate() { return m_tx_samplerate; }
    double get_rx_samplerate() { return m_rx_samplerate; }
    double get_tx_bandwidth() { return m_tx_bandwidth; }
    double get_rx_bandwidth() { return m_rx_bandwidth; }
    double get_rx_gain() { return m_rx_gain; }
    double get_tx_gain() { return m_tx_gain; }
    bool   get_tx_enabled() { return m_enable_tx; }
    bool   get_rx_enabled() { return m_enable_rx; }
    // setters
    result_t ch_num(int v) {
      if (( v == 0 ) || ( v == 1 )) {
        m_ch_num = v;
        return result_t::ok;
      } else {
        cerr << "error: channel num " << v << " is not valid.\n";
        return result_t::value_out_of_range;
      }
    }
    result_t tx_freq(double v) {
      // validate value v
      if ( v < 70e6 ) {
        cerr << v << " is to low for device"; 
        return result_t::value_out_of_range;
      }
      if ( v > 6e9 ) {
        cerr << v << " is to high for device.";
        return result_t::value_out_of_range;
      }
      m_tx_freq = v;
      return result_t::ok;
    }
    result_t rx_freq(double v) {
      // validate value v
      if ( v < 70e6 ) {
        cerr << "error: cw freq " << v << " is to low for device.\n"; 
        return result_t::value_out_of_range;
      }
      if ( v > 6e9 ) {
        cerr << "error: cw freq " << v << " is to high for device.\n";
        return result_t::value_out_of_range;
      }
      m_rx_freq = v;
      return result_t::ok;
    }
    result_t tx_samplerate(double v) {
      if ( v < 521e3 ) {
        cerr << "error: samplerate "<< v << " is to low for device.\n";
        return result_t::value_out_of_range;
      }
      if ( v > 61.44e6 ) {
        cerr << "error: samplerate "<< v << " is to high for device.\n";
        return result_t::value_out_of_range;
      }
      m_tx_samplerate = v;
      return result_t::ok;
    }
    result_t rx_samplerate(double v) {
      if ( v < 521e3 ) {
        cerr << "error: samplerate "<< v << " is to low for device.\n";
        return result_t::value_out_of_range;
      }
      if ( v > 61.44e6 ) {
        cerr << "error: samplerate "<< v << " is to high for device.\n";
        return result_t::value_out_of_range;
      }
      m_rx_samplerate = v;
      return result_t::ok;
    }
    result_t tx_bandwidth(double v) {
      if ( v > 54e6 ) {
        cerr << "error: bandwidth " << v << " exceeds max for device.\n";
        return result_t::value_out_of_range;
      }
      if ( v < 200e3 ) {
        cerr << "error: bandwidth " << v << " is under min for device.\n";
        return result_t::value_out_of_range;
      }
      m_tx_bandwidth = v;
      return result_t::value_out_of_range;
    }
    result_t rx_bandwidth(double v) {
      if ( v > 54e6 ) {
        cerr << "error: bandwidth " << v << " exceeds max for device.\n";
        return result_t::value_out_of_range;
      }
      if ( v < 200e3 ) {
        cerr << "error: bandwidth " << v << " is under min for device.\n";
        return result_t::value_out_of_range;
      }
      m_rx_bandwidth = v;
      return result_t::ok;
    }
    result_t rx_gain(double v) { 
      if ( v < 0 ) {
        cerr << "error: gain " << v << " under minimum value for device.\n";
        return result_t::value_out_of_range;
      }
      if ( v > 76 ) {
        cerr << "error: gain " << v << " over maximum value for device.\n";
        return result_t::value_out_of_range;
      }
      m_rx_gain = v;
      return result_t::ok;
    }
    result_t tx_gain(double v) { 
      if ( v < 0 ) {
        cerr << "error: gain " << v << " under minimum value for device.\n";
        return result_t::value_out_of_range;
      }
      if ( v > 76 ) {
        cerr << "error: gain " << v << " over maximum value for device.\n";
        return result_t::value_out_of_range;
      }
      m_tx_gain = v;
      return result_t::ok;
    }
    result_t tx_enabled(bool v) { 
      m_enable_tx = v;
      return result_t::ok;
    }
    result_t rx_enabled(bool v) { 
      m_enable_rx = v;
      return result_t::ok;
    }

    friend string toString( channel_cfg_t &cfg );
    
  };

  // serialize channel_cfg_t into a json string..
  string toString( channel_cfg_t &cfg ) {
    stringstream ss;
    ss << "{\n ch_num : " << cfg.m_ch_num << ",\n";
    ss << " tx_freq : " << cfg.m_tx_freq << ",\n";
    ss << " rx_freq : " << cfg.m_rx_freq << ",\n";
    ss << " tx_samplerate : " << cfg.m_tx_samplerate << ",\n";
    ss << " rx_samplerate : " << cfg.m_rx_samplerate << ",\n";
    ss << " tx_bandwidth : " << cfg.m_tx_bandwidth << ",\n";
    ss << " rx_bandwidth : " << cfg.m_rx_bandwidth << ",\n";
    ss << " rx_gain : " << cfg.m_rx_gain << ",\n";
    ss << " tx_gain : " << cfg.m_tx_gain << ",\n";
    ss << " rx_gainmode : " << cfg.m_rx_gainmode << ",\n";
    ss << " tx_gainmode : " << cfg.m_tx_gainmode << ",\n";
    ss << " enable_tx : " << cfg.m_enable_tx << ",\n";
    ss << " enable_rx : " << cfg.m_enable_rx << ",\n";
    ss << "}";
    string json = ss.str();
    return json;
  }

  // default usb transfer buffer size in samples (16bits*2)
  constexpr int default_bsize = 16384;

  struct stream_config_t {
    bladerf_channel_layout chan_layout_tx;
    bladerf_channel_layout chan_layout_rx;
    int num_buffers;
    int buffer_size;
    int num_transfers;
    int timeout_ms;
    int bytes_in_buffer;
    // initialize with sane defaults..
    stream_config_t() {
      chan_layout_rx = BLADERF_RX_X1;
      chan_layout_tx = BLADERF_TX_X1;
      num_buffers = 64;
      buffer_size = default_bsize;
      num_transfers = 16;
      timeout_ms = 10000;
      bytes_in_buffer = buffer_size*4;
    }
  };

  // convert a value to bladeRF native format
  int16_t convert_to_s16q11(double s) {
    return static_cast<int16_t>(s*2046.0);
  }

  // convert a value from bladeRF native format
  double convert_from_s16q11(int16_t s) {
    return static_cast<double>(s/2047.0);
  }

  class bladeRF {
    struct bladerf *devhndl;
    bool m_isOpen;
    bool m_isConfigured;
    bool m_isStreaming;
    stream_config_t streamcfg;

    // internal buffers for streaming
    vector<int16_t> tx_buffer;
    vector<int16_t> rx_buffer;

  public:
    int get_stream_buffer_size() {
      return streamcfg.buffer_size;
    }
    bladeRF() {
      m_isOpen = false;
      m_isConfigured = false;
      m_isStreaming = false;
    }
    ~bladeRF() {
      close();
    }
    // open blade RF device
    result_t open(const char* arg_str) {
      bladerf_set_usb_reset_on_open(true);
      int result = bladerf_open( &devhndl, arg_str );
      if ( result != 0 ) {
        print_bladerf_error(result);
        m_isOpen = false;
        return result_t::open_failed;
      }
      m_isOpen = true;
      return result_t::ok;
    }

    // configure a channel of the bladeRF device
    result_t configure( struct channel_cfg_t &ch_cfg ) {
      if ( m_isOpen == false ) {
        return result_t::not_open;
      }
      bladerf_channel channel_tx, channel_rx;
      if ( ch_cfg.m_ch_num == 0 ) {
        cout << "Configuring Channel 0\n";
        channel_tx = BLADERF_CHANNEL_TX(0);
        channel_rx = BLADERF_CHANNEL_RX(0);
      } else {
        cout << "Configuring Channel 1\n";
        channel_tx = BLADERF_CHANNEL_TX(1);
        channel_rx = BLADERF_CHANNEL_RX(1);
      }
      int result=0;
      // set frequency
      result = bladerf_set_frequency( devhndl, channel_tx, ch_cfg.m_tx_freq);
      if (result != 0) {
        cerr << "error: failed to configure tx freq for channel " << ch_cfg.m_ch_num << '\n';
        return result_t::configuration_failed;
      }

      // get configured frequency and update config struct with correct data.
      bladerf_get_frequency( devhndl, channel_tx, &ch_cfg.m_tx_freq);

      result = bladerf_set_frequency( devhndl, channel_rx, ch_cfg.m_rx_freq);
      if (result != 0) {
        cerr << "error: failed to configure rx freq for channel " << ch_cfg.m_ch_num << '\n';
        return result_t::configuration_failed;
      }
      // get configured frequency and update config struct with correct data.
      bladerf_get_frequency( devhndl, channel_rx, &ch_cfg.m_tx_freq);

      // set samplerate
      result = bladerf_set_sample_rate( devhndl, 
        channel_tx, ch_cfg.m_tx_samplerate, &ch_cfg.m_tx_samplerate);
      if (result !=0 ) {
        cerr << "error: failed to configure tx sample rate..\n";
        return result_t::configuration_failed;
      }
      result = bladerf_set_sample_rate( devhndl, channel_rx, 
        ch_cfg.m_rx_samplerate, &ch_cfg.m_rx_samplerate );
      if ( result != 0) {
        cerr << "error: failed to configure rx sample rate..\n";
        return result_t::configuration_failed;
      }

      // bandwidth
      result = bladerf_set_bandwidth( devhndl, channel_tx, 
        ch_cfg.m_tx_bandwidth, &ch_cfg.m_tx_bandwidth );
      if ( result != 0 ) {
        cerr << "error: failed to configure tx baseband bandwidth. \n";
        return result_t::configuration_failed;
      }
      result = bladerf_set_bandwidth( devhndl, channel_rx, 
        ch_cfg.m_rx_bandwidth, &ch_cfg.m_rx_bandwidth );
      if ( result != 0 ) {
        cerr << "error: failed to configure rx baseband bandwidth. \n";
        return result_t::configuration_failed;
      }

      // gains
      result = bladerf_set_gain( devhndl, channel_tx, ch_cfg.m_tx_gain);
      if ( result != 0 ) {
        cerr << "error: failed to configure tx gain value. \n";
        return result_t::configuration_failed;
      }
      result = bladerf_set_gain( devhndl, channel_rx, ch_cfg.m_rx_gain);
      if ( result != 0 ) {
        cerr << "error: failed to configure tx gain value. \n";
        return result_t::configuration_failed;
      }

      // set gain modes
      result = bladerf_set_gain_mode( devhndl, channel_rx, ch_cfg.m_rx_gainmode);
      if ( result != 0 ) {
        cerr << "error: failed to configure rx gain mode. \n";
        return result_t::configuration_failed;
      }
      // gain mode for TX doesn't do anything..
      //result = bladerf_set_gain_mode( devhndl, channel_tx, ch_cfg.m_tx_gainmode);
      //if ( result != 0 ) {
      //  cerr << "error: failed to configure tx gain mode. \n";
      //  return result_t::configuration_failed;
      //}

      // enable modules
      result = bladerf_enable_module( devhndl, channel_tx, ch_cfg.m_enable_tx);
      if ( result != 0 ) {
        cerr << "error: failed to enable tx channel.. \n";
        return result_t::configuration_failed;
      }
      //cout << "tx is enabled: " << ch_cfg.m_enable_tx << '\n';
      result = bladerf_enable_module( devhndl, channel_rx, ch_cfg.m_enable_rx);
      if ( result != 0 ) {
        cerr << "error: failed to enable rx channel.. \n";
        return result_t::configuration_failed;
      }
      //cout << "rx is enabled: " << ch_cfg.m_enable_rx << '\n';

      m_isConfigured = true;

      return result_t::ok;
    }

    // configure stream (call before using tx/rx methods..)
    result_t configure_streaming() {
      // check to see if things are configured.
      if (m_isConfigured == false) {
        return result_t::not_configured;
      }

      // resize tx/rx buffer to defined buffersize.
      cout << "Using stream buffer size: " << streamcfg.buffer_size << '\n';
      tx_buffer.resize(streamcfg.buffer_size*2);
      rx_buffer.resize(streamcfg.buffer_size*2);

      // setup receive channel streaming..
      int result = 0;
      result = bladerf_sync_config( devhndl,
                                        streamcfg.chan_layout_rx,
                                        BLADERF_FORMAT_SC16_Q11,
                                        streamcfg.num_buffers,
                                        streamcfg.buffer_size,
                                        streamcfg.num_transfers,
                                        streamcfg.timeout_ms );

      if ( result != 0 ) {
        cerr << "stream configuration error occured with rx layout.\n";
        return result_t::configuration_failed;
      }

      result = bladerf_sync_config( devhndl,
                                        streamcfg.chan_layout_tx,
                                        BLADERF_FORMAT_SC16_Q11,
                                        streamcfg.num_buffers,
                                        streamcfg.buffer_size,
                                        streamcfg.num_transfers,
                                        streamcfg.timeout_ms );

      if ( result != 0 ) {
        cerr << "stream configuration error occured with tx layout.\n";
        return result_t::configuration_failed;
      }

      m_isStreaming = true;
      return result_t::ok;
    }

    // transmit a buffer of cplx doubles
    result_t transmit( vector<cplx_double>* samples_in ) {
      if ( m_isConfigured == false) {
        return result_t::not_configured;
      }
      if ( m_isStreaming == false) {
        return result_t::not_streaming;
      }

      // verify number of samples provided matches stream config
      if ( samples_in->size() != streamcfg.buffer_size ) {
        return result_t::bad_buffer_size;
      }
      struct bladerf_metadata meta;
      meta.flags = 0;

      unsigned int num_samples = streamcfg.buffer_size;
      unsigned int num_int16_elements = num_samples*2;

      // convert provided buffer into bladeRF native format
      // and store into temp tx_buffer for transmission.
      int16_t *wptr = &tx_buffer[0];
      for (int c=0; c < num_samples; ++c) {
        *wptr = convert_to_s16q11( (*samples_in)[c].real() );
        ++wptr;
        *wptr = convert_to_s16q11( (*samples_in)[c].imag() );
        ++wptr;
      }

      bladerf_sync_tx( devhndl,
                       tx_buffer.data(),
                       num_samples,
                       &meta,
                       streamcfg.timeout_ms );

      if ( meta.flags != 0 ) {
        cout << "U" << std::flush;
      }
      return result_t::ok;
    }

    // receive a buffer of cplx doubles
    result_t receive( vector<cplx_double>* samples_out ) {
      if ( m_isConfigured == false) {
        cerr << "Device is not configured..\n";
        return result_t::not_configured;
      }
      if ( m_isStreaming == false) {
        cerr << "Device is not streaming..\n";
        return result_t::not_streaming;
      }

      // check that provided buffer can hold samples received.
      if ( samples_out->size() != streamcfg.buffer_size ) {
        return result_t::bad_buffer_size;
      }

      struct bladerf_metadata meta;
      memset(&meta, 0, sizeof(meta));
      meta.flags = BLADERF_META_FLAG_RX_NOW;

      unsigned int num_samples = streamcfg.buffer_size/2;
      unsigned int num_int16_elements = num_samples*2;

      int result=0;
      result = bladerf_sync_rx( devhndl,
                       rx_buffer.data(),
                       num_samples,
                       &meta,
                       streamcfg.timeout_ms);

      if (result != 0 ) {
        cout << "bladerf_sync_rx returned error..\n";
        print_bladerf_error(result);
        return result_t::stream_error;
      }

      // convert from bladeRF format to complex double.
      int16_t *rptr = rx_buffer.data(); // read ptr..
      for (int idx=0; idx < num_samples; ++idx ) {
        cplx_double wv; // working value
        wv.real( convert_from_s16q11(*rptr) );
        ++rptr;
        wv.imag( convert_from_s16q11(*rptr) );
        ++rptr;
        (*samples_out)[idx] = wv;
      }

      return result_t::ok;

    }

    // close bladeRF device
    result_t close() {
      cout << "bladeRF destructor called.. \n";
      if ( m_isOpen == true ) {
        // disable all channel on bladeRF
        cout << " disabling streams..  ";
        bladerf_enable_module(devhndl, BLADERF_CHANNEL_TX(0), false);
        bladerf_enable_module(devhndl, BLADERF_CHANNEL_TX(1), false);
        bladerf_enable_module(devhndl, BLADERF_CHANNEL_RX(0), false);
        bladerf_enable_module(devhndl, BLADERF_CHANNEL_RX(1), false);
        cout << " releasing device.. ";
        bladerf_close(devhndl);
      }
      m_isOpen = false;
      m_isConfigured = false;
      m_isStreaming = false;
      cout << "\n";;
      return result_t::ok;
    }
  };

}

#endif // end __bladeRF_hpp__
