#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>

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

bool is_line_near(Vec4f l_0, Vec4f l_1, float eps = 1, float k = 300) {
	//cout << abs_f(l_0[0] - l_1[0]) << '\t' << abs_f(l_0[1] - l_1[1]) << endl;
	return abs_f(l_0[0] - l_1[0]) < eps && abs_f(l_0[1] - l_1[1]) < k;
}

int main( int argc, char** argv )
{
    const char* filename = argc >= 2 ? argv[1] : "1.jpg";

 Mat src = imread(filename, 0);
 if(src.empty())
 {
     help();
     cout << "can not open " << filename << endl;
     return -1;
 }

 Mat thres, dst, cdst;
 threshold( src, thres, 150, 255, 0);
 thres(Rect(0, 0, thres.cols, thres.rows/4)) = 0;
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
  HoughLinesP(dst, lines, 1, CV_PI/200, 20, 100, 50 );

  vector<Vec4f> line_equations;

  cout << "=========Calulating line equations==========\n";

  for (size_t i = 0; i < lines.size(); i++) {
	Vec4i l = lines[i];
	cout << l[0] << '\t' << l[1] << '\t' << l[2] << '\t' << l[3] << endl;
  	
	float a,b;
	a = (float)(l[1] - l[3])/(float)(l[0] - l[2]);
	b = l[1] - a * l[0];

	Vec4f new_l = Vec4f(a,b,l[0],l[2]);
	line_equations.push_back(new_l);
	cout << "Equation: " << line_equations.back()[0] << '\t' << b << endl;
  }

  cout << "==========Starting merging lines=============\n";

  vector<Vec4f> new_lines;
  for (size_t i = 0; i < line_equations.size(); i++) {
  	bool will_add = true;
	for (size_t j = 0; j < new_lines.size(); j++) {
		if (is_line_near(line_equations[i],new_lines[j],0.01, 300)) {
			will_add = false;
			cout << "line " << line_equations[i][0] << '\t' << line_equations[i][1] << " merged to line " << new_lines[j][0] << '\t' << new_lines[j][1] << endl;
			break;
		}
	}
	if (will_add) new_lines.push_back(line_equations[i]);
  }

  cout << "Lines merged from " << line_equations.size() << " to " << new_lines.size() << endl;

 /*
   for( size_t i = 0; i < lines.size(); i++ )
  {
    Vec4i l = lines[i];
    line( cdst, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0,0,255), 1, CV_AA);
  }
*/

/* 
 for( size_t i = 0; i < line_equations.size(); i++ )
 {
   Vec4f l = line_equations[i];
   float y0, y1;
   y0 = l[0] * l[2] + l[1];
   y1 = l[0] * l[3] + l[1];
   //cout << "Draw line " << i << endl;
   //cout << lines[i][0] << '\t' << lines[i][1] << '\t' << lines[i][2] << '\t' << lines[i][3] << endl;
   //cout << endl <<l[2] << '\t' << y0 << '\t' << l[3] << '\t' << y1 << endl;
   line( cdst, Point(l[2], l[2]*l[0]+l[1]), Point(l[3], l[3]*l[0] + l[1]), Scalar(255,0,0), 1, CV_AA);
 }
 */
 
   for( size_t i = 0; i < new_lines.size(); i++ )
  {
    Vec4f l = new_lines[i];
    line( cdst, Point(l[2], l[2]*l[0]+l[1]), Point(l[3], l[3]*l[0] + l[1]), Scalar(0,0,255), 1, CV_AA);
  }
 
 #endif

 namedWindow("Source", WINDOW_NORMAL);
 resizeWindow("Source", 600,600);
 imshow("Source", thres);

 namedWindow("Detected Lines", WINDOW_NORMAL);
 resizeWindow("Detected Lines", 600,600);
 imshow("Detected Lines", cdst);

 waitKey();

 return 0; 
}

void filter_region(Mat image) {
	
}
