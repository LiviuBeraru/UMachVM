#ifndef UMACHSYMBOLINFO_H
#define UMACHSYMBOLINFO_H

#include <QString>
#include <QStringList>
#include <QMap>


enum symbolType {
    SYM_UNKNOWN = 0,
    SYM_JMP,
    SYM_INT,
    SYM_STR
};

struct symbolElement{
    unsigned address;
    symbolType type;
    size_t datumLength;
};

class Project;

class UMachSymbolInfo
{
private:

    QMap<QString,symbolElement*> m_symbols;

    QStringList m_integerLabels;
    QStringList m_stringLabels;

public:
    UMachSymbolInfo(Project *uproject);
    ~UMachSymbolInfo();

    //get functions
    QStringList getIntegerLabels();
    QStringList getStringLabels();

    //Functions returning zero if invalid label
    symbolElement* getSymbolByLabel(QString label);

    //set
    void setDatumLengthByLabel(QString label, size_t datumLength);
    unsigned getAddressByLabel(QString label);

};

#endif // UMACHSYMBOLINFO_H
