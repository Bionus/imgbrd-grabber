#ifndef EMPTY_DIRS_FIX_1_H
#define EMPTY_DIRS_FIX_1_H

#include <QDialog>
#include <QDir>
#include <QStringList>


namespace Ui
{
	class EmptyDirsFix1;
}


class Profile;

class EmptyDirsFix1 : public QDialog
{
	Q_OBJECT

	public:
		explicit EmptyDirsFix1(Profile *profile, QWidget *parent = Q_NULLPTR);
		~EmptyDirsFix1() override;

	public slots:
		void next();

	private:
		QStringList mkList(const QDir &dir);
		bool isEmpty(const QDir &dir);

	private:
		Ui::EmptyDirsFix1 *ui;
};

#endif // EMPTY_DIRS_FIX_1_H
