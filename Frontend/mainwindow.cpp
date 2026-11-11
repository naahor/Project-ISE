#include "../Frontend/mainwindow.h"
#include "../Encoder/Encoder.h"
#include "../Backend/imageGenerator/BarcodeImageGenerator.h"

#include <QTabWidget>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QRegularExpression>
#include <QRegularExpressionValidator>
#include <QPixmap>
#include <QFileDialog>
#include <QMessageBox>
#include <stdexcept>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    setWindowTitle("Barcode Generator & Reader");
    resize(700, 500);

    m_tabs = new QTabWidget(this);
    m_tabs->addTab(createGenerateTab(), "Generate");
    m_tabs->addTab(createReadTab(), "Read");
    setCentralWidget(m_tabs);
}

QWidget* MainWindow::createGenerateTab() {
    QWidget *page = new QWidget;
    QVBoxLayout *layout = new QVBoxLayout(page);

    QHBoxLayout *inputRow = new QHBoxLayout;
    inputRow->addWidget(new QLabel("6-digit number:"));

    m_Input = new QLineEdit;
    m_Input->setPlaceholderText("e.g. 123456 (first digit 1-9)");
    m_Input->setMaxLength(6);
    m_Input->setValidator(new QRegularExpressionValidator(
        QRegularExpression("[1-9][0-9]{0,5}"), m_Input));
    inputRow->addWidget(m_Input, 1);

    m_Generate = new QPushButton("Generate");
    inputRow->addWidget(m_Generate);
    layout->addLayout(inputRow);

    m_Preview = new QLabel("Barcode preview will appear here");
    m_Preview->setAlignment(Qt::AlignCenter);
    m_Preview->setMinimumSize(400, 200);
    m_Preview->setFrameShape(QFrame::Box);
    layout->addWidget(m_Preview, 1);

    m_saveBtn = new QPushButton("Save Image");
    m_saveBtn->setEnabled(false);
    layout->addWidget(m_saveBtn);

    m_Status = new QLabel;
    layout->addWidget(m_Status);

    connect(m_Generate, &QPushButton::clicked, this, &MainWindow::onGenerateClicked);
    connect(m_Input, &QLineEdit::returnPressed, this, &MainWindow::onGenerateClicked);
    connect(m_saveBtn, &QPushButton::clicked, this, &MainWindow::onSaveClicked);

    return page;
}

QWidget* MainWindow::createReadTab() {
    QWidget *page = new QWidget;
    QVBoxLayout *layout = new QVBoxLayout(page);

    m_uploadBtn = new QPushButton("Upload Barcode Image");
    layout->addWidget(m_uploadBtn);

    m_readPreview = new QLabel("Uploaded barcode will appear here");
    m_readPreview->setAlignment(Qt::AlignCenter);
    m_readPreview->setMinimumSize(400, 200);
    m_readPreview->setFrameShape(QFrame::Box);
    layout->addWidget(m_readPreview, 1);

    m_decodedLabel = new QLabel("Decoded number: -");
    QFont f = m_decodedLabel->font();
    f.setPointSize(16);
    f.setBold(true);
    m_decodedLabel->setFont(f);
    layout->addWidget(m_decodedLabel);

    m_readStatus = new QLabel;
    layout->addWidget(m_readStatus);

    connect(m_uploadBtn, &QPushButton::clicked, this, &MainWindow::onUploadClicked);
    return page;
}

void MainWindow::onGenerateClicked() {
    const QString text = m_Input->text();

    try {
        std::string bits = Encoder::encode(text.toStdString());

        BarcodeImageGenerator gen;               // defaults: 3 px bars, 150 px high
        m_currentImage = gen.generate(bits);

        m_Preview->setPixmap(QPixmap::fromImage(m_currentImage));
        m_saveBtn->setEnabled(true);
        m_Status->setStyleSheet("color: green;");
        m_Status->setText("Barcode generated for " + text);
    } catch (const std::exception &e) {
        m_saveBtn->setEnabled(false);
        m_Status->setStyleSheet("color: red;");
        m_Status->setText(QString("Error: ") + e.what());
    }
}

void MainWindow::onSaveClicked() {
    if (m_currentImage.isNull()) {
        return;
    }

    const QString path = QFileDialog::getSaveFileName(
        this, "Save barcode image",
        "barcode_" + m_Input->text() + ".png",
        "PNG image (*.png)");

    if (path.isEmpty()) {
        return;   // user cancelled
    }

    if (m_currentImage.save(path, "PNG")) {
        m_Status->setStyleSheet("color: green;");
        m_Status->setText("Saved to " + path);
    } else {
        m_Status->setStyleSheet("color: red;");
        m_Status->setText("Error: could not save the image.");
    }
}

void MainWindow::onUploadClicked() {
    const QString path = QFileDialog::getOpenFileName(
        this, "Open barcode image", QString(),
        "Images (*.png *.jpg *.jpeg *.bmp)");

    if (path.isEmpty()) {
        return;
    }

    QImage img(path);
    if (img.isNull()) {
        m_readStatus->setStyleSheet("color: red;");
        m_readStatus->setText("Error: could not open that image.");
        return;
    }

    m_readPreview->setPixmap(QPixmap::fromImage(img));

    // TODO: call the decoder here when it is ready, e.g.
    // std::string number = Decoder::decode(path.toStdString());
    // m_decodedLabel->setText("Decoded number: " + QString::fromStdString(number));
    m_decodedLabel->setText("Decoded number: -");
    m_readStatus->setStyleSheet("color: gray;");
    m_readStatus->setText("Image loaded (decoder not connected yet).");
}