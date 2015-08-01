#pragma once
#include <memory>
#include <exception>

namespace speex_resampler_cpp {

/**Thrown if we fail to allocate memory.*/
class MemoryAllocationError: public std::exception {
	public:
	virtual const char* what() const override;
};

/**Thrown if Speex itself has an error.*/
class SpeexError: public std::exception {
	public:
	SpeexError(int c);
	virtual const char* what() const override;
	int code;
};

/**Usage:
- Create a resampler with createResampler.
- Call read on some data to give it to the resampler.
- Call write to get some data out of the resampler.
- Repeat until you're done.

The constructor can throw any of the above exception classes.
*/

class Resampler {
	public:
	virtual ~Resampler() {}
	//Write some data from the resampler to an output array.
	//returns frames written, not samples.
	virtual int write(float* dest, int maxFrameCount) = 0;
	//Put some data into the resampler.
	//this copies, the buffer can be reused.
	virtual void read(float* source) = 0;
	//note the estimate: this is not necessarily sample-accurate.
	virtual int estimateAvailableFrames() = 0;
	
	protected:
	Resampler() = default;
};

/**Can throw any of the above exception classes.*/
std::shared_ptr<Resampler> createResampler(int inputFrameCount, int inputChannels, int inputSr, int outputSr);

}