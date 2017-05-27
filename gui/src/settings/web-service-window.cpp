#include "web-service-window.h"
#include "ui_web-service-window.h"


WebServiceWindow::WebServiceWindow(int index, const ReverseSearchEngine *webService, QWidget *parent)
	: QDialog(parent), ui(new Ui::WebServiceWindow), m_index(index)
{
	ui->setupUi(this);

	if (webService != nullptr)
	{
		ui->lineName->setText(webService->name());
		ui->lineUrl->setText(webService->tpl());
	}

	connect(this, SIGNAL(accepted()), this, SLOT(save()));
}

WebServiceWindow::~WebServiceWindow()
{
	delete ui;
}

void WebServiceWindow::save()
{
	emit validated(m_index, ReverseSearchEngine(QIcon(), ui->lineName->text(), ui->lineUrl->text()));
}
