//opencv
#pragma once

#include "Header.h"
#include <queue>

class MaskFactory
{
public:
	/*
	* get background mask
	*/
	static Mat getBackgroundMask(Mat &prev, Mat &frame)
	{
		return getBackgroundMaskNew(prev, frame, 5);
	}
	static Mat getBackgroundMaskAlgo(Mat &prev, Mat &frame)
	{
		Mat fgMaskMOG; //fg mask fg mask generated by MOG2 method
		Ptr<BackgroundSubtractor> pMOG = new BackgroundSubtractorMOG(); //MOG2 Background subtractor
		pMOG->operator()(prev, fgMaskMOG);
		pMOG->operator()(frame, fgMaskMOG);
		Mat res= (255 - fgMaskMOG);
		//cv::imshow("res", res);
		//cv::waitKey(0);
		return res;
	}

	static Mat getBackgroundMaskNew(Mat &prev, Mat &frame, int thresh)
	{
		// based on gray scale
		Mat mask = Mat::ones(prev.size(), CV_8UC1) * 255;
		
		Mat gray1, gray2;
		cv::cvtColor(prev, gray1, CV_BGR2GRAY);
		cv::cvtColor(frame, gray2, CV_BGR2GRAY);
		unsigned char *inp1 = (unsigned char*)gray1.data, *inp2 = (unsigned char*)gray2.data, *data = (unsigned char*)mask.data;
		int sz = gray1.cols* gray1.rows;
		for (int i = 0; i < sz; i++)
		{
			int val1 = inp1[i];
			int val2 = inp2[i];
			if ((val1 - val2) > thresh || (val2 - val1) > thresh)
			{
				data[i] = 0;
			}
		}
		//cv::imshow("res", mask);
		//cv::waitKey(0);
		return mask;
	}

	/*
	* create binary mask for the image based on canny edge detector
	*/
	static Mat getBinaryMask(Mat img)
	{
		int edgeThresh = 1;
		int lowThreshold = 10;
		int const max_lowThreshold = 100;
		int ratio = 3;
		int kernel_size = 3;
		Mat gray, detected_edges;
		/// Convert the image to grayscale
		cvtColor(img, gray, CV_BGR2GRAY);
		/// Reduce noise with a kernel 3x3
		blur(gray, detected_edges, Size(3, 3));
		/// Canny detector
		Canny(detected_edges, detected_edges, lowThreshold, lowThreshold*ratio, kernel_size);
		// 
		/// Using Canny's output as a mask, we display our result
		Mat dst = 255 + Mat::zeros(img.size(), CV_8UC1);
		dst = dst - detected_edges;
		/*Mat tmp;
		img.copyTo(tmp,dst);
		imshow("test", tmp);
		cv::waitKey(0);*/

		return dst;
	}


	/*
	* create binary mask for the image based on the colors
	* currently assume that the color is greater than or equal to certain value for each channel R,G,B
	*/
	static Mat getColorMask(Mat img, cv::Scalar color,bool connect4 = true,bool display = false)
	{
		// split into 3 channels
		vector<Mat> bgr;
		cv::split(img, bgr);
		Mat mask = bgr[0].clone() * 0;
		// search for points with the needed colors with large steps
		long size = img.rows * img.cols;
		int step = img.cols / 11;
		int test = 0;
		for (; test < size; test += step)
		{
			// check if we have those colors
			if (bgr[0].data[test] >= color.val[0] && bgr[1].data[test] >= color.val[1] && bgr[2].data[test] >= color.val[2])
			{
				break;
			}
		}
		if (test < size)
		{
			// then do bfs from this point to add all the points
			queue<int> qu;
			vector<bool> flags(size, 0);
			qu.push(test);
			mask.data[test] = 255;
			flags[test] = true;
			int connected_4[] = { 1, -1, img.cols, -img.cols };
			int connected_8[] = { 1, -1, img.cols, -img.cols, 1 + img.cols, 1 - img.cols, -1 + img.cols, -1 - img.cols };
			while (qu.size())
			{
				int temp = qu.front();
				qu.pop();
				// check the surroundings
				if (connect4)
				{
					// 4-connected: +1 , -1 , +cols , -cols
					for (int i = 0; i < 4; i++)
					{
						test = temp + connected_4[i];
						if (test >= 0 && test < size && !flags[test])
						{
							flags[test] = true;
							if (bgr[0].data[test] >= color.val[0] && bgr[1].data[test] >= color.val[1] && bgr[2].data[test] >= color.val[2])
							{
								qu.push(test);
								mask.data[test] = 255;
							}
						}
					}
				}
				else
				{
					// 8-connected: +1 + cols , +1 - cols , -1 + cols, -1 - cols
					for (int i = 0; i < 8; i++)
					{
						test = temp + connected_8[i];
						if (test >= 0 && test < size && !flags[test])
						{
							flags[test] = true;
							if (bgr[0].data[test] >= color.val[0] && bgr[1].data[test] >= color.val[1] && bgr[2].data[test] >= color.val[2])
							{
								qu.push(test);
								mask.data[test] = 255;
							}
						}
					}
				}
			}
		}
		if (display)
		{
			Mat temp;
			bgr[2].copyTo(temp, mask);
			imshow("red", temp);
			bgr[1].copyTo(temp, mask);
			imshow("green", temp);
			bgr[0].copyTo(temp, mask);
			imshow("blue", temp);
		}
		return mask;
	}
};