#ifndef QOAUTH1_SETUP_H
#define QOAUTH1_SETUP_H

#include <QOAuth1>
#include <QObject>


class QNetworkAccessManager;

class QOAuth1Setup : public QOAuth1
{
	public:
		explicit QOAuth1Setup(QNetworkAccessManager *manager, QObject *parent = nullptr)
			: QOAuth1(manager, parent)
		{}

		void publicSetup(QNetworkRequest *request, const QVariantMap &signingParameters, QNetworkAccessManager::Operation operation)
		{
            setup(request, signingParameters, operation);
        }
};

#endif // QOAUTH1_SETUP_H
