#include "updater/updater.h"
#include <QRegularExpression>
#include "custom-network-access-manager.h"


Updater::Updater()
	: m_networkAccessManager(new CustomNetworkAccessManager(this))
{}

Updater::~Updater()
{
	m_networkAccessManager->deleteLater();
}

int Updater::compareVersions(QString a, QString b)
{
	int aSub = 0;
	char aSubType = ' ';
	const int aPos = a.indexOf(QRegularExpression("[a-z]"));
	if (aPos != -1)
	{
		aSubType = a[aPos].toLatin1();
		aSub = a.midRef(aPos + 1).toInt();
		a = a.left(aPos);
	}

	int bSub = 0;
	char bSubType = ' ';
	const int bPos = b.indexOf(QRegularExpression("[a-z]"));
	if (bPos != -1)
	{
		bSubType = b[bPos].toLatin1();
		bSub = b.midRef(bPos + 1).toInt();
		b = b.left(bPos);
	}

	QStringList aSem = a.split('.');
	QStringList bSem = b.split('.');

	for (int i = 0; i < qMax(aSem.count(), bSem.count()); ++i)
	{
		const int aPart = i < aSem.count() ? aSem[i].toInt() : 0;
		const int bPart = i < bSem.count() ? bSem[i].toInt() : 0;

		if (aPart > bPart)
			return 1;
		if (aPart < bPart)
			return -1;
	}

	if (aSubType == ' ' && bSubType != ' ')
		return 1;
	if (aSubType != ' ' && bSubType == ' ')
		return -1;

	if (aSubType != ' ' && bSubType != ' ')
	{
		if (aSubType > bSubType)
			return 1;
		if (aSubType < bSubType)
			return -1;

		if (aSub > bSub)
			return 1;
		if (aSub < bSub)
			return -1;
	}

	return 0;
}
