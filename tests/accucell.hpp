#ifndef ACCUCELL_H_
#define ACCUCELL_H_

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

class Accucell6 {
  public:

  const uint8_t MENU_MODE_CONFIG  = 0;
  const uint8_t MENU_MODE_LI      = 1;
  const uint8_t MENU_MODE_NIMH    = 2;
  const uint8_t MENU_MODE_NICD    = 3;
  const uint8_t MENU_MODE_PB      = 4;
  const uint8_t MENU_MODE_SAVE    = 5;
  const uint8_t MENU_MODE_LOAD    = 6;

  Accucell()

  bool add_bytes_and_chack_for_complete_message(uint8_t *new_bytes, size_t len);

  
  protected:

  parse();
  compare_checksum();

  uint8_t message_buffer_[128];
  uint8_t message_buffer_index_;

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
  float measured_current_;
  float measured_voltage_;
  float measured_input_voltage_;
  float measured_charge_;
  float measured_cell_voltage_Li_[6];
  uint8_t elapsed_minutes_;

}

#endif /* ACCUCELL_H_ */