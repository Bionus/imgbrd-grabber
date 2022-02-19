#ifndef SOURCE_REGISTRY_WINDOW_H
#define SOURCE_REGISTRY_WINDOW_H

#include <QDialog>


namespace Ui
{
	class SourceRegistryWindow;
}


class SourceRegistry;

class SourceRegistryWindow : public QDialog
{
	Q_OBJECT

	public:
		explicit SourceRegistryWindow(SourceRegistry *sourceRegistry, QWidget *parent = nullptr);
		~SourceRegistryWindow() override;

	private:
		Ui::SourceRegistryWindow *ui;
		SourceRegistry *m_sourceRegistry;
};

#endif // SOURCE_REGISTRY_WINDOW_H
