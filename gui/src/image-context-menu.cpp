#include "image-context-menu.h"
#include <QDesktopServices>
#include <QSignalMapper>
#include "main-window.h"
#include "models/image.h"
#include "reverse-search/reverse-search-engine.h"
#include "reverse-search/reverse-search-loader.h"


ImageContextMenu::ImageContextMenu(QSettings *settings, QSharedPointer<Image> img, MainWindow *mw, QWidget *parent)
	: QMenu(parent), m_settings(settings), m_image(std::move(img)), m_mainWindow(mw)
{
	// Load reverse search engines
	ReverseSearchLoader loader(m_settings);
	m_reverseSearchEngines = loader.getAllReverseSearchEngines();

	// Open image in browser
	addAction(QIcon(":/images/icons/browser.png"), tr("Open in browser"), this, SLOT(openInBrowser()));

	// Reverse search actions
	QMenu *reverseSearchMenu = addMenu(QIcon(":/images/icons/globe.png"), tr("Web services"));
	auto *reverseSearchMapper = new QSignalMapper(this);
	connect(reverseSearchMapper, SIGNAL(mapped(int)), this, SLOT(reverseImageSearch(int)));
	for (int i = 0; i < m_reverseSearchEngines.count(); ++i)
	{
		ReverseSearchEngine engine = m_reverseSearchEngines[i];
		QAction *subMenuAct = reverseSearchMenu->addAction(engine.icon(), engine.name());
		connect(subMenuAct, SIGNAL(triggered()), reverseSearchMapper, SLOT(map()));
		reverseSearchMapper->setMapping(subMenuAct, i);
	}

	// MD5 search
	addAction(QIcon(":/images/icons/hash.png"), tr("Search MD5"), this, SLOT(searchMd5()));
}


void ImageContextMenu::openInBrowser()
{
	QDesktopServices::openUrl(m_image->pageUrl());
}

void ImageContextMenu::searchMd5()
{
	m_mainWindow->addTab("md5:" + m_image->md5());
}

void ImageContextMenu::reverseImageSearch(int i)
{
	if (m_reverseSearchEngines.count() < i)
		return;

	m_reverseSearchEngines[i].searchByUrl(m_image->fileUrl());
}
