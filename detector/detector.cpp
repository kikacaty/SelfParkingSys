#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <math.h>

using namespace cv;
using namespace std;

void help() {
	cout << "Parking line detector" << endl;
}

float dist_l(Vec4i l_0, Vec4i l_1) {
	return	(l_0[0]-l_1[0])*(l_0[0]-l_1[0]) + (l_0[0]-l_1[0])*(l_0[0]-l_1[0]) + (l_0[0]-l_1[0])*(l_0[0]-l_1[0]) + (l_0[0]-l_1[0])*(l_0[0]-l_1[0]);  
}

float abs_f(float x) {
	return x > 0 ? x : -x;
}

float line_length(Vec6f& l) {
	return (l[3] - l[2] ) * sqrt(l[0]*l[0] + 1);
}

float closer_x(Vec6f& l_0, Vec6f& l_1) {
	if (l_0[3] < l_1[2]) return l_0[3];
	else if (l_0[2] > l_1[3]) return l_1[3];
	else if (l_0[3] < l_1[3]) return l_0[3];
	else return l_0[2];
}

bool is_line_near(Vec6f& l_0, Vec6f& l_1, float eps = 5, float k = 300) {
	//cout << abs_f(l_0[0] - l_1[0]) << '\t' << abs_f(l_0[1] - l_1[1]) << endl;
	float x = closer_x(l_0, l_1);
	return abs_f(180/3.14f*(atan(l_0[0]) - atan(l_1[0]))) < eps && abs_f(x*l_0[0] + l_0[1] - x * l_1[0] - l_1[1]) < k * sqrt(l_0[0] * l_0[0] + 1);
}

bool is_line_para(Vec6f& l_0, Vec6f& l_1, float eps = 1, float k = 300, float j = 300) {
	//cout << abs_f(l_0[0] - l_1[0]) << '\t' << abs_f(l_0[1] - l_1[1]) << endl;
	float x = closer_x(l_0, l_1);
	return abs_f(180/3.14f*(l_0[0] - l_1[0])) < eps && abs_f(x*l_0[0] + l_0[1] - x * l_1[0] - l_1[1]) < k * sqrt(l_0[0] * l_0[0] + 1) &&  abs_f(x*l_0[0] + l_0[1] - x * l_1[0] - l_1[1]) > j * sqrt(l_0[0] * l_0[0] + 1) ;
}

bool lower_line(Vec6f &a, Vec6f &b) {return a[0] * b[2] + a[1] > b[0] * b[2] + b[1];}
bool lefter_line(Vec6f &a, Vec6f &b) {return a[0] * a[3] + a[1] > b[0] * b[3] + b[1];}

Point intercept_line(Vec6f &a, Vec6f &b) {float x = (b[1] - a[1])/(a[0] - b[0]);return Point(x, a[0] * x + a[1]);}

Mat detect_line(Mat &src);

Point p0, p1, p2;
bool isStartParking;

int main( int argc, char** argv )
{
	const char* filename = argc >= 2 ? argv[1] : "1.jpg";

	VideoCapture cap(filename); // open the default camera

	if(!cap.isOpened())  // check if we succeeded
		return -1;
	//VideoWriter output_cap(argv[2], 
	//       cap.get(CV_CAP_PROP_FOURCC),
	//       cap.get(CV_CAP_PROP_FPS),
	//       Size(cap.get(CV_CAP_PROP_FRAME_WIDTH),
	//       cap.get(CV_CAP_PROP_FRAME_HEIGHT))); // output video stream

	namedWindow("Detected Lines", WINDOW_NORMAL);
	resizeWindow("Detected Lines", 1200,1200);

	isStartParking = false;

	for(;;)
	{
		Mat frame;
		for (int i = 0; i < 10; i++)
			cap >> frame; // get a new frame from camera

		if (frame.empty()) break;

		detect_line(frame);
		//	output_cap.write(detect_line(frame));

		if(waitKey(10) < 0) break;
	}

	return 0;	

}

Mat detect_line(Mat &src) {

	Mat thres, dst, cdst;
	GaussianBlur( src, src, Size( 5, 5 ), 0, 0 );
	threshold( src, thres, 150, 255, 0);
	thres(Rect(0, 0, thres.cols, thres.rows*2/5)) = 0;
	Canny(thres, dst, 50, 200, 3);
	cvtColor(dst, cdst, CV_GRAY2BGR);


#if 0
	vector<Vec2f> lines;
	HoughLines(dst, lines, 1, CV_PI/180, 100, 0, 0 );

	for( size_t i = 0; i < lines.size(); i++ )
	{
		float rho = lines[i][0], theta = lines[i][1];
		Point pt1, pt2;
		double a = cos(theta), b = sin(theta);
		double x0 = a*rho, y0 = b*rho;
		pt1.x = cvRound(x0 + 1000*(-b));
		pt1.y = cvRound(y0 + 1000*(a));
		pt2.x = cvRound(x0 - 1000*(-b));
		pt2.y = cvRound(y0 - 1000*(a));
		line( cdst, pt1, pt2, Scalar(0,0,255), 3, CV_AA);
	}
#else
	vector<Vec4i> lines;
	//HoughLinesP(dst, lines, 1, CV_PI/200, 20, 100, 30 );
	HoughLinesP(dst, lines, 1, CV_PI/200, 20, 20, 50 );

	vector<Vec6f> line_equations;

	cout << "=========Calulating line equations==========\n";

	for (size_t i = 0; i < lines.size(); i++) {
		Vec4i l = lines[i];
		cout << l[0] << '\t' << l[1] << '\t' << l[2] << '\t' << l[3] << endl;

		float a,b;
		a = (float)(l[1] - l[3])/(float)(l[0] - l[2]);
		b = l[1] - a * l[0];

		Vec6f new_l = Vec6f(a,b,l[0],l[2],l[1],l[3]);
		line_equations.push_back(new_l);
		cout << "Equation: " << line_equations.back()[0] << '\t' << b << endl;
	}

	cout << "==========Starting merging lines=============\n";

	vector<Vec6f> new_lines;
	for (size_t i = 0; i < line_equations.size(); i++) {
		bool will_add = true;
		for (size_t j = 0; j < new_lines.size(); j++) {
			if (is_line_near(line_equations[i],new_lines[j],3, 5)) {
				will_add = false;
				if (new_lines[j][2] > line_equations[i][2]) {
					new_lines[j][2] = line_equations[i][2];
				//	new_lines[j][4] = line_equations[i][4];
				}
				if (new_lines[j][3] < line_equations[i][3]) {
					new_lines[j][3] = line_equations[i][3];
				//	new_lines[j][5] = line_equations[i][5];
				}

				//new_lines[j][0] = (new_lines[j][4] - new_lines[j][5])/(new_lines[j][2] - new_lines[j][3]);
				//new_lines[j][1] = new_lines[j][4] - new_lines[j][0] * new_lines[j][2];

				cout << "line " << line_equations[i][0] << '\t' << line_equations[i][1] << " merged to line " << new_lines[j][0] << '\t' << new_lines[j][1] << endl;
				cout << "Length: " << line_length(new_lines[j]) << endl;
				break;
			}
		}
		if (will_add) new_lines.push_back(line_equations[i]);
	}

	/*
	   vector<Vec6f> hor_lines, vert_lines;

	   for (size_t i = 0; i < new_lines.size(); i++) {
	   if (new_lines[i][0] < 0.15 && new_lines[i][0] > - 0.2) hor_lines.push_back(new_lines[i]);
	   else if (new_lines[i][0] > 0.25) vert_lines.push_back(new_lines[i]);
	   }

	   cout << "Lines merged from " << line_equations.size() << " to " << new_lines.size() << endl;

*/

	//sort(new_lines.begin(), new_lines.end(), near_line);

	/*	
		for( size_t i = 0; i < lines.size(); i++ )
		{
		Vec4i l = lines[i];
		line( cdst, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0,0,255), 1, CV_AA);
		line( src, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0,0,255), 5, CV_AA);
		}
		*/

	for ( size_t i = 0; i < new_lines.size(); i++) {

		Vec6f l = new_lines[i];


		line(src, Point(l[2], l[2] * l[0] + l[1]), Point(l[3], l[3] * l[0] + l[1]), Scalar(0, 255, 0), 10, CV_AA);
		line(cdst, Point(l[2], l[2] * l[0] + l[1]), Point(l[3], l[3] * l[0] + l[1]), Scalar(0, 255, 0), 1, CV_AA);

/*
		for (size_t j = 0; j < new_lines.size(); j++) {
			Vec6f l2 = new_lines[j];
			if (j != i && is_line_para(l, l2, 5, 20,0)) {	
				line(src, Point(l[2], l[2] * l[0] + l[1]), Point(l[3], l[3] * l[0] + l[1]), Scalar(0, 255, 255), 5, CV_AA);
				line(src, Point(l2[2], l2[2] * l2[0] + l2[1]), Point(l2[3], l2[3] * l2[0] + l2[1]), Scalar(0, 255, 255), 5, CV_AA);
				line(cdst, Point(l[2], l[2] * l[0] + l[1]), Point(l[3], l[3] * l[0] + l[1]), Scalar(0, 255, 255), 1, CV_AA);
				line(cdst, Point(l2[2], l2[2] * l2[0] + l2[1]), Point(l2[3], l2[3] * l2[0] + l2[1]), Scalar(0, 255, 255), 1, CV_AA);

			}

		}
*/
	}

	/*   

	     sort(vert_lines.begin(), vert_lines.end(), lefter_line);
	     sort(hor_lines.begin(), hor_lines.end(), lower_line);

	     if ( !vert_lines.empty() && !hor_lines.empty() && atan(vert_lines[0][0]) - atan(hor_lines[0][0]) > 0.5f) { 

	     Point icpt = intercept_line(vert_lines[0], hor_lines[0]);

	     line(src, icpt, Point(0, hor_lines[0][1]), Scalar(0, 200, 0), 5, CV_AA);
	     line(src, icpt, Point(vert_lines[0][3], vert_lines[0][3] * vert_lines[0][0] + vert_lines[0][1]), Scalar(0,200,0), 5, CV_AA);

	     p0 = Point(0, hor_lines[0][1]);
	     p1 = icpt;
	     p2 = Point(vert_lines[0][3], vert_lines[0][3] * vert_lines[0][0] + vert_lines[0][1]);

	     isStartParking = true;
	     cout << "Parking line info: " << hor_lines[0][0] << '\t' << vert_lines[0][0] << "\tatan: " << atan(vert_lines[0][0]) - atan(hor_lines[0][0]) << endl;
	     }
	     else if(isStartParking)
	     {
	     line(src, p0, p1, Scalar(0,200,0), 5, CV_AA);
	     line(src, p0, p1, Scalar(0,200,0), 5, CV_AA);
	     }

*/


	//if (!vert_lines.empty() && !hor_lines.empty()) {
	/*line(src, Point(vert_lines[0][2], vert_lines[0][2] * vert_lines[0][0] + vert_lines[0][1]), Point(vert_lines[0][3], vert_lines[0][3] * vert_lines[0][0] + vert_lines[0][1]), Scalar(0, 255, 0), 10, CV_AA);
	  line(cdst, Point(vert_lines[0][2], vert_lines[0][2] * vert_lines[0][0] + vert_lines[0][1]), Point(vert_lines[0][3], vert_lines[0][3] * vert_lines[0][0] + vert_lines[0][1]), Scalar(0, 255, 0), 10, CV_AA);

	  line(src, Point(hor_lines[0][2], hor_lines[0][2] * hor_lines[0][0] + hor_lines[0][1]), Point(hor_lines[0][3], hor_lines[0][3] * hor_lines[0][0] + hor_lines[0][1]), Scalar(0, 255, 0), 10, CV_AA);

	  line(cdst, Point(hor_lines[0][2], hor_lines[0][2] * hor_lines[0][0] + hor_lines[0][1]), Point(hor_lines[0][3], hor_lines[0][3] * hor_lines[0][0] + hor_lines[0][1]), Scalar(0, 255, 0), 10, CV_AA);
	  */	

	//	}

#endif

	namedWindow("Source", WINDOW_NORMAL);
	resizeWindow("Source", 1200,1200);
	imshow("Source", src);

	imshow("Detected Lines", cdst);

	cout << src.cols << '\t' << src.rows << endl;

	waitKey();

	return src;
}

void filter_region(Mat image) {

}
