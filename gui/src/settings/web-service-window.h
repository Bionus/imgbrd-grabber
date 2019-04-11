#ifndef WEB_SERVICE_WINDOW_H
#define WEB_SERVICE_WINDOW_H

#include <QDialog>


namespace Ui
{
	class WebServiceWindow;
}


class NetworkManager;
class NetworkReply;
class ReverseSearchEngine;

class WebServiceWindow : public QDialog
{
	Q_OBJECT

	public:
		explicit WebServiceWindow(const ReverseSearchEngine *webService, QWidget *parent = nullptr);
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
		NetworkReply *m_faviconReply;
		NetworkManager *m_networkAccessManager;
};

#endif // WEB_SERVICE_WINDOW_H
