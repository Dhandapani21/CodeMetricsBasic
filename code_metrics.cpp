#include "code_metrics.h"
#include <QDebug>
#include <QDirIterator>
#include <QString>
#include <QStringList>
#include <QTextStream>
#include <QFile>

CodeMetrics::CodeMetrics()
{

}


// ---------------------------------------------------------------------------
// Private Functions
// ---------------------------------------------------------------------------


// used for binary files
void CodeMetrics::processLine(const QByteArray &)
{

}

int CodeMetrics::processLineKeyword(const QString key, const QString line_text, QString& variable_name )
{
    int result =0;
    static int block_comment=0;

    // split the line into words
    QStringList fields = line_text.split(' ');
    if(fields.at(0) == "//")
    {
        return result;
    }
    // Block comment may start and end on same line or on different lines
    if (fields.at(0).startsWith("/*", Qt::CaseInsensitive) == true)
    {
        block_comment = 1;
        //qDebug() << "** BLK comment started ** " << line_text;
    }

    if (fields.last().endsWith("*/", Qt::CaseInsensitive) == true)
    {
        block_comment = 0;
        //qDebug() << "** BLK comment ended ** " << line_text;
        return result;      // this return handle when block comment start and end on same line
    }

    if(block_comment == 1) return result;       // returning here to check after checking block comment ended or not

    // check the word matches the keyword
    for(int i=0; i<fields.length();i++)
    {
        if(fields.at(i) == key)
        {
            if((i+1) < fields.length())
            {
                variable_name = fields.at(i+1);
                result=1;
                break;
            }
        }
    }

    return result;
}

//int get_variable_details(QString keyword, QStringList *file_list, QStringList *variable_list)
int CodeMetrics::getVariableDetails(const QString keyword, QStringList *file_list, QStringList *variable_list)
{
    QString file_name, variable_name;
    int line_number=0, found=0, variable_count=0;
    QFile file;

    // Iterate over the file list
    foreach (QString file_name, *file_list)
    {
        file.setFileName(file_name);
        if ( !file.open( QIODevice::ReadOnly | QIODevice::Text) )
        {
            return 0;
        }

        QTextStream file_stream(&file);
        QString line_text = file_stream.readLine();
        line_number++;

        //qDebug() << "### Current File is ####" << file_name;
        while (!line_text.isNull())
        {
            found = processLineKeyword(keyword,line_text,variable_name);
            if (found)
            {
                variable_count++;

                // construct variable details here
                QString variable_details;
                //QStringList file_name_split = file_name.split("/");
                //QString file_name_alone = file_name_split.last();
                //variable_details = QString::number(line_number) +  "," + file_name_alone + "," + variable_name;
                variable_details = QString::number(line_number) +  "," + variable_name + "," + file_name;
                variable_list->append(variable_details);
                qDebug() << variable_details;
            }

            // move to reading next line
            line_number++;
            line_text = file_stream.readLine();
            found=0;
        }

        // reset the line number and close the file
        line_number=0;
        file.close();
    }

    return variable_count;
}

int CodeMetrics::checkSkipDirectories(QString current_dir, QStringList dir_filters)
{
    int result=0;

    // check whether current dir is unwanted, then delete it, else process it
    foreach (QString skip_dir_name, dir_filters)
    {
        qDebug() << "iterating Dir filter " << skip_dir_name;

        //if (current_dir.endsWith(skip_dir_name, Qt::CaseInsensitive))
        //qDebug() << "current dir is " << current_dir;
        //QStringList folder_names_in_path = current_dir.split('/');
        //qDebug() << "temp folder names in path";
        //print_str_list(&folder_names_in_path);
        //qDebug() << "last item is" << folder_names_in_path.takeLast();
        //if(folder_names_in_path.takeLast() == skip_dir_name)

        // method 1 - comparing the current folder name
        // if (current_dir.endsWith(skip_dir_name, Qt::CaseInsensitive))

        // method 2- s;lit the folder names in path and compare the last one
        // QStringList folder_names_in_path = current_dir.split('/');
        // QString dir_name_stripped = folder_names_in_path.takeLast(); - this will remove last element
        // if( dir_name_stripped == skip_dir_name)
        // both method1 and method2 are not usefull - because, it skips only the folder that matches
        // but iterates to sub-dirs of the unwanted folder

        // method 3- split the folder names in path and compare with dir filterss
        // QStringList folder_names_in_path = current_dir.split('/');
        // if( folder_names_in_path.contains(skip_dir_name, Qt::CaseInsensitive) == true)
        // this may lead to bug - because this checks the strings on the existing base folder path itself
        // manually verify that the dir filter strings are not present in the base path
        QStringList folder_names_in_path = current_dir.split('/');
        if( folder_names_in_path.contains(skip_dir_name, Qt::CaseInsensitive) == true)
        {
            result=1;
            qDebug() << "skip Dir name " << current_dir;
            break;
        }
    }

    return result;
}

int CodeMetrics::deleteSpecificDirectories(QString current_dir, QStringList dir_filters)
{
    int result=1;
    bool delete_dir = false;

    // check whether current dir is unwanted, then delete it, else process it
    foreach (QString unwanted_dir, dir_filters)
    {
        //qDebug() << "** " << current_dir << " ** ";
        //if (dir_itr.filePath().endsWith(unwanted_dir, Qt::CaseInsensitive))
        //if (dir_itr.fileInfo().absoluteFilePath().endsWith(unwanted_dir, Qt::CaseInsensitive))
        if (current_dir.endsWith(unwanted_dir, Qt::CaseInsensitive))
        {
            delete_dir = true;
            break;
        }
        else
        {
            delete_dir = false;
        }
    }

    if (delete_dir == true)
    {
        QDir dir(current_dir);
        qDebug() << "Removing the Directory :" << current_dir;
        dir.removeRecursively();
        result=0;
    }

    return result;
}


// ---------------------------------------------------------------------------
// Static Public Functions
// ---------------------------------------------------------------------------

void CodeMetrics::printStringList( QStringList *string_list)
{
    // check empty
    if (string_list->empty() == true) qDebug() << "StringList is empty";

    // print in console
    for(int i=0;i<string_list->length();i++)
    {
        qDebug() << string_list->at(i);
    }
}

// ---------------------------------------------------------------------------
// Public Functions
// ---------------------------------------------------------------------------

int CodeMetrics::getLineCount(QString filename)
{
    int line_count=0;

    QFile file(filename);
    if ( !file.open( QIODevice::ReadOnly | QIODevice::Text) )
    {
        return 0;
    }
    QTextStream in_stream(&file);
    QString line_text = in_stream.readLine();
    while (!line_text.isNull())
    {
        line_text = in_stream.readLine();
        // process_line(line_text);
        line_count++;
    }
    file.close();

    //qDebug() << "**"  << line_count << "**";
    return line_count;
}

int CodeMetrics::getLineCountFiles(const QStringList *file_list)
{
    int line_count_files=0;

    foreach (QString file_name, *file_list)
    {
        line_count_files += getLineCount(file_name);
    }

    return line_count_files;
}

int CodeMetrics::getDirList(const QString path, QStringList *dir_list)
{
    int dir_count=0;
    QString current_dir,start_dir;

    if(path.isEmpty()) return 0;

    // Iterate the directories and get the list of directories
    QDirIterator dir_itr(path, QDir::Dirs | QDir::NoSymLinks | QDir::NoDotAndDotDot, QDirIterator::Subdirectories);
    start_dir = dir_itr.path();
    qDebug() << "start dir is " << start_dir;

    while (dir_itr.hasNext())
    {
        current_dir = dir_itr.next();
        dir_list->append(current_dir);
        dir_count++;
    }
    return dir_count;
}

// Method to traverse ALL dirs, delete the unwanted dirs and report the FILTERED dirs
int CodeMetrics::getDirList(const QString path, QStringList dir_filters, QStringList *dir_list)
{
    int dir_count=0, result=0;
    QString current_dir,start_dir;

    if(path.isEmpty()) return 0;
    qDebug() << "dir_filters are" ;
    printStringList(&dir_filters);

    // Iterate the directories and get the filtered Dirs only after skipping the directories in dir_filters
    QDirIterator dir_itr(path, QDir::Dirs | QDir::NoSymLinks | QDir::NoDotAndDotDot, QDirIterator::Subdirectories);
    start_dir = dir_itr.path();
    qDebug() << "start dir is " << start_dir;
    //qDebug() << "dir name is " << QDir::currentPath().toLatin1().data();  // this shows the project build path

    while (dir_itr.hasNext())
    {
        current_dir = dir_itr.next();
        result = checkSkipDirectories(current_dir, dir_filters);
        //result =1;
        if(result == 0)
        {
            // process the dirs
            dir_list->append(current_dir);
            dir_count++;
        }
    }
    return dir_count;
}

// Common method to list ALL files from ALL the directories
int CodeMetrics::getFileList(const QString path, QStringList *file_list)
{
    int file_count=0;

    if(path.isEmpty()) return 0;

    QDirIterator file_itr(path, QDir::Files | QDir::NoSymLinks | QDir::NoDotAndDotDot, QDirIterator::Subdirectories); //QDir::Files | QDir::NoSymLinks | QDir::NoDotAndDotDot
    while (file_itr.hasNext())
    {
        file_list->append(file_itr.next());
        file_count++;
    }

    return file_count;
}

// Common method to list FILTERED files from ALL the directories
int CodeMetrics::getFileList(const QString path, QStringList file_filters, QStringList *file_list)
{
    int file_count=0;

    if(path.isEmpty()) return 0;

    QDirIterator file_itr(path, file_filters, QDir::Files | QDir::NoSymLinks | QDir::NoDotAndDotDot, QDirIterator::Subdirectories);
    while (file_itr.hasNext())
    {
        file_list->append(file_itr.next());
        file_count++;
    }

    return file_count;
}

// with Directories filter support - skip unwanted directories
int CodeMetrics::getFileList(const QString path, QStringList file_filters, QStringList *dir_list_filtered, QStringList *file_list)
{
    int file_count=0;
    QString current_dir;

    if(path.isEmpty()) return 0;

    // traverse through the directory list and fetch the file list
    for(int j=0;j<dir_list_filtered->count();j++)
    {
        QDirIterator file_itr(dir_list_filtered->at(j), file_filters, QDir::Files | QDir::NoSymLinks | QDir::NoDotAndDotDot, QDirIterator::NoIteratorFlags); // Not browsing sub-dirs here
        while (file_itr.hasNext())
        {
            file_list->append(file_itr.next());
            file_count++;
        }
    }

    return file_count;
}









