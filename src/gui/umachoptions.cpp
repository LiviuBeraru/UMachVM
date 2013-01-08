#include "umachoptions.h"
#include <QWidget>
#include <QGridLayout>
#include <QIntValidator>
#include <QMessageBox>

UMachOptions::UMachOptions(QWidget *parent) :
    QMainWindow(parent)
{
    m_memorySizeLabel = new QLabel("Memory Size:", this);
    m_memorySizeValue = new QLineEdit(this);
    m_memorySizeValue->setAlignment(Qt::AlignRight);

    QGridLayout *mainLayout = new QGridLayout(this);

    mainLayout->addWidget(m_memorySizeLabel, 0, 0);
    mainLayout->addWidget(m_memorySizeValue, 0, 1);

    m_cancelSettings = new QPushButton("Cancel", this);
    m_cancelSettings->setFixedWidth(120);
    connect(m_cancelSettings, SIGNAL(clicked()), this, SLOT(cancelAndClose()));

    m_applySettings = new QPushButton("Apply && Close", this);
    m_applySettings->setFixedWidth(120);
    connect(m_applySettings, SIGNAL(clicked()), this, SLOT(applyAndClose()));

    QHBoxLayout *buttonLayout = new QHBoxLayout(this);
    buttonLayout->addStretch();
    buttonLayout->addWidget(m_cancelSettings);
    buttonLayout->addWidget(m_applySettings);

    mainLayout->addLayout(buttonLayout,1,1);

    QWidget *mainWidget = new QWidget(this);
    mainWidget->setLayout(mainLayout);

    this->setCentralWidget(mainWidget);

    m_memorySize = 2048;
    m_memorySizeValue->setText(QString::number(m_memorySize));
    m_intValidator = new QIntValidator(2048, (256 * 1024 * 1024), this);
}


void UMachOptions::applyAndClose()
{
    int pos = 0;
    QString value = m_memorySizeValue->text();
    if (m_intValidator->validate(value, pos) != QValidator::Acceptable) {
        QMessageBox::warning(this, "Invalid Memory Size Value", "Memory size must be a value between 2048 and 268435456(256MB)");
        return;
    }
    m_memorySize = m_memorySizeValue->text().toUInt();
    this->hide();
}

void UMachOptions::cancelAndClose()
{
    m_memorySizeValue->setText(QString::number(m_memorySize));
    this->hide();
}

unsigned UMachOptions::getMemorySize()
{
    return m_memorySize;
}
