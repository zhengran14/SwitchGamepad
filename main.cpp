#include "gamepad.h"

#include <QApplication>
//#undef slots
//#include <Python.h>
//#define slots Q_SLOTS
//#include "opencv2/core/core.hpp"
//#include "opencv2/opencv.hpp"
//#include "opencv2/highgui/highgui.hpp"
//#include <utils.h>
#include <player.h>
//#include <tesseract/baseapi.h>      // tesseract提供的关于C++的接口
//#include <tesseract/strngs.h>
//#include <leptonica/allheaders.h>

int main(int argc, char *argv[])
{
//    cv::VideoCapture videoCapture = cv::VideoCapture(0);
//    videoCapture.set(cv::CAP_PROP_FRAME_WIDTH, 1280);
//    videoCapture.set(cv::CAP_PROP_FRAME_HEIGHT, 720);
//    videoCapture.set(cv::CAP_PROP_FPS, 30);
//    cv::Mat videoFrame;
//    while(true){
//        videoCapture >> videoFrame;
//        if(videoFrame.empty()){
//            break;
//        }
//        imshow("video demo", videoFrame);
//        cv::waitKey(33);
//    }

//    videoCapture.release();
//    cv::destroyAllWindows();

//    QImage img(":/res/shine_template.jpg");
//    cv::Mat captureFrame = cv::imread("capture test.jpg");
//    cv::Mat shineTemplate = Utils::QImage2cvMat(img);
//    cv::Mat shineTemplate2;
//    cv::cvtColor(shineTemplate, shineTemplate2, cv::COLOR_BGR2RGB);
//    cv::Mat dstImg;
//    dstImg.create(captureFrame.dims, captureFrame.size, captureFrame.type());
//    cv::matchTemplate(captureFrame, shineTemplate2, dstImg, 0);
//    cv::Point minPoint;
//    cv::Point maxPoint;
//    double *minVal = 0;
//    double *maxVal = 0;
//    cv::minMaxLoc(dstImg, minVal, maxVal, &minPoint,&maxPoint);
//    maxPoint = cv::Point(minPoint.x + shineTemplate2.cols, minPoint.y + shineTemplate2.rows);
//    cv::rectangle(captureFrame, minPoint, maxPoint, cv::Scalar(0,255,0), 2, 8);
//    cv::imshow("123", captureFrame);
//    dstImg.release();
//    captureFrame.release();
//    shineTemplate2.release();
//    shineTemplate.release();

//    tesseract::TessBaseAPI *api = new tesseract::TessBaseAPI();
//    // Initialize tesseract-ocr with English, without specifying tessdata path
////    if (api->Init(NULL, NULL))
////    if (api->Init("/Users/rabbit/Downloads/SwitchGamepad/tesseract/tessdata_fast", "chi_sim+eng"))
//    if (api->Init("/Users/rabbit/Downloads/SwitchGamepad/tesseract/tessdata_best", "chi_sim"))
//    {
////        fprintf(stderr, "Could not initialize tesseract.\n");
//        return 0;
//    }

//    // Open input image with leptonica library
//    Pix *image = pixRead("/Users/rabbit/Downloads/2.png"); //absolute path of file
//    api->SetImage(image);

//    // Get OCR result
//    char *outText = api->GetUTF8Text();
//    qDebug() << QString::fromUtf8(outText);
////    printf("OCR output:\n%s", outText);

//    // Destroy used object and release memory
//    api->End();
//    delete [] outText;
//    pixDestroy(&image);

    QApplication a(argc, argv);
    Player p;
    Gamepad w;
    w.show();

    return a.exec();
}
