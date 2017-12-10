#ifndef WEB_SERVICE_WINDOW_H
#define WEB_SERVICE_WINDOW_H

#include <QDialog>
#include <QNetworkReply>
#include "custom-network-access-manager.h"
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
		~WebServiceWindow() override;

	protected slots:
		void getFavicon();
		void faviconReceived();
		void save();

	signals:
		void validated(const ReverseSearchEngine &webService, const QByteArray &favicon);

	private:
		Ui::WebServiceWindow *ui;
		const ReverseSearchEngine *m_webService;
		QNetworkReply *m_faviconReply;
		CustomNetworkAccessManager *m_networkAccessManager;
};

#endif // WEB_SERVICE_WINDOW_H
