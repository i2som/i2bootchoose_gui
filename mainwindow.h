#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QRadioButton>

namespace Ui {
class MainWindow;
}

class config_label
{
public:
    config_label() {}
    QString text_label;
    QString text_kernel;
    QString text_fdt;
    QString text_append;
};

class config_data
{
public:
    config_data(const QString &file_name);
    QString name;
    QString text_head;
    QString text_default_label;
    QList<config_label *> list_label;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    config_data *config;
    QList<QRadioButton *> list_rb;

private slots:
    void on_pushButton_2_clicked();

    void on_pushButton_clicked();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
