#ifndef BLACKLIST_FIX_2_H
#define BLACKLIST_FIX_2_H

#include <QDialog>
#include <QLabel>
#include "models/filtering/blacklist.h"


namespace Ui
{
	class BlacklistFix2;
}



class BlacklistFix2 : public QDialog
{
	Q_OBJECT

	public:
		explicit BlacklistFix2(QList<QMap<QString, QString>> details, Blacklist blacklist, QWidget *parent = nullptr);
		~BlacklistFix2() override;

	private slots:
		void loadThumbnails();
		void selectBlacklisted();
		void cancel();
		void nextStep();

	private:
		Ui::BlacklistFix2 *ui;
		QList<QMap<QString, QString>> m_details;
		QList<QLabel*> m_previews;
		Blacklist m_blacklist;
};

#endif // BLACKLIST_FIX_2_H
