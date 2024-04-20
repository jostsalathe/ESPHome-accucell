#pragma once

/**
 * 
 * This protocol decoder class is based on the reverse engineering documented here:
 * https://2007.blog.dest-unreach.be/2012/01/29/imax-b6-charger-protocol-reverse-engineered/
 * 
 **/

/*
sample recordings
                     !! !! !!       !! !! !! !! !! !! !! !! !! !! !! !! !! !! !! !!             !! !! !! !!             !! !! !! !! !! !! !! !! !! !! !! !! !! !! !! !!                                     !!
 0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31 32 33 34 35 36 37 38 39 40 41 42 43 44 45 46 47 48 49 50 51 52 53 54 55 56 57 58 59 60 61 62 63 64 65 67 68 69 70 71 72 73 74 

90 84 84 D0 85 80 E4 81 81 81 80 81 81 81 80 81 85 81 85 81 BC 86 81 80 80 81 80 81 80 C2 F8 80 80 B3 84 8E 80 80 80 80 8C C4 80 84 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 81 8F A4 3B 30    idle

90 84 84 D0 85 80 E4 81 81 81 80 81 81 81 80 81 85 81 85 81 BC 86 81 81 80 81 80 81 80 C2 F8 80 80 B3 84 8E 80 80 80 80 8C C2 80 81 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 8F A4 3A 3B    charging
90 84 84 D0 85 80 E4 81 81 81 80 81 81 81 80 81 85 81 85 81 BC 86 81 81 80 81 80 81 80 C2 F8 80 80 B3 84 8E 80 80 80 80 8C C2 80 81 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 8F A4 3A 3B    charging

90 84 84 D0 85 80 E4 80 81 81 80 81 81 81 80 81 85 81 85 81 BC 86 81 80 80 81 80 81 80 C2 F8 80 80 80 84 84 80 80 80 80 8C C3 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 8F A4 36 3C    discharging
90 84 84 D0 85 80 E4 80 81 81 80 81 81 81 80 81 85 81 85 81 BC 86 81 81 80 81 80 81 80 C2 F8 80 80 8E 84 83 80 80 80 80 8C C3 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 80 8F A4 37 3A    discharging
*/

#include <cstdint>

namespace esphome {
namespace turnigy_accucell6 {

class Accucell6_Parser {
public:

  const static uint8_t MENU_MODE_CONFIG  = 0;
  const static uint8_t MENU_MODE_LI      = 1;
  const static uint8_t MENU_MODE_NIMH    = 2;
  const static uint8_t MENU_MODE_NICD    = 3;
  const static uint8_t MENU_MODE_PB      = 4;
  const static uint8_t MENU_MODE_SAVE    = 5;
  const static uint8_t MENU_MODE_LOAD    = 6;

  const static std::size_t MAX_CELL_COUNT = 6;

  Accucell6_Parser();

  bool add_bytes_and_chack_for_complete_message(const uint8_t *new_bytes, const std::size_t len);

  bool get_is_charging() const;
  bool get_is_cycling() const;
  float get_target_battery_current() const;
  bool get_cycle_starts_with_charge() const;
  uint8_t get_cycle_count() const;
  uint8_t get_target_cell_count() const;
  uint8_t get_menu_mode() const;
  bool get_is_running() const;
  float get_measured_battery_current() const;
  float get_measured_battery_voltage() const;
  float get_measured_input_voltage() const;
  float get_measured_charge() const;
  float get_measured_cell_voltage(std::size_t index) const;
  uint16_t get_elapsed_minutes() const;
  

  
protected:

  bool compare_checksum() const;
  void parse();
  uint8_t sanitize_data_byte(const std::size_t index) const;
  float parse_A_from_dA(const std::size_t index) const;
  float parse_A_from_A_and_cA(const std::size_t index) const;
  float parse_V_from_daV_and_dV(const std::size_t index) const;
  float parse_V_from_V_and_cV(const std::size_t index) const;
  float parse_Ah_from_dAh_and_mAh(const std::size_t index) const;


  const static std::size_t MESSAGE_BUFFER_LEN = 128;
  uint8_t message_buffer_[MESSAGE_BUFFER_LEN];
  uint8_t message_buffer_index_;        // position for next received byte - the byte at this index has no centent, yet

  float set_input_undervolt_;
  bool charging_;                       // false means dischargind, true means charging
  bool cycling_;                        // false means single dis-/charge, true means cycling
  float set_current_nicd_charge_;
  float set_current_nicd_discharge_;
  float set_current_nimh_charge_;
  float set_current_nimh_discharge_;
  bool cycle_mode_charge_discharge_;    // false means discharge -> charge, true means charge -> discharge
  uint8_t cycle_count_;
  float set_current_li_charge_;
  uint8_t set_cell_count_li_charge_;
  float set_current_li_discharge_;
  uint8_t set_cell_count_li_discharge_;
  float set_current_pb_charge_;
  uint8_t set_cell_count_pb_charge_;
  uint8_t menu_mode_;
  bool running_;
  float set_voltage_nimh_discharge_;
  float set_voltage_nicd_discharge_;
  uint16_t set_time_limit_;
  float set_charge_limit_;
  float measured_current_;
  float measured_voltage_;
  float measured_input_voltage_;
  float measured_charge_;
  float measured_cell_voltage_Li_[MAX_CELL_COUNT];
  uint16_t elapsed_minutes_;
};

} // namespace turnigy_accucell6
} // namespace esphome
