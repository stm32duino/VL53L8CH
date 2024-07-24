/**
 ******************************************************************************
 * @file    VL53LMZ_ThresholdDetection.ino
 * @author  STMicroelectronics
 * @version V1.0.0
 * @date    17 July 2024
 * @brief   Arduino test application for the STMicrolectronics VL53L8CH
 *          proximity sensor satellite based on FlightSense.
 *          This application makes use of C++ classes obtained from the C
 *          components' drivers.
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
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 ******************************************************************************
 */
/*
 * To use these examples you need to connect the VL53L8CH satellite sensor directly to the Nucleo board with wires as explained below:
 * pin 1 (SPI_I2C_n) of the VL53L8CH satellite connected to pin GND of the Nucleo board
 * pin 2 (LPn) of the VL53L8CH satellite connected to pin A3 of the Nucleo board
 * pin 3 (NCS) not connected
 * pin 4 (MISO) not connected
 * pin 5 (MOSI_SDA) of the VL53L8CH satellite connected to pin D14 (SDA) of the Nucleo board
 * pin 6 (MCLK_SCL) of the VL53L8CH satellite connected to pin D15 (SCL) of the Nucleo board
 * pin 7 (PWREN) of the VL53L8CH satellite connected to pin D11 of the Nucleo board
 * pin 8 (I0VDD) of the VL53L8CH satellite not connected
 * pin 9 (3V3) of the VL53L8CH satellite connected to 3V3 of the Nucleo board
 * pin 10 (1V8) of the VL53L8CH satellite not connected
 * pin 11 (5V) of the VL53L8CH satellite not connected
 * GPIO1 of VL53L8CH satellite connected to A2 pin of the Nucleo board (not used)
 * GND of the VL53L8CH satellite connected to GND of the Nucleo board
 */

/* Includes ------------------------------------------------------------------*/

#include <vl53l8ch.h>

#ifdef ARDUINO_SAM_DUE
  #define DEV_I2C Wire1
#else
  #define DEV_I2C Wire
#endif
#define SerialPort Serial

#define LPN_PIN A3
#define PWREN_PIN 11
#define INT_PIN A2

void measure(void);
void print_result(VL53LMZ_ResultsData *Result);

// Component.
VL53L8CH sensor_vl53l8ch_top(&DEV_I2C, LPN_PIN);

bool EnableAmbient = false;
bool EnableSignal = false;
uint8_t res = VL53LMZ_RESOLUTION_4X4;
char report[256];
volatile int interruptCount = 0;
uint8_t i;
uint8_t status;

/* Setup ---------------------------------------------------------------------*/

void setup()
{

  VL53LMZ_DetectionThresholds thresholds[VL53LMZ_NB_THRESHOLDS];

  // Enable PWREN pin if present
  if (PWREN_PIN >= 0) {
    pinMode(PWREN_PIN, OUTPUT);
    digitalWrite(PWREN_PIN, HIGH);
    delay(10);
  }

  // Initialize serial for output.
  SerialPort.begin(460800);

  // Initialize I2C bus.
  DEV_I2C.begin();

  // Set interrupt pin
  pinMode(INT_PIN, INPUT_PULLUP);
  attachInterrupt(INT_PIN, measure, FALLING);

  // Configure VL53L8CH component.
  sensor_vl53l8ch_top.begin();
  status = sensor_vl53l8ch_top.init();

  // Disable thresholds detection.
  status = sensor_vl53l8ch_top.set_detection_thresholds_enable(0U);

  // Set all values to 0.
  memset(&thresholds, 0, sizeof(thresholds));

  // Configure thresholds on each active zone
  for (i = 0; i < res; i++) {
    thresholds[i].zone_num = i;
    thresholds[i].measurement = VL53LMZ_DISTANCE_MM;
    thresholds[i].type = VL53LMZ_IN_WINDOW;
    thresholds[i].mathematic_operation = VL53LMZ_OPERATION_NONE;
    thresholds[i].param_low_thresh = 200;
    thresholds[i].param_high_thresh = 600;
  }

  // Last threshold must be clearly indicated.
  thresholds[i].zone_num |= VL53LMZ_LAST_THRESHOLD;

  // Send array of thresholds to the sensor.
  status = sensor_vl53l8ch_top.set_detection_thresholds(thresholds);

  // Enable thresholds detection.
  status = sensor_vl53l8ch_top.set_detection_thresholds_enable(1U);

  // Start Measurements.
  status = sensor_vl53l8ch_top.start_ranging();
}

void loop()
{
  VL53LMZ_ResultsData Results;
  uint8_t NewDataReady = 0;
  do {
    status = sensor_vl53l8ch_top.check_data_ready(&NewDataReady);
  } while (!NewDataReady);

  if ((!status) && (NewDataReady != 0) && interruptCount) {
    interruptCount = 0;
    status = sensor_vl53l8ch_top.get_ranging_data(&Results);
    print_result(&Results);
  }

}

void print_result(VL53LMZ_ResultsData *Result)
{
  int8_t i, j, k, l;
  uint8_t zones_per_line;
  uint8_t number_of_zones = res;

  zones_per_line = (number_of_zones == 16) ? 4 : 8;

  snprintf(report, sizeof(report), "%c[2H", 27); /* 27 is ESC command */
  SerialPort.print(report);
  SerialPort.print("53L8CH Threshold Detection demo application\n");
  SerialPort.print("-------------------------------------------\n\n");
  SerialPort.print("Cell Format :\n\n");

  for (l = 0; l < VL53LMZ_NB_TARGET_PER_ZONE; l++) {
    snprintf(report, sizeof(report), " \033[38;5;10m%20s\033[0m : %20s\n", "Distance [mm]", "Status");
    SerialPort.print(report);

    if (EnableAmbient || EnableSignal) {
      snprintf(report, sizeof(report), " %20s : %20s\n", "Signal [kcps/spad]", "Ambient [kcps/spad]");
      SerialPort.print(report);
    }
  }

  SerialPort.print("\n\n");

  for (j = 0; j < number_of_zones; j += zones_per_line) {
    for (i = 0; i < zones_per_line; i++) {
      SerialPort.print(" -----------------");
    }
    SerialPort.print("\n");

    for (i = 0; i < zones_per_line; i++) {
      SerialPort.print("|                 ");
    }
    SerialPort.print("|\n");

    for (l = 0; l < VL53LMZ_NB_TARGET_PER_ZONE; l++) {
      // Print distance and status.
      for (k = (zones_per_line - 1); k >= 0; k--) {
        if (Result->nb_target_detected[j + k] > 0) {
          snprintf(report, sizeof(report), "| \033[38;5;10m%5ld\033[0m  :  %5ld ",
                   (long)Result->distance_mm[(VL53LMZ_NB_TARGET_PER_ZONE * (j + k)) + l],
                   (long)Result->target_status[(VL53LMZ_NB_TARGET_PER_ZONE * (j + k)) + l]);
          SerialPort.print(report);
        } else {
          snprintf(report, sizeof(report), "| %5s  :  %5s ", "X", "X");
          SerialPort.print(report);
        }
      }
      SerialPort.print("|\n");

      if (EnableAmbient || EnableSignal) {
        // Print Signal and Ambient.
        for (k = (zones_per_line - 1); k >= 0; k--) {
          if (Result->nb_target_detected[j + k] > 0) {
            if (EnableSignal) {
              snprintf(report, sizeof(report), "| %5ld  :  ", (long)Result->signal_per_spad[(VL53LMZ_NB_TARGET_PER_ZONE * (j + k)) + l]);
              SerialPort.print(report);
            } else {
              snprintf(report, sizeof(report), "| %5s  :  ", "X");
              SerialPort.print(report);
            }
            if (EnableAmbient) {
              snprintf(report, sizeof(report), "%5ld ", (long)Result->ambient_per_spad[j + k]);
              SerialPort.print(report);
            } else {
              snprintf(report, sizeof(report), "%5s ", "X");
              SerialPort.print(report);
            }
          } else {
            snprintf(report, sizeof(report), "| %5s  :  %5s ", "X", "X");
            SerialPort.print(report);
          }
        }
        SerialPort.print("|\n");
      }
    }
  }
  for (i = 0; i < zones_per_line; i++) {
    SerialPort.print(" -----------------");
  }
  SerialPort.print("\n");
}

void measure(void)
{
  interruptCount = 1;
}
