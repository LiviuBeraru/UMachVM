#ifndef UMACHBREAKPOINTS_H
#define UMACHBREAKPOINTS_H

#include <QMainWindow>
#include <QtGui>
#include <QIntValidator>

class IUasmFile;
class Project;

class UMachBreakPoints : public QMainWindow
{
    Q_OBJECT
public:
    explicit UMachBreakPoints(QWidget *parent = 0);
    //set project file
    void setProject(Project *project);
    //remove this and replace by an update function,
    //witch also removes invalid breakpoints
    void setFilesList(QVector<IUasmFile*> filesVector);

    void clearTable();

    void removeBreakPoints(IUasmFile *file);

    
public slots:


private slots:
    void addBreakPoint();
    void removeBreakPoint(int row);

private:

    QTableWidget *m_breakpointTable;
    QPushButton *m_addBreakPointButton;
    QAction *m_addBreakPointAction;

    int m_lastRowIndex;
    QHeaderView *m_verticalTableHeader;

    QButtonGroup *m_removeRowButtons;
    QComboBox *m_filesSelectList;
    QLineEdit *m_lineNumber;

    Project *m_project;

    QIntValidator *m_intValidator;
    
};

#endif // UMACHBREAKPOINTS_H
