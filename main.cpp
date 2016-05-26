
#include <fileref.h>
#include <mpegfile.h>
#include <mad.h>
#include <portaudio.h>

#include <fstream>
#include <vector>
#include <cstdint>
#include <iostream>

int main(int argc, char* argv[]) {

	if(argc < 2) {
		std::cerr << "Please provide an mp3 file as first argument" << std::endl;
		return -1;
	}

	std::string filename;
	if(argc >= 2) {
		filename = std::string(argv[1]);
	}

	long offset;
	{
		TagLib::FileRef ref(filename.c_str());
		TagLib::File* file = ref.file();

		TagLib::MPEG::File* mp3_file = dynamic_cast<TagLib::MPEG::File*>(file);
		if(!mp3_file) {
			throw std::runtime_error(filename + " is not an valid mp3 file.");
		}
		offset = mp3_file->firstFrameOffset();
	}

	std::ifstream mp3(filename);
	if(!mp3) {
		throw std::runtime_error("Failed to open: " + filename);
	}
	std::vector<uint8_t> data((std::istreambuf_iterator<char>(mp3)), std::istreambuf_iterator<char>());
	mp3.close();

	mad_stream mp3_stream;
	mad_stream_init(&mp3_stream);

	mad_stream_buffer(&mp3_stream, data.data(), data.size());
	mad_stream_skip(&mp3_stream, offset);

	mad_frame frame;
	mad_frame_init(&frame);

	PaStreamParameters stream_parameters;
	PaStream* stream;
	PaError error;

	error = Pa_Initialize();
	if( error != paNoError ) {
		throw std::runtime_error("Cannot initialize PortAudio");
	}

	stream_parameters.device = Pa_GetDefaultOutputDevice();
	if (stream_parameters.device == paNoDevice) {
		throw std::runtime_error("No default device detected");
	}

	stream_parameters.channelCount = 2;
	stream_parameters.sampleFormat = paFloat32;
	stream_parameters.suggestedLatency = Pa_GetDeviceInfo(stream_parameters.device)->defaultLowOutputLatency;
	stream_parameters.hostApiSpecificStreamInfo = nullptr;

	error = Pa_OpenStream(&stream, NULL, &stream_parameters, 44100, 1152, paNoFlag, NULL, NULL);

	if(error != paNoError) {
		throw std::runtime_error(std::string("Failed to open portaudio stream ") + Pa_GetErrorText(error));
	}

	error = Pa_StartStream(stream);
	if (error != paNoError) {
		throw std::runtime_error("Failed to start portaudio stream.");
	}

	float buffer[2][1152];

	for(int i = 0; i < 40 * 60; i++) {
		mad_frame_decode(&frame, &mp3_stream);

		mad_synth synth;
		mad_synth_init(&synth);

		mad_synth_frame(&synth, &frame);


		for(int i = 0; i < 2; i++) {
			for(int j = 0; j < 1152; j++) {
				buffer[i][j] = mad_f_todouble(synth.pcm.samples[i][j]);
			}
		}

		error = Pa_WriteStream(stream, buffer, synth.pcm.length);
		if(error != paNoError) {
			throw std::runtime_error("Failed to write to stream.");
		}
	}

	error = Pa_StopStream(stream);
	if(error != paNoError) {
		throw std::runtime_error("Failed to stop stream.");
	}

	error = Pa_CloseStream(stream);
	if(error != paNoError) {
		throw std::runtime_error("Failed to close stream");
	}

	Pa_Terminate();

	mad_synth_finish(&synth);
	mad_frame_finish(&frame);
	mad_stream_finish(&mp3_stream);

	return 0;
}
