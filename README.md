# TensorFlowLite

A library for using [TensorFlow Lite for Microcontrollers](https://www.tensorflow.org/lite/microcontrollers) with Particle devices. This library has been tested with the following Particle devices:

- Photon / P1
- Argon
- Boron
- Xenon

## Getting started with TensorFlow Lite

To use TensorFlow Lite for Microcontrollers with your own models, you'll need to take the following steps, as detailed in the [TensorFlow docs](https://www.tensorflow.org/lite/microcontrollers#developer_workflow):

1. Create or download a model.
2. Convert the model to a a TensorFlow Lite FlatBuffer.
3. Convert the FlatBuffer to a C byte array for inclusion in your project.
4. Integrate this library into your project using the instructions below.
5. Deploy your project to a Particle device.

## Using the library

See the [TensorFlow Lite for Microcontrollers docs](https://www.tensorflow.org/lite/microcontrollers/get_started#how_to_run_inference) for detailed instructions on running inference on embedded devices.

## Included Examples

The source contains the following examples, some of which require additional hardware and may not run across all Particle devices (especially the Photon) due to model size constraints.

1. `hello_world` - Runs a simple sine wave model (2kb in size) to predict the next wave position. Uses the output to `analogWrite` the waveform value to the onboard D7 LED. Tested on the Particle Photon and Xenon.
2. `sine_with_screen` - Runs the same sine wave model from the `hello_world` project, and outputs the result to an [Adafruit 3.5" TFT FeatherWing](https://www.adafruit.com/product/3651). Can easily be adapted to other displays.
3. `micro_speech` - Runs a speech detection model (18k in size) that can recognize the words "yes" and "no." Uses the output to flash the onboard D7 LED when "yes" is detected. Requires an [electret microphone amplifier](https://www.adafruit.com/product/1713). This example is only supported on Particle 3rd gen devices (Argon, Boron, Xenon)
4. `magic_wand` - [Not yet supported]
5. `person_detection` - [Not yet supported]

## Dealing with Large Models

[TODO]