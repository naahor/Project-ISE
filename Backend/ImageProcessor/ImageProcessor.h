#ifndef IMAGEPROCESSOR_H
#define IMAGEPROCESSOR_H

#include <opencv2/opencv.hpp>
#include <QString>
#include <QImage>
#include <vector>

using namespace std;

// Represents a single continuous run of black or white pixels along a scanned line.
struct ColorRun {
    bool isBlack;     // true if it's a black bar, false if it's a white space
    int pixelLength;  // raw width of the run measured in pixels
    int moduleCount;  // normalized width expressed in barcode modules
};

// Handles barcode image loading, preprocessing, line scanning, 
// run-length measurement, and module normalization using OpenCV.
class ImageProcessor {
private:
    cv::Mat currentImage;
    int thresholdValue;

    // Helper validation to ensure the image matrix is valid
    void validateImage(const cv::Mat& img) const;

public:
    explicit ImageProcessor(int defaultThreshold = 128);

    // Image Loading & State Management
    bool loadImage(const string& filePath);
    void setImage(const cv::Mat& img);
    cv::Mat getCurrentImage() const;

    // Preprocessing Pipeline
    cv::Mat convertToGrayscale(const cv::Mat& img) const;
    cv::Mat applyThreshold(const cv::Mat& grayImg, int threshold) const;

    // Line Scanning & Analysis
    vector<int> scanHorizontalLine(const cv::Mat& binImg, int y) const;
    vector<ColorRun> measureRuns(const vector<int>& scanLinePixels) const;
    vector<int> normalizeRuns(const vector<ColorRun>& runs, int baseModuleWidth) const;

    // Qt Integration Helper for Frontend Partner
    static QImage matToQImage(const cv::Mat& mat);

    // Configuration Getters & Setters
    void setThreshold(int threshold);
    int getThreshold() const;
};

#endif // IMAGEPROCESSOR_H