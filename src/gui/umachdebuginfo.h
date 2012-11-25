#ifndef UMACHDEBUGINFO_H
#define UMACHDEBUGINFO_H

#include <inttypes.h>
#include <QList>
#include <QHash>

class IUasmFile;
class Project;

struct debugAddressEntry {

    IUasmFile   *uasmFile;
    uint32_t    lineNumber;
    uint32_t    codeAddress;

    debugAddressEntry(IUasmFile *file, uint32_t lineNr, uint32_t address)
        : uasmFile(file), lineNumber(lineNr), codeAddress(address){}
};

//struct debugSymbolEntry {

//};

class UMachDebugInfo
{
public:
    UMachDebugInfo(Project *uproject);
    bool setAdressForEntry(debugAddressEntry *entry);
    debugAddressEntry* getFileAndLineByAddress(uint32_t address);

private:
    //Hashlist holding all the debug info
    QHash <uint32_t, debugAddressEntry*> m_addressTable;

    static uint32_t swap_uint32( uint32_t val );

};

#endif // UMACHDEBUGINFO_H
