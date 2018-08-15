#ifndef IMAGE_CONTEXT_MENU_H
#define IMAGE_CONTEXT_MENU_H

#include <QMenu>
#include "models/image.h"
#include "reverse-search/reverse-search-engine.h"


class QSettings;
class MainWindow;

class ImageContextMenu : public QMenu
{
	Q_OBJECT

	public:
		ImageContextMenu(QSettings *settings, QSharedPointer<Image> img, MainWindow *mw, QWidget *parent = nullptr);

	protected slots:
		void openInBrowser();
		void searchMd5();
		void reverseImageSearch(int i);

	private:
		QSettings *m_settings;
		QSharedPointer<Image> m_image;
		MainWindow *m_mainWindow;
		QList<ReverseSearchEngine> m_reverseSearchEngines;
};

#endif // IMAGE_CONTEXT_MENU_H
