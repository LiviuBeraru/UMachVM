#include "umachdebuginfo.h"
#include "IUasmFile.h"
#include "project.h"
#include <QDir>
#include <QTextStream>
#include <QString>
#include <QStringList>
#include <QHash>
#include <inttypes.h>
#include <byteswap.h>
#include "umachsymbolinfo.h"

UMachDebugInfo::UMachDebugInfo(Project *uproject)
{

    //first we read temporary the number / file assoziation


    QFile fmap( uproject->getProjectDir()->absoluteFilePath(QString(uproject->getName() + ".umx.fmap")) );
    if (!fmap.open(QIODevice::ReadOnly))
        throw (QString("Failed to open file"));

    QMap <uint32_t,IUasmFile*>fmapTable;

    QTextStream in(&fmap);
    QString fmapLine;
    QStringList fmapParseList;
    uint32_t fileId;
    IUasmFile *uasmFile;

    while (!(fmapLine = in.readLine()).isEmpty()) {

        fmapParseList = fmapLine.split(" ");

        //file Number
        fileId =  (uint32_t)fmapParseList.at(0).toInt();

        //file ponter
        uasmFile = uproject->getFileByAbsPath(fmapParseList.at(1));
        if (!uasmFile) throw (QString("File not in Project!"));

        //ad to temp map
        fmapTable.insert(fileId, uasmFile);
    }

    fmap.close();

    //now we read in binary data from debug and insert it into the overall table

    QFile debug( uproject->getProjectDir()->absoluteFilePath(QString(uproject->getName() + ".umx.debug")) );
    if (!debug.open(QIODevice::ReadOnly))
        throw (QString("Failed to open file"));

    //BIG-ENDIAN
    //<FILE-ID><LINE-NO><ADDRESS>
    uint32_t debugDatum[3];

    while (debug.read((char*)debugDatum, 12)) {
        //swap endianess to little
        uasmFile = fmapTable.value(swap_uint32(debugDatum[0]));
        uint32_t address = swap_uint32(debugDatum[2]);
        m_addressTable.insert(address, (new debugAddressEntry(uasmFile, swap_uint32(debugDatum[1]), address, NULL)));
    }

    debug.close();

}

bool UMachDebugInfo::setAdressForEntry(debugAddressEntry *entry, UMachSymbolInfo *symInfo)
{
    //search entry;
    //check if entry has label
    if (entry->label){
        entry->codeAddress = symInfo->getAddressByLabel(*entry->label);
        if (entry->codeAddress == 0)
            return false;
        entry->uasmFile = m_addressTable.value(entry->codeAddress)->uasmFile;
        entry->lineNumber = m_addressTable.value(entry->codeAddress)->lineNumber;
        return true;
    } else {
    //first we get a list, so that we can step trough without problems
        QList<debugAddressEntry*> entriesList = m_addressTable.values();
        for (int i = 0; i < entriesList.size(); i++) {
            if (entriesList[i]->uasmFile == entry->uasmFile
                    && entriesList[i]->lineNumber == entry->lineNumber) {
                entry->codeAddress = entriesList[i]->codeAddress;
                return true;
            }
        }
    }
    //nothing found :-(
    return false;
}

uint32_t UMachDebugInfo::swap_uint32( uint32_t val )
{
    val = ((val << 8) & 0xFF00FF00 ) | ((val >> 8) & 0xFF00FF );
    return (val << 16) | (val >> 16);
}

debugAddressEntry* UMachDebugInfo::getFileAndLineByAddress(uint32_t address)
{
    return m_addressTable.value(address, NULL);
}
