#ifndef FAILWINDOW_H
#define FAILWINDOW_H

#include <QDialog>

namespace Ui {
class FailWindow;
}

class FailWindow : public QDialog
{
    Q_OBJECT

public:
    explicit FailWindow(QWidget *parent = nullptr);
    ~FailWindow();

private slots:
    void on_okButton_clicked();

private:
    Ui::FailWindow *ui;
};

#endif // FAILWINDOW_H
