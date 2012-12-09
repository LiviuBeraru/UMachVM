#ifndef UMACHGUI_H
#define UMACHGUI_H

#include <QMainWindow>
#include <QtGui>

class UMachRegisters;
class UMachBreakPoints;
class UMachCodeEditor;
class UMachOptions;
class IUasmFile;
class Project;
class UmachSharedMemory;

class UMachGui : public QMainWindow
{
    Q_OBJECT
    
public:
   explicit UMachGui(QWidget *parent = 0);
    ~UMachGui();

private slots:
    void quit();
    void openProject();
    void newProject();
    void saveProject();
    bool closeProject();
    void addNewAssemblerFile();
    void addExsistingAssemblerFile();
    void openCodeInTabByWidget(QListWidgetItem *item);
    void openCodeInTabByFile(IUasmFile *file);
    void closeCodeInTab(int tabIndex);

    void codeInCurrentTabChanged();

    void nextStep();
    void nextBreakPoint();
    void stopExecutable();

    //true = start, false = end;
    void executionGuiSetup(bool state);
    void buildAndRun();

    void closeProjectGuiCleanup();

    void showRemoveFileContextMenu(const QPoint& pos);
    void removeFileFromProject();
    void overrideMachineData(QTableWidgetItem *itemChanged);
private:

    //Function to toggle aviable actions depending if project is open or not;
    //true: open project
    //false: no project open;
    void setActionsOpenProject(bool mode);
    void addFileListEntry(IUasmFile *fileToAdd);
    void setRegisters();
    void setWatchList();
    void runExecutable();
    //override to open by file interface

    //SubWindows
    UMachRegisters *registersWindow;
    UMachBreakPoints *breakPointsWindow;
    UMachOptions *m_optionsWindow;

    QAction *showRegistersWindowAction;
    QAction *showBreakPointsWindowAction;
    QAction *exitAction;
    QAction *openProjectAction;
    QAction *newProjectAction;
    QAction *saveProjectAction;
    QAction *closeProjectAction;
    QAction *m_showOptionsWindow;

    QAction *addNewFileAction;
    QAction *addExsistingFileAction;

    QAction *runExecutableAction;

    QDockWidget *m_fileListDock;
    QListWidget *m_fileList;
    QMenu *m_fileListContextMenu;
    QAction *m_removeFileAction;

    struct TabIndexFile {
        int tabIndex;
        IUasmFile *file;
        bool unsavedMarker;
    };

    QList<TabIndexFile> m_tabFileList;

    QDockWidget *m_watchList;
    QTableWidget *m_watchListTable;

    QTabWidget *m_codeEditorTab;

    QMenu *windowsMenu;
    QMenu *debugMenu;
    QMenu *guiMenu;

    QToolBar *toolBar;
    QAction *nextStepAction;
    QAction *nextBreakPointAction;
    QAction *runAction;

    Project *m_project;

    QProcess *m_umachCore;

    UmachSharedMemory *m_sharedMemory;

    bool m_stoppedAtBreakPoint;

    bool m_symbolDataChanged;


};

#endif // UMACHGUI_H
