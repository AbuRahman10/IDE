#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTextEdit>
#include "iostream"
#include "vector"

using namespace std;

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
    void inputtest();
    void onButtonClicked();
    void textchanged(QTextEdit *editor);
    void oneLine(QTextEdit *editor);
private:
    Ui::MainWindow *ui;
    vector<QTextEdit*> editors;
};
#endif // MAINWINDOW_H
