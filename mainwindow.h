#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTextEdit>
#include "iostream"
#include "vector"
#include "QPushButton"

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
    void consoletest();
    void onButtonClicked();
    void oneLine(QTextEdit *editor);
    void simulateRunButtonClick();
    void clearALL(QPushButton* clearButton);
    void settings();
    void save();
    void open();
private:
    Ui::MainWindow *ui;
    vector<QTextEdit*> editors;
    vector<QFrame*> frames;
    QTimer* timer;
    int counter = 0;
};
#endif // MAINWINDOW_H
