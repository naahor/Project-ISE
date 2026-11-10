#include "ImageProcessor.h"
#include <stdexcept>
#include <cmath>
#include <algorithm>

using namespace std;

// Initializes the ImageProcessor with a default binarization threshold.

ImageProcessor::ImageProcessor(int defaultThreshold)
{
    if (defaultThreshold < 0 || defaultThreshold > 255)
    {
        throw invalid_argument("Threshold must be between 0 and 255.");
    }
    thresholdValue = defaultThreshold;
}

// Validates that an image is not null or empty.

void ImageProcessor::validateImage(const QImage& img) const
{
    if (img.isNull())
    {
        throw invalid_argument("Image is null or uninitialized.");
    }
}

// Loads a barcode image from disk file path. Returns true if successful.

bool ImageProcessor::loadImage(const QString& filePath)
{
    QImage loadedImg;
    if (loadedImg.load(filePath))
    {
        currentImage = loadedImg;
        return true;
    }
    return false;
}

// Directly sets the image (useful when received from UI drag-and-drop or camera feeds).

void ImageProcessor::setImage(const QImage& img)
{
    validateImage(img);
    currentImage = img;
}


// Returns the currently stored image.

QImage ImageProcessor::getCurrentImage() const
{
    return currentImage;
}

// Converts a standard QImage to an 8-bit grayscale QImage.
// Perfect for UI preview generation on the frontend.

QImage ImageProcessor::convertToGrayscale(const QImage& img) const
{
    validateImage(img);
    return img.convertToFormat(QImage::Format_Grayscale8);
}


// Applies binary thresholding to a grayscale image. 
// Pixels below the threshold become pure black (0), others become pure white (255).

QImage ImageProcessor::applyThreshold(const QImage& grayImg, int threshold) const
{
    validateImage(grayImg);

    // Ensure image is in grayscale format first
    QImage gray = grayImg.convertToFormat(QImage::Format_Grayscale8);
    QImage binaryImg(gray.size(), QImage::Format_RGB32);

    QRgb black = qRgb(0, 0, 0);
    QRgb white = qRgb(255, 255, 255);

    for (int y = 0; y < gray.height(); y++)
    {
        const uchar* scanLine = gray.constScanLine(y);
        for (int x = 0; x < gray.width(); x++)
        {
            // Compare pixel intensity against the threshold
            if (scanLine[x] < threshold)
            {
                binaryImg.setPixel(x, y, black);
            }
            else
            {
                binaryImg.setPixel(x, y, white);
            }
        }
    }

    return binaryImg;
}

// Scans a specific horizontal row (y-coordinate) across the binary barcode image.
// Returns a vector of pixel color values (0 for black, 255 for white).

vector<int> ImageProcessor::scanHorizontalLine(const QImage& binImg, int y) const
{
    validateImage(binImg);

    if (y < 0 || y >= binImg.height())
    {
        throw out_of_range("Scan line y-coordinate is out of image bounds.");
    }

    vector<int> linePixels;
    linePixels.reserve(binImg.width());

    for (int x = 0; x < binImg.width(); x++)
    {
        QRgb pixel = binImg.pixel(x, y);
        // Check if the pixel is black (red component == 0)
        bool isBlack = (qRed(pixel) == 0);
        linePixels.push_back(isBlack ? 0 : 255);
    }

    return linePixels;
}


// Measures consecutive black and white runs along a scanned pixel line (Run-Length Encoding).

vector<ColorRun> ImageProcessor::measureRuns(const vector<int>& scanLinePixels) const
{
    if (scanLinePixels.empty())
    {
        return {};
    }

    vector<ColorRun> runs;
    bool currentIsBlack = (scanLinePixels[0] == 0);
    int currentLength = 1;

    for (size_t i = 1; i < scanLinePixels.size(); i++)
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
    // Push the final accumulated run
    runs.push_back({ currentIsBlack, currentLength, 0 });

    return runs;
}

// Normalizes raw pixel run widths into discrete module counts based on the base module width.

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
        // Round to the nearest whole module count
        int modules = static_cast<int>(round(static_cast<double>(run.pixelLength) / baseModuleWidth));
        moduleCounts.push_back(max(1, modules)); // Ensure at least 1 module
    }

    return moduleCounts;
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