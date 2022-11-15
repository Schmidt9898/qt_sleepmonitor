#ifndef SUCCESWINDOW_H
#define SUCCESWINDOW_H

#include <QDialog>

namespace Ui {
class SuccesWindow;
}

class SuccesWindow : public QDialog
{
    Q_OBJECT

public:
    explicit SuccesWindow(QWidget *parent = nullptr);
    ~SuccesWindow();

private slots:
    void on_okButton_clicked();

private:
    Ui::SuccesWindow *ui;
};

#endif // SUCCESWINDOW_H
