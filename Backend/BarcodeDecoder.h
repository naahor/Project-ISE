#pragma once
#ifndef BARCODEDECODER_H
#define BARCODEDECODER_H

#include <QVector>
#include <QString>
#include <QMap>

class BarcodeDecoder {
public:
    BarcodeDecoder();

    // Receives normalized run widths, returns true if successfully decoded
    bool decode(const QVector<int>& normalizedRuns, QString& outDecodedValue, QString& outErrorMessage);

private:
    struct SymbolPattern {
        int id;
        QVector<int> widthPattern;
    };

    void initializePatterns();
    int matchSymbol(const QVector<int>& runGroup);

    QList<SymbolPattern> m_symbolTable;
    static const QVector<int> STOP_PATTERN;
};

#endif // BARCODEDECODER_H