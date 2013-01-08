#include "umachgui.h"
#include "umachbreakpoints.h"
#include "umachregisters.h"
#include "umachcodeeditor.h"
#include "umachsymbolinfo.h"
#include "umachdebuginfo.h"
#include "IUasmFile.h"
#include "project.h"
#include <iostream>
#include <QVariant>
#include "assert.h"
#include <QMessageBox>
#include <string>
#include <QProcess>
#include <QFlags>
#include "umachmake.h"
#include "umachsharedmemory.h"
#include "../gui_core/sharedmemorystructs.h"
#include "umachoptions.h"

UMachGui::UMachGui(QWidget *parent) :
    QMainWindow(parent)
{
    //Main Menu
    exitAction = new QAction("Exit", this);
    openProjectAction = new QAction("Open Project", this);
    saveProjectAction = new QAction("Save Project", this);
    closeProjectAction = new QAction("Close Project", this);
    newProjectAction = new QAction("New Project", this);
    addExsistingFileAction = new QAction("Add Exsisting Assembler File", this);
    addNewFileAction = new QAction("Add New Assembler File", this);
    connect(exitAction, SIGNAL(triggered()), this, SLOT(quit()));
    connect(openProjectAction, SIGNAL(triggered()), this, SLOT(openProject()));
    connect(saveProjectAction, SIGNAL(triggered()), this, SLOT(saveProject()));
    connect(closeProjectAction, SIGNAL(triggered()), this, SLOT(closeProject()));
    connect(newProjectAction, SIGNAL(triggered()), this, SLOT(newProject()));
    connect(addExsistingFileAction, SIGNAL(triggered()), this, SLOT(addExsistingAssemblerFile()));
    connect(addNewFileAction, SIGNAL(triggered()), this, SLOT(addNewAssemblerFile()));
    guiMenu = menuBar()->addMenu("UMachGui");
    guiMenu->addAction(newProjectAction);
    guiMenu->addAction(openProjectAction);
    guiMenu->addAction(saveProjectAction);
    guiMenu->addAction(closeProjectAction);
    guiMenu->addSeparator();
    guiMenu->addAction(addNewFileAction);
    guiMenu->addAction(addExsistingFileAction);
    guiMenu->addSeparator();
    guiMenu->addAction(exitAction);

    runExecutableAction = new QAction("Build and Run", this);
    connect(runExecutableAction, SIGNAL(triggered()), this, SLOT(buildAndRun()));
    debugMenu = menuBar()->addMenu("Debug");
    debugMenu->addAction(runExecutableAction);
    debugMenu->setDisabled(true);

    //when there is no Project, we have to disable some Actions
    saveProjectAction->setDisabled(true);
    closeProjectAction->setDisabled(true);
    addExsistingFileAction->setDisabled(true);
    addNewFileAction->setDisabled(true);

    //Registers Menu
    showRegistersWindowAction = new QAction("Registers", this);
    showBreakPointsWindowAction = new QAction("Break Points", this);
    m_showOptionsWindow = new QAction("Options", this);
    m_optionsWindow = new UMachOptions(this);


    //Sub Windows
    registersWindow = new UMachRegisters(this);
    breakPointsWindow = new UMachBreakPoints(this);

    connect(showRegistersWindowAction, SIGNAL(triggered()), registersWindow, SLOT(show()));
    connect(showBreakPointsWindowAction, SIGNAL(triggered()), breakPointsWindow, SLOT(show()));
    connect(m_showOptionsWindow, SIGNAL(triggered()), m_optionsWindow, SLOT(show()));
    windowsMenu = menuBar()->addMenu("Windows");
    windowsMenu->addAction(showRegistersWindowAction);
    windowsMenu->addAction(showBreakPointsWindowAction);
    windowsMenu->addAction(m_showOptionsWindow);
    showBreakPointsWindowAction->setDisabled(true);

     //Toolbar
    toolBar = addToolBar("Step Control");
    runAction = toolBar->addAction(QIcon(":/toolbar/run.png"),"");
    nextBreakPointAction = toolBar->addAction(QIcon(":/toolbar/next_bp.png"),"");
    nextStepAction = toolBar->addAction(QIcon(":/toolbar/next_step.png"),"");

    runAction->setToolTip("Start Debugging");
    connect(runAction, SIGNAL(triggered()), this, SLOT(buildAndRun()));
    nextBreakPointAction->setToolTip("Run till next break point");
    nextStepAction->setToolTip("Step to next instruction");

    //this actions are disabled at start
    runAction->setDisabled(true);
    nextBreakPointAction->setDisabled(true);
    nextStepAction->setDisabled(true);
    //conect next step to core
    connect(nextStepAction, SIGNAL(triggered()), this, SLOT(nextStep()));
    connect(nextBreakPointAction, SIGNAL(triggered()), this, SLOT(nextBreakPoint()));


    //FileLis
    m_fileListDock = new QDockWidget("Project Files", this);
    m_fileList = new QListWidget(this);
    m_fileList->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(m_fileList, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showRemoveFileContextMenu(QPoint)));
    m_removeFileAction = new QAction("Remove Selected Assembler File", this);
    m_removeFileAction->setEnabled(false);
    connect(m_removeFileAction, SIGNAL(triggered()), this, SLOT(removeFileFromProject()));
    m_fileListDock->setWidget(m_fileList);
    m_fileListDock->setFeatures(QDockWidget::NoDockWidgetFeatures);
    addDockWidget(Qt::LeftDockWidgetArea, m_fileListDock);
    m_fileListContextMenu = new QMenu();
    m_fileListContextMenu->addAction(m_removeFileAction);
    m_fileListContextMenu->addAction(addExsistingFileAction);
    m_fileListContextMenu->addAction(addNewFileAction);

    //connect File list to open code
    connect(m_fileList, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(openCodeInTabByWidget(QListWidgetItem*)));

    //WatchList
    m_watchList = new QDockWidget("Symbols", this);
    m_watchListTable = new QTableWidget(this);
    m_watchListTable->setColumnCount(3);
    m_watchListTable->setHorizontalHeaderLabels(QStringList(QString("Label;Type;Value").split(";")));
    m_watchListTable->horizontalHeader()->setResizeMode(2, QHeaderView::Stretch);
    m_watchList->setWidget(m_watchListTable);
    m_watchList->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
    addDockWidget(Qt::BottomDockWidgetArea, m_watchList);

    //CodeEditTab
    m_codeEditorTab = new QTabWidget();
    m_codeEditorTab->setTabsClosable(true);

    //connect tab close
    connect(m_codeEditorTab, SIGNAL(tabCloseRequested(int)), this, SLOT(closeCodeInTab(int)));

    setCentralWidget(m_codeEditorTab);


    m_project = NULL;
    m_stoppedAtBreakPoint = false;
    m_symbolDataChanged = false;

    setWindowTitle("UMachGui");
    setWindowIcon(QIcon(":/application/app_icon.png"));
    this->resize(800,494);

    m_sharedMemory = new UmachSharedMemory();

}

UMachGui::~UMachGui()
{
    delete(registersWindow);
    delete(breakPointsWindow);

}

void UMachGui::quit()
{
    if (!closeProject())
        return;
    exit(1);
}

void UMachGui::openProject()
{
    if (!closeProject())
        return;

    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), "",
        tr("UMachProject (*.umproject)"));

    m_project = Project::Open(fileName);
    connect(m_project, SIGNAL(openCodeTab(IUasmFile*)), this, SLOT(openCodeInTabByFile(IUasmFile*)));
    QVector<IUasmFile*> files = m_project->getFiles();

    IUasmFilePointer listItemData;
    QVariant qv;


    for (int i = 0; i < files.size(); i++)
    {

        listItemData.m_filePointer = files[i];
        QListWidgetItem *listItem = new QListWidgetItem();
        qv.setValue(listItemData);

        listItem->setText(files[i]->getRelativePath());
        listItem->setData(Qt::UserRole,qv);

        m_fileList->addItem(listItem);

    }


    breakPointsWindow->setFilesList(m_project->getFiles());
    breakPointsWindow->setProject(m_project);

    //now we have a Project, so we can toggle some actions
    setActionsOpenProject(true);
    setWindowTitle("UMachGui - " + fileName);

}

void UMachGui::saveProject()
{
    assert(m_project);
    m_project->save();
    //reset editor tab titles
    for (int i = 0; i < m_tabFileList.size(); i++) {
        if (m_tabFileList[i].unsavedMarker) {
            m_codeEditorTab->setTabText(m_tabFileList[i].tabIndex, m_tabFileList[i].file->getRelativePath());
        }

    }
}

bool UMachGui::closeProject()
{
    if (!m_project)
        return true;

    if (m_project->checkIsSaved()) {
        closeProjectGuiCleanup();
    } else {
        //Ask if close with save
        QMessageBox saveOrCloseMessage;
        saveOrCloseMessage.setWindowTitle("UMachGui Warning");
        saveOrCloseMessage.setText("Save Project before closing?");
        saveOrCloseMessage.addButton(QMessageBox::Cancel); //add cancel button
        saveOrCloseMessage.addButton(QMessageBox::No); //add apply button
        saveOrCloseMessage.addButton(QMessageBox::Yes); //add ok button

        int status = saveOrCloseMessage.exec();
        switch (status) {
            case QMessageBox::Yes:
                m_project->save();
                closeProjectGuiCleanup();
                break;

            case QMessageBox::No:
                closeProjectGuiCleanup();
                break;

            case QMessageBox::Cancel:
                return false;
         }
    }
    return true;

}

void UMachGui::newProject()
{
    if (m_project && !closeProject())
        return;

    QString projectFile = QFileDialog::getSaveFileName(this, "New Project File", "", "UMachProject (*.umproject)");

    {
        QFileInfo projectFileInfo(projectFile);
        if (projectFileInfo.suffix().isEmpty()) {
            projectFile += ".umproject";
        }
     }

    QFileInfo projectFileInfo(projectFile);

    if (projectFileInfo.exists()) {
        QMessageBox warning(QMessageBox::Warning, "UMachGui Warning", "File already exsist!", QMessageBox::Ok);
        warning.exec();
        return;
    }

    if (projectFileInfo.suffix() != "umproject") {
        QMessageBox warning(QMessageBox::Warning, "UMachGui Warning", "Non falid file suffix!", QMessageBox::Ok);
        warning.exec();
        return;
    }

    m_project = Project::New(projectFile);

    setActionsOpenProject(true);
    setWindowTitle("UMachGui - " + projectFile);
}

void UMachGui::addNewAssemblerFile()
{
    assert(m_project);

   QString newFile = QFileDialog::getSaveFileName(this, "New Umach Assembler File", "", "UMachProject (*.uasm)");
   {
       QFileInfo fileInfo(newFile);
       if (fileInfo.suffix().isEmpty()) {
           newFile += ".uasm";
       }
    }

   QFileInfo fileInfo(newFile);

   if (fileInfo.exists()) {
       QMessageBox warning(QMessageBox::Warning, "UMachGui Warning", "File already exsist! Please use \"Add Exsisting File\" instead!", QMessageBox::Ok);
       warning.exec();
       return;
   }

   if (fileInfo.suffix() != "uasm") {
       QMessageBox warning(QMessageBox::Warning, "UMachGui Warning", "Non falid file suffix!", QMessageBox::Ok);
       warning.exec();
       return;
   }

   IUasmFile *file = m_project->addFile(newFile);
   if (file) {
       addFileListEntry(file);
   } else {
       QMessageBox warning(QMessageBox::Warning, "UMachGui Warning", "File already in project!", QMessageBox::Ok);
       warning.exec();
   }

   breakPointsWindow->setFilesList(m_project->getFiles());

}

void UMachGui::addExsistingAssemblerFile()
{
    assert(m_project);

    QString fileName = QFileDialog::getOpenFileName(this, tr("Add Exsisting UMach Assembler File..."), "",
        tr("UMachAssemblerFile (*.uasm)"));

    IUasmFile *file = m_project->addFile(fileName);
    if (file) {
        addFileListEntry(file);
    } else {
        QMessageBox warning(QMessageBox::Warning, "UMachGui Warning", "File already in project!", QMessageBox::Ok);
        warning.exec();
    }

    breakPointsWindow->setFilesList(m_project->getFiles());
}

void UMachGui::setActionsOpenProject(bool mode)
{
    saveProjectAction->setEnabled(mode);
    closeProjectAction->setEnabled(mode);
    addExsistingFileAction->setEnabled(mode);
    addNewFileAction->setEnabled(mode);
    runAction->setEnabled(mode);
    showBreakPointsWindowAction->setEnabled(mode);
    debugMenu->setEnabled(mode);
    m_removeFileAction->setEnabled(mode);

}

void UMachGui::addFileListEntry(IUasmFile *fileToAdd)
{
    IUasmFilePointer listItemData;
    listItemData.m_filePointer = fileToAdd;
    QListWidgetItem *listItem = new QListWidgetItem();
    QVariant qv;
    qv.setValue(listItemData);
    listItem->setText(fileToAdd->getRelativePath());
    listItem->setData(Qt::UserRole,qv);
    m_fileList->addItem(listItem);
}

void UMachGui::openCodeInTabByWidget(QListWidgetItem *item)
{
    IUasmFilePointer fp = item->data(Qt::UserRole).value<IUasmFilePointer>();
    if (fp.m_filePointer->isOpen()) {
        //if allready open, only set focus
        for (int i = 0; i < m_tabFileList.size(); i++) {
            if (m_tabFileList[i].file == fp.m_filePointer) {
                m_codeEditorTab->setCurrentIndex(m_tabFileList[i].tabIndex);
                return;
            }
        }
    }
    TabIndexFile entry;
    entry.file  = fp.m_filePointer;
    UMachCodeEditor *codeEditor = new UMachCodeEditor(m_codeEditorTab);
    connect(codeEditor, SIGNAL(changeTitleUnsaved()), this, SLOT(codeInCurrentTabChanged()));
    entry.file->setEditor(codeEditor);
    entry.tabIndex = m_codeEditorTab->addTab(codeEditor, entry.file->getRelativePath());
    entry.unsavedMarker = false;
    entry.file->open();
    m_tabFileList.append(entry);
    m_codeEditorTab->setCurrentIndex(entry.tabIndex);
}

void UMachGui::openCodeInTabByFile(IUasmFile *file)
{
    if (file->isOpen()) {
        //if allready open, only set focus
        for (int i = 0; i < m_tabFileList.size(); i++) {
            if (m_tabFileList[i].file == file) {
                m_codeEditorTab->setCurrentIndex(m_tabFileList[i].tabIndex);
                return;
            }
        }
    }
    TabIndexFile entry;
    entry.file  = file;
    UMachCodeEditor *codeEditor = new UMachCodeEditor(m_codeEditorTab);
    connect(codeEditor, SIGNAL(changeTitleUnsaved()), this, SLOT(codeInCurrentTabChanged()));
    entry.file->setEditor(codeEditor);
    entry.tabIndex = m_codeEditorTab->addTab(codeEditor, entry.file->getRelativePath());
    entry.unsavedMarker = false;
    entry.file->open();
    m_tabFileList.append(entry);
    m_codeEditorTab->setCurrentIndex(entry.tabIndex);
}

void UMachGui::closeCodeInTab(int tabIndex)
{
   //get Entry & list index

   int listIndex = 0;
   for (int i = 0; i < m_tabFileList.size(); i++) {
       if (m_tabFileList[i].tabIndex == tabIndex)
           listIndex = i;
   }

  TabIndexFile entry = m_tabFileList[listIndex];

  //check if not saved
  if (entry.file->contentChanged()) {

      QMessageBox askIfSave;
      askIfSave.setWindowTitle("UMachGui Warning");
      askIfSave.setText("Save " + entry.file->getRelativePath() + " before closing?");
      askIfSave.addButton(QMessageBox::Cancel); //add cancel button
      askIfSave.addButton(QMessageBox::No); //add apply button
      askIfSave.addButton(QMessageBox::Yes); //add ok button

      int status = askIfSave.exec();
      switch (status) {
          case QMessageBox::Yes:
              entry.file->save();
              entry.file->close();
              m_codeEditorTab->removeTab(entry.tabIndex);
              m_tabFileList.removeAt(listIndex);
              //rearrange tab index
              for (int i = 0; i < m_tabFileList.size(); i++) {
                  if (m_tabFileList[i].tabIndex > tabIndex)
                      m_tabFileList[i].tabIndex--;
              }
              return;

          case QMessageBox::No:
              entry.file->close();
              m_codeEditorTab->removeTab(entry.tabIndex);
              m_tabFileList.removeAt(listIndex);
              //rearrange tab index
              for (int i = 0; i < m_tabFileList.size(); i++) {
                  if (m_tabFileList[i].tabIndex > tabIndex)
                      m_tabFileList[i].tabIndex--;
              }
              return;

          case QMessageBox::Cancel:
              return;
       }
  }

  entry.file->close();
  m_codeEditorTab->removeTab(entry.tabIndex);
  m_tabFileList.removeAt(listIndex);
  //rearrange tab index
  for (int i = 0; i < m_tabFileList.size(); i++) {
      if (m_tabFileList[i].tabIndex > tabIndex)
          m_tabFileList[i].tabIndex--;
  }

}

void UMachGui::runExecutable()
{
    executionGuiSetup(true);

    QString param(m_project->getProjectDir()->absolutePath() + "/" + m_project->getName() + ".umx -d");
    param.append(" -m " + QString::number(m_optionsWindow->getMemorySize()));

    QSystemSemaphore *processStarted = new QSystemSemaphore(KEY_DONE_RUN, 0, QSystemSemaphore::Create);
    m_umachCore = new QProcess();
    m_umachCore->start(QString("xterm -e ./UMachCore " + param));
    processStarted->acquire();
    delete(processStarted);
    //init shared mem
    m_sharedMemory->init();

    //doing the first fetch
    m_sharedMemory->m_waitForFetchRequest->release();
    m_sharedMemory->m_waitForFetchDone->acquire();
    //initial Memory copy should be done by now, so we can set up watchlist
    setWatchList();
    //call nextBreakPoint() for the start
    this->nextBreakPoint();
}

void UMachGui::nextStep()
{
    m_sharedMemory->m_sharedHeaderMemory->attach(QSharedMemory::ReadOnly);

    runAction->setDisabled(true);
    //we now that we are before an execution, so we do request one
    //run execution
    m_sharedMemory->m_waitForExecuteReqest->release();
    m_sharedMemory->m_waitForExecuteDone->acquire();

    m_stoppedAtBreakPoint = false;


    m_sharedMemory->m_sharedHeaderMemory->lock();
    //check if still running after that
    if (!m_sharedMemory->m_sharedHeaderData->m_runFlag) {
        m_sharedMemory->m_sharedHeaderMemory->unlock();
        m_sharedMemory->m_sharedHeaderMemory->detach();
        executionGuiSetup(false);
        return;
        }
    m_sharedMemory->m_sharedHeaderMemory->unlock();

    //now we do the next step to be at the right pos in cycle
    m_sharedMemory->m_waitForFetchRequest->release();
    m_sharedMemory->m_waitForFetchDone->acquire();

    m_sharedMemory->m_sharedHeaderMemory->lock();
    //we also need to set line focus
    uint32_t address = m_sharedMemory->m_sharedHeaderData->m_registers[32];
        //set focus
        if (m_project->isBreakPoint(address)) {
             m_project->setBreakPointFocus(address);
        } else {
            m_project->setFocusByAdress(address);
        }
    m_sharedMemory->m_sharedHeaderMemory->unlock();

    setRegisters();
    m_sharedMemory->m_sharedHeaderMemory->detach();
    runAction->setEnabled(true);
}

void UMachGui::setRegisters()
{
    m_sharedMemory->m_sharedHeaderMemory->lock();
    for (int i = 0; i < 32; i++) {
        registersWindow->setRegisterValue(i +1, m_sharedMemory->m_sharedHeaderData->m_registers[i]);
    }
    m_sharedMemory->m_sharedHeaderMemory->unlock();
}

void UMachGui::setWatchList()
{
    //clear old list
    for (int i = 0; i < m_watchListTable->rowCount(); i++) {
        m_watchListTable->takeItem(i,0);
        m_watchListTable->takeItem(i,1);
        m_watchListTable->takeItem(i,3);
        m_watchListTable->removeRow(i);
    }

    m_sharedMemory->m_sharedMachineMemory->attach();
    UMachSymbolInfo *symInfo = m_project->getSymbolInfo();
    m_sharedMemory->m_sharedMachineMemory->lock();
    void *data = m_sharedMemory->m_sharedMachineMemory->data();

    QStringList intLabels = symInfo->getIntegerLabels();
    QStringList strLabels = symInfo->getStringLabels();

    m_watchListTable->setRowCount(intLabels.size() + strLabels.size());

    int row = 0;
    for (int i = 0; i < intLabels.size(); i++){

        QTableWidgetItem *labelItem = new QTableWidgetItem(intLabels[i]);
        labelItem->setFlags(labelItem->flags()&~ Qt::ItemIsEditable &~ Qt::ItemIsSelectable);
        m_watchListTable->setItem(row, 0, labelItem);

        QTableWidgetItem *typeItem = new QTableWidgetItem("Integer");
        typeItem->setFlags(labelItem->flags()&~ Qt::ItemIsEditable &~ Qt::ItemIsSelectable);
        m_watchListTable->setItem(row, 1, typeItem);

        int *val_data = (int*)((uint8_t*)data + symInfo->getAddressByLabel(intLabels[i]));
        int value = UMachDebugInfo::swap_uint32(*val_data);
        m_watchListTable->setItem(row, 2, new QTableWidgetItem(QString::number(value, 10)));
        row++;
    }
    for (int i = 0; i < strLabels.size(); i++){

        QTableWidgetItem *labelItem = new QTableWidgetItem(strLabels[i]);
        labelItem->setFlags(labelItem->flags()&~ Qt::ItemIsEditable &~ Qt::ItemIsSelectable);
        m_watchListTable->setItem(row, 0, labelItem);

        QTableWidgetItem *typeItem = new QTableWidgetItem("String");
        typeItem->setFlags(labelItem->flags()&~ Qt::ItemIsEditable &~ Qt::ItemIsSelectable);
        m_watchListTable->setItem(row, 1, typeItem);

        char* val_data = (char*)((uint8_t*)data + symInfo->getAddressByLabel(strLabels[i]));
        QString value(val_data);
        //set correct datum length
        symInfo->setDatumLengthByLabel(strLabels[i], strlen(val_data));
        m_watchListTable->setItem(row, 2, new QTableWidgetItem(value));
        row++;
    }

    m_sharedMemory->m_sharedMachineMemory->unlock();
    m_sharedMemory->m_sharedMachineMemory->detach();

    //now we can connect to only get user changes
    connect(m_watchListTable, SIGNAL(itemChanged(QTableWidgetItem*)), this, SLOT(overrideMachineData(QTableWidgetItem*)));

}

void UMachGui::stopExecutable()
{
     //we set the run flag to false
    m_sharedMemory->m_sharedHeaderMemory->attach(QSharedMemory::ReadWrite);
    m_sharedMemory->m_sharedHeaderMemory->lock();
    m_sharedMemory->m_sharedHeaderData->m_runFlag = false;
    m_sharedMemory->m_sharedHeaderMemory->unlock();
    m_sharedMemory->m_sharedHeaderMemory->detach();

     //run execution
     m_sharedMemory->m_waitForExecuteReqest->release();

     executionGuiSetup(false);

}

void UMachGui::buildAndRun()
{
    if (!UMachMake::makeUMXfromProject(m_project))
        return;
    this->runExecutable();
}

void UMachGui::nextBreakPoint()
{
    m_sharedMemory->m_sharedHeaderMemory->attach(QSharedMemory::ReadOnly);

    runAction->setDisabled(true);
    nextBreakPointAction->setDisabled(true);

    while (true) {

       //check if breakpoint
       m_sharedMemory->m_sharedHeaderMemory->lock();
       uint32_t address = m_sharedMemory->m_sharedHeaderData->m_registers[32];
       //if breakpoint, we stop and wait before execution
       if (m_project->isBreakPoint(address) && !m_stoppedAtBreakPoint) {
               m_project->setBreakPointFocus(address);
               nextBreakPointAction->setEnabled(true);
               setRegisters();
               m_stoppedAtBreakPoint = true;
               break;
       }
       m_sharedMemory->m_sharedHeaderMemory->unlock();

       //not a break point? execute next
       m_sharedMemory->m_waitForExecuteReqest->release();
       m_sharedMemory->m_waitForExecuteDone->acquire();

       m_stoppedAtBreakPoint = false;

        //check run flag

       m_sharedMemory->m_sharedHeaderMemory->lock();
             if (m_sharedMemory->m_sharedHeaderData->m_runFlag == false) {
                 m_sharedMemory->m_sharedHeaderMemory->unlock();
                 m_sharedMemory->m_sharedHeaderMemory->detach();
                 executionGuiSetup(false);
                 return;
             }
        m_sharedMemory->m_sharedHeaderMemory->unlock();

        //now we can do the next fetch
        m_sharedMemory->m_waitForFetchRequest->release();
        m_sharedMemory->m_waitForFetchDone->acquire();

    }

    setRegisters();
    runAction->setEnabled(true);
    nextBreakPointAction->setEnabled(true);

    m_sharedMemory->m_sharedHeaderMemory->detach();

}

void UMachGui::codeInCurrentTabChanged()
{
    int idx = m_codeEditorTab->currentIndex();
    int listIndex = -1;
    int i = -1;

    while (listIndex == -1) {
        listIndex = (m_tabFileList[++i].tabIndex == idx) ? m_tabFileList[i].tabIndex : -1;
    }

    if(!m_tabFileList[listIndex].unsavedMarker) {
        m_codeEditorTab->setTabText(idx, QString("*" + m_codeEditorTab->tabText(idx)));
        m_tabFileList[listIndex].unsavedMarker = true;
    }
}

void UMachGui::showRemoveFileContextMenu(const QPoint &pos)
{
    QPoint gloablPos = m_fileList->mapToGlobal(pos);
    m_fileListContextMenu->exec(gloablPos);
}

void UMachGui::removeFileFromProject()
{
    //get selected file from List
    IUasmFilePointer fp = m_fileList->currentItem()->data(Qt::UserRole).value<IUasmFilePointer>();
    //check, if opened
    if (fp.m_filePointer->isOpen()) {
        QMessageBox warning(QMessageBox::Warning, "UMachGui Warning", "Please close file before removing from project!", QMessageBox::Ok);
        warning.exec();
        return;
    }

    //from project
    m_project->removeFileFromProject(fp.m_filePointer);

    //from gui
    //breakpointstable
    breakPointsWindow->removeBreakPoints(fp.m_filePointer);

    //from list
    delete (m_fileList->takeItem(m_fileList->currentRow()));

    //project unsaved
    m_project->setIsNotSaved();
}

//start = true, end = false
void UMachGui::executionGuiSetup(bool state)
{
    //menus
    debugMenu->setDisabled(state);
    guiMenu->setDisabled(state);
    //context Menu
    m_removeFileAction->setDisabled(state);
    addNewFileAction->setDisabled(state);
    addExsistingFileAction->setDisabled(state);

    //set code editable
    m_project->setCodeEditable(!state);

    //menubar
    nextBreakPointAction->setEnabled(state);
    nextStepAction->setEnabled(state);
    //exectution end
    if (!state) {
        //cange stop to run action
        runAction->disconnect(this);
        runAction->setIcon(QIcon(":/toolbar/run.png"));
        connect(runAction, SIGNAL(triggered()), this, SLOT(buildAndRun()));
        runAction->setEnabled(true);
        //remove debug info
        m_project->removeDebugInfo();
        //disconect watch table change
        disconnect(m_watchListTable, SIGNAL(itemChanged(QTableWidgetItem*)), this, SLOT(overrideMachineData(QTableWidgetItem*)));
    } else {
        runAction->disconnect(this);
        runAction->setIcon(QIcon(":/toolbar/stop.png"));
        connect(runAction, SIGNAL(triggered()), this, SLOT(stopExecutable()));
        runAction->setEnabled(true);
    }
}

void UMachGui::closeProjectGuiCleanup()
{
    disconnect(m_project, SIGNAL(openCodeTab(IUasmFile*)), this, SLOT(openCodeInTabByFile(IUasmFile*)));
    delete(m_project);
    m_project = NULL;
    m_fileList->clear();
    breakPointsWindow->clearTable();
    breakPointsWindow->hide();
    registersWindow->hide();
    m_codeEditorTab->clear();
    m_tabFileList.clear();
    setActionsOpenProject(false);
    setWindowTitle("UMachGui");
}

void UMachGui::overrideMachineData(QTableWidgetItem *itemChanged)
{
    //get the symbol Info
    UMachSymbolInfo *symInfo = m_project->getSymbolInfo();
    //first get label
    QString label = m_watchListTable->item(itemChanged->row(),0)->text();
    //next we want symbol
    symbolElement *symbol = symInfo->getSymbolByLabel(label);
    //now we can overwrite the data

    m_sharedMemory->m_sharedMachineMemory->attach();
    m_sharedMemory->m_sharedMachineMemory->lock();
    //calculate data pos
    void *data = m_sharedMemory->m_sharedMachineMemory->data();

    if (symbol->type == SYM_INT) {
        int value = UMachDebugInfo::swap_uint32(itemChanged->text().toInt());
        int *val_data = (int*)((uint8_t*)data + symbol->address);
        *val_data = value;
    } else if (symbol->type == SYM_STR) {
        //data as byte array
        QByteArray value = itemChanged->text().toAscii();
        void *val_data = (uint8_t*)data + symbol->address;
        //first we set original data to zero
        memset(val_data, 0, (symbol->datumLength + 1));
        //smaller or bigger? 0 char at end so +1
        if (value.size() > (int)(symbol->datumLength + 1)) {
            memcpy(val_data, value.data(), symbol->datumLength);
        } else {
            memcpy(val_data, value.data(), value.size());
        }
    }

    m_sharedMemory->m_sharedMachineMemory->unlock();
    m_sharedMemory->m_sharedMachineMemory->detach();

    //now we mus set up the flag for the machine to copy the changes

    bool attached = m_sharedMemory->m_sharedHeaderMemory->isAttached();
    if (!attached) {
        m_sharedMemory->m_sharedHeaderMemory->attach();
    }

    m_sharedMemory->m_sharedHeaderMemory->lock();
    m_sharedMemory->m_sharedHeaderData->m_copyMemory = true;
    m_sharedMemory->m_sharedHeaderMemory->unlock();

    if (!attached) {
        m_sharedMemory->m_sharedHeaderMemory->detach();
    }
}
