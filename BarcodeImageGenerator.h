#ifndef BARCODEIMAGEGENERATOR_H
#define BARCODEIMAGEGENERATOR_H

#include <string>
#include <QImage>
#include <QString>
using namespace std;

/// <summary>
/// Turns the binary string from Encoder into a barcode image.
/// 1 becomes a black bar, 0 becomes a white space, with quiet zones on both sides.
/// </summary>
class BarcodeImageGenerator
{
private:
    static int quietZone;

    int barWidth;
    int barHeight;

    static void validateBits(const string& bits);

public:
    BarcodeImageGenerator(int barWidth = 3, int barHeight = 150);

    int getBarWidth() const;
    int getBarHeight() const;
    static int getQuietZone();

    QImage generate(const string& bits) const;
    bool saveAsPng(const string& bits, const QString& filePath) const;
};

#endif
