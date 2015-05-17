include "mainwindow.h"
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

using namespace std;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

//Qstring to string
string MainWindow::qtos(const QString s)
{
    return string((const char *)s.toLocal8Bit());
}
//string to QString
QString MainWindow::stoq(const string s)
{
    return QString(QString::fromLocal8Bit(s.c_str()));
}


//a list function that complement the save function
//save file
void MainWindow::on_actionSave_triggered()
{
    saveasmfile();
}
//do save file
void MainWindow::saveasmfile()
{
    QFile file(filename);

    //Open file with only write
    file.open(QFile::WriteOnly|QFile::Text);
    if(Is_save)
    {
        QTextStream out(&file);
        out << ui->source->toPlainText();
        Is_save = true;
        curfile = QFileInfo(filename).canonicalFilePath();
        setWindowTitle(curfile);
    }
    else
        saveasasmfile();
}

//save as asm file
void MainWindow::on_save_as_asm_triggered()
{
    saveasasmfile();
}
//do save as asm file
void MainWindow::saveasasmfile()
{
    filename = QFileDialog::getSaveFileName(this,tr("save as"),curfile);

    if(!filename.isEmpty())
    {
        Is_save = true;
        saveasmfile();
    }
}

//save as bin file
void MainWindow::on_save_as_bin_triggered()
{
    saveasbinfile();
}
//do save as bin file
void MainWindow::saveasbinfile()
{
    filename = QFileDialog::getSaveFileName(this,tr("save as"),curfile);

    if(!filename.isEmpty())
    {
        string s="";
        vector<string> bicode;
        bicode = asse.bicode_out();
        for(unsigned int i=0;i<bicode.size();i++)
            s += (bicode[i]+"\n");
        QFile file(filename);
        file.open(QFile::WriteOnly|QFile::Text);
        QTextStream out(&file);
        out << stoq(s);
        curfile = QFileInfo(filename).canonicalFilePath();
    }
}

//save as coe file
void MainWindow::on_save_as_coe_triggered()
{
    filename = QFileDialog::getSaveFileName(this,tr("save as"),curfile);

    if(!filename.isEmpty())
    {
        string s="";
        s += "memory_initialization_radix=16;\n";
        s += "memory_initialization_vector=\n";

        vector<string> bicode;
        bicode = asse.bicode_out();
        for(unsigned int i=0; i<bicode.size();i++)
            bicode[i] = asse.trans_d_hex(asse.trans_bin_dec(bicode[i]));
        for(unsigned int i=0;i<bicode.size();i++)
        {
            s += bicode[i];
            if(i==bicode.size()-1)
                s += ";";
            else
                s += ", ";
            if(i%10 == 9)
                s += "\n";
        }
        QFile file(filename);
        file.open(QFile::WriteOnly|QFile::Text);
        QTextStream out(&file);
        out << stoq(s);
        curfile = QFileInfo(filename).canonicalFilePath();
    }
}


//do new file
void MainWindow::newfile()
{
    do_file_saveornot();
    Is_save = false;
    curfile = tr("undefined.txt");
    setWindowTitle(curfile);
    ui->source->clear();//clear the content
    ui->source->setVisible(true);//
}
//file save or not
void MainWindow::do_file_saveornot()
{
    if(ui->source->document()->isModified())
    {
        QMessageBox box;
        box.setWindowTitle("Warning!");
        box.setIcon(QMessageBox::Warning);
        box.setText("The text has not save, save or not?");
        box.setStandardButtons(QMessageBox::Yes|QMessageBox::No);
        if(box.exec()==QMessageBox::Yes)
        {
            if(Is_save)
                saveasasmfile();
            else
                saveasasmfile();
        }
    }
}

//open asm file
void MainWindow::on_openasm_triggered()
{
    do_file_saveornot();
    openasmfile();
    Is_save = true;
}
//do open asm file
void MainWindow::openasmfile()
{
    filename = QFileDialog::getOpenFileName(this);
    QFile file(filename);

    if(!file.open(QFile::ReadOnly|QFile::Text))//open file with read only
        return ;    //open file failure
    QTextStream in(&file);//new file stream
    ui->source->setPlainText(in.readAll());
    curfile = QFileInfo(filename).canonicalFilePath();
    setWindowTitle(curfile);
    ui->source->setVisible(true);
}

//open bin file
void MainWindow::on_openBin_triggered()
{
    vector<string> bin;
    binread(bin);
    for(unsigned int i=0; i<bin.size();i++)
        bin[i] = asse.trans_d_hex(asse.trans_bin_dec(bin[i]));
    loadbinfile(bin, 0);
}
//read the source code from a file
void MainWindow::binread(vector<string> &bin)
{
    filename = QFileDialog::getOpenFileName(this);
    QFile file(filename);
    string line;

    if(!file.open(QFile::ReadOnly|QFile::Text))//open file with read only
        return ;
    QTextStream in(&file);//new file stream

    while(!in.atEnd())
    {
       line = qtos(in.readLine());
       bin.push_back(line);
    }
    asse.bicode_read(bin);
}
//load binary file
void MainWindow::loadbinfile(vector<string> &hex, unsigned int pc)
{
    ui->bintable->setShowGrid(true);
    ui->bintable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    QStandardItemModel  *model = new QStandardItemModel();
    bin_model = model;
    string baseaddr = asse.trans_d_hex(pc);

    ui->bintable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->bintable->setModel(model);  //assign the model to the tabelview
    ui->bintable->verticalHeader()->hide(); //hide the vertical header
    //ui->bintable->horizontalHeader()->setResizeMode(0,QHeaderView::Stretch);
    //ui->bintable->horizontalHeader()->set

    model->setColumnCount(5);
    model->setHeaderData(0, Qt::Horizontal, tr("Address"));
    model->setHeaderData(1, Qt::Horizontal, tr("00"));
    model->setHeaderData(2, Qt::Horizontal, tr("01"));
    model->setHeaderData(3, Qt::Horizontal, tr("02"));
    model->setHeaderData(4, Qt::Horizontal, tr("03"));

    for(unsigned int i=0;i<hex.size();i++)
    {
        //add content
        model->setItem(i, 0, new QStandardItem(stoq(baseaddr)));
        model->setItem(i, 1, new QStandardItem(stoq(hex[i].substr(0,2))));
        model->setItem(i, 2, new QStandardItem(stoq(hex[i].substr(2,2))));
        model->setItem(i, 3, new QStandardItem(stoq(hex[i].substr(4,2))));
        model->setItem(i, 4, new QStandardItem(stoq(hex[i].substr(6,2))));
        //set in center
        model->item(i,0)->setTextAlignment(Qt::AlignCenter);
        model->item(i,1)->setTextAlignment(Qt::AlignCenter);
        model->item(i,2)->setTextAlignment(Qt::AlignCenter);
        model->item(i,3)->setTextAlignment(Qt::AlignCenter);
        model->item(i,4)->setTextAlignment(Qt::AlignCenter);
        pc += 4;
        baseaddr = asse.trans_d_hex(pc);
    }
}

//open coe file
void MainWindow::on_opencoe_triggered()
{
    vector<string> bin;
    filename = QFileDialog::getOpenFileName(this);//open file with read only
    QFile file(filename);
    string line, code;

    if(!file.open(QFile::ReadOnly|QFile::Text))
        return ;
    QTextStream in(&file);
    code = qtos(in.readAll());
    code = asse.spa_eli(code);
    string::iterator it;
    for(it=code.end()-1;*it=='\n';it--)
        code.erase(it);
    it = code.begin();
    while(1)
    {
        if(code[0]==';')
        {
            while(*it!='\n')
                it = code.erase(it);
            it = code.erase(it);
        }
        else
            break;
    }

    while(*it!='\n')
        it = code.erase(it);
    it = code.erase(it);
    while(*it!='\n')
        it = code.erase(it);
    it = code.erase(it);
    if(code[code.size()-1]!=';')
    {
        asse.Error(0, "lack semicolon at the end!");
        return;
    }
    while(1)
    {
        while(*it!=','&&*it!=';')
        {
            if((*it)!='\n')
                line += *it;
            it++;
        }
        line = asse.spa_eli(line);
        line = asse.trans_hex_bin(line);
        bin.push_back(line);
        if(*it==';')
            break;
        else
            it++;
        line.erase(line.begin(), line.end());
    }
    asse.bicode_read(bin);
    for(unsigned int i=0; i<bin.size();i++)
        bin[i] = asse.trans_d_hex(asse.trans_bin_dec(bin[i]));
    loadbinfile(bin, 0);
}

//new file
void MainWindow::on_actionNew_triggered()
{
    newfile();
}

//undo
void MainWindow::on_actionUndo_triggered()
{
    ui->source->undo();
}

//cut
void MainWindow::on_actionCut_triggered()
{
    ui->source->cut();
}

//copy
void MainWindow::on_actionCopy_triggered()
{
    ui->source->copy();
}

//paste
void MainWindow::on_actionPaste_triggered()
{
    ui->source->paste();
}

//delete
void MainWindow::on_actionDelete_triggered()
{
    ui->source->deleteLater();
}

//find
void MainWindow::on_actionFind_triggered()
{
    /*QDialog *findDlg = new QDialog(this);
    findDlg->setWindowTitle("Find");
    find_textLineEdit = new QLineEdit(findDlg);
    QPushButton *find_Btn = new QPushButton(tr("find"),findDlg);//join a find button
    connect(find_Btn,SIGNAL(clicked()),this,SLOT(show_findText()));//connnect the button to the findText
    QVBoxLayout* layout = new QVBoxLayout(findDlg);
    layout->addWidget(find_textLineEdit);
    layout->addWidget(find_Btn);
    findDlg->show();*/
}
/*void MainWindow::show_findText()//“开始查找”摁钮槽函数
{
    QString findText = find_textLineEdit->text();//get the text
    ui->source->find(findText,QTextDocument::FindBackward);//find the text
    if(!ui->textEdit->find(findText,QTextDocument::FindWholeWords))
        QMessageBox::warning(this,tr("can't"),tr("find %1").arg(findText));
}*/

//replace
void MainWindow::on_actionReplace_triggered()
{

}

//asm the source code
bool MainWindow::on_actionAsm_triggered()
{
    vector<string> scode, bicode;
    loadasm(scode);
    unsigned int pc;

    asse.scode_read(scode);
    asse.translate();

    //error out
    vector<string> err;
    err = asse.err_out();
    if(err.size()!=0)   //some errors exit
    {
        string s="";
        s += "some error exit:\n";
        for(unsigned int i=0;i<err.size();i++)
            s+=(err[i]+"\n");
        ui->errtext->setPlainText(stoq(s));
        return false;
    }
    else                //dispaly the binary code
    {
        bicode = asse.bicode_out();
        pc = asse.baseaddr_out();
        for(unsigned int i=0; i<bicode.size();i++)
            bicode[i] = asse.trans_d_hex(asse.trans_bin_dec(bicode[i]));
        loadbinfile(bicode, pc);
        ui->errtext->setPlainText("Compile successfully!");
    }

    return true;
}

//load the source code into a vector
void MainWindow::loadasm(vector<string> &scode)
{
    string s, line;

    s = qtos(ui->source->toPlainText());
    stringstream stream(s);
    while(getline(stream, line))
        scode.push_back(line);
}

//run
void MainWindow::on_Run_triggered()
{
    if(!on_actionAsm_triggered())
        return;
    unsigned int i=0;
    while(i<asse.scodesize())
    {
        i = asse.simulator(i);
        update_reg();
    }
    pc = 0;
    asse.reg_clear();
}
//update the registers data
void MainWindow::update_reg()
{
    ui->regtable->setShowGrid(true);
    ui->regtable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    QStandardItemModel *model = new QStandardItemModel();

    ui->regtable->setModel(model);
    ui->regtable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->regtable->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->regtable->verticalHeader()->hide();
    ui->regtable->horizontalHeader()->hide();


    model->setColumnCount(8);
    //set the registers header
    model->setItem(0, 0, new QStandardItem("R0($zero)"));
    model->setItem(0, 1, new QStandardItem("R1($at)"));
    model->setItem(0, 2, new QStandardItem("R2($v0)"));
    model->setItem(0, 3, new QStandardItem("R3($v1)"));
    model->setItem(0, 4, new QStandardItem("R4($a0)"));
    model->setItem(0, 5, new QStandardItem("R5($a1)"));
    model->setItem(0, 6, new QStandardItem("R6($a2)"));
    model->setItem(0, 7, new QStandardItem("R7($a3)"));

    model->setItem(2, 0, new QStandardItem("R8($t0)"));
    model->setItem(2, 1, new QStandardItem("R9($t1)"));
    model->setItem(2, 2, new QStandardItem("R10($t2)"));
    model->setItem(2, 3, new QStandardItem("R11($t3)"));
    model->setItem(2, 4, new QStandardItem("R12($t4)"));
    model->setItem(2, 5, new QStandardItem("R13($t5)"));
    model->setItem(2, 6, new QStandardItem("R14($t6)"));
    model->setItem(2, 7, new QStandardItem("R15($t7)"));

    model->setItem(4, 0, new QStandardItem("R16($s0)"));
    model->setItem(4, 1, new QStandardItem("R17($s1)"));
    model->setItem(4, 2, new QStandardItem("R18($s2)"));
    model->setItem(4, 3, new QStandardItem("R19($s3)"));
    model->setItem(4, 4, new QStandardItem("R20($s4)"));
    model->setItem(4, 5, new QStandardItem("R21($s5)"));
    model->setItem(4, 6, new QStandardItem("R22($s6)"));
    model->setItem(4, 7, new QStandardItem("R23($s7)"));

    model->setItem(6, 0, new QStandardItem("R24($t8)"));
    model->setItem(6, 1, new QStandardItem("R25($t9)"));
    model->setItem(6, 2, new QStandardItem("R26($k0)"));
    model->setItem(6, 3, new QStandardItem("R27($k1)"));
    model->setItem(6, 4, new QStandardItem("R28($gp)"));
    model->setItem(6, 5, new QStandardItem("R29($sp)"));
    model->setItem(6, 6, new QStandardItem("R30($fp)"));
    model->setItem(6, 7, new QStandardItem("R31($ra)"));

    for(unsigned int i=0;i<8;i+=2)
        for(unsigned int j=0;j<8;j++)
        {
            model->item(i, j)->setBackground(QBrush(QColor(225,225,225)));
            model->item(i, j)->setForeground(QBrush(QColor("Black")));
            model->item(i, j)->setFont(QFont("Times New Roman", 12, QFont::Bold));
        }
    unsigned int *reg = asse.registers_out();

    //set the registers data
    for(unsigned int i=1;i<8;i+=2)
        for(unsigned int j=0;j<8;j++)
        {
            //cout << qtos(model->item(i, j)->data().toString());
            model->setItem(i, j, new QStandardItem(stoq(asse.trans_d_hex(reg[i/2*8+j]))));
        }
    for(unsigned int i=0;i<8;i++)
        for(unsigned int j=0;j<8;j++)
            model->item(i,j)->setTextAlignment(Qt::AlignCenter);
}

//step
void MainWindow::on_actionStep_triggered()
{
    //bin_model-> item(pc_last, 0)->setBackground(QBrush("White"));
    //bin_model-> item(pc, 0)->setBackground(QBrush("Red"));
    //pc_last = pc;
	on_actionAsm_triggered();
    string text = "executed intruction: ";
    text += asse.getinstruction(pc);
    ui->plainTextEdit->setPlainText(stoq(text));
    pc = asse.simulator(pc);
    update_reg();
    if(pc == asse.scodesize())
    {
        bin_model-> item(pc_last, 0)->setBackground(QBrush("White"));
        ui->regtable->clearSpans();
        pc = 0;
        asse.reg_clear();
        return;
    }
}
//stop step
void MainWindow::on_actionStop_step_triggered()
{
    pc = 0;
    asse.reg_clear();
}

void MainWindow::on_actionDisAsm_triggered()
{
    string text;

    asse.disasm();
    //cout << "Disasm "<< endl;
    text = asse.bintext_out();
    ui->source->setPlainText(stoq(text));
}
