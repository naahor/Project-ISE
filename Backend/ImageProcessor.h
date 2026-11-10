#ifndef IMAGEPROCESSOR_H
#define IMAGEPROCESSOR_H

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


// Handles barcode image loading, preprocessing (grayscale, thresholding),
// line scanning, run-length measurement, and module normalization.
// Designed for clean decoupling and easy integration with a Qt frontend.

class ImageProcessor {
private:
    QImage currentImage;
    int thresholdValue;

    // Helper validation to ensure the image is loaded and valid
    void validateImage(const QImage& img) const;

public:
    explicit ImageProcessor(int defaultThreshold = 128);

    // Image Loading & State Management
    bool loadImage(const QString& filePath);
    void setImage(const QImage& img);
    QImage getCurrentImage() const;

    // Preprocessing Pipeline
    QImage convertToGrayscale(const QImage& img) const;
    QImage applyThreshold(const QImage& grayImg, int threshold) const;

    // Line Scanning & Analysis
    vector<int> scanHorizontalLine(const QImage& binImg, int y) const;
    vector<ColorRun> measureRuns(const vector<int>& scanLinePixels) const;
    vector<int> normalizeRuns(const vector<ColorRun>& runs, int baseModuleWidth) const;

    // Configuration
    void setThreshold(int threshold);
    int getThreshold() const;
};

#endif