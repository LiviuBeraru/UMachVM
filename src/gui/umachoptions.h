#ifndef UMACHOPTIONS_H
#define UMACHOPTIONS_H

#include <QMainWindow>
#include <QLineEdit>
#include <QLabel>
#include <QBoxLayout>

class UMachOptions : public QMainWindow
{
    Q_OBJECT
public:
    explicit UMachOptions(QWidget *parent = 0);
    
signals:
    
public slots:

private:
    QLineEdit   *m_memorySizeValue;
    QLabel      *m_memorySizeLabel;

};

#endif // UMACHOPTIONS_H
