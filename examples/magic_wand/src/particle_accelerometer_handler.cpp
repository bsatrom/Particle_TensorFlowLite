/* Copyright 2019 The TensorFlow Authors. All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
==============================================================================*/

#include "accelerometer_handler.h"

#include <Particle.h>
#include <LSM9DS1_FIFO.h>
#include <Adafruit_Sensor.h>

LSM9DS1_FIFO accel = LSM9DS1_FIFO();

#include "constants.h"

// A buffer holding the last 200 sets of 3-channel values
float save_data[600] = {0.0};
// Most recent position in the save_data buffer
int begin_index = 0;
// True if there is not yet enough data to run inference
bool pending_initial_data = true;
// The number of measurements since we last saved one
int sample_skip_counter = 1;

bool accelerometer_started = false;

// Determine how many measurements to keep in order to meet kTargetHz
float sample_rate = 119.f;
// How often we should save a measurement during downsampling
int sample_every_n = static_cast<int>(roundf(sample_rate / kTargetHz));

TfLiteStatus SetupAccelerometer(tflite::ErrorReporter *error_reporter)
{
  // Wait until we know the serial port is ready
  while (!Serial)
  {
  }

  // Switch on the IMU
  if (!accel.begin())
  {
    error_reporter->Report("Failed to initialize accelerometer. Please reset");
    return kTfLiteError;
  }

  accelerometer_started = true;

  accel.setupAccel(accel.LSM9DS1_ACCELRANGE_2G);
  accel.setupMag(accel.LSM9DS1_MAGGAIN_4GAUSS);
  accel.setupGyro(accel.LSM9DS1_GYROSCALE_245DPS);

  error_reporter->Report("Magic starts!");

  return kTfLiteOk;
}

bool ReadAccelerometer(tflite::ErrorReporter *error_reporter, float *input,
                       int length, bool reset_buffer)
{
  // Clear the buffer if required, e.g. after a successful prediction
  if (reset_buffer)
  {
    memset(save_data, 0, 600 * sizeof(float));
    begin_index = 0;
    pending_initial_data = true;
  }
  // Keep track of whether we stored any new data
  bool new_data = false;
  // Loop through new samples and add to buffer

  if (!accelerometer_started)
  {
    if (!accel.begin())
    {
      return false;
    }

    accel.setupAccel(accel.LSM9DS1_ACCELRANGE_2G);
    accel.setupMag(accel.LSM9DS1_MAGGAIN_4GAUSS);
    accel.setupGyro(accel.LSM9DS1_GYROSCALE_245DPS);

    error_reporter->Report("Accelerometer enabled");
    accelerometer_started = true;
  }

  while (accel.accelerationAvailable())
  {
    accel.read();

    sensors_event_t accelData, magData, gyroData, temp;
    // Read each sample, removing it from the device's FIFO buffer
    if (!accel.getEvent(&accelData, &magData, &gyroData, &temp))
    {
      error_reporter->Report("Failed to read data");
      break;
    }

    // Throw away this sample unless it's the nth
    if (sample_skip_counter != sample_every_n)
    {
      sample_skip_counter += 1;
      continue;
    }
    // Write samples to our buffer, converting to milli-Gs
    // and flipping y and x order for compatibility with
    // model (sensor orientation is different on Arduino
    // Nano BLE Sense compared with SparkFun Edge)
    save_data[begin_index++] = accelData.acceleration.x * 1000; //accelData.acceleration.y * 1000;
    save_data[begin_index++] = accelData.acceleration.y * 1000; //accelData.acceleration.x * 1000;
    save_data[begin_index++] = accelData.acceleration.z * 1000;

    // Since we took a sample, reset the skip counter
    sample_skip_counter = 1;
    // If we reached the end of the circle buffer, reset
    if (begin_index >= 600)
    {
      begin_index = 0;
    }
    new_data = true;

    break;
  }

  // Skip this round if data is not ready yet
  if (!new_data)
  {
    return false;
  }

  // Check if we are ready for prediction or still pending more initial data
  if (pending_initial_data && begin_index >= 200)
  {
    pending_initial_data = false;
  }

  // Return if we don't have enough data
  if (pending_initial_data)
  {
    return false;
  }

  // Copy the requested number of bytes to the provided input tensor
  for (int i = 0; i < length; ++i)
  {
    int ring_array_index = begin_index + i - length;
    if (ring_array_index < 0)
    {
      ring_array_index += 600;
    }
    input[i] = save_data[ring_array_index];
  }

  return true;
}