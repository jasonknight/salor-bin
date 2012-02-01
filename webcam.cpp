#include "webcam.h"
#include <QDebug>
#include <QFile>
#include <cv.h> //main OpenCV functions
#include <highgui.h> //OpenCV GUI functions ̄
#include <stdio.h>
WebCam::WebCam(QObject *parent) :
    QThread(parent)
{
}
void WebCam::run() {
	int ret;
	CvCapture* cap = cvCaptureFromCAM( this->addy );
	ret = cvGrabFrame(cap);
	if (ret) {
	IplImage * img = cvRetrieveFrame(cap);
	if (img) {
		qDebug() << "writing file to: " << this->filePath;
		cvSaveImage(this->filePath.toLatin1().data(),img);
		cvReleaseCapture(&cap);
		if (this->id > 0) {
			qDebug() << "emitting signale";
			emit imageSaved(this->id,this->filePath);
		}
	} else {
		qDebug() << "Failed..";
	}
	} else {
		qDebug() << "epic failure";
	}
}
