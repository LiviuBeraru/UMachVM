#include "umachmake.h"
#include "project.h"
#include <QStringList>
#include <QVector>
#include "IUasmFile.h"
#include <QProcess>
#include <QString>
#include <QDir>

UMachMake::UMachMake()
{
}

bool UMachMake::makeUMXfromProject(Project *project)
{
    //first we save
    project->save();

    //the we get the files in a list
    QString argument;

    QString path = project->getProjectDir()->absolutePath();
    QVector <IUasmFile*> files = project->getFiles();

    //we have to run sh in xterm and safe the error number in a file, cause xterm won't return it

    for (int i = 0; i < files.size(); i++) {
        argument.append(QString(path + "/" + files[i]->getRelativePath() + " "));
    }
    argument.append(QString("-g -w -o "));
    argument.append(QString(path + "/" + project->getName() + ".umx; echo $? > errorcode.tmp\""));
    QProcess::execute(QString("xterm -T \"Uasm\" -e sh -c \"./uasm ").append(argument));

    QFile errorFile("errorcode.tmp");
    errorFile.open(QIODevice::ReadWrite);
    char error;
    errorFile.read(&error,1);
    errorFile.close();

    int8_t errno = (int8_t)error;

    QFile::remove("errorcode.tmp");

    if ((errno -= 48) != 0)
        return false;

    project->generateDebugInfo();
    return true;
}
