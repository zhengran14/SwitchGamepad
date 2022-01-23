#include "utils.h"
#include <QDebug>
#include "opencv2/imgproc/types_c.h"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/opencv.hpp"

Utils::Utils(QObject *parent) : QObject(parent)
{

}

QImage Utils::cvMat2QImage(const cv::Mat& mat, bool clone, bool rb_swap)
{
    const uchar *pSrc = (const uchar*)mat.data;
    // 8-bits unsigned, NO. OF CHANNELS = 1
    if(mat.type() == CV_8UC1)
    {
        //QImage image(mat.cols, mat.rows, QImage::Format_Grayscale8);
        QImage image(pSrc, mat.cols, mat.rows, mat.step, QImage::Format_Grayscale8);
        if(clone) return image.copy();
        return image;
    }
    // 8-bits unsigned, NO. OF CHANNELS = 3
    else if(mat.type() == CV_8UC3)
    {
        // Create QImage with same dimensions as input Mat
        QImage image(pSrc, mat.cols, mat.rows, mat.step, QImage::Format_RGB888);
        if(clone)
        {
            if(rb_swap) return image.rgbSwapped();
            return image.copy();
        }
        else
        {
            if(rb_swap)
            {
                cv::cvtColor(mat, mat, CV_BGR2RGB);
            }
            return image;
        }

    }
    else if(mat.type() == CV_8UC4)
    {
        qDebug() << "CV_8UC4";
        QImage image(pSrc, mat.cols, mat.rows, mat.step, QImage::Format_ARGB32);
        if(clone) return image.copy();
        return image;
    }
    else
    {
        qDebug() << "ERROR: Mat could not be converted to QImage.";
        return QImage();
    }
}

cv::Mat Utils::QImage2cvMat(QImage &image, bool clone, bool rb_swap)
{
    cv::Mat mat;
    //qDebug() << image.format();
    switch(image.format())
    {
    case QImage::Format_ARGB32:
    case QImage::Format_RGB32:
    case QImage::Format_ARGB32_Premultiplied:
        mat = cv::Mat(image.height(), image.width(), CV_8UC4, (void *)image.constBits(), image.bytesPerLine());
        if(clone)  mat = mat.clone();
        break;
    case QImage::Format_RGB888:
        mat = cv::Mat(image.height(), image.width(), CV_8UC3, (void *)image.constBits(), image.bytesPerLine());
        if(clone)  mat = mat.clone();
        if(rb_swap) cv::cvtColor(mat, mat, CV_BGR2RGB);
        break;
    case QImage::Format_Indexed8:
    case QImage::Format_Grayscale8:
        mat = cv::Mat(image.height(), image.width(), CV_8UC1, (void *)image.bits(), image.bytesPerLine());
        if(clone)  mat = mat.clone();
        break;
    }
    return mat;
}

QString Utils::AHash(const cv::Mat& mat)
{
    // 均值哈希算法
    // 缩放为8*8
    cv::Mat img;
    cv::resize(mat, img, cv::Size(8, 8));
    // 转换为灰度图
    cv::Mat gray;
    cv::cvtColor(img, gray, cv::COLOR_BGR2GRAY);
    // s为像素和初值为0，hash_str为hash值初值为''
    int s = 0;
    QString hash_str = "";
    // 遍历累加求像素和
    for(int i = 0; i < 8; i++) {
        for(int j = 0; j < 8; j++) {
            s = s + gray.at<uchar>(i, j);
        }
    }
    // 求平均灰度
    double avg = s / 64.0;
    // 灰度大于平均值为1相反为0生成图片的hash值
    for (int i = 0; i < 8; i++) {
        for(int j = 0; j < 8; j++) {
            if (gray.at<uchar>(i, j) > avg) {
                hash_str = hash_str + '1';
            }
            else {
                hash_str = hash_str + '0';
            }
        }
    }
    img.release();
    gray.release();
    return hash_str;
}

QString Utils::DHash(const cv::Mat& mat)
{
    // 差值哈希算法
    // 缩放8*8
    cv::Mat img;
    cv::resize(mat, img, cv::Size(9, 8));
    // 转换灰度图
    cv::Mat gray;
    cv::cvtColor(img, gray, cv::COLOR_BGR2GRAY);
    QString hash_str = "";
    // 每行前一个像素大于后一个像素为1，相反为0，生成哈希
    for (int i = 0; i < 8; i++) {
        for(int j = 0; j < 8; j++) {
            if (gray.at<uchar>(i, j) > gray.at<uchar>(i, j + 1)) {
                hash_str = hash_str + '1';
            }
            else {
                hash_str = hash_str + '0';
            }
        }
    }
    img.release();
    gray.release();
    return hash_str;
}

double Utils::GrayHistogram(const cv::Mat& mat1, const cv::Mat& mat2)
{
    // 灰度直方图算法
    // 计算单通道的直方图的相似值
    int histBinNum = 256;
    float range[] = { 0.0, 255.0 };
    const float* histRange = { range };
    int channels = 0;
    cv::Mat hist1;
    cv::calcHist(&mat1, 1, &channels, cv::Mat(), hist1, 1, &histBinNum, &histRange);
    cv::Mat hist2;
    cv::calcHist(&mat2, 1, &channels, cv::Mat(), hist2, 1, &histBinNum, &histRange);
    // 计算直方图的重合度
    double degree = 0.0;
    for (int i = 0; i < hist1.rows; i++) {
        if (hist1.at<uchar>(i) != hist2.at<uchar>(i)) {
            degree = degree +
                    (1.0 - (double)qAbs(hist1.at<uchar>(i) - hist2.at<uchar>(i)) / (double)qMax(hist1.at<uchar>(i), hist2.at<uchar>(i)));
        }
        else {
            degree = degree + 1;
        }
    }
    degree = degree / (double)hist1.rows;
    hist1.release();
    hist2.release();
    return degree;
}

double Utils::RGBHistogram(const cv::Mat& mat1, const cv::Mat& mat2)
{
    // RGB每个通道的直方图相似度
    // 将图像resize后，分离为RGB三个通道，再计算每个通道的相似值
    cv::Mat image1;
    cv::resize(mat1, image1, cv::Size(256, 256));
    cv::Mat image2;
    cv::resize(mat2, image2, cv::Size(256, 256));
    std::vector<cv::Mat> sub_image1(3);
    cv::split(image1, sub_image1);
    std::vector<cv::Mat> sub_image2(3);
    cv::split(image2, sub_image2);
    double sub_data = 0.0;
    for (int i = 0; i < sub_image1.size(); i++) {
        sub_data += GrayHistogram(sub_image1[i], sub_image2[i]);
        sub_image1[i].release();
        sub_image2[i].release();
    }
    sub_data = sub_data / 3.0;
    image1.release();
    image2.release();
    return sub_data;
}

int Utils::HashCompare(QString hash1, QString hash2)
{
    // Hash值对比
    // 算法中1和0顺序组合起来的即是图片的指纹hash。顺序不固定，但是比较的时候必须是相同的顺序。
    // 对比两幅图的指纹，计算汉明距离，即两个64位的hash值有多少是不一样的，不同的位数越小，图片越相似
    // 汉明距离：一组二进制数据变成另一组数据所需要的步骤，可以衡量两图的差异，汉明距离越小，则相似度越高。汉明距离为0，即两张图片完全一样
    int n = 0;
    // hash长度不同则返回-1代表传参出错
    if (hash1.length() != hash2.length()) {
        return -1;
    }
    // 遍历判断
    for (int i = 0; i < hash1.length(); i++) {
        // 不相等则n计数+1，n最终为相似度
        if (hash1[i] != hash2[i]) {
            n = n + 1;
        }
    }
    return n;
}

int Utils::SimilarImage(const cv::Mat &mat1, const cv::Mat &mat2, int method)
{
    // 均值、差值、感知哈希算法三种算法值越小，则越相似,相同图片值为0
    // 三直方图算法和单通道的直方图 0-1之间，值越大，越相似。 相同图片为1
    double res = 0.0;

    switch (method) {
    case 1: {
        QString hash1 = Utils::AHash(mat1);
        QString hash2 = Utils::AHash(mat2);
        int n = Utils::HashCompare(hash1, hash2);
        res = 1.0 - float(n / 64.0);
        break;
    }
    case 2: {
        QString hash1 = Utils::DHash(mat1);
        QString hash2 = Utils::DHash(mat2);
        int n = Utils::HashCompare(hash1, hash2);
        res = 1.0 - float(n / 64.0);
        break;
    }
    case 3: {
        double n = Utils::RGBHistogram(mat1, mat2);
        res = n;
        break;
    }
    case 4: {
        double n = Utils::GrayHistogram(mat1, mat2);
        res = n;
        break;
    }
    }
    return (int)(res* 100);
}
