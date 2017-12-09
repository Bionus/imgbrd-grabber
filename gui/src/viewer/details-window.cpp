#include "viewer/details-window.h"
#include <QLabel>
#include <ui_details-window.h>
#include "helpers.h"
#include "models/image.h"


DetailsWindow::DetailsWindow(Profile *profile, QWidget *parent)
	: QDialog(parent), ui(new Ui::DetailsWindow), m_profile(profile)
{
	ui->setupUi(this);
}

DetailsWindow::~DetailsWindow()
{
	delete ui;
}

void DetailsWindow::setImage(QSharedPointer<Image> image)
{
	clearLayout(ui->formLayout);

	for (QPair<QString, QString> row : image->detailsData())
	{
		if (row.first.isEmpty() && row.second.isEmpty())
		{
			ui->formLayout->addItem(new QSpacerItem(10, 10));
		}
		else
		{
			auto label = new QLabel(QString("<b>%1</b>").arg(row.first), this);
			auto field = new QLabel(row.second, this);
			field->setWordWrap(true);
			ui->formLayout->addRow(label, field);
		}
	}

	resize(QSize(400, 200));
}
