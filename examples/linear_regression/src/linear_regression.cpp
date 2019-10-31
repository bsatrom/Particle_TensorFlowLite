/* Copyright 2019 Particle. All Rights Reserved.

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

#include <TensorFlowLite.h>
#include <Particle.h>
#include "Adafruit_HX8357.h"
#include "Adafruit_STMPE610.h"

SYSTEM_MODE(MANUAL);
SYSTEM_THREAD(ENABLED);

#include "output_handler.h"
#include "linear_regression_model_data.h"
#include "tensorflow/lite/experimental/micro/kernels/all_ops_resolver.h"
#include "tensorflow/lite/experimental/micro/micro_error_reporter.h"
#include "tensorflow/lite/experimental/micro/micro_interpreter.h"
#include "tensorflow/lite/schema/schema_generated.h"
#include "tensorflow/lite/version.h"

#define TFT_DC D5
#define TFT_CS D4
#define STMPE_CS D3
#define SD_CS D2
#define TFT_RST -1

Adafruit_HX8357 tft = Adafruit_HX8357(TFT_CS, TFT_DC, TFT_RST);
Adafruit_STMPE610 touch = Adafruit_STMPE610(STMPE_CS);

namespace
{
tflite::ErrorReporter *error_reporter = nullptr;
const tflite::Model *model = nullptr;
tflite::MicroInterpreter *interpreter = nullptr;
TfLiteTensor *input = nullptr;
TfLiteTensor *output = nullptr;
bool startInference = false;
bool inferenceDone = false;
bool resetInference = false;

// Create an area of memory to use for input, output, and intermediate arrays.
// Finding the minimum value for your model may require some trial and error.
constexpr int kTensorArenaSize = 2 * 1024;
uint8_t tensor_arena[kTensorArenaSize];
} // namespace

float randFloat(float min, float max)
{
  return ((max - min) * ((float)rand() / RAND_MAX)) + min;
}

void setup()
{
  tft.begin();
  tft.fillScreen(HX8357_BLACK);
  tft.setRotation(1);
  tft.setTextSize(3);
  tft.setCursor(70, 10);
  tft.setTextColor(HX8357_WHITE);
  tft.println("TFLite for Particle");

  touch.begin();
  tft.setTextSize(2);
  tft.setCursor(50, 300);
  tft.println("Touch screen to begin inference");

  // Set up logging. Google style is to avoid globals or statics because of
  // lifetime uncertainty, but since this has a trivial destructor it's okay.
  // NOLINTNEXTLINE(runtime-global-variables)
  static tflite::MicroErrorReporter micro_error_reporter;
  error_reporter = &micro_error_reporter;

  model = tflite::GetModel(g_linear_regresion_model_data);
  if (model->version() != TFLITE_SCHEMA_VERSION)
  {
    error_reporter->Report(
        "Model provided is schema version %d not equal "
        "to supported version %d.",
        model->version(), TFLITE_SCHEMA_VERSION);
    return;
  }

  // This pulls in all the operation implementations we need.
  // NOLINTNEXTLINE(runtime-global-variables)
  static tflite::ops::micro::AllOpsResolver resolver;

  // Build an interpreter to run the model with.
  static tflite::MicroInterpreter static_interpreter(
      model, resolver, tensor_arena, kTensorArenaSize, error_reporter);
  interpreter = &static_interpreter;

  // Allocate memory from the tensor_arena for the model's tensors.
  TfLiteStatus allocate_status = interpreter->AllocateTensors();
  if (allocate_status != kTfLiteOk)
  {
    error_reporter->Report("AllocateTensors() failed");
    return;
  }

  // Obtain pointers to the model's input and output tensors.
  input = interpreter->input(0);
  output = interpreter->output(0);
}

void loop()
{
  if (touch.touched())
  {
    if (!inferenceDone)
    {
      // Clear the text at the bottom
      tft.setCursor(50, 300);
      tft.fillRect(50, 300, 400, 30, HX8357_BLACK);

      // Set-up a Graph on the screen
      tft.drawFastVLine(10, 10, 300, HX8357_WHITE);
      tft.drawFastHLine(10, 310, 460, HX8357_WHITE);

      startInference = true;
    }
    else
    {
      resetInference = true;
      inferenceDone = false;
    }
  }

  if (startInference)
  {
    for (size_t i = 0; i < 100; i++)
    {
      // Place our calculated x value in the model's input tensor
      float x_val = randFloat(0, 1);
      input->data.f[0] = x_val;

      TfLiteStatus invoke_status = interpreter->Invoke();

      if (invoke_status != kTfLiteOk)
      {
        error_reporter->Report("Invoke failed on x_val: %f\n",
                               static_cast<double>(x_val));
        return;
      }

      float y_val = output->data.f[0];

      // Plot each X and Y value on the TFT Screen as they come in
      float xPos = map(x_val, 0.f, 1.f, 20.f, 460.f);
      float yPos = map(y_val, -1.f, 1.f, 330.f, 50.f);
      tft.fillCircle((int)xPos, (int)yPos, 5, HX8357_RED);

      // Output the results. A custom HandleOutput function can be implemented
      // for each supported hardware target.
      HandleOutput(error_reporter, x_val, y_val);
    }

    startInference = false;
    inferenceDone = true;
  }

  if (resetInference)
  {
    tft.fillScreen(HX8357_BLACK);
    tft.setRotation(1);
    tft.setTextSize(3);
    tft.setCursor(70, 10);
    tft.setTextColor(HX8357_WHITE);
    tft.println("TFLite for Particle");

    tft.setTextSize(2);
    tft.setCursor(50, 300);
    tft.println("Touch screen to begin inference");

    resetInference = false;
  }
}