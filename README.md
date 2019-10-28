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

[TODO]

## Included Examples

The source contains the following examples, some of which require additional hardware and may not run across all Particle devices (especially the Photon) due to model size constraints.

1. `hello_world` - Runs a simple sine wave model (2kb in size) to predict the next wave position. Uses the output to `analogWrite` the waveform value to the onboard D7 LED. Tested on the Particle Photon and Xenon.
2. `magic_wand` - [TODO]
3. `micro_speech` - [TODO]
4. `person_detection` - [TODO]

## Dealing with Large Models

[TODO]