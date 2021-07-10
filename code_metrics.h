#ifndef CODEMETRICS_H
#define CODEMETRICS_H

#include <QString>
#include <QStringList>


class CodeMetrics
{

public:
    CodeMetrics();
    //virtual ~CodeMetrics();

    // public variables
    QString m_srcPath;
    QStringList m_fileFilters, m_dirFilters;
    QStringList *m_fileList, *m_dirList;
    int m_skipDirectoryEnable;
    int m_lineCount, m_dirCount, m_fileCount, m_classCount;

    // public functions
    int getLineCount(QString filename);
    int getLineCountFiles(const QStringList *file_list);
    int getDirList(const QString path, QStringList *dir_list);
    int getDirList(const QString path, QStringList dir_filters, QStringList *dir_list);
    int getFileList(const QString path, QStringList *file_list);
    int getFileList(const QString path, QStringList file_filters, QStringList *file_list);
    int getFileList(const QString path, QStringList file_filters, QStringList *dir_list_filtered, QStringList *file_list);

    // Static functions that act as utility functions freely used without object
    static void printStringList( QStringList *string_list);


private:

    // private functions
    void processLine(const QByteArray &);
    int processLineKeyword(const QString key, const QString line_text, QString& variable_name );
    int getVariableDetails(const QString keyword, QStringList *file_list, QStringList *variable_list);
    int checkSkipDirectories(QString current_dir, QStringList dir_filters);
    int deleteSpecificDirectories(QString current_dir, QStringList dir_filters);


};

#endif // CODEMETRICS_H
