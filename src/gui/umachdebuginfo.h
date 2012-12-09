#ifndef UMACHDEBUGINFO_H
#define UMACHDEBUGINFO_H

#include <inttypes.h>
#include <QList>
#include <QHash>
#include <QString>

class IUasmFile;
class Project;
class UMachSymbolInfo;

struct debugAddressEntry {

    IUasmFile   *uasmFile;
    uint32_t    lineNumber;
    uint32_t    codeAddress;
    QString     *label;

    debugAddressEntry(IUasmFile *file, uint32_t lineNr, uint32_t address, QString *label)
        : uasmFile(file), lineNumber(lineNr), codeAddress(address), label(label) {}
    ~debugAddressEntry()
    {
        delete (label);
    }
};

//struct debugSymbolEntry {

//};

class UMachDebugInfo
{
public:
    UMachDebugInfo(Project *uproject);
    bool setAdressForEntry(debugAddressEntry *entry, UMachSymbolInfo *symInfo);
    debugAddressEntry* getFileAndLineByAddress(uint32_t address);

    static uint32_t swap_uint32( uint32_t val );

private:
    //Hashlist holding all the debug info
    QHash <uint32_t, debugAddressEntry*> m_addressTable;


};

#endif // UMACHDEBUGINFO_H
