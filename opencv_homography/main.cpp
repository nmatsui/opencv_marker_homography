#include <iostream>
#include <sstream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/calib3d/calib3d.hpp>

#define AREA_MAX 100000.0
#define AREA_MIN   1000.0
#define EPSILON       0.01

void findApices(cv::Mat& src, cv::Mat& out, std::vector<cv::Point2f>& apices);
cv::Point findCenter(std::vector<cv::Point> points, std::vector<cv::Point> contour);
cv::Point getIntersection(cv::Point p1, cv::Point p2, cv::Point p3, cv::Point p4);
cv::Point2f convertApex(cv::Point2f src, cv::Mat H);
class Exception {};

void onMouseClick(int event, int x, int y, int flag, void* m);

cv::Mat toBaseMat;
cv::Mat fromBaseMat;
cv::Point2f target;
bool foundMarker = false;
bool setTarget = false;

int main (int argc, const char * argv[]) {
    cv::VideoCapture cap(0);
    cv::Mat frm, gray, binaried, out;
    std::vector<cv::Point2f> apices(4);
    
    cv::Point2f base[] = {
        cv::Point2f( 100,  100),
        cv::Point2f( 100, -100),
        cv::Point2f(-100, -100),
        cv::Point2f(-100,  100)};
    std::vector<cv::Point2f> baseApices(&base[0], &base[4]);
    
    cv::namedWindow("Camera", CV_WINDOW_AUTOSIZE);
    cv::setMouseCallback("Camera", onMouseClick);
    while (cv::waitKey(1) != '\x1b') {
        foundMarker = false;
        cap >> frm;
        cv::cvtColor(frm, gray, CV_RGB2GRAY);
        cv::threshold(gray, binaried, 128, 255, cv::THRESH_BINARY);
        cv::Canny(binaried, binaried, 64, 255);
        frm.copyTo(out);
        
        findApices(binaried, out, apices);
        if (foundMarker) {
            toBaseMat = cv::findHomography(apices, baseApices);
            fromBaseMat = cv::findHomography(baseApices, apices);
            
            if (setTarget) {
                cv::Point2f dst = convertApex(target, fromBaseMat);
                cv::circle(out, dst, 4, CV_RGB(0,255,0), -1);
                cv::putText(out, "target", dst, cv::FONT_HERSHEY_PLAIN, 1, CV_RGB(0,255,0));
            }
        }
        
        cv::imshow("Camera", out);
    }
    cv::destroyWindow("Camera");
    return 0;
}

void onMouseClick(int event, int x, int y, int flag, void* m) {
    if (foundMarker && event == cv::EVENT_LBUTTONUP) {
        setTarget = true;
        target = convertApex(cv::Point2f(x, y), toBaseMat);
    }
}

void findApices(cv::Mat& src, cv::Mat& out, std::vector<cv::Point2f>& apices) {
    std::vector<std::vector<cv::Point> > contours;
    std::vector<cv::Point> approx;
    
    cv::findContours(src, contours, CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE);
    for (unsigned int i = 0; i < contours.size(); i++) {
        double area = fabs(cv::contourArea(contours[i]));
        cv::approxPolyDP(cv::Mat(contours[i]), approx, cv::arcLength(cv::Mat(contours[i]), true) * EPSILON, true);
        if (AREA_MIN < area && area < AREA_MAX && approx.size() == 4) {
            try {
                cv::Point c = findCenter(approx, contours[i]);
                cv::drawContours(out, contours, i, CV_RGB(255,0,0));
                cv::circle(out, c, 4, CV_RGB(0,0,255), -1);
                cv::putText(out, "center", c, cv::FONT_HERSHEY_PLAIN, 1, CV_RGB(0,0,255));
                
                for (int j = 0; j < 4; j++) {
                    if (approx[j].x - c.x > 0 && approx[j].y - c.y > 0) apices[0] = cv::Point2f(approx[j].x, approx[j].y);
                    if (approx[j].x - c.x > 0 && approx[j].y - c.y < 0) apices[1] = cv::Point2f(approx[j].x, approx[j].y);
                    if (approx[j].x - c.x < 0 && approx[j].y - c.y < 0) apices[2] = cv::Point2f(approx[j].x, approx[j].y);
                    if (approx[j].x - c.x < 0 && approx[j].y - c.y > 0) apices[3] = cv::Point2f(approx[j].x, approx[j].y);
                }
                for (int j = 0; j < 4; j++) {
                    std::ostringstream os;
                    os << "apex " << j;
                    cv::circle(out, apices[j], 4, CV_RGB(255,0,0), -1);
                    cv::putText(out, os.str(), apices[j], cv::FONT_HERSHEY_PLAIN, 1, CV_RGB(255,0,0));
                }
                foundMarker = true;
                break;
            }
            catch (const Exception& ex) {
                std::cerr << "can't find center" << std::endl;
            }
        }
    }
}

cv::Point findCenter(std::vector<cv::Point> points, std::vector<cv::Point> contour) {
    for (int i = 0; i < 3; i++) {
        cv::Point c = getIntersection(points[0], points[(i+0)%3+1], points[(i+1)%3+1], points[(i+2)%3+1]);
        if (cv::pointPolygonTest(contour, cv::Point2f(c.x, c.y), false) > 0) {
            return c;
        }
    }
    throw Exception();
}

cv::Point getIntersection(cv::Point p1, cv::Point p2, cv::Point p3, cv::Point p4) {
    double s1 = ((p4.x - p2.x) * (p1.y - p2.y) - (p4.y - p2.y) * (p1.x - p2.x)) / 2.0;
    double s2 = ((p4.x - p2.x) * (p2.y - p3.y) - (p4.y - p2.y) * (p2.x - p3.x)) / 2.0;
    double x = p1.x + (p3.x - p1.x) * s1 / (s1 + s2);
    double y = p1.y + (p3.y - p1.y) * s1 / (s1 + s2);
    return cv::Point(x, y);
}

cv::Point2f convertApex(cv::Point2f src, cv::Mat H) {
    std::vector<cv::Point2f> srcVec;
    srcVec.push_back(src);
    cv::Mat srcMat = cv::Mat(srcVec);
    cv::Mat dstMat = srcMat.clone();
    cv::perspectiveTransform(srcMat, dstMat, H);
    return dstMat.at<cv::Point2f>(0,0);
}

