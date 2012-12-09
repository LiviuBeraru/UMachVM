#include "project.h"
#include "uasmfile.h"
#include <QFile>
#include <QTextStream>
#include <QString>
#include <QFileInfo>
#include "assert.h"
#include <QDir>
#include "umachdebuginfo.h"
#include "umachsymbolinfo.h"


Project::Project(QString projectFile)
{
    m_projectFile = projectFile;
    QFileInfo fi(projectFile);
    m_projectDir = new QDir(fi.absolutePath());
    m_name = fi.baseName();
    m_isSaved = false;
    m_debugInfo = NULL;
    m_symbolInfo = NULL;
}

Project *Project::New(QString projectFile)
{
    return new Project(projectFile);
}

Project *Project::Open(QString projectFile)
{
    Project *newProject = new Project(projectFile);

    //opening file
    QFile file(newProject->m_projectFile);
    if (!file.open(QIODevice::ReadOnly))
        throw (newProject->m_projectFile);
    QTextStream in(&file);

    //parsing ATM only uasm Files
    QString readLine;
    while (!(readLine = in.readLine()).isEmpty()
           && !(readLine == ".BREAKPOINTS")) {

        if (readLine == ".SOURCES")
            continue;
        newProject->addFile(newProject->m_projectDir->absoluteFilePath(readLine));
    }
    //close File
    file.close();
    //is saved
    newProject->m_isSaved = true;
    //return created project
    return newProject;
}

IUasmFile* Project::addFile(QString absoluteFilePath)
{

    //check if allready in list

    for (int i = 0; i < m_projectUasmFiles.size(); i++)
    {
        if (m_projectUasmFiles[i]->getRelativePath()
                == getProjectDir()->relativeFilePath(absoluteFilePath))
            return NULL;
    }

    setIsNotSaved();

    //file Info
    QFileInfo newFileInfo(absoluteFilePath);

    //create empty file if non exsist
    if (!newFileInfo.exists()) {
        QFile filePointer(absoluteFilePath);
        filePointer.open( QIODevice::ReadWrite );
        filePointer.flush();
        filePointer.close();
    }

    UAsmFile *file = new UAsmFile(this, absoluteFilePath);
    m_projectUasmFiles.push_back(file);
    return file;
}

void Project::save()
{
    assert(!m_projectFile.isEmpty());

    for (int i = 0; i < m_projectUasmFiles.size(); i++) {
        m_projectUasmFiles[i]->save();
    }

    //open file
    QFile file(m_projectFile);
    if (!file.open(QIODevice::WriteOnly))
        throw (m_projectFile);
    QTextStream out(&file);

    //writing Sources
    out << ".SOURCES" << '\n';
    for (int i = 0; i < m_projectUasmFiles.size(); i++) {
        out << m_projectUasmFiles[i]->getRelativePath() << '\n';
    }

    //writing breakpoints

    out.flush();
    file.close();
    m_isSaved = true;
}

QDir* Project::getProjectDir()
{
    return m_projectDir;
}

QVector<IUasmFile*> Project::getFiles()
{
    QVector<IUasmFile*> interfaceVector;
    for (int i = 0; i < m_projectUasmFiles.size(); i++)
    {
        interfaceVector.push_back(m_projectUasmFiles[i]);
    }
    return interfaceVector;
}

void Project::setIsNotSaved()
{
    m_isSaved = false;
}

bool Project::checkIsSaved()
{
    if (!m_isSaved)
        return m_isSaved;

    for (int i = 0; i < m_projectUasmFiles.size(); i++)
    {
        if (m_projectUasmFiles[i]->contentChanged())
            return false;
    }
    return true;
}

Project::~Project()
{
    m_projectUasmFiles.clear();
}

QString Project::getName()
{
    return m_name;
}

IUasmFile* Project::getFileByAbsPath(QString absPath)
{
    for (int i = 0; i < m_projectUasmFiles.size(); i++)
    {
        if (m_projectUasmFiles[i]->isAbsFilePath(absPath))
            return m_projectUasmFiles[i];
    }

    //not found
    return NULL;
}

void Project::generateDebugInfo()
{
    delete (m_symbolInfo);
    m_symbolInfo = new UMachSymbolInfo(this);

    delete(m_debugInfo);
    m_debugInfo = new UMachDebugInfo(this);

    //build debug Map
    rebuildBreakPointMap();

}

bool Project::addBreakPoint(IUasmFile *file, uint32_t lineNr, uint32_t address, QString *label)
{
    //check if allready in list
    for (int i = 0; i < m_breakPoints.size(); i++) {
        if (m_breakPoints[i]->uasmFile == file && m_breakPoints[i]->lineNumber == lineNr)
            return false;
        if (m_breakPoints[i]->label == label)
            return false;
    }

    m_breakPoints.append(new debugAddressEntry(file, lineNr, address, label));

    return true;
}

bool Project::removeBreakPoint(IUasmFile *file, uint32_t lineNr)
{

    //search entry
    int entrynr = 0;
    while (!(m_breakPoints[entrynr]->uasmFile == file)
           && !(m_breakPoints[entrynr]->lineNumber == lineNr)
           && entrynr++ < m_breakPoints.size())

    if (entrynr == m_breakPoints.size())
        return false;

    m_breakPoints.removeAt(entrynr);

    //need to rebuild map, to have correct entries when removing a breakpoint
    if (m_debugInfo)
        rebuildBreakPointMap();

    return true;
}

void Project::rebuildBreakPointMap()
{
    if (!m_debugInfo)
        return;

    m_addressBreakPointMap.clear();
    //then fill in addresses for breakpoints
    for (int i = 0; i < m_breakPoints.size(); i++) {
        if (m_debugInfo->setAdressForEntry(m_breakPoints[i], m_symbolInfo))
            m_addressBreakPointMap.insert(m_breakPoints[i]->codeAddress,i);
    }
}

void Project::setBreakPointFocus(uint32_t address)
{
    if (!m_debugInfo)
        return;

    debugAddressEntry *entry = m_breakPoints[m_addressBreakPointMap.value(address)];
    emit openCodeTab(entry->uasmFile);
    entry->uasmFile->setLineFocus(entry->lineNumber, 2);
}

bool Project::isBreakPoint(uint32_t address)
{
    return m_addressBreakPointMap.contains(address);
}

void Project::setFocusByAdress(uint32_t address)
{
    assert(m_debugInfo);

    debugAddressEntry *entry = m_debugInfo->getFileAndLineByAddress(address);

    assert(entry);

    emit openCodeTab(entry->uasmFile);
    entry->uasmFile->setLineFocus(entry->lineNumber, 1);

}

void Project::setCodeEditable(bool flag)
{
    for(int i = 0; i < m_projectUasmFiles.size(); i++) {
        m_projectUasmFiles[i]->setEditable(flag);
    }
}

bool Project::removeFileFromProject(IUasmFile *file)
{
    if (!m_projectUasmFiles.contains((UAsmFile*)file))
        return false;

    for(int i = 0; i < m_projectUasmFiles.size(); i++){
        if (m_projectUasmFiles[i] == file) {
            if (m_projectUasmFiles[i]->isOpen())
                return false;
            m_projectUasmFiles.remove(i);
        }
    }
    return true;
}

void Project::removeDebugInfo()
{
    delete(m_debugInfo);
    m_debugInfo = NULL;
    delete(m_symbolInfo);
    m_symbolInfo = NULL;
}

UMachSymbolInfo* Project::getSymbolInfo()
{
    return m_symbolInfo;
}
