#include "umachregisters.h"

UMachRegisters::UMachRegisters(QWidget *parent) :
    QMainWindow(parent)
{
    this->setWindowTitle("UMachGui - Registers");
    this->setFixedWidth(710);

    //set up toolbar
    m_toolbar = addToolBar("Show Registers");
    m_toolbar->setFloatable(false);

    m_registersSelector = new QComboBox();
    m_toolbar->addWidget(m_registersSelector);

    m_showRegisterButton = new QPushButton("+", m_toolbar);
    m_showRegisterButton->setFixedWidth(m_showRegisterButton->height());
    connect(m_showRegisterButton, SIGNAL(clicked()), this, SLOT(watchRegister()));
    m_toolbar->addWidget(m_showRegisterButton);
    m_registerTable = NULL;
    m_hideButtons = NULL;
    buildTable();
}

void UMachRegisters::watchRegister()
{
    int index = m_registersSelector->itemData(m_registersSelector->currentIndex()).value<int>();
    m_registerTable->showRow(index);
    m_isWatched[index] = true;
    m_registersSelector->removeItem(m_registersSelector->currentIndex());
}

void UMachRegisters::unwatchRegister(int index)
{
    m_registerTable->hideRow(index);
    m_isWatched[index] = false;
    m_registersSelector->addItem(QString("R").append(QString::number(index + 1)), QVariant(index));
}

void UMachRegisters::setRegisterValue(int index, int32_t value)
{

    QTableWidgetItem *value_bin = m_registerTable->item(index - 1, 0);
    if (!value_bin) {
        value_bin = new QTableWidgetItem(QString("%1").arg(value, 32, 2, QChar('0')));
        value_bin->setTextAlignment(Qt::AlignRight);
        m_registerTable->setItem(index - 1, 0, value_bin);
    } else {
        value_bin->setText(QString("%1").arg(value, 32, 2, QChar('0')));
    }

    QTableWidgetItem *value_hex = m_registerTable->item(index - 1, 1);
    if (!value_hex) {
        value_hex = new QTableWidgetItem(QString("%1").arg(value, 8, 16, QChar('0')).toUpper());
        value_hex->setTextAlignment(Qt::AlignRight);
        m_registerTable->setItem(index - 1, 1, value_hex);
    } else {
        value_hex->setText(QString("%1").arg(value, 8, 16, QChar('0')).toUpper());
    }

    QTableWidgetItem *value_sdez = m_registerTable->item(index - 1, 2);
    if (!value_sdez) {
        value_sdez = new QTableWidgetItem(QString::number((unsigned int32_t)value));
        value_sdez->setTextAlignment(Qt::AlignRight);
        m_registerTable->setItem(index - 1, 2, value_sdez);
    } else {
        value_sdez->setText(QString::number((unsigned int32_t)value));
    }

    QTableWidgetItem *value_udez = m_registerTable->item(index - 1, 3);
    if (!value_udez) {
        value_udez = new QTableWidgetItem(QString::number(value));
        value_udez->setTextAlignment(Qt::AlignRight);
        m_registerTable->setItem(index - 1, 3, value_udez);
    } else {
        value_udez->setText(QString::number(value));
    }

}

bool UMachRegisters::isWatched(int index)
{
    return m_isWatched[index - 1];
}

void UMachRegisters::clearTable()
{
    //is watched array
    memset(m_isWatched, false, sizeof(bool)*32);
    //table items
    buildTable();
}

void UMachRegisters::buildTable()
{
    delete(m_registerTable);
    delete(m_hideButtons);

    m_registerTable = new QTableWidget;

    //set up Tabel
    m_registerTable->setColumnCount(5);
    m_registerTable->setRowCount(32);

    m_registerTable->setSortingEnabled(false);
    m_registerTable->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

    //set rows
    m_hideButtons = new QButtonGroup(m_registerTable);
    QHeaderView *verticHeaders = m_registerTable->verticalHeader();
    verticHeaders->setFixedWidth(50);
    QStringList labelList;
    for (int i = 0; i < 32; i++) {
        verticHeaders->setDefaultAlignment(Qt::AlignRight);
        verticHeaders->setResizeMode(QHeaderView::Fixed);
        m_registerTable->setRowHeight(i, 25);
        m_registerTable->hideRow(i);
        labelList.push_back(QString("R").append(QString::number(i + 1)));
        QPushButton *hideButton = new QPushButton("-",m_registerTable);
        m_registerTable->setCellWidget(i,4,hideButton);
        m_hideButtons->addButton(hideButton, i);
        m_registersSelector->addItem(QString("R").append(QString::number(i + 1)), QVariant(i));
    }
    connect(m_hideButtons, SIGNAL(buttonClicked(int)), this, SLOT(unwatchRegister(int)));

    m_registerTable->setVerticalHeaderLabels(labelList);


    m_registerTable->setHorizontalHeaderLabels(QString("Binary;Hexadecimal;Unsigned;Signed;").split(";"));


    m_registerTable->setColumnWidth(0,320);
    m_registerTable->setColumnWidth(1,100);
    m_registerTable->setColumnWidth(2,100);
    m_registerTable->setColumnWidth(3,100);
    m_registerTable->setColumnWidth(4,25);

    QHeaderView *horizontalTableHeader = m_registerTable->horizontalHeader();
    horizontalTableHeader->setFixedHeight(25);
    horizontalTableHeader->setResizeMode(0, QHeaderView::Fixed);
    horizontalTableHeader->setResizeMode(1, QHeaderView::Fixed);
    horizontalTableHeader->setResizeMode(2, QHeaderView::Fixed);
    horizontalTableHeader->setResizeMode(3, QHeaderView::Fixed);
    horizontalTableHeader->setResizeMode(4, QHeaderView::Fixed);

    m_registerTable->setContentsMargins(10,10,10,10);
    setCentralWidget(m_registerTable);

}
