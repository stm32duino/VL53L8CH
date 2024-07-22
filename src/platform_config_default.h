/**
 ******************************************************************************
 * @file    platform_config_default.h
 * @author  STMicroelectronics
 * @version V1.0.0
 * @date    17 July 2024
 * @brief   Header file with the default platform settings.
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

#ifndef _PLATFORM_CONFIG_DEFAULT_H_
#define _PLATFORM_CONFIG_DEFAULT_H_
#ifdef __cplusplus
extern "C" {
#endif
/*
 * @brief If you want to customize these defines you can add in the application
 * code the file platform_config_custom.h file where you can override some of
 * these defines.
 */

/*
 * @brief The macro below is used to define the number of target per zone sent
 * through I2C. This value can be changed by user, in order to tune I2C
 * transaction, and also the total memory size (a lower number of target per
 * zone means a lower RAM). The value must be between 1 and 4.
 */

#ifndef VL53LMZ_NB_TARGET_PER_ZONE
#define   VL53LMZ_NB_TARGET_PER_ZONE   1U
#endif

/*
 * @brief The macro below can be used to avoid data conversion into the driver.
 * By default there is a conversion between firmware and user data. Using this macro
 * allows to use the firmware format instead of user format. The firmware format allows
 * an increased precision.
 */

// #define  VL53LMZ_USE_RAW_FORMAT

/*
 * @brief All macro below are used to configure the sensor output. User can
 * define some macros if he wants to disable selected output, in order to reduce
 * I2C access.
 */

//  #define VL53LMZ_DISABLE_AMBIENT_PER_SPAD
//  #define VL53LMZ_DISABLE_NB_SPADS_ENABLED
//  #define VL53LMZ_DISABLE_NB_TARGET_DETECTED
//  #define VL53LMZ_DISABLE_SIGNAL_PER_SPAD
//  #define VL53LMZ_DISABLE_RANGE_SIGMA_MM
//  #define VL53LMZ_DISABLE_DISTANCE_MM
//  #define VL53LMZ_DISABLE_REFLECTANCE_PERCENT
//  #define VL53LMZ_DISABLE_TARGET_STATUS
//  #define VL53LMZ_DISABLE_MOTION_INDICATOR
#ifdef __cplusplus
}
#endif
#endif  // _PLATFORM_CONFIG_DEFAULT_H_
