#ifndef PROJECT_H
#define PROJECT_H
#include <QString>
#include <QVector>
#include <QHash>
#include <inttypes.h>

class IUasmFile;
class UAsmFile;
class QDir;
class UMachDebugInfo;
struct debugAddressEntry;


class Project : public QObject
{
    Q_OBJECT

public:
    static Project* Open(QString projectFile);
    static Project* New(QString projectFile);

    ~Project();

    void save();

    bool checkIsSaved();
    void setIsNotSaved();

    IUasmFile* addFile(QString absoluteFilePath);

    QDir* getProjectDir();
    QString getName();

    QVector<IUasmFile*> getFiles();
    IUasmFile* getFileByAbsPath(QString absPath);

    void generateDebugInfo();

    bool addBreakPoint(IUasmFile *file, uint32_t lineNr);
    bool removeBreakPoint(IUasmFile *file, uint32_t lineNr);

    bool isBreakPoint(uint32_t address);
    void setBreakPointFocus(uint32_t address);

    void setFocusByAdress(uint32_t address);

    void setCodeEditable(bool flag);

    bool removeFileFromProject(IUasmFile *file);

    void removeDebugInfo();

private:
    Project(QString projectFile);
    QVector<UAsmFile*> m_projectUasmFiles;
    QString m_projectFile;
    QDir *m_projectDir;
    QString m_name;
    bool m_isSaved;

    UMachDebugInfo *m_debugInfo;

    QList <debugAddressEntry*> m_breakPoints;
    QHash <uint32_t,int> m_addressBreakPointMap;

    void rebuildBreakPointMap();

signals:
    void openCodeTab(IUasmFile *file);


};

#endif // PROJECT_H
