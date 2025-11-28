/**
******************************************************************************
* @file    VL53L8CH_CNHData.ino
* @brief   Example Arduino sketch using VL53L8CH class API with CNH feature.
******************************************************************************
*/

#include <vl53l8ch.h>
#include <vl53lmz_plugin_cnh.h>

#ifdef ARDUINO_SAM_DUE
  #define DEV_I2C Wire1
#else
  #define DEV_I2C Wire
#endif
#define SerialPort Serial

#define LPN_PIN A3
#define PWREN_PIN 11

VL53L8CH sensor(&DEV_I2C, LPN_PIN);

VL53LMZ_Motion_Configuration cnh_config;
cnh_data_buffer_t cnh_data_buffer;
uint32_t cnh_data_size = 0;

int32_t  *p_hist = NULL;
int8_t   *p_hist_scaler = NULL;
int32_t  *p_ambient = NULL;
int8_t   *p_ambient_scaler = NULL;

uint8_t status;
uint8_t resolution = VL53LMZ_RESOLUTION_4X4;

void setup()
{
  SerialPort.begin(460800);

  if (PWREN_PIN >= 0) {
    pinMode(PWREN_PIN, OUTPUT);
    digitalWrite(PWREN_PIN, HIGH);
    delay(10);
  }

  DEV_I2C.begin();

  status = sensor.begin();
  if (status != VL53LMZ_STATUS_OK) {
    SerialPort.println("Sensor begin failed");
    while (1);
  }

  if (status != VL53LMZ_STATUS_OK) {
    SerialPort.println("Sensor init failed");
    while (1);
  }

  status = sensor.set_resolution(resolution);
  if (status != VL53LMZ_STATUS_OK) {
    SerialPort.println("Set resolution failed");
    while (1);
  }

  status = sensor.cnh_init_config(&cnh_config, 0, 24, 4);
  if (status != VL53LMZ_STATUS_OK) {
    SerialPort.println("CNH init config failed");
    while (1);
  }

  status = sensor.cnh_create_agg_map(&cnh_config,
                                     16, 0, 0, 1, 1, 4, 4);
  if (status != VL53LMZ_STATUS_OK) {
    SerialPort.println("CNH create agg map failed");
    while (1);
  }

  status = sensor.cnh_calc_required_memory(&cnh_config, &cnh_data_size);
  if (status != VL53LMZ_STATUS_OK || cnh_data_size > VL53LMZ_CNH_MAX_DATA_BYTES) {
    SerialPort.println("CNH memory size error");
    while (1);
  }

  status = sensor.cnh_send_config(&cnh_config);
  if (status != VL53LMZ_STATUS_OK) {
    SerialPort.println("CNH send config failed");
    while (1);
  }

  status = sensor.create_output_config();
  if (status != VL53LMZ_STATUS_OK) {
    SerialPort.println("Create output config failed");
    while (1);
  }

  union Block_header cnh_data_bh;
  cnh_data_bh.idx = VL53LMZ_CNH_DATA_IDX;
  cnh_data_bh.type = 4;
  cnh_data_bh.size = cnh_data_size / 4;

  status = sensor.add_output_block(cnh_data_bh.bytes);
  if (status != VL53LMZ_STATUS_OK) {
    SerialPort.println("Add output block CNH failed");
    while (1);
  }

  status = sensor.send_output_config_and_start();
  if (status != VL53LMZ_STATUS_OK) {
    SerialPort.println("Start ranging failed");
    while (1);
  }

  SerialPort.println("Sensor and CNH configured, ranging started");
}

void loop()
{
  VL53LMZ_ResultsData results;
  uint8_t data_ready = 0;

  do {
    status = sensor.check_data_ready(&data_ready);
  } while (!data_ready);

  if (status == VL53LMZ_STATUS_OK && data_ready) {
    status = sensor.get_ranging_data(&results);
    if (status == VL53LMZ_STATUS_OK) {
      print_results(&results);
    }

    status = sensor.results_extract_block(VL53LMZ_CNH_DATA_IDX, (uint8_t *)cnh_data_buffer, cnh_data_size);
    if (status == VL53LMZ_STATUS_OK) {
      for (int agg_id = 0; agg_id < cnh_config.nb_of_aggregates; agg_id++) {
        sensor.cnh_get_block_addresses(&cnh_config, agg_id, cnh_data_buffer,
                                       &p_hist, &p_hist_scaler,
                                       &p_ambient, &p_ambient_scaler);

        float ambient = ((float) * p_ambient) / (1 << *p_ambient_scaler);
        SerialPort.print("Aggregate ");
        SerialPort.print(agg_id);
        SerialPort.print(", Ambient: ");
        SerialPort.print(ambient, 1);
        SerialPort.print(", Bins: ");

        for (int bin = 0; bin < cnh_config.feature_length; bin++) {
          float bin_val = ((float)p_hist[bin]) / (1 << p_hist_scaler[bin]);
          SerialPort.print(bin_val, 1);
          SerialPort.print(", ");
        }
        SerialPort.println();
      }
    } else {
      SerialPort.println("Failed to extract CNH data block");
    }
  }
  delay(100);
}

void print_results(VL53LMZ_ResultsData *res)
{
  SerialPort.println("Ranging Results:");
  for (int i = 0; i < resolution; i++) {
    if (res->nb_target_detected[i] > 0) {
      SerialPort.print("Zone ");
      SerialPort.print(i);
      SerialPort.print(": Distance=");
      SerialPort.print(res->distance_mm[i]);
      SerialPort.print(" mm, Status=");
      SerialPort.println(res->target_status[i]);
    } else {
      SerialPort.print("Zone ");
      SerialPort.print(i);
      SerialPort.println(": No target");
    }
  }
}