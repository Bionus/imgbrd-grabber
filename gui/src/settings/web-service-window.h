#ifndef WEB_SERVICE_WINDOW_H
#define WEB_SERVICE_WINDOW_H

#include <QDialog>
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
		void save();

	signals:
		void validated(int index, ReverseSearchEngine webService);

	private:
		Ui::WebServiceWindow *ui;
		int m_index;
};

#endif // WEB_SERVICE_WINDOW_H
