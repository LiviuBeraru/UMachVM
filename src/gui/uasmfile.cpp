#include "uasmfile.h"
#include "icodeeditor.h"
#include "assert.h"
#include "project.h"
#include <QFile>
#include <QMessageBox>
#include <QTextStream>
#include <QString>
#include <QDir>

UAsmFile::UAsmFile(Project *parentProject, QString absoluteFileDir)
{
   m_parentProject = parentProject;
   m_absoluteFileDir = absoluteFileDir;
   m_openFile = false;
   m_editable = true;
   m_codeEditor = NULL;
}

UAsmFile::~UAsmFile()
{

}

QString UAsmFile::getRelativePath()
{
    return m_parentProject->getProjectDir()->relativeFilePath(m_absoluteFileDir);
}

void UAsmFile::setEditor(ICodeEditor *codeEditor)
{
    m_codeEditor = codeEditor;
}


void UAsmFile::open()
{
    assert(m_codeEditor);
    assert(!m_openFile);
    assert(!m_absoluteFileDir.isEmpty());

    QFile file(m_absoluteFileDir);
    if (!file.open(QIODevice::ReadOnly))
        throw (m_absoluteFileDir);
    QTextStream in(&file);
    m_codeEditor->setContent(in.readAll());
    file.close();

    m_openFile = true;
    m_codeEditor->setEditable(m_editable);
}


void UAsmFile::save()
{
    if (!m_openFile)
        return;

    assert(m_codeEditor);
    assert(!m_absoluteFileDir.isEmpty());

    if (!m_codeEditor->contentHasChanged())
        return;

    QFile file(m_absoluteFileDir);
    if (!file.open(QIODevice::WriteOnly))
        return;
    QTextStream out(&file);
    out << m_codeEditor->getContent();
    out.flush();
    file.close();

    m_codeEditor->setSaved();
    return;
}

void UAsmFile::close()
{
    m_codeEditor = NULL;
    m_openFile = false;
}

bool UAsmFile::contentChanged()
{
    if (m_openFile)
        return m_codeEditor->contentHasChanged();
    return false;
}

bool UAsmFile::isOpen()
{
    return m_openFile;
}

bool UAsmFile::isAbsFilePath(QString absFilePath)
{
    return (m_absoluteFileDir == absFilePath);
}

bool UAsmFile::setLineFocus(uint32_t lineNr, int color)
{
    if (!isOpen() || !m_codeEditor)
        return false;

    m_codeEditor->setLineFocus(lineNr, color);
    return true;
}

void UAsmFile::setEditable(bool flag)
{
    m_editable = flag;
    if (m_codeEditor)
        m_codeEditor->setEditable(flag);
}
