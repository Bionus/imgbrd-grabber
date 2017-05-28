#ifndef WEB_SERVICE_WINDOW_H
#define WEB_SERVICE_WINDOW_H

#include <QDialog>
#include <QNetworkReply>
#include "reverse-search/reverse-search-engine.h"


namespace Ui
{
	class WebServiceWindow;
}


class WebServiceWindow : public QDialog
{
	Q_OBJECT
	
	public:
		explicit WebServiceWindow(int index, const ReverseSearchEngine *webService, QWidget *parent = Q_NULLPTR);
		~WebServiceWindow();

	protected slots:
		void getFavicon();
		void save();

	signals:
		void validated(int index, ReverseSearchEngine webService, QByteArray favicon);

	private:
		Ui::WebServiceWindow *ui;
		int m_index;
		QNetworkReply *m_faviconReply;
};

#endif // WEB_SERVICE_WINDOW_H
