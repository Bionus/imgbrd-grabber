#include "settings/source-registry-window.h"
#include <ui_source-registry-window.h>
#include "models/source-registry.h"


SourceRegistryWindow::SourceRegistryWindow(SourceRegistry *sourceRegistry, QWidget *parent)
	: QDialog(parent), ui(new Ui::SourceRegistryWindow), m_sourceRegistry(sourceRegistry)
{
	ui->setupUi(this);

	ui->labelName->setText(m_sourceRegistry->name());
	ui->labelJsonUrl->setText(QStringLiteral("<a href='%1'>%1</a>").arg(m_sourceRegistry->jsonUrl()));
	ui->labelHome->setText(QStringLiteral("<a href='%1'>%1</a>").arg(m_sourceRegistry->home()));
	ui->labelSources->setText(m_sourceRegistry->sources().keys().join(", "));
}

SourceRegistryWindow::~SourceRegistryWindow()
{
	delete ui;
}
