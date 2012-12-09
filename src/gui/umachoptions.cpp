#include "umachoptions.h"
#include <QWidget>

UMachOptions::UMachOptions(QWidget *parent) :
    QMainWindow(parent)
{
    m_memorySizeLabel = new QLabel("Memory Size:", this);
    m_memorySizeValue = new QLineEdit(this);

    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    mainLayout->addWidget(m_memorySizeLabel);
    mainLayout->addWidget(m_memorySizeValue);

    QWidget *mainWidget = new QWidget(this);
    mainWidget->setLayout(mainLayout);

    this->setCentralWidget(mainWidget);
}
