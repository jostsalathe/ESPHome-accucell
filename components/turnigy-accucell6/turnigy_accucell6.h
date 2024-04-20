#pragma once

#include "esphome/core/component.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/uart/uart.h"

#include "accucell6_parser.h"

namespace esphome {
namespace turnigy_accucell6 {

class TurnigyAccucell6Component : public Component, public uart::UARTDevice {
 public:
  TurnigyAccucell6Component() = default;

  /// Manually set the rx-only mode. Defaults to false.
  void set_rx_mode_only(bool rx_mode_only);

  void setup() override;
  void dump_config() override;
  void loop() override;

  float get_setup_priority() const override;

  void set_update_interval(uint32_t val) { /* ignore */
  }
  void set_update_interval_min(uint8_t update_interval_min) { /* ignore */
  }
  void set_working_state(bool working_state);

 protected:
  Accucell6_Parser parser_;
};

} // namespace turnigy_accucell6
} // namespace esphome
