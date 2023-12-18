#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTextEdit>
#include "iostream"
#include "vector"
#include "QPushButton"
#include "../src/CFG.h"
#include "../src/SLR.h"

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
    SLR parser;
    CFG cfg;
    int counter = 0;
    QVector<QString> datatypes = {"string","bool", "int", "char", "vector", "set","//","for","if","else","while"};
    QVector<QString> symbols = {"<",">","{","}","(",")"};
    vector<string> alreadyAccepted = {"","","","","","","","","",""};
};
#endif // MAINWINDOW_H
