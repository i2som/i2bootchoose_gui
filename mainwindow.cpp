#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFile>
#include <qdebug.h>
#include <QRadioButton>

config_data::config_data(const QString &file_name)
{
    QFile file(file_name);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;
    name = file_name;

    QTextStream in(&file);
    bool read_label_flag = false;
    config_label *new_label;
    while (!in.atEnd()) {
        QString line = in.readLine();
        line = line.trimmed();
        if (line.startsWith("LABEL")) {
            read_label_flag = true;
        }

        if (line.startsWith("LABEL") || read_label_flag) {

            if (line.startsWith("LABEL")) {
                new_label = new config_label();
                new_label->text_label = line;
            } else if (line.startsWith("KERNEL")) {
                if (new_label)
                    new_label->text_kernel = line;
            } else if (line.startsWith("FDT")) {
                if (new_label)
                    new_label->text_fdt = line;
            } else if (line.startsWith("APPEND")) {
                if (new_label)
                    new_label->text_append = line;
                list_label.append(new_label);
                new_label = NULL;
            }
        } else if (line.startsWith("DEFAULT") || read_label_flag) {
            text_default_label = line;
        }
        else {
            text_head += line + "\n";
        }
    }
    file.close();
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    config = new config_data("/boot/mmc1_panguboard_extlinux/extlinux.conf");

    QString default_label = config->text_default_label.section(' ', 1);
    for (int i=0; i!=config->list_label.size(); ++i )
    {
        config_label *label_new = config->list_label.at(i);

        QRadioButton *w = new QRadioButton(this);
        w->setText(label_new->text_label.section(' ', 1));
        w->adjustSize();
        if (default_label == label_new->text_label.section(' ', 1))
            w->setChecked(true);
        ui->verticalLayout->addWidget(w);
        ui->verticalLayout->setAlignment(Qt::AlignCenter);

        list_rb.append(w);
    }
    setStyleSheet("QRadioButton:hover, QCheckBox:hover {background-color: wheat;}");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);

    qDebug() << endl << "Window Resize:" << this->size();

    // 屏幕长宽
    QList<QScreen*> scr = QGuiApplication::screens();
    QRect rect = scr.at(0)->geometry();
    quint32 desktop_wid = rect.width();
    quint32 desktop_hei = rect.height();

    // 根据不同屏幕分辨率, 调节字体大小
    QString style_str;
    QString label_str;
    QString radio_str;
    if (desktop_wid >= 1280 && desktop_hei >= 720) {
        qDebug() << "1280*720";
        style_str = QString("font: 20pt \"WenQuanYi Micro Hei Mono\";");
        label_str = QString("font: 60pt \"WenQuanYi Micro Hei Mono\";");
        radio_str = QString("QRadioButton { font: 30pt \"WenQuanYi Micro Hei Mono\"; }");
    } else if (desktop_wid >= 800 && desktop_hei >= 600) {
        qDebug() << "800*600";
        style_str = QString("font: 10pt \"WenQuanYi Micro Hei Mono\";");
        label_str = QString("font: 20pt \"WenQuanYi Micro Hei Mono\";");
        radio_str = QString("QRadioButton { font: 10pt \"WenQuanYi Micro Hei Mono\"; }");
    }

    this->setStyleSheet(radio_str);
    ui->label->setStyleSheet(label_str);
    ui->pushButton->setStyleSheet(style_str);
    ui->pushButton_2->setStyleSheet(style_str);
}

void MainWindow::on_pushButton_2_clicked()
{
    close();
}

void MainWindow::on_pushButton_clicked()
{
    QFile file(config->name);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return;

    QTextStream in(&file);
    QString default_label;

    for (int i = 0; i != list_rb.size(); ++i )
    {
        QRadioButton *rb = list_rb.at(i);
        if (rb->isChecked()) {
            default_label = rb->text();
            break;
        }
    }

    in << config->text_head;
    if (default_label.isEmpty())
        in << config->text_default_label << endl;
    else
        in << "DEFAULT " << default_label << endl;

    for (int i=0; i!=config->list_label.size(); ++i )
    {
        config_label *label_new = config->list_label.at(i);
        in << label_new->text_label << endl;
        in << "\t" << label_new->text_kernel << endl;
        in << "\t" << label_new->text_fdt << endl;
        in << "\t" << label_new->text_append << endl;
    }
    file.close();
}
