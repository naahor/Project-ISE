#include "Encoder.h"
#include "BarcodeImageGenerator.h"
#include <iostream>
#include <stdexcept>
using namespace std;

int totalTests = 0;
int failedTests = 0;

void report(string name, bool passed)
{
    totalTests++;
    if (!passed)
    {
        failedTests++;
    }

    if (passed)
    {
        cout << "[PASS] " << name << endl;
    }
    else
    {
        cout << "[FAIL] " << name << endl;
    }
}

bool isBlackColumn(const QImage& image, int x)
{
    for (int y = 0; y < image.height(); y++)
    {
        if (image.pixel(x, y) != qRgb(0, 0, 0))
        {
            return false;
        }
    }
    return true;
}

bool isWhiteColumn(const QImage& image, int x)
{
    for (int y = 0; y < image.height(); y++)
    {
        if (image.pixel(x, y) != qRgb(255, 255, 255))
        {
            return false;
        }
    }
    return true;
}

bool throwsOnBits(string bits)
{
    BarcodeImageGenerator generator(2, 50);
    try
    {
        generator.generate(bits);
    }
    catch (invalid_argument&)
    {
        return true;
    }
    return false;
}

int main()
{
    int barWidth = 2;
    int barHeight = 50;
    int quietZone = BarcodeImageGenerator::getQuietZone();

    BarcodeImageGenerator generator(barWidth, barHeight);
    string bits = Encoder::encode("123456");
    QImage image = generator.generate(bits);

    report("image width is (68 + 2 quiet zones) * bar width", image.width() == (68 + 2 * quietZone) * barWidth);
    report("image height equals bar height", image.height() == barHeight);

    bool quietZonesWhite = true;
    for (int x = 0; x < quietZone * barWidth; x++)
    {
        if (!isWhiteColumn(image, x) || !isWhiteColumn(image, image.width() - 1 - x))
        {
            quietZonesWhite = false;
        }
    }
    report("quiet zones on both sides are white", quietZonesWhite);

    bool barsCorrect = true;
    for (int i = 0; i < (int)bits.length(); i++)
    {
        for (int k = 0; k < barWidth; k++)
        {
            int x = (quietZone + i) * barWidth + k;
            if (bits[i] == '1' && !isBlackColumn(image, x))
            {
                barsCorrect = false;
            }
            if (bits[i] == '0' && !isWhiteColumn(image, x))
            {
                barsCorrect = false;
            }
        }
    }
    report("every 1 is a black bar and every 0 is a white space", barsCorrect);

    report("first bar after quiet zone is black", isBlackColumn(image, quietZone * barWidth));
    report("saves a PNG file", generator.saveAsPng(bits, "test_barcode.png"));

    QImage loaded("test_barcode.png");
    report("saved PNG can be loaded with the same size", loaded.width() == image.width() && loaded.height() == image.height());

    report("rejects empty binary string", throwsOnBits(""));
    report("rejects binary string with other characters", throwsOnBits("1012"));

    bool badSizeRejected = false;
    try
    {
        BarcodeImageGenerator bad(0, 100);
    }
    catch (invalid_argument&)
    {
        badSizeRejected = true;
    }
    report("rejects bar width of 0", badSizeRejected);

    cout << endl << "Total: " << totalTests << "  Failed: " << failedTests << endl;

    if (failedTests == 0)
    {
        return 0;
    }
    return 1;
}
