#ifndef IUASMFILE_H
#define IUASMFILE_H

#include <QString>
#include <inttypes.h>

class ICodeEditor;

class IUasmFile
{
public:
    virtual void setEditor(ICodeEditor *codeEditor) = 0;
    virtual void open() = 0;
    virtual void save() = 0;
    virtual void close() = 0;
    virtual QString getRelativePath() = 0;
    virtual bool contentChanged() = 0;
    virtual bool isOpen() = 0;
    virtual bool isAbsFilePath(QString absFilePath) = 0;
    virtual bool setLineFocus(uint32_t lineNr, int color) = 0;
    virtual void setEditable(bool flag) = 0;
};

class IUasmFilePointer
{
public:
   // IUasmFilePointer() { };
   // ~IUasmFilePointer() { };
    IUasmFile *m_filePointer;
};

//QVariant Definition, to be able to store interface as QListWidgetItem in QListWidget
#include <QMetaType>
Q_DECLARE_METATYPE(IUasmFilePointer)

#endif // IUASMFILE_H

