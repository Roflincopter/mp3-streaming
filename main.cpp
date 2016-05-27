
#include <fileref.h>
#include <mpegfile.h>
#include <mad.h>
#include <portaudio.h>

#include <fstream>
#include <vector>
#include <cstdint>
#include <iostream>
#include <memory>

class Mp3FrameGenerator {
	mad_stream stream;
	mad_frame frame;
	mad_synth synth;

	std::vector<uint8_t> mp3_contents;
public:

	Mp3FrameGenerator(Mp3FrameGenerator const&)	= delete;

	Mp3FrameGenerator(std::string filename)
	: stream()
	, frame()
	, synth()
	{
		mad_stream_init(&stream);
		mad_frame_init(&frame);
		mad_synth_init(&synth);

		long offset;
		{
			TagLib::FileRef ref(filename.c_str());
			TagLib::File* file = ref.file();

			TagLib::MPEG::File* mp3_file = dynamic_cast<TagLib::MPEG::File*>(file);
			if(!mp3_file) {
				throw std::runtime_error(filename + " is not an valid mp3 file.");
			}
			offset = mp3_file->firstFrameOffset();
			//offset = mp3_file->nextFrameOffset(offset + 2000000);
		}

		std::ifstream mp3(filename);
		if(!mp3) {
			throw std::runtime_error("Failed to open: " + filename);
		}
		mp3_contents = std::vector<uint8_t>((std::istreambuf_iterator<char>(mp3)), std::istreambuf_iterator<char>());
		mp3.close();

		mad_stream_buffer(&stream, mp3_contents.data(), mp3_contents.size());
		mad_stream_skip(&stream, offset);
	}

	std::vector<float> generate_pcm_data() {
		mad_frame_decode(&frame, &stream);
		mad_synth_frame(&synth, &frame);

		unsigned short nr_channels = synth.pcm.channels;
		unsigned short nr_samples = synth.pcm.length;

		std::vector<float> buffer;
		buffer.resize(nr_channels * nr_samples);
		for(int i = 0; i < nr_channels; i++) {
			for(int j = 0; j < synth.pcm.length; j++) {
				buffer[(nr_channels*j) + i] = static_cast<float>(mad_f_todouble(synth.pcm.samples[i][j])) / 8;
			}
		}

		return buffer;
	}

	virtual ~Mp3FrameGenerator() {
		mad_stream_finish(&stream);
		mad_frame_finish(&frame);
		mad_synth_finish(&synth);
	}
};

int play_callback(const void*, void *output, unsigned long frameCount, const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags, void *userData) {
	auto& gen = *static_cast<Mp3FrameGenerator*>(userData);

	std::vector<float> buffer = gen.generate_pcm_data();
	std::copy_n(buffer.data(), buffer.size(), (float*) output);

	return paContinue;
}


int main(int argc, char* argv[]) {

	if(argc < 2) {
		std::cerr << "Please provide an mp3 file as first argument" << std::endl;
		return -1;
	}

	std::string filename;
	if(argc >= 2) {
		filename = std::string(argv[1]);
	}

	Mp3FrameGenerator gen(filename);

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

	error = Pa_OpenStream(&stream, NULL, &stream_parameters, 44100, 1152, paNoFlag, &play_callback, &gen);

	if(error != paNoError) {
		throw std::runtime_error(std::string("Failed to open portaudio stream ") + Pa_GetErrorText(error));
	}

	error = Pa_StartStream(stream);
	if (error != paNoError) {
		throw std::runtime_error("Failed to start portaudio stream.");
	}

	//play for 10 seconds
	Pa_Sleep(10000);

	error = Pa_StopStream(stream);
	if(error != paNoError) {
		throw std::runtime_error("Failed to stop stream.");
	}

	error = Pa_CloseStream(stream);
	if(error != paNoError) {
		throw std::runtime_error("Failed to close stream");
	}

	Pa_Terminate();

	return 0;
}
