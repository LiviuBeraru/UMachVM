#include "umachbreakpoints.h"
#include "IUasmFile.h"
#include "project.h"
#include "assert.h"
#include "QTableView"
#include <QtGui>

UMachBreakPoints::UMachBreakPoints(QWidget *parent) :
    QMainWindow(parent)
{
    this->setWindowTitle("UMachGui - Break Points");
    setContentsMargins(10,10,10,10);


    m_breakpointTable = new QTableWidget(this);
    m_breakpointTable->setColumnCount(3);
    m_breakpointTable->setRowCount(1);
    m_breakpointTable->setColumnWidth(2,25);
    m_breakpointTable->setSortingEnabled(false);
    m_breakpointTable->setHorizontalHeaderLabels(QString("Line;File;").split(";"));
    QHeaderView *horizontalTableHeader = m_breakpointTable->horizontalHeader();
    //horizontalTableHeader->setResizeMode(0, QHeaderView::Fixed);
    horizontalTableHeader->setResizeMode(1, QHeaderView::Stretch);
    horizontalTableHeader->setResizeMode(2, QHeaderView::Fixed);
    horizontalTableHeader->setFixedHeight(25);

    m_verticalTableHeader = m_breakpointTable->verticalHeader();
    m_verticalTableHeader->setResizeMode(QHeaderView::Fixed);
    m_verticalTableHeader->setFixedHeight(25);
    m_verticalTableHeader->hide();

    m_breakpointTable->setRowHeight(0,25);

    m_addBreakPointButton = new QPushButton("+",m_breakpointTable);
    connect(m_addBreakPointButton, SIGNAL(clicked()), this, SLOT(addBreakPoint()));
    m_breakpointTable->setCellWidget(0,2,m_addBreakPointButton);

    m_filesSelectList = new QComboBox(m_breakpointTable);
    m_breakpointTable->setCellWidget(0,1,m_filesSelectList);

    m_lineNumber = new QLineEdit(m_breakpointTable);
    m_lineNumber->setAlignment(Qt::AlignRight);
    m_breakpointTable->setCellWidget(0,0,m_lineNumber);

    m_lastRowIndex = 0;

    this->setCentralWidget(m_breakpointTable);

    setFixedHeight(270);

    m_removeRowButtons = new QButtonGroup(this);
    connect(m_removeRowButtons, SIGNAL(buttonClicked(int)), this, SLOT(removeBreakPoint(int)));

    m_project = NULL;

    m_intValidator = new QIntValidator(1, 1024*1024*256, this);
}

void UMachBreakPoints::addBreakPoint()
{
    //add breakpoint to project
    QVariant listItem = m_filesSelectList->itemData(m_filesSelectList->currentIndex(), Qt::UserRole);
    IUasmFilePointer filePointer = listItem.value<IUasmFilePointer>();

    assert(m_project);

    //if int add as lineNR
    QString *line = new QString(m_lineNumber->text());
    int pos = 0;

    if (m_intValidator->validate(*line, pos) == QIntValidator::Acceptable) {
        if (!m_project->addBreakPoint(filePointer.m_filePointer, line->toInt(), 0, NULL)) {
            //TODO: messagebox
            return;
        }
    } else {
        if (!m_project->addBreakPoint(NULL, 0, 0, line)) {
            //TODO: messagebox
            return;
        }
    }

    //add a new empty row before last row
    if (m_lastRowIndex >= 7)
        setFixedHeight(this->height() + 25);
    m_breakpointTable->insertRow(m_lastRowIndex);
    m_breakpointTable->setRowHeight(m_lastRowIndex, 25);

    //add line numver
    QTableWidgetItem *lnr = new QTableWidgetItem(m_lineNumber->text());
    lnr->setFlags(lnr->flags()&~ Qt::ItemIsEditable &~ Qt::ItemIsSelectable);
    m_breakpointTable->setItem(m_lastRowIndex,0,lnr);

    //add file
    QTableWidgetItem *tableItem = new QTableWidgetItem(filePointer.m_filePointer->getRelativePath());
    tableItem->setData(Qt::UserRole, listItem);
    tableItem->setFlags(lnr->flags()&~ Qt::ItemIsEditable &~ Qt::ItemIsSelectable);
    m_breakpointTable->setItem(m_lastRowIndex,1,tableItem);

    //add remove button
    m_removeRowButtons->addButton(new QPushButton("-"), m_lastRowIndex);
    m_breakpointTable->setCellWidget(m_lastRowIndex, 2, m_removeRowButtons->button(m_lastRowIndex));

    m_lastRowIndex++;

}

void UMachBreakPoints::removeBreakPoint(int row)
{
    assert(m_project);

    //remove breakpoint from project
    QVariant tableItem = m_breakpointTable->item(row,1)->data(Qt::UserRole);
    IUasmFile *file = tableItem.value<IUasmFilePointer>().m_filePointer;
    int line = m_breakpointTable->itemAt(row,1)->text().toInt();
    m_project->removeBreakPoint(file, line);

    //remove row and then button
    m_removeRowButtons->removeButton(m_removeRowButtons->button(row));
    m_breakpointTable->removeRow(row);
    //reduce last Row index
    m_lastRowIndex--;

    //reduce the ids of the following buttons by one to be syncronized with row number
    for (int i = row; i < m_lastRowIndex; i++) {
        m_removeRowButtons->setId(m_removeRowButtons->button(i + 1), i);
    }

    if (this->height() > 270)
        this->setFixedHeight(this->height() - 25);
}

void UMachBreakPoints::setFilesList(QVector<IUasmFile*> fileVector)
{
    m_filesSelectList->clear();

    for (int i = 0; i < fileVector.size(); i++) {
        IUasmFilePointer listItemData;
        listItemData.m_filePointer = fileVector.at(i);
        QVariant qv;
        qv.setValue(listItemData);
        m_filesSelectList->addItem(listItemData.m_filePointer->getRelativePath(), qv);
    }
}

void UMachBreakPoints::clearTable()
{
    for (int i = 0; i < m_lastRowIndex; i++) {
        m_breakpointTable->removeRow(0);
        m_removeRowButtons->removeButton(m_removeRowButtons->button(i));
    }
    m_filesSelectList->clear();
    m_lineNumber->clear();
    m_lastRowIndex = 0;
}

void UMachBreakPoints::setProject(Project *project)
{
    m_project = project;
}

void UMachBreakPoints::removeBreakPoints(IUasmFile *file)
{
    for (int i = 0; i < (m_breakpointTable->rowCount() - 1); i++) {
        QVariant tableItem = m_breakpointTable->item(i,1)->data(Qt::UserRole);
        IUasmFile *tableFile = tableItem.value<IUasmFilePointer>().m_filePointer;
        if (tableFile == file)
            removeBreakPoint(i);
    }
}
