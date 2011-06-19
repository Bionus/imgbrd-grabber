#include "detailswindow.h"
#include "ui_detailswindow.h"
#include "functions.h"



/**
 * Constructor of the detailsWindow class, completing its window.
 * @param	tags		Image's tags (colored or not)
 */
detailsWindow::detailsWindow(QMap<QString,QString> details) : QWidget(0), ui(new Ui::detailsWindow)
{
	ui->setupUi(this);

	if (details.contains("tags"))			{ ui->labelTags->setText(details.value("tags"));		}
	if (details.contains("id"))				{ ui->labelId->setText(details.value("id"));			}
	if (details.contains("md5"))			{ ui->labelMd5->setText(details.value("md5"));			}
	if (details.contains("score"))			{ ui->labelScore->setText(details.value("score"));		}
	if (details.contains("created_at"))		{ ui->labelDate->setText(details.value("created_at"));	}
	if (details.contains("has_children"))	{ ui->labelChildren->setText(details.value("has_children") == "false" ? tr("non") : tr("oui"));	}
	if (details.contains("has_notes"))		{ ui->labelNotes->setText(details.value("has_notes") == "false" ? tr("non") : tr("oui"));		}
	if (details.contains("has_comments"))	{ ui->labelComments->setText(details.value("has_comments") == "false" ? tr("non") : tr("oui"));	}
	if (details.contains("parent_id"))		{ ui->labelParent->setText(details.value("parent_id").isEmpty() ? tr("non") : tr("oui (#%1)").arg(details.value("parent_id")));	}
	if (details.contains("source"))			{ ui->labelSource->setText("<a href=\""+details.value("source")+"\">"+details.value("source")+"</a>");				}
	if (details.contains("file_url"))		{ ui->labelUrl->setText("<a href=\""+details.value("file_url")+"\">"+details.value("file_url")+"</a>");				}
	if (details.contains("sample_url"))		{ ui->labelSample->setText("<a href=\""+details.value("sample_url")+"\">"+details.value("sample_url")+"</a>");		}
	if (details.contains("preview_url"))	{ ui->labelPreview->setText("<a href=\""+details.value("preview_url")+"\">"+details.value("preview_url")+"</a>");	}
	if (details.contains("author"))			{ ui->labelUser->setText(details.value("author")+(details.contains("creator_id") ? " (#"+details.value("creator_id")+")" : ""));	}
	if (details.contains("rating"))
	{
		QMap<QString, QString> assoc;
			assoc["s"] = tr("Safe");
			assoc["q"] = tr("Questionable");
			assoc["e"] = tr("Explicit");
		ui->labelRating->setText(assoc.value(details.value("rating")));
	}
	if (details.contains("file_size"))
	{
		int size = details.value("file_size").toInt();
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
	if (details.contains("height") && details.contains("width"))
	{ ui->labelSize->setText(details.value("width")+"x"+details.value("height")); }

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
