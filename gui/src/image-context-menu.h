#ifndef IMAGE_CONTEXT_MENU_H
#define IMAGE_CONTEXT_MENU_H

#include <QMenu>
#include "mainwindow.h"
#include "models/image.h"
#include "reverse-search/reverse-search-engine.h"


class ImageContextMenu : public QMenu
{
	Q_OBJECT

	public:
		ImageContextMenu(QSettings *settings, QSharedPointer<Image> img, mainWindow *mw, QWidget *parent = Q_NULLPTR);

	protected slots:
		void openInBrowser();
		void searchMd5();
		void reverseImageSearch(int i);

	private:
		QSettings *m_settings;
		QSharedPointer<Image> m_image;
		mainWindow *m_mainWindow;
		QList<ReverseSearchEngine> m_reverseSearchEngines;
};

#endif // IMAGE_CONTEXT_MENU_H
