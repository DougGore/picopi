Pico TTS for Raspberry Pi

This is a port of the offline text-to-speech engine from Google's Android operating system to the Raspberry Pi. Pico TTS is specifically designed for embedded devices making it ideal for memory and processing constrained platforms like the Raspberry Pi. The original code was licensed under the Apache 2 license and this derivative keeps that license.

This port is a little rough around the edges due to it being a fairly quick and dirty port and my lack of skills in makefile voodoo. I have tried to remove the Android specific files and put the minimal code from the rest of the Android ecosystem. I have left the JNI code in the repository untouched in case anyone fancies getting the Java integration working.

The structure of the respository is as follows:

1) pico_resources/docs/ - Pico TTS documentation
A very comprehensive set of documents from the original Pico TTS vendor SVOX.

2) pico_resources/tools/ - Pico TTS tools
Some of the tools used to build the TTS data. I have no idea whether they are useful or not.

3) pico/lib - The Pico TTS engine itself
This this original Pico TTS engine source code. It is very portable and didn't require any changes to get compiling. The makefile will build libsvoxpico.so which can be used directly using the documentation mentioned above.

4) pico/tts - High level C++ interface, SSML parser and test application
This directory implements a class TtsEngine and a limited Speech Synthesis Markup Language (SSML) implementation. I have also written a simple test program called "ttstest" using these classes to get you started using Pico TTS on the Raspberry Pi.

Building the library and testing it

To build the low level TTS library use the following commands:

cd pico/lib
make && sudo make install

This will build the library and put the resulting libsvoxpico.so into the /usr/lib/ folder so it can easily be incorporated into any application.

To build the test application

After completing the above commands now put in the following:

cd ../tts
make

You will get an executable file called "ttstest". This application takes a string as input and will output the raw audio to stdout or to a file using the -o command

Usage:
testtts <-o filename.raw> "String to say here"

Options:
-o      Optional command to output data to a file rather than stdout

Please note the TTS engine outputs audio in the following format: 16000 Hz, 16-bit mono (little endian).

As the test program outputs to stdout you can feed the raw audio data straight into ALSA's aplay tool to hear the audio on the HDMI or 3.5mm analogue output. Here is an example of how you can do this including the appropriate configuration parameters for aplay:

./testtts "This is a test of the Pico TTS engine on the Raspberry Pi" | aplay --rate=16000 --channels=1 --format=S16_LE


I hope you find this port useful for building your Raspberry Pi based applications. Please feel free to adapt the project to your needs. If you do use this work in your project I would love to hear about it, please drop me a tweet. Thanks.

- Doug (@DougGore)