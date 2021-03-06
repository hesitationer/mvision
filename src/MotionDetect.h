#pragma once

namespace sx
{
class Image;

/* A very simple motion detector.
First, we downsample each frame until it's width is 20 pixels or less. We downsample with
a simple box filter. We don't do sRGB filtering, just simple cheap gamma-space filtering.
For every frame, we compare it to the previous frame, and estimate the average noise
per pixel, which is just Total Difference / Number of Pixels. This average is then
lerp-ed into our Noise.
We do a similar thing to generate a our Stable image, which is what we believe the scene
ought to look like. The stable image is initialized to the first frame that we see, and
thereafter, every subsequent frame is lerp-ed into our current stable frame, so that
over time our stable converges to recent history.
We define motion as 3 or more pixels from the current frame, which differ by a large
enough threshold from our stable image.
*/
class MotionDetector
{
public:
	Image*		DebugImage = nullptr;
	Image*		Prev = nullptr;				// Our previous frame
	Image*		Stable = nullptr;			// Our long-term idea of what the scene is supposed to look like
	Image*		NoiseImg = nullptr;			// Noisy-ness of each pixel
	float		Noise = 0;
	int64_t		NFrames = 0;
	float		GlobalAvgDiff = 0;			// This is used to determine IsGlobalMotion
	bool		IsLocalMotion = false;		// Is there localized, acute motion in a smallish region
	bool		IsGlobalMotion = false;		// Is there global, possible minor motion, across the entire frame (ie camera is being shaken - aka stolen)
	bool		OutputDebugImage = true;

	MotionDetector();
	~MotionDetector();

	// frame: a u8 single-channel image
	void					Frame(Image* frame);

private:
	void					ComputeNoise(Image* a, Image* b);
	void					ComputePixelsInMotion(Image* frame);
	void					MergeIntoPrev(Image* frame);

	inline static uint32	AbsDiff(uint8 a, uint8 b) { int32 d = (int32) a - (int32) b; return (uint32) (d < 0 ? -d : d); }

};

}