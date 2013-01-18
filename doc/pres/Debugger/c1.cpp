m_menu = new QMenu(this);
m_actionOpen = m_menu->addAction("Open");
connect (m_actionOpen, SIGNAL(triggered()),
	this, SLOT(openFile()));
