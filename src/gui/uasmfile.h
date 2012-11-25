#ifndef UASMFILE_H
#define UASMFILE_H

#include "IUasmFile.h"

class QDir;
class Project;

class UAsmFile : public IUasmFile
{
public:
    UAsmFile(Project *parentProject, QString absoluteFileDir);
    ~UAsmFile();

    void setEditor(ICodeEditor *codeEditor);
    void open();
    void save();
    void close();
    QString getRelativePath();
    bool contentChanged();
    bool isOpen();
    bool isAbsFilePath(QString absFilePath);
    bool setLineFocus(uint32_t lineNr, int color);
    void setEditable(bool flag);

private:
    QString m_absoluteFileDir;
    Project *m_parentProject;
    bool m_openFile;
    ICodeEditor *m_codeEditor;
    bool m_editable;

};

#endif // UASMFILE_H
