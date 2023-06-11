#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include "code_metrics.h"


QT_BEGIN_NAMESPACE
namespace Ui { class Dialog; }
QT_END_NAMESPACE

class Dialog : public QDialog
{
    Q_OBJECT

public:
    Dialog(QWidget *parent = nullptr);
    ~Dialog();


    enum ScanListType
    {
        SCAN_LIST_DIRECTORIES = 0,
        SCAN_LIST_FILES,
        SCAN_LIST_CLASSES
    };

    // Public member Variables
    CodeMetrics *m_codeMetrics;
    static QStringList m_scanListTypes;
    int m_scanListSelection;
    QString m_scanListString;

    // Public member Functions
    void scanFiles();
    void displayScanList();
    void clearScanList();
    void processFiles();
    void prepareReport();
    void scanResult_textChanged();

private slots:
    void on_le_srcPath_editingFinished();
    void on_le_fileFilters_editingFinished();
    void on_le_dirFilters_editingFinished();
    void on_cb_skipDirEnable_stateChanged(int arg1);
    void on_pb_generateReport_clicked();
//    void on_cbox_scanResult_activated(const QString &arg1);
    void on_cbox_scanResult_activated(int index);
    void on_pb_scanFiles_clicked();
    void on_pb_browsePath_clicked();
    void on_pb_updateReport_clicked();
    void on_pb_clearAllList_clicked();

private:
    Ui::Dialog *ui;
};
#endif // DIALOG_H
