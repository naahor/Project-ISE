#include "BarcodeImage.h"
#include <QPainter>
#include <QDebug>

const QVector<int> BarcodeImage::STOP_PATTERN = { 2, 3, 3, 1, 1, 1, 2 };

BarcodeImage::BarcodeImage()
    : m_barWidth(2)
    , m_barHeight(150)
    , m_quietZoneModules(10)
{
    initializePatterns();
}

void BarcodeImage::setBarWidth(int pixelsPerModule)  { m_barWidth = pixelsPerModule; }
void BarcodeImage::setBarHeight(int pixels)          { m_barHeight = pixels; }
void BarcodeImage::setQuietZoneModules(int modules)  { m_quietZoneModules = modules; }

void BarcodeImage::initializePatterns() {
    // Full Code 128 symbol table (values 0-105)
    // Each entry: { symbolValue, {B1, S1, B2, S2, B3, S3} } — 6 widths summing to 11
    m_symbolTable = {
        {0,  {2, 1, 2, 2, 2, 2}}, {1,  {2, 2, 2, 1, 2, 2}}, {2,  {2, 2, 2, 2, 2, 1}},
        {3,  {1, 2, 1, 2, 2, 3}}, {4,  {1, 2, 1, 3, 2, 2}}, {5,  {1, 3, 1, 2, 2, 2}},
        {6,  {1, 2, 2, 2, 1, 3}}, {7,  {1, 2, 2, 3, 1, 2}}, {8,  {1, 3, 2, 2, 1, 2}},
        {9,  {2, 2, 1, 2, 1, 3}}, {10, {2, 2, 1, 3, 1, 2}}, {11, {2, 3, 1, 2, 1, 2}},
        {12, {1, 1, 2, 2, 3, 2}}, {13, {1, 2, 2, 1, 3, 2}}, {14, {1, 2, 2, 2, 3, 1}},
        {15, {1, 1, 3, 2, 2, 2}}, {16, {1, 2, 3, 1, 2, 2}}, {17, {1, 2, 3, 2, 2, 1}},
        {18, {2, 2, 3, 2, 1, 1}}, {19, {2, 2, 1, 1, 3, 2}}, {20, {2, 2, 1, 2, 3, 1}},
        {21, {2, 1, 3, 2, 1, 2}}, {22, {2, 2, 3, 1, 1, 2}}, {23, {3, 1, 2, 1, 3, 1}},
        {24, {3, 1, 1, 2, 2, 2}}, {25, {3, 2, 1, 1, 2, 2}}, {26, {3, 2, 1, 2, 2, 1}},
        {27, {3, 1, 2, 2, 1, 2}}, {28, {3, 2, 2, 1, 1, 2}}, {29, {3, 2, 2, 2, 1, 1}},
        {30, {2, 1, 2, 1, 2, 3}}, {31, {2, 1, 2, 3, 2, 1}}, {32, {2, 3, 2, 1, 2, 1}},
        {33, {1, 1, 1, 3, 2, 3}}, {34, {1, 3, 1, 1, 2, 3}}, {35, {1, 3, 1, 3, 2, 1}},
        {36, {1, 1, 2, 3, 1, 3}}, {37, {1, 3, 2, 1, 1, 3}}, {38, {1, 3, 2, 3, 1, 1}},
        {39, {2, 1, 1, 3, 1, 3}}, {40, {2, 3, 1, 1, 1, 3}}, {41, {2, 3, 1, 3, 1, 1}},
        {42, {1, 1, 2, 1, 3, 3}}, {43, {1, 1, 2, 3, 3, 1}}, {44, {1, 3, 2, 1, 3, 1}},
        {45, {1, 1, 3, 1, 2, 3}}, {46, {1, 1, 3, 3, 2, 1}}, {47, {1, 3, 3, 1, 2, 1}},
        {48, {3, 1, 3, 1, 2, 1}}, {49, {2, 1, 1, 3, 3, 1}}, {50, {2, 3, 1, 1, 3, 1}},
        {51, {2, 1, 3, 1, 1, 3}}, {52, {2, 1, 3, 3, 1, 1}}, {53, {2, 1, 3, 1, 3, 1}},
        {54, {3, 1, 1, 1, 2, 3}}, {55, {3, 1, 1, 3, 2, 1}}, {56, {3, 3, 1, 1, 2, 1}},
        {57, {3, 1, 2, 1, 1, 3}}, {58, {3, 1, 2, 3, 1, 1}}, {59, {3, 3, 2, 1, 1, 1}},
        {60, {3, 1, 4, 1, 1, 1}}, {61, {2, 2, 1, 4, 1, 1}}, {62, {4, 3, 1, 1, 1, 1}},
        {63, {1, 1, 1, 2, 2, 4}}, {64, {1, 1, 1, 4, 2, 2}}, {65, {1, 2, 1, 1, 2, 4}},
        {66, {1, 2, 1, 4, 2, 1}}, {67, {1, 4, 1, 1, 2, 2}}, {68, {1, 4, 1, 2, 2, 1}},
        {69, {1, 1, 2, 2, 1, 4}}, {70, {1, 1, 2, 4, 1, 2}}, {71, {1, 2, 2, 1, 1, 4}},
        {72, {1, 2, 2, 4, 1, 1}}, {73, {1, 4, 2, 1, 1, 2}}, {74, {1, 4, 2, 2, 1, 1}},
        {75, {2, 4, 1, 2, 1, 1}}, {76, {2, 2, 1, 1, 1, 4}}, {77, {4, 1, 3, 1, 1, 1}},
        {78, {2, 4, 1, 1, 1, 2}}, {79, {1, 3, 4, 1, 1, 1}}, {80, {1, 1, 1, 2, 4, 2}},
        {81, {1, 2, 1, 1, 4, 2}}, {82, {1, 2, 1, 2, 4, 1}}, {83, {1, 1, 4, 2, 1, 2}},
        {84, {1, 2, 4, 1, 1, 2}}, {85, {1, 2, 4, 2, 1, 1}}, {86, {4, 1, 1, 2, 1, 2}},
        {87, {4, 2, 1, 1, 1, 2}}, {88, {4, 2, 1, 2, 1, 1}}, {89, {2, 1, 2, 1, 4, 1}},
        {90, {2, 1, 4, 1, 2, 1}}, {91, {4, 1, 2, 1, 2, 1}}, {92, {1, 1, 1, 1, 4, 3}},
        {93, {1, 1, 1, 3, 4, 1}}, {94, {1, 3, 1, 1, 4, 1}}, {95, {1, 1, 4, 1, 1, 3}},
        {96, {1, 1, 4, 3, 1, 1}}, {97, {4, 1, 1, 1, 1, 3}}, {98, {4, 1, 1, 3, 1, 1}},
        {99, {1, 1, 3, 1, 4, 1}},
        {105, {2, 1, 1, 4, 1, 2}} // Start C
    };
}

QVector<int> BarcodeImage::getPatternForId(int symbolId) const {
    for (const auto& sym : m_symbolTable) {
        if (sym.id == symbolId)
            return sym.widthPattern;
    }
    return {};
}

int BarcodeImage::computeChecksum(int startId, const QVector<int>& dataIds) const {
    int sum = startId;  // Start C value = 105
    for (int i = 0; i < dataIds.size(); ++i) {
        sum += (i + 1) * dataIds[i];
    }
    return sum % 103;
}

QString BarcodeImage::buildBinaryString(const QVector<QVector<int>>& allPatterns) const {
    QString binary;
    for (int patIdx = 0; patIdx < allPatterns.size(); ++patIdx) {
        const QVector<int>& pattern = allPatterns[patIdx];
        for (int i = 0; i < pattern.size(); ++i) {
            // Even indices (0, 2, 4, ...) are bars (black = '1')
            // Odd indices (1, 3, 5, ...) are spaces (white = '0')
            QChar c = (i % 2 == 0) ? '1' : '0';
            for (int j = 0; j < pattern[i]; ++j) {
                binary.append(c);
            }
        }
    }
    return binary;
}

QImage BarcodeImage::renderToImage(const QString& binaryStr) const {
    int quietZonePixels = m_quietZoneModules * m_barWidth;
    int barcodePixelWidth = binaryStr.length() * m_barWidth;
    int totalWidth = barcodePixelWidth + 2 * quietZonePixels;

    QImage image(totalWidth, m_barHeight, QImage::Format_RGB32);
    image.fill(Qt::white);  // white background including quiet zones

    QPainter painter(&image);
    painter.setPen(Qt::NoPen);

    int x = quietZonePixels;
    for (int i = 0; i < binaryStr.length(); ++i) {
        if (binaryStr[i] == '1') {
            painter.fillRect(x, 0, m_barWidth, m_barHeight, Qt::black);
        }
        x += m_barWidth;
    }

    painter.end();
    return image;
}

bool BarcodeImage::generate(const QString& sixDigitInput, QImage& outImage, QString& outError) {
    m_lastBinaryString.clear();

    // Validate input
    if (sixDigitInput.length() != 6) {
        outError = QString("Input must be exactly 6 digits. Got %1 characters.").arg(sixDigitInput.length());
        return false;
    }
    for (int i = 0; i < sixDigitInput.length(); ++i) {
        if (!sixDigitInput[i].isDigit()) {
            outError = QString("Input must contain only digits. Invalid character '%1' at position %2.")
                           .arg(sixDigitInput[i]).arg(i + 1);
            return false;
        }
    }

    // Split into 3 pairs for Code C encoding
    QVector<int> dataIds;
    for (int i = 0; i < 6; i += 2) {
        int pairValue = sixDigitInput.mid(i, 2).toInt();
        dataIds.append(pairValue);
    }

    // Compute checksum
    int startId = 105; // Start C
    int checksumId = computeChecksum(startId, dataIds);

    qDebug() << "BarcodeImage: Input =" << sixDigitInput;
    qDebug() << "BarcodeImage: Data pairs =" << dataIds;
    qDebug() << "BarcodeImage: Checksum =" << checksumId;

    // Build the list of all patterns: Start C, data symbols, checksum, stop
    QVector<QVector<int>> allPatterns;

    // Start C pattern
    QVector<int> startPattern = getPatternForId(startId);
    if (startPattern.isEmpty()) {
        outError = "Internal error: Start C pattern not found.";
        return false;
    }
    allPatterns.append(startPattern);

    // Data symbol patterns
    for (int id : dataIds) {
        QVector<int> pat = getPatternForId(id);
        if (pat.isEmpty()) {
            outError = QString("Internal error: Pattern for value %1 not found.").arg(id);
            return false;
        }
        allPatterns.append(pat);
    }

    // Checksum pattern
    QVector<int> checksumPattern = getPatternForId(checksumId);
    if (checksumPattern.isEmpty()) {
        outError = QString("Internal error: Checksum pattern for value %1 not found.").arg(checksumId);
        return false;
    }
    allPatterns.append(checksumPattern);

    // Stop pattern (7 elements)
    allPatterns.append(STOP_PATTERN);

    // Build binary string
    m_lastBinaryString = buildBinaryString(allPatterns);
    qDebug() << "BarcodeImage: Binary string length =" << m_lastBinaryString.length();
    qDebug() << "BarcodeImage: Binary string =" << m_lastBinaryString;

    // Render to image
    outImage = renderToImage(m_lastBinaryString);
    return true;
}

QString BarcodeImage::getBinaryString() const {
    return m_lastBinaryString;
}
