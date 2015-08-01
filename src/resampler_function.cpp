#include <algorithm>
#include <math.h>

namespace libaudioverse_implementation {

void staticResamplerKernel(int inputSr, int outputSr, int channels, int frames, float* data, int *outLength, float** outData) {
	if(inputSr== outputSr) { //copy
		float* o = allocArray<float>(channels*frames);
		std::copy(data, data+channels*frames, o);
		*outLength = frames;
		*outData = o;
		return;
	}
	int err;
	auto resampler=speex_resampler_init(channels, inputSr, outputSr, 10, &err);
	if(resampler==nullptr) ERROR(Lav_ERROR_MEMORY, "Could not allocate speex resampler.");
	if(err != RESAMPLER_ERR_SUCCESS) ERROR(Lav_ERROR_INTERNAL, "Resampler error.");
	unsigned int numer, denom;
	speex_resampler_get_ratio(resampler, &numer, &denom);
	//The 200 makes sure that we grab all of it.
	//It is not inconceivable that we might resample by a huge rate.
	//And speex doesn't let us estimate output.
	//Instead of reallocating, we waste a small amount of ram here.
	//this is input rate/output rate, so multiply by denom.
	int size=frames*channels*denom/numer+channels*200;
	float* o = allocArray<float>(size);
	//uframes is because speex needs an address to an unsigned int.
	unsigned int written = 0, consumed;
	*outLength= 0;
	int remaining_in =frames, remaining_out = frames*denom/numer+200;
	float* tempData=data, *tempO=o;
	while(remaining_in > 0 && remaining_out > 0) {
		consumed=std::min<int>(1024, remaining_in);
		written=std::min(1024, remaining_out);
		speex_resampler_process_interleaved_float(resampler, tempData, &consumed, tempO, &written);
		remaining_in -= consumed;
		remaining_out -= written;
		*outLength +=written;
		tempData += consumed*channels;
		tempO += written*channels;
	}
	*outData = o;
	speex_resampler_destroy(resampler);
}

}