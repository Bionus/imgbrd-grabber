#include "detailswindow.h"
#include "ui_detailswindow.h"
#include "functions.h"



/**
 * Constructor of the detailsWindow class, completing its window.
 * @param	tags		Image's tags (colored or not)
 */
detailsWindow::detailsWindow(Image *image) : QWidget(0), ui(new Ui::detailsWindow)
{
	ui->setupUi(this);

	if (!image->tags().isEmpty())
	{
		QString t;
		for (int i = 0; i < image->tags().count(); i++)
		{ t += " "+image->tags().at(i)->text(); }
		ui->labelTags->setText(t.trimmed());
	}
	if (image->id() != 0)				{ ui->labelId->setText(QString::number(image->id()));	}
	if (!image->md5().isEmpty())		{ ui->labelMd5->setText(image->md5());					}
	if (!image->rating().isEmpty())		{ ui->labelRating->setText(image->rating());			}
	if (image->createdAt().isValid())	{ ui->labelDate->setText(image->createdAt().toString(tr("le dd/MM/yyyy à hh:mm")));		}
	if (!image->source().isEmpty())		{ ui->labelSource->setText("<a href=\""+image->source()+"\">"+image->source()+"</a>");	}
	if (!image->pageUrl().isEmpty())	{ ui->labelPage->setText("<a href=\""+image->pageUrl().toString()+"\">"+image->pageUrl().toString()+"</a>");			}
	if (!image->fileUrl().isEmpty())	{ ui->labelUrl->setText("<a href=\""+image->fileUrl().toString()+"\">"+image->fileUrl().toString()+"</a>");				}
	if (!image->sampleUrl().isEmpty())	{ ui->labelSample->setText("<a href=\""+image->sampleUrl().toString()+"\">"+image->sampleUrl().toString()+"</a>");		}
	if (!image->previewUrl().isEmpty())	{ ui->labelPreview->setText("<a href=\""+image->previewUrl().toString()+"\">"+image->previewUrl().toString()+"</a>");	}
	if (!image->author().isEmpty())		{ ui->labelUser->setText(image->author()+(image->authorId() != 0 ? " (#"+QString::number(image->authorId())+")" : ""));	}
	if (image->fileSize() != 0)
	{
		int size = image->fileSize();
		QString unit = "o";
		if (size > 2048)
		{
			size /= 1024;
			unit = "ko";
			if (size > 2048)
			{
				size /= 1024;
				unit = "mo";
			}
		}
		ui->labelFilesize->setText(QString::number(size)+" "+unit);
	}
	if (!image->size().isEmpty())
	{ ui->labelSize->setText(QString::number(image->width())+"x"+QString::number(image->height())); }
	ui->labelScore->setText(QString::number(image->score()));
	ui->labelChildren->setText(image->hasChildren() ? tr("oui") : tr("non"));
	ui->labelNotes->setText(image->hasNote() ? tr("oui") : tr("non"));
	ui->labelComments->setText(image->hasComments() ? tr("oui") : tr("non"));
	ui->labelParent->setText(image->parentId() != 0 ? tr("oui (#%1)").arg(image->parentId() != 0) : tr("non"));

	QSettings settings(savePath("settings.ini"), QSettings::IniFormat);
	QFont fontArtists, fontCopyrights, fontCharacters, fontModels, fontGenerals;
		fontArtists.fromString(settings.value("Coloring/Fonts/artists").toString());
		fontCopyrights.fromString(settings.value("Coloring/Fonts/copyrights").toString());
		fontCharacters.fromString(settings.value("Coloring/Fonts/characters").toString());
		fontModels.fromString(settings.value("Coloring/Fonts/models").toString());
		fontGenerals.fromString(settings.value("Coloring/Fonts/generals").toString());
	ui->labelTags->setStyleSheet(
		"a				{ text-decoration:none; font-weight:normal;																		} "\
		".blacklisted	{ font-weight:bold; text-decoration:underline; color:#000000;													} "\
		".character		{ color:"+settings.value("Coloring/Colors/characters", "#00aa00").toString()+"; "+qfonttocss(fontCharacters)+"	} "\
		".copyright		{ color:"+settings.value("Coloring/Colors/copyrights", "#00aa00").toString()+"; "+qfonttocss(fontCharacters)+"	} "\
		".artist		{ color:"+settings.value("Coloring/Colors/artists", "#00aa00").toString()+"; "+qfonttocss(fontCharacters)+"		} "\
		".model			{ color:"+settings.value("Coloring/Colors/models", "#00aa00").toString()+"; "+qfonttocss(fontCharacters)+"		} "\
		".general		{ color:"+settings.value("Coloring/Colors/generals", "#00aa00").toString()+"; "+qfonttocss(fontCharacters)+"	} "
	);

	resize(sizeHint());
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
