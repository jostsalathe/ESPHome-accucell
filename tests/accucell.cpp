#include "accucell.hpp"

// #include "HardwareSerial.h" //TMP

// extern HardwareSerial accucellRx; //TMP

Accucell6::Accucell6() :
    message_buffer_({0}),
    message_buffer_index_(0),
    set_input_undervolt_(0.0),
    charging_(false),
    cycling_(false),
    set_current_nicd_charge_(0.0),
    set_current_nicd_discharge_(0.0),
    set_current_nimh_charge_(0.0),
    set_current_nimh_discharge_(0.0),
    cycle_mode_charge_discharge_(false),
    cycle_count_(0),
    set_current_li_charge_(0.0),
    set_cell_count_li_charge_(0),
    set_current_li_discharge_(0.0),
    set_cell_count_li_discharge_(0),
    set_current_pb_charge_(0.0),
    set_cell_count_pb_charge_(0),
    menu_mode_(0),
    running_(false),
    set_voltage_nimh_discharge_(0.0),
    set_voltage_nicd_discharge_(0.0),
    set_time_limit_(0),
    set_charge_limit_(0.0),
    measured_current_(0.0),
    measured_voltage_(0.0),
    measured_input_voltage_(0.0),
    measured_charge_(0.0),
    measured_cell_voltage_Li_({0.0}),
    elapsed_minutes_(0) {
  ;
}

bool Accucell6::add_bytes_and_chack_for_complete_message(const uint8_t *new_bytes, const std::size_t len) {
  bool ret = false;
  // Serial.print("buf:'"); //TMP
  // for (int i=0; i < this->message_buffer_index_; ++i) { //TMP
  //   Serial.print(this->message_buffer_[i], HEX); //TMP
  //   Serial.print(" "); //TMP
  // } //TMP
  // Serial.println("'"); //TMP
  // Serial.print("rx:'"); //TMP
  // for (int i=0; i < len; ++i) { //TMP
  //   Serial.print(new_bytes[i], HEX); //TMP
  //   Serial.print(" "); //TMP
  // } //TMP
  // Serial.println("'"); //TMP
  // Serial.print("len:"); //TMP
  // Serial.print(len); //TMP
  // Serial.print(",idx0:"); //TMP
  // Serial.print(this->message_buffer_index_); //TMP

  if (len != 0) {
    if (this->message_buffer_index_ + len >= Accucell6::MESSAGE_BUFFER_LEN) {
      // buffer overflow: resetting buffer
      this->message_buffer_index_ = 0;
    } else {
      for (std::size_t i = 0; i < len; ++i) {
        if (new_bytes[i] == '{') {
          this->message_buffer_index_ = 0;
        } else if (new_bytes[i] == '}') {
          if (this->message_buffer_index_ == 74 && this->compare_checksum()) {
            this->parse();
            ret = true;
          }
          this->message_buffer_index_ = 0;
        } else {
          this->message_buffer_[this->message_buffer_index_] = new_bytes[i];
          ++this->message_buffer_index_;
        }
      }
    }
  }

  // Serial.print(",idx42:"); //TMP
  // Serial.print(this->message_buffer_index_); //TMP
  // Serial.print(",rx:'"); //TMP
  // Serial.println("'"); //TMP
  return ret;
}

bool Accucell6::get_is_charging() const {
  return this->charging_;
}

bool Accucell6::get_is_cycling() const {
  return this->cycling_;
}

float Accucell6::get_target_battery_current() const {
  if (running_) {
    switch (this->menu_mode_)
    {
    case Accucell6::MENU_MODE_CONFIG:
      return 0.0;
    
    case Accucell6::MENU_MODE_LI:
      return this->charging_ ? this->set_current_li_charge_ : -this->set_current_li_discharge_;
    
    case Accucell6::MENU_MODE_NIMH:
      return this->charging_ ? this->set_current_nimh_charge_ : -this->set_current_nimh_discharge_;
    
    case Accucell6::MENU_MODE_NICD:
      return this->charging_ ? this->set_current_nicd_charge_ : -this->set_current_nicd_discharge_;
    
    case Accucell6::MENU_MODE_PB:
      return this->charging_ ? this->set_current_pb_charge_ : -this->set_current_pb_charge_;
    
    case Accucell6::MENU_MODE_SAVE:
    case Accucell6::MENU_MODE_LOAD:
    default:
      return 0.0;
    }
  } else {
    return 0.0;
  }
}

bool Accucell6::get_cycle_starts_with_charge() const {
  return this->cycle_mode_charge_discharge_;
}

uint8_t Accucell6::get_cycle_count() const {
  return this->cycle_count_;
}

uint8_t Accucell6::get_target_cell_count() const {
  if (running_) {
    switch (this->menu_mode_)
    {
    case Accucell6::MENU_MODE_CONFIG:
      return 0;
    
    case Accucell6::MENU_MODE_LI:
      return this->charging_ ? this->set_cell_count_li_charge_ : this->set_cell_count_li_discharge_;
    
    case Accucell6::MENU_MODE_NIMH:
    case Accucell6::MENU_MODE_NICD:
      return 0;
    
    case Accucell6::MENU_MODE_PB:
      return this->set_cell_count_pb_charge_;
    
    case Accucell6::MENU_MODE_SAVE:
    case Accucell6::MENU_MODE_LOAD:
    default:
      return 0;
    }
  } else {
    return 0;
  }
}

uint8_t Accucell6::get_menu_mode() const {
  return this->menu_mode_;
}

bool Accucell6::get_is_running() const {
  return this->running_;
}

float Accucell6::get_measured_battery_current() const {
  if (running_) {
    return this->charging_ ? this->measured_current_ : -this->measured_current_;
  } else {
    return 0.0;
  }
}

float Accucell6::get_measured_battery_voltage() const {
  if (running_) {
    return this->measured_voltage_;
  } else {
    return 0.0;
  }
}

float Accucell6::get_measured_input_voltage() const {
  return this->measured_input_voltage_;
}

float Accucell6::get_measured_charge() const {
  if (running_) {
    return this->charging_ ? this->measured_charge_ : -this->measured_charge_;
  } else {
    return 0.0;
  }
}

float Accucell6::get_measured_cell_voltage(std::size_t index) const {
  if (running_) {
    return index < Accucell6::MAX_CELL_COUNT ? this->measured_cell_voltage_Li_[index] : 0.0;
  } else {
    return 0.0;
  }
}

uint16_t Accucell6::get_elapsed_minutes() const {
  if (running_) {
    return this->elapsed_minutes_;
  } else {
    return 0.0;
  }
}



bool Accucell6::compare_checksum() const {
  uint16_t check = 0;
  uint16_t sum = 0;

  check = ((this->message_buffer_[72] & 0xF) << 4) + (this->message_buffer_[73] & 0xF);

  for (std::size_t i = 0; i < 72; ++i) {
    sum += this->message_buffer_[i];
  }
  sum %= 0x100;

  return check == sum;
}

void Accucell6::parse() {
  this->set_input_undervolt_ = this->sanitize_data_byte(6) * 0.1;
  this->charging_ = (this->sanitize_data_byte(7) & 0x01) != 0;
  this->cycling_ = (this->sanitize_data_byte(7) & 0x10) != 0;
  this->set_current_nicd_charge_ = this->parse_A_from_dA(8);
  this->set_current_nicd_discharge_ = this->parse_A_from_dA(9);
  this->set_current_nimh_charge_ = this->parse_A_from_dA(12);
  this->set_current_nimh_discharge_ = this->parse_A_from_dA(13);
  this->cycle_mode_charge_discharge_ = (this->sanitize_data_byte(14) & 0x01) != 0;
  this->cycle_count_ = this->sanitize_data_byte(15);
  this->set_current_li_charge_ = this->parse_A_from_dA(16);
  this->set_cell_count_li_charge_ = this->sanitize_data_byte(17);
  this->set_current_li_discharge_ = this->parse_A_from_dA(18);
  this->set_cell_count_li_discharge_ = this->sanitize_data_byte(19);
  this->set_current_pb_charge_ = this->parse_A_from_dA(20);
  this->set_cell_count_pb_charge_ = this->sanitize_data_byte(21);
  this->menu_mode_ = this->sanitize_data_byte(22);
  this->running_ = (this->sanitize_data_byte(23) & 0x01) != 0;
  this->set_voltage_nimh_discharge_ = this->parse_V_from_daV_and_dV(24);
  this->set_voltage_nicd_discharge_ = this->parse_V_from_daV_and_dV(26);
  this->set_time_limit_ = this->sanitize_data_byte(29) * 10;
  this->set_charge_limit_ = this->parse_Ah_from_dAh_and_mAh(30);
  this->measured_current_ = this->parse_A_from_A_and_cA(32);
  this->measured_voltage_ = this->parse_V_from_V_and_cV(34);
  this->measured_input_voltage_ = this->parse_V_from_V_and_cV(40);
  this->measured_charge_ = this->parse_Ah_from_dAh_and_mAh(42);
  for (std::size_t i = 0; i < 6; ++i) {
    this->measured_cell_voltage_Li_[i] = this->parse_V_from_V_and_cV(44+i*2);
  }
  this->elapsed_minutes_ = this->sanitize_data_byte(68) * 100 + this->sanitize_data_byte(69);
}

uint8_t Accucell6::sanitize_data_byte(const std::size_t index) const {
  return this->message_buffer_[index] & 0x7F;
}

float Accucell6::parse_A_from_dA(const std::size_t index) const {
  return this->sanitize_data_byte(index) * 0.1;
}

float Accucell6::parse_A_from_A_and_cA(const std::size_t index) const {
  return this->sanitize_data_byte(index) + this->sanitize_data_byte(index+1) * 0.01;
}

float Accucell6::parse_V_from_daV_and_dV(const std::size_t index) const {
  return this->sanitize_data_byte(index) * 10.0 + this->sanitize_data_byte(index+1) * 0.1;
}

float Accucell6::parse_V_from_V_and_cV(const std::size_t index) const {
  return this->parse_A_from_A_and_cA(index);
}

float Accucell6::parse_Ah_from_dAh_and_mAh(const std::size_t index) const {
  return this->sanitize_data_byte(index) * 0.1 + this->sanitize_data_byte(index+1) * 0.001;
}

