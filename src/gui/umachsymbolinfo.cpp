#include "umachsymbolinfo.h"
#include "project.h"

#include <QFile>
#include <QList>
#include <QDir>
#include <QStringList>
#include <QTextStream>
#include <assert.h>

UMachSymbolInfo::UMachSymbolInfo(Project *uproject)
{

    QFile symbolFile( uproject->getProjectDir()->absoluteFilePath(QString(uproject->getName() + ".umx.sym")) );

    if (!symbolFile.open(QIODevice::ReadOnly))
        throw (QString("Failed to open file"));

    //<ADRESS><TYPE><LABEL>

    QTextStream in(&symbolFile);
    QString line = in.readLine();
    while (!line.isNull()){

        symbolElement *symbol = new symbolElement;
        QStringList split = line.split(" ");
        //address
        symbol->address = split[0].toUInt(NULL, 16);
        //typ
        if (split[1] == "jmp") {
            symbol->type = SYM_JMP;
            symbol->datumLength = 0;
        } else if (split[1] == "int") {
            symbol->type = SYM_INT;
            m_integerLabels.append(split[2]);
            symbol->datumLength = 4;
        } else if (split[1] == "str") {
            symbol->type = SYM_STR;
            m_stringLabels.append(split[2]);
            symbol->datumLength = 0;
        } else {
            symbol->type = SYM_UNKNOWN;
        }

        m_symbols.insert(split[2], symbol);

        line = in.readLine();
    }

    symbolFile.close();
}

symbolElement* UMachSymbolInfo::getSymbolByLabel(QString label)
{
    return m_symbols.value(label, NULL);
}

UMachSymbolInfo::~UMachSymbolInfo()
{
    m_symbols.clear();
}

QStringList UMachSymbolInfo::getIntegerLabels()
{
    return m_integerLabels;
}

QStringList UMachSymbolInfo::getStringLabels()
{
    return m_stringLabels;
}

void UMachSymbolInfo::setDatumLengthByLabel(QString label, size_t datumLength)
{
    symbolElement *element = m_symbols.value(label, NULL);
    assert(element);

    element->datumLength = datumLength;
}

unsigned UMachSymbolInfo::getAddressByLabel(QString label)
{
    symbolElement *element = m_symbols.value(label, NULL);
    if(element)
        return element->address;
    return 0;
}

