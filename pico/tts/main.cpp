#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <unistd.h>


#include "TtsEngine.h"

#define OUTPUT_BUFFER_SIZE (128 * 1024)

using namespace android;

static bool synthesis_complete = false;

static FILE *outfp = stdout;

// @param [inout] void *&       - The userdata pointer set in the original
//                                 synth call
// @param [in]    uint32_t      - Track sampling rate in Hz
// @param [in] tts_audio_format - The audio format
// @param [in]    int           - The number of channels
// @param [inout] int8_t *&     - A buffer of audio data only valid during the
//                                execution of the callback
// @param [inout] size_t  &     - The size of the buffer
// @param [in] tts_synth_status - indicate whether the synthesis is done, or
//                                 if more data is to be synthesized.
// @return TTS_CALLBACK_HALT to indicate the synthesis must stop,
//         TTS_CALLBACK_CONTINUE to indicate the synthesis must continue if
//            there is more data to produce.
tts_callback_status synth_done(void *& userdata, uint32_t sample_rate,
        tts_audio_format audio_format, int channels, int8_t *& data, size_t& size, tts_synth_status status)
{
    fprintf(stderr, "TTS callback, rate: %d, data size: %d, status: %i\n", sample_rate, size, status);

    if (status == TTS_SYNTH_DONE)
            synthesis_complete = true;

    if ((size == OUTPUT_BUFFER_SIZE) || (status == TTS_SYNTH_DONE))
            fwrite(data, size, 1, outfp);

    return TTS_CALLBACK_CONTINUE;
}

static void usage(void)
{
    fprintf(stderr, "\nUsage:\n\n" \
                                    "testtts [-o filename] \"Text to speak\"\n\n" \
                                    "  -o\tFile to write audio to (default outputfile.wav)\n");
        exit(0);
}

static char* PackageInt(int source, int length=2)
{
    if((length!=2)&&(length!=4))
        return NULL;
    char* retVal = new char[length];
    retVal[0] = (char)(source & 0xFF);
    retVal[1] = (char)((source >> 8) & 0xFF);
    if (length == 4)
    {
        retVal[2] = (char) ((source >> 0x10) & 0xFF);
        retVal[3] = (char) ((source >> 0x18) & 0xFF);
    }
    return retVal;
}

int main(int argc, char *argv[])
{
    tts_result result;
    TtsEngine* ttsEngine = getTtsEngine();
    int8_t* synthBuffer;
    char* synthInput = NULL;
    int currentOption;

    std::string outputFilename = "outputfile.wav";

    fprintf(stderr, "Pico TTS Test App\n");
    while ( (currentOption = getopt(argc, argv, "o:h")) != -1)
    {
        switch (currentOption)
        {
        case 'o':
                outputFilename = optarg;
            fprintf(stderr, "Output audio to file '%s'\n", outputFilename.c_str());
            break;

        case 'h':
        	usage();
            break;
        default:
            printf ("Getopt returned character code 0%o ??\n", currentOption);
        }
    }

    if (optind < argc)
    	synthInput = argv[optind];

    if (!synthInput)
    {
    	fprintf(stderr, "Error: no input string\n");
    	usage();
    }

    fprintf(stderr, "Input string: \"%s\"\n", synthInput);

    char* inputString = (char*) malloc (sizeof(char) * strlen(synthInput));
    strcpy(inputString, synthInput);

    synthBuffer = new int8_t[OUTPUT_BUFFER_SIZE];

    result = ttsEngine->init(synth_done, "/usr/local/etc/lang/");

    if (result != TTS_SUCCESS)
        fprintf(stderr, "Failed to init TTS\n");

    // Force English for now
    result = ttsEngine->setLanguage("eng", "GBR", "");

    if (result != TTS_SUCCESS)
        fprintf(stderr, "Failed to load language\n");

    const char RIFF_HEADER[]    = { 0x52, 0x49, 0x46, 0x46 }; //RIFF
    const char FORMAT_WAVE[]    = { 0x57, 0x41, 0x56, 0x45 }; //WAVE
    const char FORMAT_TAG[]     = { 0x66, 0x6d, 0x74, 0x20 }; //fmt<space>
    const char AUDIO_FORMAT[]   = { 0x01, 0x00 };             //'SOH' 'NUL' {10}
    const char SUBCHUNK_ID[]    = { 0x64, 0x61, 0x74, 0x61 }; //data
    const int BYTES_PER_SAMPLE  = 2;


    const int sampleRate = 16000;
    int channelCount = 1;

    fpos_t pos_file_size;
    fpos_t pos_data_size;

    int charRate = sampleRate*channelCount*BYTES_PER_SAMPLE;
    int blockAlign = channelCount*BYTES_PER_SAMPLE;

    outfp = fopen(outputFilename.c_str(), "wb");

    fwrite (RIFF_HEADER, 4, 1, outfp);                      // RIFF marker                              0  [4] Bytes
    fgetpos (outfp, &pos_file_size);
    fwrite (PackageInt(0, 4), 4, 1, outfp);                 // file-size (equals file-size - 8)         4  [4] Bytes
    fwrite (FORMAT_WAVE, 4, 1, outfp);                      // Mark it as type "WAVE"                   8  [4] Bytes
    fwrite (FORMAT_TAG, 4, 1, outfp);                       // Mark the format section                  12 [4] Bytes
    fwrite (PackageInt(16, 4), 4, 1, outfp);                // Length of format data. Always 16         16 [4] Bytes
    fwrite (AUDIO_FORMAT, 2, 1, outfp);                     // Wave type PCM                            20 [2] Bytes
    fwrite (PackageInt(1, 2), 2, 1, outfp);                 // 1 Channel                                22 [2] Bytes
    fwrite (PackageInt(16000, 4), 4, 1, outfp);             // 16 kHz sample rate                       24 [4] Bytes
    fwrite (PackageInt(32000, 4), 4, 1, outfp);             // (Sample Rate * Bit Size * Channels) / 8  28 [4] Bytes
    fwrite (PackageInt(2, 2), 2, 1, outfp);                 // (Bit Size * Channels) / 8                32 [2] Bytes
    fwrite (PackageInt(16, 2), 2, 1, outfp);                // Bits per sample (=Bit Size * Samples)    34 [4] Bytes
    fwrite (SUBCHUNK_ID, 4, 1, outfp);                      // "data" marker                            36 [4] Bytes
    fgetpos (outfp, &pos_data_size);
    fwrite (PackageInt(0, 4), 4, 1, outfp);                 // data-size (equals file-size - 44         40 [4] Bytes

    /*Output
     * IR FF (size) AW EV mf ' 't (16) 'NULL''NULL' 'NUL''SOH' 'NUL''SOH' >'ç' 'NUL''NUL'
     *
     *
     */

    fprintf(stderr, "Synthesising text...\n");

    result = ttsEngine->synthesizeText(inputString, synthBuffer, OUTPUT_BUFFER_SIZE, NULL);

    if (result != TTS_SUCCESS)
        fprintf(stderr, "Failed to synth text\n");

    while(!synthesis_complete)
        usleep(100);

    fprintf(stderr, "Completed.\n");

    int sizeOfFile = 0;
    sizeOfFile = ftell(outfp);

    fsetpos (outfp, &pos_file_size);
    fwrite (PackageInt((sizeOfFile -  8), 4), 4, 1, outfp); // file-size (equals file-size - 8)         8  Bytes

    fsetpos (outfp, &pos_data_size);
    fwrite (PackageInt((sizeOfFile - 44), 4), 4, 1, outfp); // data-size (equals file-size - 44         44 Bytes

    fseek(outfp,0,SEEK_END);
    fclose(outfp);

    result = ttsEngine->shutdown();

    if (result != TTS_SUCCESS)
        fprintf(stderr, "Failed to shutdown TTS\n");

    free (inputString);
    delete [] synthBuffer;

    return 0;
}
