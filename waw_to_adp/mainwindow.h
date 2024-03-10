#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStringListModel>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_btn_load_clicked();
    void on_btn_output_clicked();
    void on_btn_converter_clicked();
    void on_progressBar_valueChanged(int value);

private:
    Ui::MainWindow *ui;
    QStringListModel *model;  // Modelo para armazenar os nomes dos arquivos e da pasta
};

#endif // MAINWINDOW_H
