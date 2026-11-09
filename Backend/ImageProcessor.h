#pragma once
#ifndef IMAGEPROCESSOR_H
#define IMAGEPROCESSOR_H

#include <QImage>
#include <QVector>
#include <QString>

class ImageProcessor {
public:
    ImageProcessor();

    // Loads image, thresholds, scans mid-line, and returns normalized module widths
    bool processBarcodeImage(const QString& imagePath, QVector<int>& outNormalizedRuns);
    bool processBarcodeImage(const QImage& image, QVector<int>& outNormalizedRuns);

private:
    QImage convertToGrayscale(const QImage& src);
    QImage applyOtsuThreshold(const QImage& grayImg);
    QVector<int> extractRunLengths(const QImage& binaryImg, int scanlineY);
    QVector<int> normalizeRuns(const QVector<int>& rawRuns);
};

#endif // IMAGEPROCESSOR_H