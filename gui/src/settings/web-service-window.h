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
		explicit WebServiceWindow(const ReverseSearchEngine *webService, QWidget *parent = Q_NULLPTR);
		~WebServiceWindow();

	protected slots:
		void getFavicon();
		void save();

	signals:
		void validated(ReverseSearchEngine webService, QByteArray favicon);

	private:
		Ui::WebServiceWindow *ui;
		const ReverseSearchEngine *m_webService;
		QNetworkReply *m_faviconReply;
};

#endif // WEB_SERVICE_WINDOW_H
