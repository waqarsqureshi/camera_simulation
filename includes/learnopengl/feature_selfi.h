#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/core/mat.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/video.hpp>
#include "opencv2/objdetect.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/videoio.hpp"
#include <opencv/cv.h>
#include <iostream>
#include <string>
#include <math.h>
#include <stdio.h>
using namespace std;
using namespace cv;

Point2f shoulder_2(Mat fs,string rl, int fw, int dd_mode);
bool findface(Mat face);
struct param {
    float desc1, desc2, desc3, desc4, desc5;
};
param find_parameters(Mat frame);

param find_parameters(Mat img)
{
    param data;
    CascadeClassifier face;
    vector<Rect> faces;
    Mat gray,face_roi;
    Rect ROI,Face_ROI;
    Point p,p1,p2;
    size_t i;
    Point face_p1,face_p2;
    int face_b;
    face.load("haarcascade_frontalface_alt.xml");
    bool face_present = false;
    clock_t begin = clock();
    int width = img.cols, face_height,face_width,check,check1,start,l,r=0,b,x,lx;
    cvtColor(img,gray,CV_BGR2GRAY);
    face.detectMultiScale( gray, faces, 1.1,2, 0|CASCADE_SCALE_IMAGE, Size(30, 30) );
    for ( i = 0; i < faces.size(); i++ )
    {
        Face_ROI = Rect(faces[i].x, faces[i].y, faces[i].width, faces[i].height);
        face_roi = img(Face_ROI);
        face_present = findface(face_roi);
        if(face_present==false)
            break;
        //face detection
        face_b = faces[i].y + faces[i].height;
        p2.y = faces[i].y + faces[i].height ;
        p.y = faces[i].y + faces[i].height;
        face_width = faces[i].width;
        p1.y = p.y + faces[i].height;
        rectangle(img,faces[i],Scalar(255,0,0),4);
        ROI = Rect(0, p.y+5, width, faces[i].height*2);
        face_height = faces[i].height;
        check = p.y + face_height;
        check1 = p.y + (face_height*4);
        l = width;
        //face and body parameters
        face_p1.x = faces[i].x + face_width/2;
        face_p1.y = faces[i].y;
        face_p2.x = face_p1.x;
        face_p2.y = face_p1.y + face_height;
        line(img,face_p1,face_p2,Scalar(0,255,0),4);
    }
    if(!face_present == false){
    Mat img_clone = img.clone();
    cvtColor(img,img,CV_BGR2GRAY);
    // LEFT POINT
    for(x = check;x<check1;x+=5){
    for(start = 0;start<img.cols;start++){
        int val = (int)img.at<uchar>(x,start);
        if(val > 15){
            if(start < l ){
                l= start;
                lx = x;
            }
                img.at<uchar>(x,start) = 255;
                break;
                }
    }}
    for(int y = check;y<check1;y+=5){
    for(start = img.cols;start>1;start--){
        int val = (int)img.at<uchar>(y,start);
        if(val > 15){
            if(start > r ){
                r= start;
            }
                           img.at<uchar>(y,start) = 255;
                           break;
                       }
    }}
    // BOTTOM POINT
    bool exit;
    for(check = img.rows-15;check>8;check-=5){
    for(start = img.cols;start>1;start--){
        int val = (int)img.at<uchar>(check,start);
        if(val > 15){
                           b = check;
                           img.at<uchar>(check,start) = 255;
                           exit = true;
                           break;
                       }
                       else{
                           exit =  false;
                       }
    }
    if(exit==true){break;}
    }
    Mat clone2 = img_clone.clone();
    Point b1 = p2, b2;
    b1.x = l;
    b2.x = r;
    b2.y = b;
    rectangle(img_clone,b1,b2,Scalar(0,255,0),4,8);
    Point b1_c=b1,b2_c=b2;
    int body_width = b2.x - b1.x;
    int body_height = (b2_c.y - b1.y);
    b1_c.x +=body_width/2;
    b2_c.x -=body_width/2;
    line(img_clone,b1_c,b2_c,Scalar(0,0,255),4,8);
    Mat image_roi = clone2(ROI);
    Point2f shoulder_right = shoulder_2(image_roi,"left",face_width,1);
    Point2f shoulder_left = shoulder_2(image_roi,"right",face_width,1);
    shoulder_left.y += face_b;
    shoulder_right.y += face_b;
    line(img_clone,shoulder_left,shoulder_right,Scalar(0,0,255),4,8);
    int shoulder_width = shoulder_right.x - shoulder_left.x;
    float slope;
    slope = (shoulder_right.y-shoulder_left.y)/(shoulder_right.x-shoulder_left.x);
    data.desc1 = slope;
    data.desc2 = (float)shoulder_width/body_height;
    data.desc3 = (float)shoulder_width/width;
    data.desc4 = (float)face_height/body_height;
    data.desc5 = (float) face_height/shoulder_width;
    clock_t end = clock();
    cout<<"Total Time = "<< double(end - begin) / CLOCKS_PER_SEC<<endl;
    namedWindow("OUTPUT",WINDOW_NORMAL);
    moveWindow("OUTPUT",500,100);
    resizeWindow("OUTPUT",Size(640,480));
    imshow("OUTPUT",img_clone);
    waitKey(1);
    return data;
    }
    else{
        cout<<"NO FACE"<<endl;
        data.desc1 = data.desc2 = data.desc3 = data.desc4 = 0;
        return data;
    }

}

Point2f shoulder_2(Mat fs,string rl, int fw, int dd_mode)
{
    Mat cdst = Mat::zeros(fs.size(), CV_8UC1);
    Point2f shoulder;
    Point sc;
    if(rl == "right")
    sc = Point(fs.cols , fs.rows);
    else if(rl==  "left")
    sc = Point(0 , fs.rows);
    int erosion_size = 2;
    int dilation_size = 2;
    cvtColor (fs, fs, COLOR_BGR2GRAY);
    threshold (fs, fs, 15, 255, CV_THRESH_BINARY);
    Mat element = getStructuringElement (MORPH_ELLIPSE, Size (2 * dilation_size + 1, 2 * dilation_size + 1), Point (-1, -1));
    element = getStructuringElement (MORPH_ELLIPSE, Size (2 * erosion_size + 1, 2 * erosion_size + 1), Point (-1, -1));
    blur (fs, fs, Size (3, 3));
    vector<vector<Point> > contours1;
    vector<cv::Vec4i> hierarchy1;
    int largest_index = 0;
    int largest_area = 0;
    findContours (fs , contours1 , hierarchy1 , CV_RETR_EXTERNAL , CV_CHAIN_APPROX_SIMPLE , Point (0, 0));
    if (!contours1.empty())
      {
        for (size_t k1 = 0; k1 < contours1.size (); k1++)
    {
      double a = contourArea (contours1[k1], false); //  Find the area of contour
      if (a > largest_area)
        {
          largest_area = a;
          largest_index = k1; //Store the index of largest contour
          }
    }
      }
    Mat drawing2 = Mat::zeros (cdst.size (), CV_8UC1);
    drawContours (drawing2, contours1, largest_index, Scalar(255,255,255), -1, 8, hierarchy1, 0, Point ());
    morphologyEx (drawing2, drawing2, MORPH_CLOSE, getStructuringElement (MORPH_ELLIPSE, Size (15, 15) ) );
    cdst = drawing2.clone();
    Canny (drawing2, cdst, 300, 600, 5);
    element = getStructuringElement (MORPH_ELLIPSE, Size (2,2), Point (-1, -1));
    dilate (cdst, cdst, element);
    std::vector<std::vector<cv::Point> > contours;
    std::vector<cv::Vec4i> hierarchy;
    findContours (cdst , contours , hierarchy , CV_RETR_EXTERNAL , CV_CHAIN_APPROX_SIMPLE , Point (0, 0));
    std::vector<Moments> mu (contours.size ());
    float sx[100] = {} , tx[100] = {}, ty[100] =  {};
    if (!contours.empty())
      {
        for (size_t k1 = 0; k1 < contours.size (); k1++)
    {
      mu[k1] = moments (contours[k1], false);
    }
        std::vector<Point2f> mc (contours.size ());
        for (size_t k2 = 0; k2 < contours.size (); k2++)
    {
      mc[k2] = Point2f (float (mu[k2].m10 / mu[k2].m00),
                     float (mu[k2].m01 / mu[k2].m00));
    }
        Mat drawing = Mat::zeros (cdst.size (), CV_8UC3);
        int k4 = 0;
        RNG rng2 (12345);
        for (size_t k3 = 0; k3 < contours.size (); k3++)
    {
      Scalar color = Scalar (rng2.uniform (0, 255), rng2.uniform (0, 255), rng2.uniform (0, 255));
      drawContours (drawing, contours, k3, color, 2, 8, hierarchy, 0, Point ());
      circle (drawing, mc[k3], 4, Scalar(100,200,0), -1, 8, 0);
      if (mc[k3].x > 0 && mc[k3].y > 0)
        {
          sx[k4] = mc[k3].x;
          tx[k4] = mc[k3].x;
          ty[k4] = mc[k3].y;
          k4++;
        }
    }
        sort (sx, sx + k4);
        if (rl == "right")	{	  shoulder.x = sx[0];	}
        else if (rl == "left")	{	  shoulder.x = sx[k4-1];	}
        for (int j2 = 0; j2 < k4; j2++)
    {
      if (tx[j2] == shoulder.x)	{	shoulder.y = ty[j2];	}
    }       drawing.release ();
      }
    fs.release ();
    return shoulder;
}

bool findface(Mat face)
{
  bool found = false ;
  resize (face, face, Size (face.cols * 2, face.rows * 2), 0, 0, 3);
  Mat f_ycrcb, skin;
  cvtColor (face, f_ycrcb, COLOR_BGR2YCrCb);
  //inRange(f_ycrcb,Scalar(60, 135, 90), Scalar (255, 170, 135),skin);
  inRange(f_ycrcb,Scalar(60, 135, 90), Scalar (255, 255, 255),skin);
  Mat f_s;
  int num=0;
  face.copyTo(f_s,skin);
  for(int i =0;i<skin.rows;i++){
      for(int j =0;j<skin.cols;j++){
          int val = (int)skin.at<uchar>(i,j);
          if(val > 15){
              num++;
          }
      }
  }
  float total = skin.rows*skin.cols;
  float per = (num/total)*100;
  if(per > 69){
      found = true;
  }
 return found;
}
