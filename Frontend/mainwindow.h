#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QImage>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private slots:
    void onGenerateClicked();
    void onSaveClicked();
    void onUploadClicked();
    void onDecodeClicked();

private:
    void displayBarcode(const QImage& barcodeImage);
    void showDecodedResult(const QString& result, bool success);

    Ui::MainWindow *ui;
    QImage m_currentBarcode;  // Currently displayed barcode image
};
#endif // MAINWINDOW_H
