connect (m_project,
        SIGNAL(requestOpenFileInTab(IFile)),
        this,
        SLOT(openFileInTab(IFile)));
