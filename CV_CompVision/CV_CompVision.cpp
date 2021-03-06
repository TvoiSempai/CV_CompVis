#include <iostream>
#include <vector>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>
#include <cmath>
#include <chrono>

using namespace cv;
using namespace std;
using namespace std::chrono;

int main(int argc, char** argv)
{	
	string filename = "source";
	int num = stoi(argv[1]);
	filename += to_string(num) + ".png";
	vector<Point2f> corners;
	Mat source, gray, histNorm, edges, distance, bw, integ, filtered;
	// [[ 1 ]]
	source = imread(filename);
	imshow("source", source);
	if (source.empty()) return -1;
	// [[ 2 ]]
	auto start = high_resolution_clock::now();
	cvtColor(source, gray, cv::COLOR_RGB2GRAY);
	imshow("gray", gray);
	// [[ 3 ]]
	equalizeHist(gray, histNorm);
	imshow("histNorm", histNorm);
	// [[ 4 ]]
	Canny(gray, edges, 60, 120);
	imshow("Canny", edges);
	// [[ 5 ]]
	goodFeaturesToTrack(gray, corners, 200, 0.5, 3);
	for (int i = 0; i < corners.size(); ++i) {
		circle(edges, corners[i], 2, Scalar(255), 2 ,8, 0);
	}
	imshow("corners + edges", edges);
	// [[ 6 ]]
	gray.copyTo(bw);
	threshold(bw, bw, 40, 255, CV_THRESH_BINARY);
	distanceTransform(bw, distance, CV_DIST_L2, 3);
	normalize(distance, distance, 0, 1, NORM_MINMAX);
	imshow("distance transform", distance);
	// [[ 7 + 8]]
	integral(gray, integ);
	filtered = gray;
	for (int i = 0; i < source.rows; ++i)
	{
		for (int j = 0; j < source.cols; ++j)
		{
			int kernel = distance.at<uchar>(i, j);
			if (kernel % 2 == 0) kernel++;
			double radius = kernel / 2;
			if ((kernel <= 1) || (i <= radius) || (j <= radius) || (i > source.rows - radius - 1) || (j > source.cols - radius - 1)) continue;
			float sum = integ.at<uchar>(i - radius - 1, j - radius - 1) - integ.at<uchar>(i - radius - 1, j + radius) + 
						integ.at<uchar>(i + radius, j + radius) - integ.at<uchar>(i + radius, j - radius - 1);
			filtered.at<uchar>(i, j) = sum / (float)(pow(kernel, 2));
		}
	}
	imshow("Filtered", filtered);
	
	auto end = high_resolution_clock::now();
	auto time = duration_cast<milliseconds>(end - start).count();
	printf("work time : %d", time);
	// [[ 9 ]]
	printf("fps : %10.4f", 6 / (time / 1000.f));
	waitKey(0);
	return 0;
}