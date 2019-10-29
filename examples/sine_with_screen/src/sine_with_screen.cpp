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

#include <TensorFlowLite.h>
#include <Particle.h>
#include "Adafruit_HX8357.h"

SYSTEM_MODE(MANUAL);
SYSTEM_THREAD(ENABLED);

#include "main_functions.h"

#include "constants.h"
#include "output_handler.h"
#include "sine_model_data.h"
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

int waveBoxWidth = 480;
float waveBoxHeight = 300.f;
int lastYPos = 0;
double currentXPos = 0.d;
double xStep = 0.75;

Adafruit_HX8357 tft = Adafruit_HX8357(TFT_CS, TFT_DC, TFT_RST);

namespace
{
tflite::ErrorReporter *error_reporter = nullptr;
const tflite::Model *model = nullptr;
tflite::MicroInterpreter *interpreter = nullptr;
TfLiteTensor *input = nullptr;
TfLiteTensor *output = nullptr;
int inference_count = 0;

// Create an area of memory to use for input, output, and intermediate arrays.
// Finding the minimum value for your model may require some trial and error.
constexpr int kTensorArenaSize = 2 * 1024;
uint8_t tensor_arena[kTensorArenaSize];
} // namespace

// The name of this function is important for Arduino compatibility.
void setup()
{
  tft.begin();
  tft.fillScreen(HX8357_BLACK);
  tft.setRotation(1);
  tft.setTextSize(3);
  tft.setCursor(70, 10);
  tft.setTextColor(HX8357_WHITE);
  tft.println("TFLite for Particle");

  // Set up logging. Google style is to avoid globals or statics because of
  // lifetime uncertainty, but since this has a trivial destructor it's okay.
  // NOLINTNEXTLINE(runtime-global-variables)
  static tflite::MicroErrorReporter micro_error_reporter;
  error_reporter = &micro_error_reporter;

  // Map the model into a usable data structure. This doesn't involve any
  // copying or parsing, it's a very lightweight operation.
  model = tflite::GetModel(g_sine_model_data);
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

  // Keep track of how many inferences we have performed.
  inference_count = 0;
}

// The name of this function is important for Arduino compatibility.
void loop()
{
  // Calculate an x value to feed into the model. We compare the current
  // inference_count to the number of inferences per cycle to determine
  // our position within the range of possible x values the model was
  // trained on, and use this to calculate a value.
  float position = static_cast<float>(inference_count) /
                   static_cast<float>(kInferencesPerCycle);
  float x_val = position * kXrange;

  // Place our calculated x value in the model's input tensor
  input->data.f[0] = x_val;

  // Run inference, and report any error
  TfLiteStatus invoke_status = interpreter->Invoke();
  if (invoke_status != kTfLiteOk)
  {
    error_reporter->Report("Invoke failed on x_val: %f\n",
                           static_cast<double>(x_val));
    return;
  }

  // Read the predicted y value from the model's output tensor
  float y_val = output->data.f[0];

  // Draw a circle on the tft_screen that corresponds with the y value
  // if is -1, we'll draw the circle at the bottom (y = 300) of the screen
  // if y is 1, we'll draw the circle at the top (y = 20) of the screen
  // The x value will increment each time through and roll back to 0 after
  // the screen widge of 480 is reached.
  float yPos = map(y_val, -1.f, 1.f, 40.f, waveBoxHeight);

  if (currentXPos == 0.d)
  {
    tft.fillCircle(waveBoxWidth, lastYPos, 5, HX8357_BLACK);
  }
  else
  {
    tft.fillCircle(currentXPos - xStep, lastYPos, 5, HX8357_BLACK);
  }
  tft.fillCircle(currentXPos, (int)yPos, 5, HX8357_RED);

  lastYPos = (int)yPos;
  currentXPos = currentXPos + xStep;
  if (currentXPos > waveBoxWidth)
  {
    currentXPos = 0.d;
  }

  // Output the results. A custom HandleOutput function can be implemented
  // for each supported hardware target.
  HandleOutput(error_reporter, x_val, y_val);

  // Increment the inference_counter, and reset it if we have reached
  // the total number per cycle
  inference_count += 1;
  if (inference_count >= kInferencesPerCycle)
    inference_count = 0;
}
