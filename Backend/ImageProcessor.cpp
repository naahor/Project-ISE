#include "ImageProcessor.h"
#include <QColor>
#include <algorithm>
#include <cmath>
#include <QDebug>

ImageProcessor::ImageProcessor() {}

bool ImageProcessor::processBarcodeImage(const QString& imagePath, QVector<int>& outNormalizedRuns) {
    QImage img(imagePath);
    if (img.isNull()) {
        qDebug() << "ImageProcessor: Failed to load image from" << imagePath;
        return false;
    }
    return processBarcodeImage(img, outNormalizedRuns);
}

bool ImageProcessor::processBarcodeImage(const QImage& image, QVector<int>& outNormalizedRuns) {
    if (image.isNull()) return false;

    QImage gray = convertToGrayscale(image);
    QImage binary = applyOtsuThreshold(gray);

    // Scan horizontally across the vertical midpoint
    int midY = binary.height() / 2;
    QVector<int> rawRuns = extractRunLengths(binary, midY);

    if (rawRuns.isEmpty()) {
        qDebug() << "ImageProcessor: No runs extracted from scanline.";
        return false;
    }

    qDebug() << "ImageProcessor: Raw runs (" << rawRuns.size() << "):" << rawRuns;

    outNormalizedRuns = normalizeRuns(rawRuns);

    qDebug() << "ImageProcessor: Normalized runs (" << outNormalizedRuns.size() << "):" << outNormalizedRuns;

    return !outNormalizedRuns.isEmpty();
}

QImage ImageProcessor::convertToGrayscale(const QImage& src) {
    // Convert to ARGB32 first so scanLine() always yields QRgb* safely
    QImage argbSrc = src.convertToFormat(QImage::Format_ARGB32);

    QImage gray(argbSrc.size(), QImage::Format_Grayscale8);
    for (int y = 0; y < argbSrc.height(); ++y) {
        const QRgb* scan = reinterpret_cast<const QRgb*>(argbSrc.constScanLine(y));
        uchar* dest = gray.scanLine(y);
        for (int x = 0; x < argbSrc.width(); ++x) {
            dest[x] = static_cast<uchar>(qGray(scan[x]));
        }
    }
    return gray;
}

QImage ImageProcessor::applyOtsuThreshold(const QImage& grayImg) {
    int histogram[256] = { 0 };
    int totalPixels = grayImg.width() * grayImg.height();

    for (int y = 0; y < grayImg.height(); ++y) {
        const uchar* line = grayImg.constScanLine(y);
        for (int x = 0; x < grayImg.width(); ++x) {
            histogram[line[x]]++;
        }
    }

    float sum = 0;
    for (int i = 0; i < 256; ++i) sum += i * histogram[i];

    float sumB = 0;
    int wB = 0;
    int wF = 0;
    float varMax = 0;
    int threshold = 128;

    for (int t = 0; t < 256; ++t) {
        wB += histogram[t];
        if (wB == 0) continue;
        wF = totalPixels - wB;
        if (wF == 0) break;

        sumB += static_cast<float>(t * histogram[t]);
        float mB = sumB / wB;
        float mF = (sum - sumB) / wF;

        float varBetween = static_cast<float>(wB) * static_cast<float>(wF) * (mB - mF) * (mB - mF);
        if (varBetween > varMax) {
            varMax = varBetween;
            threshold = t;
        }
    }

    QImage binary(grayImg.size(), QImage::Format_Mono);
    binary.setColor(0, qRgb(0, 0, 0));     // index 0 = black (bar)
    binary.setColor(1, qRgb(255, 255, 255)); // index 1 = white (space)

    for (int y = 0; y < grayImg.height(); ++y) {
        const uchar* srcLine = grayImg.constScanLine(y);
        for (int x = 0; x < grayImg.width(); ++x) {
            // 1 for white (background), 0 for black (bar)
            binary.setPixel(x, y, (srcLine[x] > threshold) ? 1 : 0);
        }
    }
    return binary;
}

QVector<int> ImageProcessor::extractRunLengths(const QImage& binaryImg, int scanlineY) {
    QVector<int> rawRuns;
    if (scanlineY < 0 || scanlineY >= binaryImg.height()) return rawRuns;

    int width = binaryImg.width();
    if (width == 0) return rawRuns;

    int currentPixel = binaryImg.pixelIndex(0, scanlineY);
    int currentRunLength = 1;

    for (int x = 1; x < width; ++x) {
        int pixel = binaryImg.pixelIndex(x, scanlineY);
        if (pixel == currentPixel) {
            currentRunLength++;
        }
        else {
            rawRuns.append(currentRunLength);
            currentPixel = pixel;
            currentRunLength = 1;
        }
    }
    rawRuns.append(currentRunLength);

    // Trim leading quiet zone: if the first run is white (pixel index 1), remove it
    if (!rawRuns.isEmpty() && binaryImg.pixelIndex(0, scanlineY) == 1) {
        rawRuns.removeFirst();
    }

    // Trim trailing quiet zone: if the last run is white, remove it
    // After removing the leading white, the runs should start with black.
    // A valid barcode ends with the Stop pattern's final black bar, so
    // any trailing white run is a quiet zone.
    if (rawRuns.size() >= 2) {
        // Determine if the last run is white: after trimming leading white,
        // runs alternate starting with black (index 0). Even-indexed runs are
        // black, odd-indexed are white. If total count is even, the last run
        // is white (odd index) — that's a trailing quiet zone.
        if (rawRuns.size() % 2 == 0) {
            rawRuns.removeLast();
        }
    }

    return rawRuns;
}

QVector<int> ImageProcessor::normalizeRuns(const QVector<int>& rawRuns) {
    QVector<int> normalized;
    if (rawRuns.size() < 6) return normalized;

    // Code 128 symbol patterns consist of 6 runs per symbol (Start, Data, Checksum).
    // The first 6 runs correspond to the Start C character, summing to exactly 11 module units.
    int startSymbolPixelWidth = 0;
    for (int i = 0; i < 6 && i < rawRuns.size(); ++i) {
        startSymbolPixelWidth += rawRuns[i];
    }

    double moduleWidth = static_cast<double>(startSymbolPixelWidth) / 11.0;
    if (moduleWidth <= 0.0) return normalized;

    for (int run : rawRuns) {
        int modules = static_cast<int>(std::round(static_cast<double>(run) / moduleWidth));
        normalized.append(std::max(1, modules));
    }

    return normalized;
}