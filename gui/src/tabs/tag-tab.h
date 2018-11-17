#ifndef TAG_TAB_H
#define TAG_TAB_H

#include <QJsonObject>
#include "tabs/search-tab.h"


namespace Ui
{
	class TagTab;
}


class MainWindow;
class TextEdit;

class TagTab : public SearchTab
{
	Q_OBJECT

	public:
		explicit TagTab(Profile *profile, MainWindow *parent);
		~TagTab() override;
		Ui::TagTab *ui;
		QString tags() const override;
		void write(QJsonObject &json) const override;
		bool read(const QJsonObject &json, bool preload = true);

	protected:
		void changeEvent(QEvent *event) override;

	public slots:
		// Zooms
		void setTags(const QString &tags, bool preload = true) override;
		// Loading
		void load() override;
		// Batch
		void getPage();
		void getAll();
		// Others
		void closeEvent(QCloseEvent *) override;
		void on_buttonSearch_clicked();
		void focusSearch() override;
		void updateTitle() override;

	private:
		TextEdit *m_search;
};

#endif // TAG_TAB_H
