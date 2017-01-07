#include <QSettings>
#include "detailswindow.h"
#include "ui_detailswindow.h"
#include "functions.h"



/**
 * Constructor of the detailsWindow class, completing its window.
 * @param	tags		Image's tags (colored or not)
 */
detailsWindow::detailsWindow(Profile *profile, QWidget *parent)
	: QDialog(parent), ui(new Ui::detailsWindow), m_profile(profile)
{
	ui->setupUi(this);

	resize(QSize(400, 431));
}

/**
 * Destructor of the detailsWindow class
 */
detailsWindow::~detailsWindow()
{
	delete ui;
}

/**
 * Setter for tags
 */
void detailsWindow::setTags(QString tags)
{
	ui->labelTags->setText(tags);
}

void detailsWindow::setImage(QSharedPointer<Image> image)
{
	setTags(image->stylishedTags(m_profile).join(' '));

	if (image->id() != 0)				{ ui->labelId->setText(QString::number(image->id()));	}
	if (!image->md5().isEmpty())		{ ui->labelMd5->setText(image->md5());					}
	if (!image->rating().isEmpty())		{ ui->labelRating->setText(image->rating());			}
	if (image->createdAt().isValid())	{ ui->labelDate->setText(image->createdAt().toString(tr("'the' MM/dd/yyyy 'at' hh:mm")));		}
	if (!image->source().isEmpty())		{ ui->labelSource->setText("<a href=\""+image->source()+"\">"+image->source()+"</a>");	}
	if (!image->pageUrl().isEmpty())	{ ui->labelPage->setText("<a href=\""+image->pageUrl().toString()+"\">"+image->pageUrl().toString()+"</a>");			}
	if (!image->fileUrl().isEmpty())	{ ui->labelUrl->setText("<a href=\""+image->fileUrl().toString()+"\">"+image->fileUrl().toString()+"</a>");				}
	if (!image->sampleUrl().isEmpty())	{ ui->labelSample->setText("<a href=\""+image->sampleUrl().toString()+"\">"+image->sampleUrl().toString()+"</a>");		}
	if (!image->previewUrl().isEmpty())	{ ui->labelPreview->setText("<a href=\""+image->previewUrl().toString()+"\">"+image->previewUrl().toString()+"</a>");	}
	if (!image->author().isEmpty())		{ ui->labelUser->setText(image->author()+(image->authorId() != 0 ? " (#"+QString::number(image->authorId())+")" : ""));	}
	if (image->fileSize() != 0)			{ ui->labelFilesize->setText(formatFilesize(image->fileSize()));														}
	if (!image->size().isEmpty())		{ ui->labelSize->setText(QString::number(image->width())+"x"+QString::number(image->height()));	}

	ui->labelScore->setText(QString::number(image->score()));
	ui->labelChildren->setText(image->hasChildren() ? tr("yes") : tr("no"));
	ui->labelNotes->setText(image->hasNote() ? tr("yes") : tr("no"));
	ui->labelComments->setText(image->hasComments() ? tr("yes") : tr("no"));
	ui->labelParent->setText(image->parentId() != 0 ? tr("yes (#%1)").arg(image->parentId() != 0) : tr("no"));
}
