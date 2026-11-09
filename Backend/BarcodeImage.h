#ifndef BARCODEIMAGE_H
#define BARCODEIMAGE_H

#include <QString>
#include <QImage>
#include <QVector>

class BarcodeImage {
public:
    BarcodeImage();

    // Encode a 6-digit numeric string into a Code 128 Code C barcode image.
    // Returns true on success, false if input is invalid.
    bool generate(const QString& sixDigitInput, QImage& outImage, QString& outError);

    // Get the binary pattern string (for debugging/testing)
    QString getBinaryString() const;

    // Configuration
    void setBarWidth(int pixelsPerModule);
    void setBarHeight(int pixels);
    void setQuietZoneModules(int modules);

private:
    struct SymbolPattern {
        int id;
        QVector<int> widthPattern;  // 6 elements for data/start, 7 for stop
    };

    void initializePatterns();
    QVector<int> getPatternForId(int symbolId) const;
    int computeChecksum(int startId, const QVector<int>& dataIds) const;
    QString buildBinaryString(const QVector<QVector<int>>& allPatterns) const;
    QImage renderToImage(const QString& binaryStr) const;

    QList<SymbolPattern> m_symbolTable;
    static const QVector<int> STOP_PATTERN;

    int m_barWidth;       // pixels per module
    int m_barHeight;      // barcode height in pixels
    int m_quietZoneModules; // number of quiet zone modules on each side
    QString m_lastBinaryString;
};

#endif // BARCODEIMAGE_H
