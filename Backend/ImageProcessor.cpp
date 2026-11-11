#include "ImageProcessor.h"
#include <stdexcept>
#include <cmath>

using namespace std;

ImageProcessor::ImageProcessor(int defaultThreshold)
{
    if (defaultThreshold < 0 || defaultThreshold > 255)
    {
        throw invalid_argument("Threshold must be between 0 and 255.");
    }
    thresholdValue = defaultThreshold;
}

void ImageProcessor::validateImage(const cv::Mat& img) const
{
    if (img.empty())
    {
        throw invalid_argument("Image matrix is empty or uninitialized.");
    }
}

bool ImageProcessor::loadImage(const string& filePath)
{
    cv::Mat loadedImg = cv::imread(filePath, cv::IMREAD_COLOR);
    if (!loadedImg.empty())
    {
        currentImage = loadedImg;
        return true;
    }
    return false;
}

void ImageProcessor::setImage(const cv::Mat& img)
{
    validateImage(img);
    currentImage = img.clone();
}

cv::Mat ImageProcessor::getCurrentImage() const
{
    return currentImage;
}

cv::Mat ImageProcessor::convertToGrayscale(const cv::Mat& img) const
{
    validateImage(img);
    cv::Mat grayImg;

    if (img.channels() == 1)
    {
        return img.clone();
    }

    cv::cvtColor(img, grayImg, cv::COLOR_BGR2GRAY);
    return grayImg;
}

cv::Mat ImageProcessor::applyThreshold(const cv::Mat& grayImg, int threshold) const
{
    validateImage(grayImg);
    cv::Mat gray = convertToGrayscale(grayImg);
    cv::Mat binaryImg;

    cv::threshold(gray, binaryImg, threshold, 255, cv::THRESH_BINARY);
    return binaryImg;
}

vector<int> ImageProcessor::scanHorizontalLine(const cv::Mat& binImg, int y) const
{
    validateImage(binImg);

    if (y < 0 || y >= binImg.rows)
    {
        throw out_of_range("Scan line y-coordinate is out of image bounds.");
    }

    vector<int> linePixels;
    linePixels.reserve(binImg.cols);

    const uchar* rowPtr = binImg.ptr<uchar>(y);
    for (int x = 0; x < binImg.cols; x++)
    {
        linePixels.push_back(rowPtr[x] < 128 ? 0 : 255);
    }

    return linePixels;
}


vector<ColorRun> ImageProcessor::measureRuns(const vector<int>& scanLinePixels) const
{
    if (scanLinePixels.empty())
    {
        return {};
    }

    int start = 0;
    int end = static_cast<int>(scanLinePixels.size()) - 1;

    // Skip the white quiet zone at the beginning
    while (start <= end && scanLinePixels[start] == 255)
    {
        start++;
    }

    // Skip the white quiet zone at the end
    while (end >= start && scanLinePixels[end] == 255)
    {
        end--;
    }

    if (start > end)
    {
        return {};
    }

    vector<ColorRun> runs;

    bool currentIsBlack = (scanLinePixels[start] == 0);
    int currentLength = 1;

    for (int i = start + 1; i <= end; i++)
    {
        bool isBlack = (scanLinePixels[i] == 0);

        if (isBlack == currentIsBlack)
        {
            currentLength++;
        }
        else
        {
            runs.push_back({ currentIsBlack, currentLength, 0 });

            currentIsBlack = isBlack;
            currentLength = 1;
        }
    }

    runs.push_back({ currentIsBlack, currentLength, 0 });

    return runs;
}


vector<int> ImageProcessor::normalizeRuns(const vector<ColorRun>& runs, int baseModuleWidth) const
{
    if (baseModuleWidth <= 0)
    {
        throw invalid_argument("Base module width must be greater than 0.");
    }

    vector<int> moduleCounts;
    moduleCounts.reserve(runs.size());

    for (const auto& run : runs)
    {
        int modules = static_cast<int>(
            round(static_cast<double>(run.pixelLength) / baseModuleWidth)
            );

        if (modules < 1)
        {
            throw invalid_argument("Invalid barcode run width.");
        }

        moduleCounts.push_back(modules);
    }

    return moduleCounts;
}

QImage ImageProcessor::matToQImage(const cv::Mat& mat)
{
    if (mat.empty())
    {
        return QImage();
    }

    if (mat.type() == CV_8UC1)
    {
        return QImage(mat.data, mat.cols, mat.rows, static_cast<int>(mat.step), QImage::Format_Grayscale8).copy();
    }
    else if (mat.type() == CV_8UC3)
    {
        cv::Mat rgbMat;
        cv::cvtColor(mat, rgbMat, cv::COLOR_BGR2RGB);
        return QImage(rgbMat.data, rgbMat.cols, rgbMat.rows, static_cast<int>(rgbMat.step), QImage::Format_RGB888).copy();
    }

    return QImage();
}

void ImageProcessor::setThreshold(int threshold)
{
    if (threshold < 0 || threshold > 255)
    {
        throw invalid_argument("Threshold must be between 0 and 255.");
    }
    thresholdValue = threshold;
}

int ImageProcessor::getThreshold() const
{
    return thresholdValue;
}