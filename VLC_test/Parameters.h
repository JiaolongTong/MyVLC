#pragma once
#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include <iostream>
#include "opencv2/core/core.hpp"

using namespace std;

enum
{
	FFT_RANDOM_GUESS,
	FFT_NO_RANDOM_GUESS,
	CROSS_CORRELATION,
};
enum
{
	ALPHA_CHANNEL,
	V_CHANNEL,
	V_CHANNEL_DIFF,
	BR_CHANNELS_DIFF,
};


// try to read this from file
struct SymbolData
{
	string symbol;
	double amplitude;
	double frequency;
	double phase; // currently zeros only
	vector<short> getSymbol()
	{
		vector<short> ret;
		for (int i = 0; i < symbol.size(); i++)
		{
			ret.push_back(symbol[i] - '0');
		}
		return ret;
	}
};

struct AllSymbolsData
{
	string fileName;
	double FREQ[2];
	vector<SymbolData> allData;
	// assume the fiel is sorted
	void readData(string fileName)
	{
		vector<SymbolData> tmp;
		ifstream ifstr(fileName);
		if (ifstr.is_open())
		{
			SymbolData obj;
			while (ifstr >> obj.symbol >> obj.frequency >> obj.amplitude >> obj.phase)
			{
				tmp.push_back(obj);
			}
		}
		if (tmp.size() >= 2)
		{
			this->fileName = fileName;
			allData = tmp;
		}
	}
	string toString()
	{
		if (fileName.size() > 0)
		{
			return fileName;
		}
		ostringstream ostr;
		ostr << FREQ[0] << "Hz_" << FREQ[1] << "Hz";
		return ostr.str();
	}
	void addSymbol(string symbol, double frequency, double amplitude = 0.004, double phase = 0)
	{
		for (int i = 0; i < allData.size(); i++)
		{
			if (symbol == allData[i].symbol)
			{
				cout << "symbol " << symbol << "already exists!" << endl;
				return;
			}
		}
		SymbolData obj;
		obj.symbol = symbol;
		obj.frequency = frequency;
		obj.amplitude = amplitude;
		obj.phase = phase;
		allData.push_back(obj);
		cout << "symbol " << symbol << " added successfully!" << endl;
	}
	SymbolData* getSymbol(string symbol)
	{
		for (int i = 0; i < allData.size(); i++)
		{
			if (symbol == allData[i].symbol)
			{
				return &allData[i];
			}
		}
		return 0;
	}
	vector<SymbolData> getMsgSymbols(vector<short>& msg)
	{
		vector<SymbolData> res;
		for (int i = 0; i < msg.size();i++)
		{
			string ostr;
			for (int j = 0; i < msg.size(); j++,i++)
			{
				ostr += '0' + msg[i];
				SymbolData* symbolPtr = getSymbol(ostr);
				if (symbolPtr != 0)
				{
					res.push_back(*symbolPtr);
					ostr.clear();
					//i += j;
					break;
				}
			}
			while (ostr.size() != 0)
			{
				// this should be in the last symbol only otherwise there is something wrong
				ostr += '0'; // pad zeros
				SymbolData* symbolPtr = getSymbol(ostr);
				if (symbolPtr != 0)
				{
					res.push_back(*symbolPtr);
					ostr.clear();
				}
			}
		}
		return res;
	}
};

struct Parameters
{
	static int startingIndex;
	static int endingIndex;
	static float symbolTime;
	static cv::Rect globalROI;
	static vector<float> amplitudes;
	static int fps;
	static int DecodingMethod;
	//static double LUMINANCE;
	//enum{ ZERO = 0, ONE };
	
	static cv::Size DefaultFrameSize;
	static cv::Size patternsize;
	static AllSymbolsData symbolsData;
	static int sideA;
	static int sideB;
	static int CommunicatorSpecificSplit; // split = 1, original = 0
	static map<long long, cv::Mat> vLayers;
	static double start_second;
	static double end_second;
	static string endSecondFile;
	static int BKGMaskThr;
	static int fullScreen;
	static int amplitudeExtraction;
	static string getSide()
	{
		ostringstream ostr;
		if (sideA == sideB)
		{
			ostr << "side" << sideA;
		}
		else
		{
			ostr << "sideA" << sideA << "_sideB" << sideB;
		}
		return ostr.str();
	}
	static string getFull()
	{
		ostringstream ostr;
		ostr << "full" << fullScreen;
		return ostr.str();
	}
	static bool setCodec(string code)
	{
		vector<string> codecs({"I420", "DIB", "DIVX", "XVID"});
		//cout << code << endl;
		if (find(codecs.begin(), codecs.end(),code) != codecs.end())
		{
			codec = code;
			return true;
		}
		return false;
	}
	static string getCodec()
	{
		return codec;
	}
private:
	static string codec; //I420, DIB ,DIVX, XVID
};
int Parameters::startingIndex = 0;
int Parameters::endingIndex = 0;
cv::Rect Parameters::globalROI = cv::Rect(0,0,1,1);
vector<float> Parameters::amplitudes = vector<float>();
int Parameters::fps = 0;
float Parameters::symbolTime = 1000;
int Parameters::DecodingMethod = FFT_RANDOM_GUESS;
//double Parameters::LUMINANCE = 0.005;
//enum{ ZERO = 0, ONE };
string Parameters::codec = "I420"; //I420, DIB ,DIVX, XVID
cv::Size Parameters::DefaultFrameSize = cv::Size(640, 480);
cv::Size Parameters::patternsize = cv::Size(11, 11);
AllSymbolsData Parameters::symbolsData;
map<long long, cv::Mat> Parameters::vLayers;
int Parameters::sideA = 1;
int Parameters::sideB = 1;
double Parameters::start_second = 0;
double Parameters::end_second = 0;
string Parameters::endSecondFile = "";
int Parameters::CommunicatorSpecificSplit = 0;
int Parameters::BKGMaskThr = 5;
int Parameters::fullScreen = false;
int Parameters::amplitudeExtraction = V_CHANNEL_DIFF;