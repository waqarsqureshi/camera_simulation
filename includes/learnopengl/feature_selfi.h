#include<opencv2/opencv.hpp>

#include <iostream>
#include <string>
#include <math.h>
#include <stdio.h>
using namespace std;
using namespace cv;

// structure to store feature from images
struct param {
    float desc1, desc2, desc3, desc4, desc5, desc6, desc7;
};

param find_parameters(Mat frame, string pathToHaar);
Point shoulder(Mat img, string side );

param find_parameters(Mat img,string pathToHaar)
{
    param data;
    CascadeClassifier face_cascade;
    string fileHaar = pathToHaar + "haarcascade_frontalface_alt.xml";
    face_cascade.load(fileHaar);
    Mat  clone,clone1, gray,clone_gray,shoulder_roi;
    Rect ROI;
    vector<Rect> faces;
    size_t i;
    bool face_present = 0;
    int name = 0,face_height = 0,face_width = 0,frame_width = 0, frame_height = 0,index,xn,yn,FH2,FW2;
    Point face_bottom,left,right,shifted_left,shifted_right,face_center;
    shifted_left.x = shifted_left.y = 0;
    shifted_right.x = shifted_right.y = 0;
    //cap >> img;
    face_present = false;
    frame_width = img.cols;
    frame_height = img.rows;
    FH2 = frame_height/2;
    FW2 = frame_width/2;
    cvtColor(img,gray,CV_BGR2GRAY);
    face_cascade.detectMultiScale( gray, faces, 1.1,5, 0|CASCADE_SCALE_IMAGE, Size(10, 10) );
    for ( i = 0; i < faces.size(); i++ )
    {
        if(faces.size()==0 || faces.size()>1)
            break;
        else{
            face_present = true;
            clone = img.clone();
            clone1 = img.clone();
            face_height = faces[i].height;
            face_bottom.y = faces[i].y + faces[i].height + 1;
            face_bottom.x = faces[i].x;
            face_width = faces[i].width;
            cout<<face_height<<"___"<<face_width<<endl;
            face_center.x = faces[i].x + face_width/2;
            face_center.y = faces[i].y + face_height/2;
            xn = face_center.x - frame_width/2;
            yn = face_center.y - frame_height/2;
            circle(clone,face_center,3,Scalar(0,0,255),2);
            rectangle(clone,faces[i],Scalar(0,255,0),2);
            break;
        }
    }
    if(face_present==true){
        cvtColor(clone1,clone_gray,CV_BGR2GRAY);
        threshold(clone_gray,clone_gray,10,255,THRESH_BINARY);
        vector<vector<Point> > contours;
        findContours( clone_gray, contours, RETR_TREE, CHAIN_APPROX_SIMPLE );
        vector<vector<Point> > contours_poly( contours.size() );
        vector<Rect> boundRect( contours.size() );
        for( size_t i = 0; i < contours.size(); i++ ){
            approxPolyDP( contours[i], contours_poly[i], 3, true );
            boundRect[i] = boundingRect( contours_poly[i] );
            if(boundRect[i].width>face_width*1.5 && boundRect[i].height>face_height*4){
                ROI = Rect(Point(boundRect[i].tl().x,face_bottom.y),Point(boundRect[i].br().x,face_bottom.y+face_width*1.2));
                rectangle( clone, Point(boundRect[i].tl().x,face_bottom.y), boundRect[i].br(), Scalar(0,0,255), 2 );
                shoulder_roi = clone1(ROI);
                index = i;
                break;
            }
        }
        // FIND SHOULDER POINTS
        left = shoulder(shoulder_roi,"left");
        right = shoulder(shoulder_roi,"right");
        // SHIFT POINTS TO ORIGINAL IMAGE
        shifted_left.x = left.x + boundRect[index].tl().x;
        shifted_left.y = left.y+face_bottom.y;
        shifted_right.x = right.x + boundRect[index].tl().x;
        shifted_right.y = right.y+face_bottom.y;
        circle(clone,shifted_left,4,Scalar(255,0,0),-1);
        circle(clone,shifted_right,4,Scalar(0,0,255),-1);
        line(clone,shifted_left,shifted_right,Scalar(0,0,255),2);
        // FIND PARAMETERS
        float slope = (float)(shifted_right.y - shifted_left.y) / (shifted_right.x - shifted_left.x);
        int shoulder_width = shifted_right.x - shifted_left.x;
        int body_height = boundRect[index].br().y - boundRect[index].tl().y;
        data.desc1 = slope;
        data.desc2 = (float)shoulder_width/body_height;
        data.desc3 = (float)shoulder_width/clone.cols;
        data.desc4 = (float)face_height/body_height;
        data.desc5 = (float)face_height/shoulder_width;
        data.desc6 = (float)xn/FW2;
        data.desc7 = (float)yn/FH2;
        namedWindow("Template Detected",WINDOW_NORMAL);
        imshow("Template Detected",clone);
        int key = cv::waitKey(0);
        if (key==27)
            destroyWindow("Template Detected");
    }
    else
        data.desc1 = data.desc2 = data.desc3 = data.desc4 = data.desc5 = data.desc6 = data.desc7 = 0;
    return data;
}

Point shoulder(Mat img, string side )
{
  Mat src_gray,canny_output,left,right;
  Rect left_roi,right_roi;
  vector<vector<Point> > contours;
  vector<Vec4i> hierarchy;
  cvtColor(img,src_gray,CV_BGR2GRAY);
  threshold(src_gray,src_gray,15,255,THRESH_BINARY);
  Canny( src_gray, canny_output, 200, 255, 3 );
  if(side == "right"){
      left_roi = Rect(canny_output.cols/2,0,canny_output.cols/2,canny_output.rows);
      left = canny_output(left_roi);
      canny_output = left.clone();
  }
  else{
      right_roi = Rect(0,0,canny_output.cols/2,canny_output.rows);
      right = canny_output(right_roi);
      canny_output = right.clone();
  }
  findContours( canny_output, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0) );
  vector<Moments> mu(contours.size() );
  for( size_t i = 0; i < contours.size(); i++ )
     { mu[i] = moments( contours[i], false );}
  vector<Point2f> mc( contours.size() );
  for( size_t i = 0; i < contours.size(); i++ )
     { mc[i] = Point2f( static_cast<float>(mu[i].m10/mu[i].m00) , static_cast<float>(mu[i].m01/mu[i].m00) ); }
  int largest_area1 = 0,largest_contour_index1 = 0;
  for( int i = 0; i< contours.size(); i++ )
      {
          int a=arcLength(contours[i],true);
          if(a>largest_area1){
              largest_area1=a;
              largest_contour_index1=i;
          }
      }
  if(side == "right"){
      mc[largest_contour_index1].x = mc[largest_contour_index1].x + canny_output.cols;
      return mc[largest_contour_index1];
  }
  else
      return mc[largest_contour_index1];
}
