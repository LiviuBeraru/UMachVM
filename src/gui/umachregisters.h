#ifndef UMACHREGISTERS_H
#define UMACHREGISTERS_H

#include <QMainWindow>
#include <QtGui>
#include <QMap>

#define REGISTER_COUNT 32
#define REGISTER_LAYOUT_DIVIDE 8

class UMachRegisters : public QMainWindow
{
    Q_OBJECT
public:
    explicit UMachRegisters(QWidget *parent = 0);
    void setRegisterValue(int index, int32_t value);
    bool isWatched(int index);
    void clearTable();
    
signals:
    
public slots:
    void watchRegister();
    void unwatchRegister(int index);

private:
    QTableWidget *m_registerTable;
    QButtonGroup *m_hideButtons;

    QToolBar *m_toolbar;
    QComboBox *m_registersSelector;
    QAction *m_showRegister;
    QPushButton *m_showRegisterButton;

    bool m_isWatched[32];

    void buildTable();

};

#endif // UMACHREGISTERS_H
