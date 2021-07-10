#include "dialog.h"
#include "./ui_dialog.h"

#include "code_metrics.h"
#include <QDebug>
#include <QFileDialog>

// Scan types - this should match enum sequence
QStringList Dialog::m_scanListTypes =  {"List Directories", "List Files", "List Classes"};


Dialog::Dialog(QWidget *parent) : QDialog(parent), ui(new Ui::Dialog)
{

    m_codeMetrics = new(CodeMetrics);
    ui->setupUi(this);

    // Initialize the pointers for Bulk QStringList Objects
    m_codeMetrics->m_fileList = new(QStringList);
    m_codeMetrics->m_dirList = new(QStringList);

    // Initialize the GUI element values
    //ui->lineEdit->setText("D:/Programs/Poco/poco-master-unzip");
    QString defaultDir = "D:/Programs/Poco/temp-test";
    m_codeMetrics->m_srcPath = defaultDir;
    ui->le_srcPath->setText(m_codeMetrics->m_srcPath);


    ui->le_fileFilters->setText("*.cpp,*.c");
    m_codeMetrics->m_fileFilters = ui->le_fileFilters->text().split(',');

    // Advanced Controls
    ui->cb_skipDirEnable->setCheckState(Qt::Unchecked);
    ui->le_dirFilters->setText(".git,.hg,.vscode");
    m_codeMetrics->m_dirFilters = ui->le_dirFilters->text().split(',');
    ui->le_dirFilters->setDisabled(true);
    m_codeMetrics->m_skipDirectoryEnable = 0;

    // Load Scan result types
    ui->cbox_scanResult->addItems(Dialog::m_scanListTypes);
    m_scanListSelection = Dialog::SCAN_LIST_DIRECTORIES;
}

Dialog::~Dialog()
{
    delete ui;
}

void Dialog::on_le_srcPath_editingFinished()
{
    // set src path
    m_codeMetrics->m_srcPath =  ui->le_srcPath->text();
    qDebug() << "set path is" << m_codeMetrics->m_srcPath;

}

void Dialog::on_le_fileFilters_editingFinished()
{
    // set file filter
    m_codeMetrics->m_fileFilters = ui->le_fileFilters->text().split(',');
    qDebug() << "set file filters are" << m_codeMetrics->m_fileFilters;
}

void Dialog::on_le_dirFilters_editingFinished()
{
    // set Dir filter
    m_codeMetrics->m_dirFilters = ui->le_dirFilters->text().split(',');
    qDebug() << "set dir filters are" << m_codeMetrics->m_dirFilters;
}

void Dialog::on_cb_skipDirEnable_stateChanged(int arg1)
{
    if(arg1 == Qt::Checked)
    {
        m_codeMetrics->m_skipDirectoryEnable = 1;
        ui->le_dirFilters->setDisabled(false);
    }
    else
    {
        m_codeMetrics->m_skipDirectoryEnable = 0;
        ui->le_dirFilters->setDisabled(true);
    }
}

void Dialog::on_ple_scanResult_textChanged()
{
 qDebug() << "text changed";
}

// scan type selection with string
void Dialog::on_cbox_scanResult_activated(const QString &arg1)
{
   //qDebug() << "activated string" << arg1;
}

// scan type selection with index
void Dialog::on_cbox_scanResult_activated(int index)
{
    m_scanListSelection = index;
    this->displayScanList();
    qDebug() << "activated Scan Type" << m_scanListSelection;
}

void Dialog::clearScanList()
{
    ui->ple_scanResult->clear();
}

void Dialog::displayScanList()
{
    this->clearScanList();

    if(this->m_scanListSelection == Dialog::SCAN_LIST_DIRECTORIES)
    {
        // Display Dir list
        if (m_codeMetrics->m_dirList->isEmpty())
          ui->ple_scanResult->appendPlainText( "Directory List Empty" );
        else
          ui->ple_scanResult->appendPlainText( m_codeMetrics->m_dirList->join("\n") );
    }
    else if (this->m_scanListSelection == Dialog::SCAN_LIST_FILES)
    {
        // Display File List
        if (m_codeMetrics->m_fileList->isEmpty())
          ui->ple_scanResult->appendPlainText( "File List Empty" );
        else
          ui->ple_scanResult->appendPlainText( m_codeMetrics->m_fileList->join("\n") );
    }
    else if (this->m_scanListSelection == Dialog::SCAN_LIST_CLASSES)
    {
        // TODO
    }
    else
    {
        // Nothing is selected
    }
}

void Dialog::scanFiles()
{

    if(m_codeMetrics->m_skipDirectoryEnable == 1)
    {
        // Get Filtered Dir list
        m_codeMetrics->m_dirCount  = m_codeMetrics->getDirList(m_codeMetrics->m_srcPath, m_codeMetrics->m_dirFilters, m_codeMetrics->m_dirList);
        qDebug() << "Filtered Dir count is " << m_codeMetrics->m_dirCount;

        // Get list of Directories after applying the skip dir filters and then get the files
        m_codeMetrics->m_fileCount = m_codeMetrics->getFileList(m_codeMetrics->m_srcPath, m_codeMetrics->m_fileFilters, m_codeMetrics->m_dirList, m_codeMetrics->m_fileList);
        qDebug() << "Total File count is " << m_codeMetrics->m_fileCount;
    }
    else
    {
        // Get Directory List
        m_codeMetrics->m_dirCount = m_codeMetrics->getDirList(m_codeMetrics->m_srcPath, m_codeMetrics->m_dirList);
        qDebug() << "Total Directory count is " << m_codeMetrics->m_dirCount;

        // Get the list of Files from ALL Directories
        m_codeMetrics->m_fileCount = m_codeMetrics->getFileList(m_codeMetrics->m_srcPath, m_codeMetrics->m_fileFilters, m_codeMetrics->m_fileList);
        qDebug() << "Total File count is " << m_codeMetrics->m_fileCount;
    }
}

// --------------------------------------------
// Push button Handles
// --------------------------------------------

void Dialog::on_pb_browsePath_clicked()
{
    QString dir;
    dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"), "C:/", QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if(dir.isEmpty() == false)
    {
        m_codeMetrics->m_srcPath  = dir;
        ui->le_srcPath->setText(m_codeMetrics->m_srcPath);
    }
}

void Dialog::on_pb_scanFiles_clicked()
{

}

void Dialog::on_pb_modAndUpdate_clicked()
{

}

void Dialog::on_pb_generateReport_clicked()
{
    // clear previous values
    m_codeMetrics->m_fileList->clear();
    m_codeMetrics->m_dirList->clear();
    ui->ple_scanResult->clear();
    ui->tb_report->clear();

    if(m_codeMetrics->m_skipDirectoryEnable == 1)
    {
    }

    // scan the files
    scanFiles();

    // display the scanned files
    displayScanList();

    // Count the number of Lines
    m_codeMetrics->m_lineCount = m_codeMetrics->getLineCountFiles(m_codeMetrics->m_fileList);

    // print results
    QTextCharFormat format;
    format.setFontWeight(QFont::ExtraBold);
    ui->tb_report->textCursor().mergeCharFormat(format);
    ui->tb_report->setTextColor(0x8000ff);
    ui->tb_report->append("Total Directories: \t" + QString::number(m_codeMetrics->m_dirCount));
    ui->tb_report->append("Total Files: \t\t" + QString::number(m_codeMetrics->m_fileCount));
    ui->tb_report->append("Total Lines: \t\t" + QString::number(m_codeMetrics->m_lineCount));
    qDebug() << "Total files is " << m_codeMetrics->m_fileCount << "Lines count is " << m_codeMetrics->m_lineCount;
}


