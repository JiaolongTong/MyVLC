#pragma once
#include "Header.h"

// this class will be generating waves
class WaveGenerator
{
public:
	// symbol_time: how many milliseconds will the symbol last
	static vector<float> createWaveGivenFPS(vector<SymbolData> &msg,bool sampling = true)
	{
		if (sampling)
		{
			return createWaveBySampling(Parameters::fps, msg, Parameters::symbolTime);
		}
		return createWaveAssumingIntegers(Parameters::fps, msg, Parameters::symbolTime);
	}
	static vector<float> createWaveAssumingIntegers(double frequency, vector<SymbolData> &msg, int symbol_time)
	{
		vector<float> amplitudes;
		int framerate = frequency; //get the frame rate
		int frames_per_symbol = (framerate * 1000) / symbol_time; // symbol time in milliseconds and framerate in frames per second
		//int frequencies[] = { ZeroFrequency, OneFrequency };
		// create the video writer
		VideoWriter vidWriter;
		for (int i = 0; i < msg.size(); i++)
		{
			int needed_frequency = msg[i].frequency;
			int frames_per_half_cycle = framerate / (needed_frequency * 2);
			// start high
			int luminance_sign = -1;
			for (int k = 0; k < frames_per_symbol; k++)
			{
				if ((k%frames_per_half_cycle) == 0)
				{
					luminance_sign = -luminance_sign;
				}
				//cout << luminance_index;
				amplitudes.push_back(msg[i].amplitude * luminance_sign);
			}
			cout << msg[i].symbol;
		}
		cout << endl;
		return amplitudes;
	}
	static vector<float> createWaveBySampling(double frequency, vector<SymbolData> &msg, int symbol_time)
	{
		vector<float> amplitudes;
		int framerate = frequency; //get the frame rate
		int frames_per_symbol = (framerate * symbol_time) / 1000; // symbol time in milliseconds and framerate in frames per second
		//int frequencies[] = { ZeroFrequency, OneFrequency };
		// create the video writer
		//vector<float> sample[2];
		//sample[0] = WaveGenerator::createSampledSquareWave(frequency, frames_per_symbol, ZeroFrequency, luminance,-luminance);
		//sample[1] = WaveGenerator::createSampledSquareWave(frequency, frames_per_symbol, OneFrequency, luminance, -luminance);
		VideoWriter vidWriter;
		int msgSize = msg.size();
		cout << msgSize << endl;
		for (int i = 0; i < msgSize; i++)
		{
			vector<float> sample = WaveGenerator::createSampledSquareWave(frequency, frames_per_symbol, msg[i].frequency, msg[i].amplitude, -msg[i].amplitude);
			amplitudes.insert(amplitudes.end(), sample.begin(), sample.end());
			//cout << msg[i].symbol;
		}
		//cout << endl;
		return amplitudes;
	}
	// create sampled sine wave
	static vector<float> createSampledSineWave(int fps, int frames_per_symbol, float freq,double phase)
	{
		vector<float> result;
		for (int i = 0; i < frames_per_symbol; i++)
		{
			double x = 2 * MM_PI * freq * i / fps + phase;
			result.push_back(sin(x));
		}

		return result;
	}

	// create sampled square wave
	static vector<float> createSampledSquareWave(int fps, int frames_per_symbol, float freq, float pos, float neg)
	{
		vector<float> result;
		for (int i = 0; i < frames_per_symbol; i++)
		{
			float val = sin(2 * MM_PI * freq * i / fps + MM_PI);
			if (val > 0)
			{
				result.push_back(pos);
			}
			else if (val < 0)
			{
				result.push_back(neg);
			}
			else
			{
				result.push_back(0);
			}
		}

		return result;
	}
	// create sampled square wave
	static vector<float> createSampledTriangleWave(int fps, int frames_per_symbol, float freq, float amp)
	{
		vector<float> result;
		for (int i = 0; i < frames_per_symbol; i++)
		{
			float pos = fmod((freq * i) / fps, 1.0);
			result.push_back((1 - fabs(pos - 0.5) * 4)*amp);
		}

		return result;
	}
};