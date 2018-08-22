#ifndef RENAME_EXISTING_1_H
#define RENAME_EXISTING_1_H

#include <QDialog>
#include <QMap>
#include "models/filename.h"
#include "rename-existing-file.h"


namespace Ui
{
	class RenameExisting1;
}


class Site;
class Page;

class RenameExisting1 : public QDialog
{
	Q_OBJECT

	public:
		explicit RenameExisting1(Profile *profile, QWidget *parent = nullptr);
		~RenameExisting1() override;

	private slots:
		void getAll(Page *p);
		void getTags();
		void loadNext();
		void on_buttonCancel_clicked();
		void on_buttonContinue_clicked();

	private:
		void setImageResult(Image *img);

	private:
		Ui::RenameExisting1 *ui;
		Profile *m_profile;
		QMap<QString, Site*> m_sites;
		Filename m_filename;
		int m_needDetails;
		QList<RenameExistingFile> m_details;
		QMap<QString, RenameExistingFile> m_getAll;
};

#endif // RENAME_EXISTING_1_H
