#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "BarcodeImage.h"
#include "ImageProcessor.h"
#include "BarcodeDecoder.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Connect signals
    connect(ui->btnGenerate, &QPushButton::clicked, this, &MainWindow::onGenerateClicked);
    connect(ui->btnSave,     &QPushButton::clicked, this, &MainWindow::onSaveClicked);
    connect(ui->btnUpload,   &QPushButton::clicked, this, &MainWindow::onUploadClicked);
    connect(ui->btnDecode,   &QPushButton::clicked, this, &MainWindow::onDecodeClicked);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onGenerateClicked()
{
    QString input = ui->lineEditInput->text().trimmed();

    BarcodeImage generator;
    generator.setBarWidth(3);
    generator.setBarHeight(180);
    generator.setQuietZoneModules(10);

    QImage barcode;
    QString error;

    if (!generator.generate(input, barcode, error)) {
        showDecodedResult("Generation Error: " + error, false);
        return;
    }

    m_currentBarcode = barcode;
    displayBarcode(barcode);
    ui->btnSave->setEnabled(true);
    ui->btnDecode->setEnabled(true);

    showDecodedResult(QString("Barcode generated for: %1").arg(input), true);

    // Immediately verify by decoding the generated image
    ImageProcessor processor;
    QVector<int> normalizedRuns;
    if (processor.processBarcodeImage(barcode, normalizedRuns)) {
        BarcodeDecoder decoder;
        QString decoded, decError;
        if (decoder.decode(normalizedRuns, decoded, decError)) {
            showDecodedResult(QString("Generated & Verified: %1 → Decoded: %2").arg(input, decoded), true);
        } else {
            showDecodedResult(QString("Generated for %1, but verification failed: %2").arg(input, decError), false);
        }
    }
}

void MainWindow::onSaveClicked()
{
    if (m_currentBarcode.isNull()) {
        QMessageBox::warning(this, "No Barcode", "Generate a barcode first.");
        return;
    }

    QString filePath = QFileDialog::getSaveFileName(
        this, "Save Barcode Image", "barcode.png", "PNG Images (*.png);;All Files (*)");

    if (filePath.isEmpty()) return;

    if (m_currentBarcode.save(filePath, "PNG")) {
        showDecodedResult("Barcode saved to: " + filePath, true);
    } else {
        showDecodedResult("Failed to save barcode image.", false);
    }
}

void MainWindow::onUploadClicked()
{
    QString filePath = QFileDialog::getOpenFileName(
        this, "Open Barcode Image", "", "Images (*.png *.jpg *.bmp);;All Files (*)");

    if (filePath.isEmpty()) return;

    QImage loaded(filePath);
    if (loaded.isNull()) {
        showDecodedResult("Failed to load image: " + filePath, false);
        return;
    }

    m_currentBarcode = loaded;
    displayBarcode(loaded);
    ui->btnDecode->setEnabled(true);
    ui->btnSave->setEnabled(true);

    // Auto-decode the uploaded image
    onDecodeClicked();
}

void MainWindow::onDecodeClicked()
{
    if (m_currentBarcode.isNull()) {
        showDecodedResult("No barcode image loaded.", false);
        return;
    }

    // Member 3: Image Processing
    ImageProcessor processor;
    QVector<int> normalizedRuns;

    if (!processor.processBarcodeImage(m_currentBarcode, normalizedRuns)) {
        showDecodedResult("Image processing failed: Could not extract barcode runs.", false);
        return;
    }

    // Member 4: Barcode Decoder
    BarcodeDecoder decoder;
    QString decodedValue, errorMsg;

    if (decoder.decode(normalizedRuns, decodedValue, errorMsg)) {
        showDecodedResult(QString("Decoded Value: %1").arg(decodedValue), true);
    } else {
        showDecodedResult(QString("Decoding failed: %1").arg(errorMsg), false);
    }
}

void MainWindow::displayBarcode(const QImage& barcodeImage)
{
    QPixmap pixmap = QPixmap::fromImage(barcodeImage);
    // Scale to fit the label while keeping aspect ratio
    ui->labelBarcodePreview->setPixmap(
        pixmap.scaled(ui->labelBarcodePreview->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
}

void MainWindow::showDecodedResult(const QString& result, bool success)
{
    ui->labelResult->setText(result);
    if (success) {
        ui->labelResult->setStyleSheet("QLabel { color: green; font-weight: bold; font-size: 14px; }");
    } else {
        ui->labelResult->setStyleSheet("QLabel { color: red; font-weight: bold; font-size: 14px; }");
    }
    ui->statusbar->showMessage(result, 5000);
}
