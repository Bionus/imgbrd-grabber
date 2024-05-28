#include "viewer/details-window.h"
#include <QLabel>
#include <ui_details-window.h>
#include "helpers.h"
#include "models/image.h"
#include "models/profile.h"


DetailsWindow::DetailsWindow(Profile *profile, QWidget *parent)
	: QDialog(parent), ui(new Ui::DetailsWindow), m_profile(profile)
{
	ui->setupUi(this);
	setupDialogShortcuts(this, m_profile->getSettings());
}

DetailsWindow::~DetailsWindow()
{
	delete ui;
}

void DetailsWindow::setImage(const QSharedPointer<Image> &image)
{
	clearLayout(ui->formLayout);

	for (const QPair<QString, QString> &row : image->detailsData()) {
		if (row.first.isEmpty() && row.second.isEmpty()) {
			ui->formLayout->addItem(new QSpacerItem(10, 10));
		} else {
			auto *label = new QLabel(QString("<b>%1</b>").arg(row.first), this);
			auto *field = new QLabel(row.second, this);
			field->setWordWrap(true);
			field->setOpenExternalLinks(true);
			field->setTextInteractionFlags(Qt::TextBrowserInteraction);
			field->setSizePolicy(QSizePolicy::Ignored, field->sizePolicy().horizontalPolicy());
			field->setMinimumWidth(200);
			ui->formLayout->addRow(label, field);
		}
	}

	update();
	resize(500, 0);
}
