#ifndef PAGE_INFORMATION_H
#define PAGE_INFORMATION_H

#include <QUrl>


struct PageInformation
{
	int page;

	qulonglong minId;
	qulonglong maxId;
	QString minDate;
	QString maxDate;

	QUrl previousPage;
	QUrl nextPage;
};

#endif // PAGE_INFORMATION_H
