/**
******************************************************************************
* @file    vl53l8ch.cpp
* @author  STMicroelectronics
* @version V1.0.0
* @date    17 July 2024
* @brief   Implementation of of a VL53L8CH Time of Flight(TOF) sensor.
******************************************************************************
* @attention
*
* <h2><center>&copy; COPYRIGHT(c) 2024 STMicroelectronics</center></h2>
*
* Redistribution and use in source and binary forms, with or without modification,
* are permitted provided that the following conditions are met:
*   1. Redistributions of source code must retain the above copyright notice,
*      this list of conditions and the following disclaimer.
*   2. Redistributions in binary form must reproduce the above copyright notice,
*      this list of conditions and the following disclaimer in the documentation
*      and/or other materials provided with the distribution.
*   3. Neither the name of STMicroelectronics nor the names of its contributors
*      may be used to endorse or promote products derived from this software
*      without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
* FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
* DAMAGES(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
* CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
* OR TORT(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
* OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
******************************************************************************
*/


/* Includes ------------------------------------------------------------------*/
#include "vl53l8ch.h"


/** Constructor
  * @param i2c device I2C to be used for communication
  * @param _lpn_pin pin to be used as component LPn
  * @param _i2c_rst_pin pin to be used as component I2C_RST
  */
VL53L8CH::VL53L8CH(TwoWire *i2c, int _lpn_pin, int _i2c_rst_pin): dev_i2c(i2c), lpn_pin(_lpn_pin), i2c_rst_pin(_i2c_rst_pin)
{
  memset((void *)&_dev, 0x0, sizeof(VL53LMZ_Configuration));
  dev_spi = NULL;
  _dev.platform.address = VL53LMZ_DEFAULT_I2C_ADDRESS;
  _dev.platform.Write = VL53L8CH_io_write;
  _dev.platform.Read = VL53L8CH_io_read;
  _dev.platform.Wait = VL53L8CH_io_wait;
  _dev.platform.handle = (void *)this;
  p_dev = &_dev;
}
/** Constructor
  * @param spi object of an helper class which handles the SPI peripheral
  * @param _cs_pin the chip select pin
  * @param _lpn_pin pin to be used as component LPn
  * @param _i2c_rst_pin pin to be used as component I2C_RST
  * @param _spi_speed the SPI speed in Hz
  */
VL53L8CH::VL53L8CH(SPIClass *spi, int _cs_pin, int _lpn_pin, int _i2c_rst_pin, uint32_t _spi_speed)  : dev_spi(spi), cs_pin(_cs_pin), lpn_pin(_lpn_pin), i2c_rst_pin(_i2c_rst_pin),  spi_speed(_spi_speed)
{
  memset((void *)&_dev, 0x0, sizeof(VL53LMZ_Configuration));
  dev_i2c = NULL;
  _dev.platform.address = VL53LMZ_DEFAULT_I2C_ADDRESS;
  _dev.platform.Write = VL53L8CH_io_write;
  _dev.platform.Read = VL53L8CH_io_read;
  _dev.platform.Wait = VL53L8CH_io_wait;
  _dev.platform.handle = (void *)this;
  p_dev = &_dev;
}

/** Destructor
  */
VL53L8CH::~VL53L8CH() {}

/**
  * @brief  Initialize the pins of the sensor
  * @return Status 0 if check is OK.
  */
int VL53L8CH::begin()
{
  if (lpn_pin >= 0) {
    pinMode(lpn_pin, OUTPUT);
    digitalWrite(lpn_pin, LOW);
    delay(10);
    digitalWrite(lpn_pin, HIGH);
  }
  if (i2c_rst_pin >= 0) {
    pinMode(i2c_rst_pin, OUTPUT);
    if (dev_i2c) {
      digitalWrite(i2c_rst_pin, LOW);
    }
    if (dev_spi) {
      digitalWrite(i2c_rst_pin, HIGH);
    }
  }
  if (dev_spi) {
    // Configure CS pin
    pinMode(cs_pin, OUTPUT);
    digitalWrite(cs_pin, HIGH);
  }
  return 0;
}

/**
  * @brief  Deinitialize the pins of the sensor
  * @return Status 0 if check is OK.
  */
int VL53L8CH::end()
{
  if (lpn_pin >= 0) {
    pinMode(lpn_pin, INPUT);
  }
  if (i2c_rst_pin >= 0) {
    pinMode(i2c_rst_pin, INPUT);
  }
  /* Reset CS configuration */
  if (dev_spi) {
    // Configure CS pin
    pinMode(cs_pin, INPUT);
  }
  return 0;
}

/**
  * @brief  PowerOn the sensor
  * @return void
  */
void VL53L8CH::on(void)
{
  if (lpn_pin >= 0) {
    digitalWrite(lpn_pin, HIGH);
  }
  delay(10);
}

/**
  * @brief  PowerOff the sensor
  * @return void
  */
void VL53L8CH::off(void)
{
  if (lpn_pin >= 0) {
    digitalWrite(lpn_pin, LOW);
  }
  delay(10);
}

/**
  * @brief  Reset I2C peripheral of the sensor
  * @return void
  */
void VL53L8CH::i2c_reset(void)
{
  if (dev_i2c && i2c_rst_pin >= 0) {
    digitalWrite(i2c_rst_pin, LOW);
    delay(10);
    digitalWrite(i2c_rst_pin, HIGH);
    delay(10);
    digitalWrite(i2c_rst_pin, LOW);
    delay(10);
  }
}

/**
  * @brief Check if the VL53L8CH sensor is alive(responding to I2C communication).
  * @param p_is_alive Pointer to the variable that will be set to indicate if the sensor is alive.
  * @return Status 0 if check is OK.
  */
uint8_t VL53L8CH::is_alive(uint8_t  *p_is_alive)
{
  return vl53lmz_is_alive(p_dev, p_is_alive);
}

/**
  * @brief Initialize the VL53L8CH sensor.
  * @return Status 0 if initialization is OK.
  */
uint8_t VL53L8CH::init(void)
{
  return vl53lmz_init(p_dev);
}

/**
  * @brief Set the I2C address of the VL53L8CH sensor.
  * @param i2c_address The new I2C address to be set for the sensor.
  * @return Status 0 if the address is set correctly.
  */
uint8_t VL53L8CH::set_i2c_address(uint16_t i2c_address)
{
  return vl53lmz_set_i2c_address(p_dev, i2c_address);
}

/**
  * @brief Get the current power mode of the VL53L8CH sensor.
  * @param p_power_mode Pointer to the variable that will be set to the current power mode.
  * @return Status 0 if the power mode is retrieved successfully.
  */
uint8_t VL53L8CH::get_power_mode(uint8_t *p_power_mode)
{
  return vl53lmz_get_power_mode(p_dev, p_power_mode);
}

/**
  * @brief Set the power mode of the VL53L8CH sensor.
  * @param power_mode The power mode to be set(e.g., sleep or wakeup).
  * @return Status 0 if the power mode is set successfully, or 127 if the requested power mode is not valid.
  */
uint8_t VL53L8CH::set_power_mode(uint8_t power_mode)
{
  return vl53lmz_set_power_mode(p_dev, power_mode);
}

/**
  * @brief Start a ranging session with the VL53L8CH sensor.
  * @return Status 0 if the ranging session starts successfully.
  */
uint8_t VL53L8CH::start_ranging(void)
{
  return vl53lmz_start_ranging(p_dev);
}

/**
  * @brief Stop the current ranging session with the VL53L8CH sensor.
  * @return Status 0 if the ranging session stops successfully.
  */
uint8_t VL53L8CH::stop_ranging(void)
{
  return vl53lmz_stop_ranging(p_dev);
}

/**
  * @brief Check if new ranging data is ready.
  * @param p_isReady Pointer to the variable that will be updated to indicate if new data is ready.
  * @return Status 0 if the data ready check is successful.
  */
uint8_t VL53L8CH::check_data_ready(uint8_t *p_isReady)
{
  return vl53lmz_check_data_ready(p_dev, p_isReady);
}

/**
  * @brief Get the ranging data from the VL53L8CH sensor.
  * @param p_results Pointer to the results data structure where the ranging data will be stored.
  * @return Status 0 if the data is retrieved successfully.
  */
uint8_t VL53L8CH::get_ranging_data(VL53LMZ_ResultsData *p_results)
{
  return vl53lmz_get_ranging_data(p_dev, p_results);
}

/**
  * @brief Get the current resolution of the VL53L8CH sensor.
  * @param p_resolution Pointer to the variable that will be set to the current resolution.
  * @return Status 0 if the resolution is retrieved successfully.
  */
uint8_t VL53L8CH::get_resolution(uint8_t *p_resolution)
{
  return vl53lmz_get_resolution(p_dev, p_resolution);
}

/**
  * @brief Set a new resolution for the VL53L8CH sensor.
  * @param resolution The new resolution to be set.
  * @return Status 0 if the resolution is set successfully.
  */
uint8_t VL53L8CH::set_resolution(uint8_t resolution)
{
  return vl53lmz_set_resolution(p_dev, resolution);
}

/**
  * @brief Get the current ranging frequency of the VL53L8CH sensor in Hz.
  * @param p_frequency_hz Pointer to the variable that will be set to the current ranging frequency.
  * @return Status 0 if the ranging frequency is retrieved successfully.
  */
uint8_t VL53L8CH::get_ranging_frequency_hz(uint8_t *p_frequency_hz)
{
  return vl53lmz_get_ranging_frequency_hz(p_dev, p_frequency_hz);
}

/**
  * @brief Set a new ranging frequency for the VL53L8CH sensor in Hz.
  * @param frequency_hz The new ranging frequency to be set.
  * @return Status 0 if the ranging frequency is set successfully, or 127 if the value is not correct.
  */
uint8_t VL53L8CH::set_ranging_frequency_hz(uint8_t frequency_hz)
{
  return vl53lmz_set_ranging_frequency_hz(p_dev, frequency_hz);
}

/**
  * @brief Get the current integration time of the VL53L8CH sensor in ms.
  * @param p_time_ms Pointer to the variable that will be set to the current integration time.
  * @return Status 0 if the integration time is retrieved successfully.
  */
uint8_t VL53L8CH::get_integration_time_ms(uint32_t *p_time_ms)
{
  return vl53lmz_get_integration_time_ms(p_dev, p_time_ms);
}

/**
  * @brief Set a new integration time for the VL53L8CH sensor in ms.
  * @param integration_time_ms The new integration time to be set.
  * @return Status 0 if the integration time is set successfully.
  */
uint8_t VL53L8CH::set_integration_time_ms(uint32_t integration_time_ms)
{
  return vl53lmz_set_integration_time_ms(p_dev, integration_time_ms);
}

/**
  * @brief Get the current sharpener percentage of the VL53L8CH sensor.
  * @param p_sharpener_percent Pointer to the variable that will be set to the current sharpener percentage.
  * @return Status 0 if the sharpener percentage is retrieved successfully.
  */
uint8_t VL53L8CH::get_sharpener_percent(uint8_t *p_sharpener_percent)
{
  return vl53lmz_get_sharpener_percent(p_dev, p_sharpener_percent);
}

/**
  * @brief Set a new sharpener percentage for the VL53L8CH sensor.
  * @param sharpener_percent The new sharpener percentage to be set.
  * @return Status 0 if the sharpener percentage is set successfully.
  */
uint8_t VL53L8CH::set_sharpener_percent(uint8_t sharpener_percent)
{
  return vl53lmz_set_sharpener_percent(p_dev, sharpener_percent);
}
/**
  * @brief Get the current target order of the VL53L8CH sensor(closest or strongest).
  * @param p_target_order Pointer to the variable that will be set to the current target order.
  * @return Status 0 if the target order is retrieved successfully.
  */
uint8_t VL53L8CH::get_target_order(uint8_t *p_target_order)
{
  return vl53lmz_get_target_order(p_dev, p_target_order);
}

/**
  * @brief Set a new target order for the VL53L8CH sensor.
  * @param target_order The new target order to be set.
  * @return Status 0 if the target order is set successfully, or 127 if the target order is unknown.
  */
uint8_t VL53L8CH::set_target_order(uint8_t target_order)
{
  return vl53lmz_set_target_order(p_dev, target_order);
}

/**
  * @brief Get the current ranging mode of the VL53L8CH sensor.
  * @param p_ranging_mode Pointer to the variable that will be set to the current ranging mode.
  * @return Status 0 if the ranging mode is retrieved successfully.
  */
uint8_t VL53L8CH::get_ranging_mode(uint8_t *p_ranging_mode)
{
  return vl53lmz_get_ranging_mode(p_dev, p_ranging_mode);
}

/**
  * @brief Set the ranging mode of the VL53L8CH sensor.
  * @param ranging_mode The new ranging mode to be set.
  * @return Status 0 if the ranging mode is set successfully.
  */
uint8_t VL53L8CH::set_ranging_mode(uint8_t ranging_mode)
{
  return vl53lmz_set_ranging_mode(p_dev, ranging_mode);
}

/**
  * @brief Check if the synchronization pin of the VL53L8CH sensor is enabled.
  * @param p_is_sync_pin_enabled Pointer to the variable that will be updated to indicate if the sync pin is enabled.
  * @return Status 0 if the sync pin status is retrieved successfully.
  */
uint8_t VL53L8CH::get_external_sync_pin_enable(uint8_t *p_is_sync_pin_enabled)
{
  return vl53lmz_get_external_sync_pin_enable(p_dev, p_is_sync_pin_enabled);
}

/**
 * @brief This function is used to disable the VCSEL charge pump
 * This optimizes the power consumption of the device
 * To be used only if AVDD = 3.3V
 */
uint8_t VL53L8CH::enable_internal_cp(void)
{
  return vl53lmz_enable_internal_cp(p_dev);
}
/**
* @brief This function is used to disable the VCSEL charge pump
* This optimizes the power consumption of the device
* To be used only if AVDD = 3.3V
*/
uint8_t VL53L8CH::disable_internal_cp(void)
{
  return vl53lmz_disable_internal_cp(p_dev);
}

/**
  * @brief Enable or disable the synchronization pin of the VL53L8CH sensor.
  * @param enable_sync_pin Set to 1 to enable the sync pin, or 0 to disable it.
  * @return Status 0 if the sync pin is set successfully.
  */
uint8_t VL53L8CH::set_external_sync_pin_enable(uint8_t enable_sync_pin)
{
  return vl53lmz_set_external_sync_pin_enable(p_dev, enable_sync_pin);
}

/**
 * @brief This function gets the current Glare Filter config.
 * @param (uint8_t) *p_threshold_x10: Pointer to variable to place threshold_x10 value.
 * @param (int16_t) *p_max_range: Pointer to variable to place max_range value.
 * @return (uint8_t) status : 0 if settings where applied successfully.
 */
uint8_t VL53L8CH::get_glare_filter_cfg(uint8_t *p_threshold_pc_x10, int16_t *p_max_range)
{
  return vl53lmz_get_glare_filter_cfg(p_dev, p_threshold_pc_x10, p_max_range);
}

/**
 * @brief This function sets the current Glare Filter config.
 * @param (uint8_t) threshold_x10: GlareFilter threshold value. Percentage scaled by a factor of 10, i.e. to set a threshold of 2.5% set a value of 25. Setting to zero completely disables the GlareFilter.
 * @param (int16_t) max_range: Maximum range for GlareFilter to operate. Valid range of values is 10mm to 1000mm.
 * @return (uint8_t) status : 0 if settings where applied successfully.
 */
uint8_t VL53L8CH::set_glare_filter_cfg(uint8_t threshold_pc_x10, int16_t max_range)
{
  return vl53lmz_set_glare_filter_cfg(p_dev, threshold_pc_x10, max_range);
}
/**
  * @brief Read 'extra data' from the VL53L8CH sensor using DCI(Device Configuration Interface).
  * @param data Pointer to the data array or casted structure where the read data will be stored.
  * @param index Index of the required value to be read.
  * @param data_size Size of the data array or casted structure(use sizeof() function).
  * @return Status 0 if the read operation is successful.
  */
uint8_t VL53L8CH::dci_read_data(uint8_t *data, uint32_t index, uint16_t data_size)
{
  return vl53lmz_dci_read_data(p_dev, data, index, data_size);
}

/**
  * @brief Write 'extra data' to the VL53L8CH sensor using DCI(Device Configuration Interface).
  * @param data Pointer to the data array or casted structure containing the data to be written.
  * @param index Index of the required value to be written.
  * @param data_size Size of the data array or casted structure(use sizeof() function).
  * @return Status 0 if the write operation is successful.
  */
uint8_t VL53L8CH::dci_write_data(uint8_t *data, uint32_t index, uint16_t data_size)
{
  return vl53lmz_dci_write_data(p_dev, data, index, data_size);
}

/**
  * @brief Replace 'extra data' in the VL53L8CH sensor using DCI(Device Configuration Interface).
  * @param data Pointer to the data array or casted structure where the current data is stored.
  * @param index Index of the required value to be replaced.
  * @param data_size Size of the data array or casted structure(use sizeof() function).
  * @param new_data Pointer to the new data array containing the fields to be replaced.
  * @param new_data_size Size of the new data array.
  * @param new_data_pos Position of the new data in the buffer.
  * @return Status 0 if the replace operation is successful.
  */
uint8_t VL53L8CH::dci_replace_data(uint8_t *data, uint32_t index, uint16_t data_size, uint8_t *new_data, uint16_t new_data_size, uint16_t new_data_pos)
{
  return vl53lmz_dci_replace_data(p_dev,
                                  data,
                                  index,
                                  data_size,
                                  new_data,
                                  new_data_size,
                                  new_data_pos);
}

/**
 * @brief This function creates the output configuration that will be sent to
 * the device by the vl53lmz_send_output_config_and_start() functions.
 * @return (uint8_t) status : 0 if OK
 */
uint8_t VL53L8CH::create_output_config(void)
{
  return vl53lmz_create_output_config(p_dev);
}

/**
 * @brief This function sends the output configuration previously created by
 * vl53lmz_create_output_config() to the device. It then commands the device
 *  to start streaming data.
 * @return (uint8_t) status : 0 if OK
 */
uint8_t VL53L8CH::send_output_config_and_start(void)
{
  return vl53lmz_send_output_config_and_start(p_dev);
}

/**
 * @brief This function adds the specified block header to the end of the
 * g_output_config list. It also enables this output in the g_output_bh_enable
 * structure.
 * @param (uint32_t) block_header : Block Header to add to output_config list.
 *  * @return (uint8_t) status : 0 if OK
 */
uint8_t VL53L8CH::add_output_block(uint32_t block_header)
{
  return vl53lmz_add_output_block(p_dev, block_header);
}

/**
 * @brief This function disables the specified block output in the
 * g_output_bh_enable structure. If the block is not in the g_output_config list
 * ,or, not enabled then it will do nothing (and return success).
 * @param (uint32_t) block_header : Block Header to disable output_config list.
 *  * @return (uint8_t) status : 0 if OK
 */
uint8_t VL53L8CH::disable_output_block(uint32_t block_header)
{
  return vl53lmz_disable_output_block(p_dev, block_header);
}

/**
 * @brief This function extracts the specified data block from a Results
 * packet of data.
 * @param (uint32_t) blk_index : Index of required block..
 * @param (uint8_t) *p_data : Pointer to the destination area to place the data.
 * @param (uint16_t)*data_size : This field must be the structure or array size
 * (using sizeof() function).
 * @return (uint8_t) status : 0 if OK
 */
uint8_t VL53L8CH::results_extract_block(uint32_t blk_index, uint8_t *p_data, uint16_t data_size)
{
  return vl53lmz_results_extract_block(p_dev, blk_index, p_data, data_size);
}

/**
  * @brief Check if the detection thresholds are enabled on the VL53L8CH sensor.
  * @param p_enabled Pointer to the variable that will be set to 1 if thresholds are enabled, or 0 if disabled.
  * @return Status 0 if the check is successful.
  */
uint8_t VL53L8CH::get_detection_thresholds_enable(uint8_t *p_enabled)
{
  return vl53lmz_get_detection_thresholds_enable(p_dev, p_enabled);
}

/**
  * @brief Enable or disable the detection thresholds on the VL53L8CH sensor.
  * @param enabled Set to 1 to enable thresholds, or 0 to disable them.
  * @return Status 0 if the thresholds are set successfully.
  */
uint8_t VL53L8CH::set_detection_thresholds_enable(uint8_t enabled)
{
  return vl53lmz_set_detection_thresholds_enable(p_dev, enabled);
}

/**
  * @brief Get the detection thresholds from the VL53L8CH sensor.
  * @param p_thresholds Pointer to the array where the detection thresholds will be stored.
  * @return Status 0 if the thresholds are retrieved successfully.
  */
uint8_t VL53L8CH::get_detection_thresholds(VL53LMZ_DetectionThresholds *p_thresholds)
{
  return vl53lmz_get_detection_thresholds(p_dev, p_thresholds);
}

/**
  * @brief Set the detection thresholds on the VL53L8CH sensor.
  * @param p_thresholds Pointer to the array containing the new detection thresholds.
  * @return Status 0 if the thresholds are programmed successfully.
  */
uint8_t VL53L8CH::set_detection_thresholds(VL53LMZ_DetectionThresholds *p_thresholds)
{
  return vl53lmz_set_detection_thresholds(p_dev, p_thresholds);
}

/**
  * @brief Get the status of the auto-stop feature on the VL53L8CH sensor when using detection thresholds.
  * @param p_auto_stop Pointer to the variable that will be set to 1 if auto-stop is enabled, or 0 if disabled.
  * @return Status 0 if the auto-stop status is retrieved successfully.
  */
uint8_t VL53L8CH::get_detection_thresholds_auto_stop(uint8_t *p_auto_stop)
{
  return vl53lmz_get_detection_thresholds_auto_stop(p_dev, p_auto_stop);
}

/**
  * @brief Enable or disable the auto-stop feature on the VL53L8CH sensor when using detection thresholds.
  * @param auto_stop Set to 1 to enable auto-stop, or 0 to disable it.
  * @return Status 0 if the auto-stop feature is set successfully.
  */
uint8_t VL53L8CH::set_detection_thresholds_auto_stop(uint8_t auto_stop)
{
  return vl53lmz_set_detection_thresholds_auto_stop(p_dev, auto_stop);
}


/**
  * @brief Initialize the motion indicator with the default monitoring range.
  * @param p_motion_config Pointer to the structure containing the initialized motion configuration.
  * @param resolution The desired resolution, defined by macros VL53LMZ_RESOLUTION_4X4 or VL53LMZ_RESOLUTION_8X8.
  * @return Status 0 if initialization is successful, or 127 if the resolution is unknown.
  */
uint8_t VL53L8CH::motion_indicator_init(VL53LMZ_Motion_Configuration *p_motion_config, uint8_t resolution)
{
  return vl53lmz_motion_indicator_init(p_dev, p_motion_config, resolution);
}

/**
  * @brief Change the working distance range of the motion indicator.
  * @param p_motion_config Pointer to the structure containing the motion configuration.
  * @param distance_min_mm Minimum distance for the motion indicator(minimum value 400mm, maximum 4000mm).
  * @param distance_max_mm Maximum distance for the motion indicator(minimum value 400mm, maximum 4000mm).
  * @return Status 0 if the configuration is successful, or 127 if an argument is invalid.
  */
uint8_t VL53L8CH::motion_indicator_set_distance_motion(VL53LMZ_Motion_Configuration  *p_motion_config, uint16_t distance_min_mm, uint16_t distance_max_mm)
{
  return vl53lmz_motion_indicator_set_distance_motion(p_dev, p_motion_config, distance_min_mm, distance_max_mm);
}

/**
  * @brief Update the internal motion indicator map to a new resolution.
  * @param p_motion_config Pointer to the structure containing the motion configuration.
  * @param resolution The desired SCI resolution, defined by macros VL53LMZ_RESOLUTION_4X4 or VL53LMZ_RESOLUTION_8X8.
  * @return Status 0 if the update is successful, or 127 if the resolution is unknown.
  */
uint8_t VL53L8CH::motion_indicator_set_resolution(VL53LMZ_Motion_Configuration *p_motion_config, uint8_t resolution)
{
  return vl53lmz_motion_indicator_set_resolution(p_dev, p_motion_config, resolution);
}

/**
  * @brief Start the VL53L8CH sensor to calibrate Xtalk, recommended for use with a coverglass.
  * @param reflectance_percent Target reflectance in percent, between 1 and 99%. ST recommends a 3% target reflectance for better efficiency.
  * @param nb_samples Number of samples used for calibration. More samples increase accuracy but also calibration time. Minimum is 1, maximum is 16.
  * @param distance_mm Target distance in mm for calibration. Minimum allowed is 600mm, maximum is 3000mm. The target must stay in Full FOV.
  * @return Status 0 if calibration is successful, 127 if an argument has an incorrect value, or 255 if something failed.
  */
uint8_t VL53L8CH::calibrate_xtalk(uint16_t reflectance_percent, uint8_t nb_samples, uint16_t distance_mm)
{
  return vl53lmz_calibrate_xtalk(p_dev, reflectance_percent, nb_samples, distance_mm);
}

/**
  * @brief Get the Xtalk calibration data buffer, available after using vl53l8ch_calibrate_xtalk().
  * @param p_xtalk_data Buffer to store Xtalk data, size defined by VL53LMZ_XTALK_SIZE macro.
  * @return Status 0 if buffer reading is successful.
  */
uint8_t VL53L8CH::get_caldata_xtalk(uint8_t *p_xtalk_data)
{
  return vl53lmz_get_caldata_xtalk(p_dev, p_xtalk_data);
}

/**
  * @brief Set the Xtalk calibration data buffer, can be used to override the default Xtalk buffer.
  * @param p_xtalk_data Buffer containing Xtalk data, size defined by VL53LMZ_XTALK_SIZE macro.
  * @return Status 0 if buffer is set successfully.
  */
uint8_t VL53L8CH::set_caldata_xtalk(uint8_t *p_xtalk_data)
{
  return vl53lmz_set_caldata_xtalk(p_dev, p_xtalk_data);
}

/**
  * @brief Get the Xtalk margin, used to increase the Xtalk threshold and avoid false positives after calibration.
  * @param p_xtalk_margin Pointer to store the current Xtalk margin in kcps/spads.
  * @return Status 0 if reading is successful.
  */
uint8_t VL53L8CH::get_xtalk_margin(uint32_t *p_xtalk_margin)
{
  return vl53lmz_get_xtalk_margin(p_dev, p_xtalk_margin);
}

/**
  * @brief Set the Xtalk margin, used to increase the Xtalk threshold and avoid false positives after calibration.
  * @param xtalk_margin New Xtalk margin in kcps/spads. Minimum value is 0, maximum is 10,000 kcps/spads.
  * @return Status 0 if the new margin is set successfully, or 127 if the margin is invalid.
  */
uint8_t VL53L8CH::set_xtalk_margin(uint32_t xtalk_margin)
{
  return vl53lmz_set_xtalk_margin(p_dev, xtalk_margin);
}

/**
 * @brief Function to initialise the CNH configuration structure.
 * @param (int16_t) start_bin : Start bin within device histogram to for CNH data.
 * @param (int16_t) num_bins : Number of bin from device histogram for CNH data.
 * @param (int16_t) sub_sample : Sub-sample factor to reduce histogram bins by for CNH data.
 * @return (uint8_t) status :  0 if configuration is OK
 */
uint8_t VL53L8CH::cnh_init_config(VL53LMZ_Motion_Configuration *p_mi_config, int16_t start_bin, int16_t num_bins, int16_t sub_sample)
{
  return vl53lmz_cnh_init_config(p_mi_config, start_bin, num_bins, sub_sample);
}

/**
 * @brief Function to create aggregate map CNH.
 * @param (int16_t) resolution : Mode sensor is operating in, 16 for 4x4 mode, 64 for 8x8 mode.
 * @param (int16_t) start_x : Start zone X location.
 * @param (int16_t) start_y : Start zone Y location.
 * @param (int16_t) merge_x : Merge factor for zones in X direction.
 * @param (int16_t) merge_y : Merge factor for zones in Y direction.
 * @param (int16_t) cols : Number of columns for the aggregate map.
 * @param (int16_t) rows : Number of rows for the aggregate map.
 * @return (uint8_t) status :  0 if configuration is OK
 */
uint8_t VL53L8CH::cnh_create_agg_map(VL53LMZ_Motion_Configuration *p_mi_config, int16_t resolution, int16_t start_x, int16_t start_y, int16_t merge_x, int16_t merge_y, int16_t cols, int16_t rows)
{
  return vl53lmz_cnh_create_agg_map(p_mi_config, resolution, start_x, start_y, merge_x, merge_y, cols, rows);
}

/**
 * @brief Calculate the size of persistent memory required on the sensor for the MI or CNH configuration.
 * @param (int32_t) *p_mem_size : Positive value if CNH configuration is good. Returns negative value if bad CNH configuration.
 * @return (uint8_t) status : 0 if configuration is OK
 */
uint8_t VL53L8CH::cnh_calc_required_memory(VL53LMZ_Motion_Configuration *p_mi_config, uint32_t *p_mem_size)
{
  return vl53lmz_cnh_calc_required_memory(p_mi_config, p_mem_size);
}

/**
 * @brief Function to calculate minimum and maximum distances for the CNH configuration.
 * @param (int16_t) *p_min_distance : Minimum distance, in mm.
 * @param (int16_t) *p_max_distance_x : Maximum distance, in mm.
 * @return (uint8_t) status :  0 if configuration is OK
 */
uint8_t VL53L8CH::cnh_calc_min_max_distance(VL53LMZ_Motion_Configuration *p_mi_config, int16_t *p_min_distance, int16_t *p_max_distance)
{
  return vl53lmz_cnh_calc_min_max_distance(p_mi_config, p_min_distance, p_max_distance);
}

/**
 * @brief Function to send the CNH configuration to the sensor.
 * @param (VL53LMZ_Motion_Configuration) *p_mi_config : Motion Indicator configuration structure used by CNH.
 * @return (uint8_t) status : 0 if programming is OK
 */
uint8_t VL53L8CH::cnh_send_config(VL53LMZ_Motion_Configuration *p_mi_config)
{
  return vl53lmz_cnh_send_config(p_dev, p_mi_config);
}

/**
 * @brief Function to calculate location within the CNH buffer of various blocks.
 * @param (int32_t) agg_id : aggregate ID to get the dat locations for
 * @param (cnh_data_buffer_t) mi_persistent_array : raw CNH data buffer
 * @param (int32_t) **p_hist : Pointer to histogram array
 * @param (int8_t) **p_hist_scaler : Pointer to histogram data scaler array
 * @param (int32_t) **p_ambient : Pointer to pointer to ambient value
 * @param (int8_t) **p_ambient_scaler : Pointer to pointer to ambient data scaler value
 * @return (uint8_t) status : 0 if no error
 */

uint8_t VL53L8CH::cnh_get_block_addresses(VL53LMZ_Motion_Configuration *p_mi_config, int32_t agg_id, cnh_data_buffer_t mi_persistent_array, int32_t **p_hist, int8_t  **p_hist_scaler, int32_t **p_ambient, int8_t **p_ambient_scaler)
{
  return vl53lmz_cnh_get_block_addresses(p_mi_config, agg_id, mi_persistent_array, p_hist, p_hist_scaler, p_ambient, p_ambient_scaler);
}

/**
 * @brief Function to retrieve the Reference Residual value from the raw CNH buffer
 * @param (cnh_data_buffer_t) mi_persistent_array : raw CNH data buffer
 * @return (uint32_t) ref_residual : Reference Residual value (11 fractional bits)
 */
uint32_t VL53L8CH::cnh_get_ref_residual(cnh_data_buffer_t mi_persistent_array)
{
  return vl53lmz_cnh_get_ref_residual(mi_persistent_array);
}

/**
 * @brief This function gets the current ranging frequency in Hz scaled up by
 * a factor of 256 to enable non-integer values to be returned.
 * Ranging frequency corresponds to the time between each measurement.
 * @param (VL53LMZ_Configuration) *p_dev : VL53LMZ configuration structure.
 * @param (uint8_t) *p_frequency_x256: Contains the ranging frequency scaled
 * to 256 x Hz.
 * @return (uint8_t) status : 0 if ranging frequency is OK.
 */
uint8_t VL53L8CH::get_ranging_frequency_x256(VL53LMZ_Configuration *p_dev, uint16_t *p_frequency_x256)
{
  return vl53lmz_get_ranging_frequency_x256(p_dev, p_frequency_x256);
}

/**
 * @brief This function sets a new ranging frequency in Hz scaled up by 256
 * to enable non-integer values to be set.
 * Ranging frequencycorresponds to the measurements frequency. This setting
 * depends on the resolution, so please select your resolution before using
 * this function.
 * @param (VL53LMZ_Configuration) *p_dev : VL53LMZ configuration structure.
 * @param (uint8_t) frequency_x256 : Contains the ranging frequency in Hz.
 * - For 4x4, min and max allowed values are : [1*256;60*256]
 * - For 8x8, min and max allowed values are : [1*256;15*256]
 * @return (uint8_t) status : 0 if ranging frequency is OK, or 127 if the value
 * is not correct.
 */
uint8_t VL53L8CH::set_ranging_frequency_x256(VL53LMZ_Configuration *p_dev, uint16_t frequency_x256)
{
  return vl53lmz_set_ranging_frequency_x256(p_dev, frequency_x256);
}

/**
 * @brief This function is used to get the number of frames between 2 temperature
 * compensation.
 * @param (VL53LMZ_Configuration) *p_dev : VL53LMZ configuration structure.
 * @param (uint32_t) *p_repeat_count : Number of frames before next temperature
 * compensation. Set to 0 to disable the feature (default configuration).
 */
uint8_t VL53L8CH::get_VHV_repeat_count(VL53LMZ_Configuration *p_dev, uint32_t *p_repeat_count)
{
  return vl53lmz_get_VHV_repeat_count(p_dev, p_repeat_count);
}

/**
 * @brief This function is used to set a periodic temperature compensation. By
 * setting a repeat count different to 0 the firmware automatically runs a
 * temperature calibration every N frames.
 * default the repeat count is set to 0
 * @param (VL53LMZ_Configuration) *p_dev : VL53LMZ configuration structure.
 * @param (uint32_t) repeat_count : Number of frames between temperature
 * compensation. Set to 0 to disable the feature (default configuration).
 */
uint8_t VL53L8CH::set_VHV_repeat_count(VL53LMZ_Configuration *p_dev, uint32_t repeat_count)
{
  return vl53lmz_set_VHV_repeat_count(p_dev, repeat_count);
}

uint8_t VL53L8CH_io_write(void *handle, uint16_t RegisterAddress, uint8_t *p_values, uint32_t size)
{
  return ((VL53L8CH *)handle)->IO_Write(RegisterAddress, p_values, size);
}

uint8_t VL53L8CH_io_read(void *handle, uint16_t RegisterAddress, uint8_t *p_values, uint32_t size)
{
  return ((VL53L8CH *)handle)->IO_Read(RegisterAddress, p_values, size);
}

uint8_t VL53L8CH_io_wait(void *handle, uint32_t ms)
{
  return ((VL53L8CH *)handle)->IO_Wait(ms);
}