#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QFileDialog>
#include <QTextStream>
#include <QHeaderView>
#include <QtGui>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QTextCursor>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include "assemble.h"

using namespace std;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    QString stoq(const string s);
    string qtos(const QString s);
    void do_file_saveornot();
    void do_file_save();
    void saveasmfile();
    void saveasasmfile();
    void saveasbinfile();
    void newfile();
    void openasmfile();
    void openbinfile();
    void opencoefile();
    void binread(vector<string> &bin);
    void loadbinfile(vector<string> &hex, unsigned int pc);
    void loadasm(vector<string> &scode);
    void update_reg();

private slots:
    void on_actionNew_triggered();

    void on_actionSave_triggered();

    void on_actionUndo_triggered();

    void on_actionCut_triggered();

    void on_actionCopy_triggered();

    void on_actionPaste_triggered();

    void on_actionDelete_triggered();

    void on_actionFind_triggered();

    void on_actionReplace_triggered();

    bool on_actionAsm_triggered();

    void on_openasm_triggered();

    void on_openBin_triggered();

    void on_opencoe_triggered();

    void on_save_as_asm_triggered();

    void on_save_as_bin_triggered();

    void on_save_as_coe_triggered();

    void on_Run_triggered();

    void on_actionStep_triggered();

    void on_actionStop_step_triggered();

    void on_actionDisAsm_triggered();

private:
    Ui::MainWindow *ui;
    assemble asse;
    bool Is_save=false;
    QString filename="undefined", curfile;
    unsigned int pc=0, pc_last=0;
    QStandardItemModel  *bin_model;
};

#endif // MAINWINDOW_H
