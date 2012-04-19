#include "searchwindow.h"
#include "functions.h"
#include "ui_searchwindow.h"

SearchWindow::SearchWindow(QString tags, QDate server, QWidget *parent) : QDialog(parent), ui(new Ui::SearchWindow)
{
	ui->setupUi(this);

	QSettings *settings = new QSettings(savePath("settings.ini"), QSettings::IniFormat);
	m_calendar = new QCalendarWidget;
		m_calendar->setLocale(QLocale(settings->value("language", "English").toString().toLower().left(2)));
		m_calendar->setWindowIcon(QIcon(":/images/icon.ico"));
		m_calendar->setWindowTitle(tr("Grabber - Choisir une date"));
		m_calendar->setDateRange(QDate(2000, 1, 1), server);
		m_calendar->setSelectedDate(server);
		connect(m_calendar, SIGNAL(activated(QDate)), this, SLOT(setDate(QDate)));
		connect(m_calendar, SIGNAL(activated(QDate)), m_calendar, SLOT(close()));
	connect(ui->buttonCalendar, SIGNAL(clicked()), m_calendar, SLOT(show()));

	QStringList favs = loadFavorites().keys();
	m_tags = new TextEdit(favs, this);
		m_tags->setContextMenuPolicy(Qt::CustomContextMenu);
		QStringList completion;
			QFile words("words.txt");
			if (words.open(QIODevice::ReadOnly | QIODevice::Text))
			{
				while (!words.atEnd())
				{
					QByteArray line = words.readLine();
					completion.append(QString(line).remove("\r\n").remove("\n").split(" ", QString::SkipEmptyParts));
				}
				completion.append(favs);
				completion.removeDuplicates();
				completion.sort();
				QCompleter *completer = new QCompleter(completion, this);
					completer->setCaseSensitivity(Qt::CaseInsensitive);
				m_tags->setCompleter(completer);
			}
		connect(m_tags, SIGNAL(returnPressed()), this, SLOT(accept()));

	ui->formLayout->insertRow(0, tr("Tags"), m_tags);

	QStringList orders = QStringList() << "id" << "id_desc" << "score_asc" << "score" << "mpixels_asc" << "mpixels" << "filesize" << "landscape" << "portrait" << "favcount" << "rank";
	QStringList ratings = QStringList() << "rating:safe" << "-rating:safe" << "rating:questionable" << "-rating:questionable" << "rating:explicit" << "-rating:explicit";
	QStringList status = QStringList() << "deleted" << "active" << "flagged" << "pending" << "any";

	if (tags.contains("order:"))
	{
		QRegExp reg("order:([^ ]+)");
		reg.indexIn(tags);
		ui->comboOrder->setCurrentIndex(orders.indexOf(reg.cap(1))+1);
		tags.remove(reg.cap(0));
	}
	if (tags.contains("rating:"))
	{
		QRegExp reg("-?rating:[^ ]+");
		reg.indexIn(tags);
		ui->comboRating->setCurrentIndex(ratings.indexOf(reg.cap(0))+1);
		tags.remove(reg.cap(0));
	}
	if (tags.contains("status:"))
	{
		QRegExp reg("status:([^ ]+)");
		reg.indexIn(tags);
		ui->comboStatus->setCurrentIndex(status.indexOf(reg.cap(1))+1);
		tags.remove(reg.cap(0));
	}
	if (tags.contains("date:"))
	{
		QRegExp reg("date:([^ ]+)");
		reg.indexIn(tags);
		m_calendar->setSelectedDate(QDate::fromString(reg.cap(1), "MM/dd/yyyy"));
		ui->lineDate->setText(reg.cap(1));
		tags.remove(reg.cap(0));
	}

	m_tags->setText(tags);
}
SearchWindow::~SearchWindow()
{
	delete ui;
}

void SearchWindow::setDate(QDate d)
{
	ui->lineDate->setText(d.toString("MM/dd/yyyy"));
}

void SearchWindow::accept()
{
	QStringList orders = QStringList() << "id" << "id_desc" << "score_asc" << "score" << "mpixels_asc" << "mpixels" << "filesize" << "landscape" << "portrait" << "favcount" << "rank";
	QStringList ratings = QStringList() << "rating:safe" << "-rating:safe" << "rating:questionable" << "-rating:questionable" << "rating:explicit" << "-rating:explicit";
	QStringList status = QStringList() << "deleted" << "active" << "flagged" << "pending" << "any";

	emit accepted(QString(m_tags->toPlainText()+" "+(ui->comboStatus->currentIndex() != 0 ? "status:"+status.at(ui->comboStatus->currentIndex()-1) : "")+" "+(ui->comboOrder->currentIndex() != 0 ? "order:"+orders.at(ui->comboOrder->currentIndex()-1) : "")+" "+(ui->comboRating->currentIndex() != 0 ? ratings.at(ui->comboRating->currentIndex()-1) : "")+" "+(!ui->lineDate->text().isEmpty() ? "date:"+ui->lineDate->text() : "")).trimmed());
	QDialog::accept();
}

void SearchWindow::on_buttonImage_clicked()
{
	QStringList orders = QStringList() << "id" << "id_desc" << "score_asc" << "score" << "mpixels_asc" << "mpixels" << "filesize" << "landscape" << "portrait" << "favcount" << "rank";
	QStringList ratings = QStringList() << "rating:safe" << "-rating:safe" << "rating:questionable" << "-rating:questionable" << "rating:explicit" << "-rating:explicit";
	QStringList status = QStringList() << "deleted" << "active" << "flagged" << "pending" << "any";

	QSettings *settings = new QSettings(savePath("settings.ini"), QSettings::IniFormat);
	QString path = QFileDialog::getOpenFileName(this, tr("Chercher une image"), settings->value("Save/path").toString(), "Images (*.png *.gif *.jpg *.jpeg)");
	QFile f(path);
	QString md5 = "";
	if (f.exists())
	{
		f.open(QFile::ReadOnly);
		md5 = QCryptographicHash::hash(f.readAll(), QCryptographicHash::Md5).toHex();
	}
	emit accepted(QString((!md5.isEmpty() ? "md5:"+md5 : "")+" "+m_tags->toPlainText()+" "+(ui->comboStatus->currentIndex() != 0 ? "status:"+status.at(ui->comboStatus->currentIndex()-1) : "")+" "+(ui->comboOrder->currentIndex() != 0 ? "order:"+orders.at(ui->comboOrder->currentIndex()-1) : "")+" "+(ui->comboRating->currentIndex() != 0 ? ratings.at(ui->comboRating->currentIndex()-1) : "")+" "+(!ui->lineDate->text().isEmpty() ? "date:"+ui->lineDate->text() : "")).trimmed());
	QDialog::accept();
}
