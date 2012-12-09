#ifndef UMACHOPTIONS_H
#define UMACHOPTIONS_H

#include <QMainWindow>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>
#include <QIntValidator>

class UMachOptions : public QMainWindow
{
    Q_OBJECT
public:
    explicit UMachOptions(QWidget *parent = 0);

    unsigned getMemorySize();
    
signals:
    
public slots:

private slots:
    void applyAndClose();
    void cancelAndClose();

private:
    QLineEdit   *m_memorySizeValue;
    QLabel      *m_memorySizeLabel;
    QPushButton *m_cancelSettings;
    QPushButton *m_applySettings;
    QIntValidator *m_intValidator;

    unsigned m_memorySize;

};

#endif // UMACHOPTIONS_H
