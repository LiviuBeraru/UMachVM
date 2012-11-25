#ifndef ICODEEDITOR_H
#define ICODEEDITOR_H

#include <QString>
#include <inttypes.h>
#define YELLOW 1;
#define RED 2;

class ICodeEditor
{
public:
    virtual bool contentHasChanged() = 0;
    virtual void setSaved() = 0;
    virtual QString getContent() const = 0;
    virtual void setContent(const QString &text) = 0;
    virtual void setLineFocus(uint32_t lineNr, int color) = 0;
    virtual void setEditable(bool flag) = 0;
};


#endif // ICODEEDITOR_H

