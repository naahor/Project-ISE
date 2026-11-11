#include "BarcodeImageGenerator.h"
#include <stdexcept>
using namespace std;

int BarcodeImageGenerator::quietZone = 10;

/// <summary>
/// Sets the width of one module (one 0 or 1) and the height of the barcode, in pixels.
/// </summary>
BarcodeImageGenerator::BarcodeImageGenerator(int barWidth, int barHeight)
{
    if (barWidth <= 0 || barHeight <= 0)
    {
        throw invalid_argument("Bar width and bar height must be greater than 0.");
    }

    this->barWidth = barWidth;
    this->barHeight = barHeight;
}

int BarcodeImageGenerator::getBarWidth() const
{
    return barWidth;
}

int BarcodeImageGenerator::getBarHeight() const
{
    return barHeight;
}

int BarcodeImageGenerator::getQuietZone()
{
    return quietZone;
}

/// <summary>
/// Throws invalid_argument if the string is empty or contains anything except 0 and 1.
/// </summary>
void BarcodeImageGenerator::validateBits(const string& bits)
{
    if (bits.length() == 0)
    {
        throw invalid_argument("Binary string is empty.");
    }

    for (int i = 0; i < (int)bits.length(); i++)
    {
        if (bits[i] != '0' && bits[i] != '1')
        {
            throw invalid_argument("Binary string must contain only 0 and 1.");
        }
    }
}

/// <summary>
/// Builds the image: white background, quiet zone, then one black bar for every 1.
/// </summary>
QImage BarcodeImageGenerator::generate(const string& bits) const
{
    validateBits(bits);

    int totalModules = (int)bits.length() + 2 * quietZone;
    int imageWidth = totalModules * barWidth;

    QImage image(imageWidth, barHeight, QImage::Format_RGB32);
    image.fill(Qt::white);

    QRgb black = qRgb(0, 0, 0);

    for (int i = 0; i < (int)bits.length(); i++)
    {
        if (bits[i] == '1')
        {
            int startX = (quietZone + i) * barWidth;

            for (int x = startX; x < startX + barWidth; x++)
            {
                for (int y = 0; y < barHeight; y++)
                {
                    image.setPixel(x, y, black);
                }
            }
        }
    }

    return image;
}

/// <summary>
/// Generates the barcode and saves it as a PNG file. Returns false if saving fails.
/// </summary>
bool BarcodeImageGenerator::saveAsPng(const string& bits, const QString& filePath) const
{
    QImage image = generate(bits);
    return image.save(filePath, "PNG");
}
