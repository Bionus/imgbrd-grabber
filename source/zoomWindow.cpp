#include <string>
#include <QApplication>
#include <QtNetwork>
#include "functions.h"
#include "zoomWindow.h"
#include "QAffiche.h"

using namespace std;



zoomWindow::zoomWindow(QString m_program, QString site, QStringList regex, QString id, QString url, QString tags, QString md5, QString rating, QString score, QString user, mainWindow *parent) : regex(regex), timeout(300), loaded(0), oldsize(0), site(site), id(id), url(url), tags(tags), md5(md5), score(score), user(user), link(""), m_program(m_program)
{
	this->parent = parent;
	
	this->resize(600, 800);
	this->setWindowIcon(QIcon(":/images/icon.ico"));
	this->setWindowTitle("DB Viewer - "+tr("Image"));
	
	QFile file("favorites.txt");
	if (file.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		while (!file.atEnd())
		{ favorites.append(QString(file.readLine()).split(' ', QString::SkipEmptyParts)); }
		file.close();
	}

	QSettings settings("settings.ini", QSettings::IniFormat);
		settings.beginGroup("Zoom");
		this->setWindowState(Qt::WindowStates(settings.value("state", 0).toInt()));
		if (!this->isMaximized())
		{
			this->resize(settings.value("size", QSize(800, 600)).toSize());
			this->move(settings.value("pos", QPoint(200, 200)).toPoint());
		}
	
	QHBoxLayout *buttons = new QHBoxLayout();
		QPushButton *buttonSave = new QPushButton("");
			connect(buttonSave, SIGNAL(clicked()), this, SLOT(saveImage()));
			buttons->addWidget(buttonSave);
		QPushButton *buttonSaveNQuit = new QPushButton(tr("Enregistrer et fermer"));
			connect(buttonSaveNQuit, SIGNAL(clicked()), this, SLOT(saveNQuit()));
			buttons->addWidget(buttonSaveNQuit);
		QPushButton *buttonOpenSaveDir = new QPushButton(tr("Ouvrir le dossier de destination"));
			connect(buttonOpenSaveDir, SIGNAL(clicked()), this, SLOT(openSaveDir()));
			buttons->addWidget(buttonOpenSaveDir);
		QPushButton *buttonSaveas = new QPushButton(tr("Enregistrer sous..."));
			connect(buttonSaveas, SIGNAL(clicked()), this, SLOT(saveImageAs()));
			buttons->addWidget(buttonSaveas);
	QAffiche *labelImage = new QAffiche;
		labelImage->setSizePolicy(QSizePolicy(QSizePolicy::Ignored,QSizePolicy::Ignored));
		connect(labelImage, SIGNAL(doubleClicked()), this, SLOT(fullScreen()));
	QStringList taglist = tags.split(' ');
	QString hreftags;
	for (int i = 0; i < taglist.count(); i++)
	{ hreftags += " <a href=\""+taglist.at(i)+"\" style=\"text-decoration:none;color:#000000\">"+taglist.at(i)+"</a>"; }
	QLabel *labelTags = new QLabel(tr("<b>Tags :</b> %1").arg(hreftags.trimmed()));
		labelTags->setWordWrap(true);
		labelTags->setContextMenuPolicy(Qt::CustomContextMenu);
		connect(labelTags, SIGNAL(linkActivated(QString)), this, SLOT(openUrl(QString)));
		connect(labelTags, SIGNAL(linkHovered(QString)), this, SLOT(linkHovered(QString)));
		connect(labelTags, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(contextMenu()));
	QMap<QString, QString> assoc;
		assoc["s"] = tr("Safe");
		assoc["q"] = tr("Questionable");
		assoc["e"] = tr("Explicit");
	this->rating = assoc.value(rating);
	QLabel *labelRating = new QLabel(tr("<b>Classe :</b> %1").arg(this->rating));
	QLabel *labelScore = new QLabel(tr("<b>Score :</b> %1").arg(score));
	QLabel *labelUser = new QLabel(tr("<b>Posteur :</b> %1").arg(user));
	QVBoxLayout *l = new QVBoxLayout();
		l->addWidget(labelTags);
		l->addWidget(labelRating);
		l->addWidget(labelScore);
		l->addWidget(labelUser);
		l->addWidget(labelImage);
			l->setStretchFactor(labelImage, 2);
		l->addLayout(buttons);
	this->setLayout(l);
	
	this->labelTags = labelTags;
	this->labelImage = labelImage;
	this->buttonSave = buttonSave;
	this->buttonSaveas = buttonSaveas;

	this->format = this->url.section('.', -1).toUpper();
	
	QTimer *timer = new QTimer(this);
		connect(timer, SIGNAL(timeout()), this, SLOT(update()));
		timer->setSingleShot(true);
		this->timer = timer;

	QString u = this->regex.at(4);
		u.replace("{id}", this->id);
	QUrl rl(u);
	QNetworkAccessManager *manager = new QNetworkAccessManager(this);
	connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyFinished(QNetworkReply*)));
	QNetworkRequest rq(rl);
		rq.setRawHeader("Referer", u.toAscii());
	this->r = manager->get(rq);
}
void zoomWindow::openUrl(QString url)
{
	this->parent->setTags(url);
	this->parent->webUpdate();
	this->parent->activateWindow();
}
void zoomWindow::openSaveDir()
{
	QSettings settings("settings.ini", QSettings::IniFormat);
	QDesktopServices::openUrl(QUrl("file:///"+settings.value("Save/path").toString()+"\\"+this->getSavePath().section('\\', 0, -2)));
}

void zoomWindow::linkHovered(QString url)
{ this->link = url; }
void zoomWindow::contextMenu()
{
	QMenu *menu = new QMenu(this);
	if (!this->link.isEmpty())
	{
		if (favorites.contains(link, Qt::CaseInsensitive))
		{ menu->addAction(tr("Retirer des favoris"), this, SLOT(unfavorite())); }
		else
		{ menu->addAction(tr("Ajouter aux favoris"), this, SLOT(favorite())); }
		menu->addAction(tr("Ouvrir dans une nouvelle fenêtre"), this, SLOT(openInNewWindow()));
	}
	menu->exec(QCursor::pos());
}
void zoomWindow::openInNewWindow()
{
	QProcess myProcess;
	myProcess.startDetached(m_program, QStringList(link));
}
void zoomWindow::favorite()
{
	favorites.append(link);
	QString ct(favorites.join(" "));
	QFile f("favorites.txt");
	f.open(QIODevice::WriteOnly);
	f.write(ct.toAscii());
	f.close();
}
void zoomWindow::unfavorite()
{
	favorites.removeAll(link);
	QString ct(favorites.join(" "));
	QFile f("favorites.txt");
	f.open(QIODevice::WriteOnly);
	f.write(ct.toAscii());
	f.close();
}



void zoomWindow::load()
{
	QNetworkAccessManager *m = new QNetworkAccessManager(this);
	connect(m, SIGNAL(finished(QNetworkReply*)), this, SLOT(replyFinishedZoom(QNetworkReply*)));
	QNetworkRequest request(QUrl(this->url));
		request.setRawHeader("Referer", this->url.toAscii());
	QNetworkReply *r = m->get(request);
	connect(r, SIGNAL(readyRead()), this, SLOT(rR()));
	this->r = r;
	this->m = m;
}

void zoomWindow::rR()
{
	if (this->r->size() > 66535 || this->oldsize == this->r->size())
	{
		this->d.append(this->r->readAll());
		this->image.loadFromData(this->d, (const char*)this->format.toAscii().data());
		this->update(true);
	}
	else
	{ this->oldsize = this->r->size(); }
}

void zoomWindow::replyFinished(QNetworkReply* reply)
{
	if (reply->error() == QNetworkReply::NoError)
	{
		QSettings settings("settings.ini", QSettings::IniFormat);
		bool under = settings.value("Save/remplaceblanksbyunderscores", false).toBool();
		QStringList blacklistedtags(settings.value("blacklistedtags", "blood gore futa futanari shota").toString().split(' '));
		QString source = reply->readAll();
		QRegExp rx(this->regex.at(5));
		rx.setMinimal(true);
		int pos = 0;
		QString tags = " "+this->tags+" ";
		while ((pos = rx.indexIn(source, pos)) != -1)
		{
			pos += rx.matchedLength();
			QString type = rx.cap(1), tag = rx.cap(2).replace(" ", "_"), normalized = tag;
			normalized.remove('\\').remove('/').remove(':').remove('*').remove('?').remove('"').remove('<').remove('>').remove('|');
			if (!under)
			{ normalized.replace('_', ' '); }
			if (blacklistedtags.contains(tag, Qt::CaseInsensitive))
			{ tags.replace(" "+tag+" ", " <a href=\""+tag+"\" style=\"font-weight:bold;text-decoration:underline;color:#000000\">"+tag+"</a> ");	}
			if (type == "character")		{ this->details["characters"].append(normalized);	tags.replace(" "+tag+" ", " <a href=\""+tag+"\" style=\"text-decoration:none;color:#00aa00\">"+tag+"</a> ");	}
			else if (type == "copyright")	{ this->details["copyrights"].append(normalized);	tags.replace(" "+tag+" ", " <a href=\""+tag+"\" style=\"text-decoration:none;color:#aa00aa\">"+tag+"</a> ");	}
			else if (type == "artist")		{ this->details["artists"].append(normalized);		tags.replace(" "+tag+" ", " <a href=\""+tag+"\" style=\"text-decoration:none;color:#aa0000\">"+tag+"</a> ");	}
			else if (type == "model")		{ this->details["models"].append(normalized);		tags.replace(" "+tag+" ", " <a href=\""+tag+"\" style=\"text-decoration:none;color:#0000ee\">"+tag+"</a> ");	}
			else							{ this->details["generals"].append(normalized);		tags.replace(" "+tag+" ", " <a href=\""+tag+"\" style=\"text-decoration:none;color:#000000\">"+tag+"</a> "); }
			this->details["alls"].append(normalized);
		}
		this->labelTags->setText(tr("<b>Tags :</b> %1").arg(tags.trimmed()));
		QString pth = this->getSavePath();
		QString path = settings.value("Save/path").toString()+"\\"+pth;
		QFile file(path);
		if (file.exists() && !settings.value("Save/path").toString().isEmpty() && !pth.isEmpty())
		{
			this->buttonSave->setText(tr("Fichier déjà existant"));
			this->d.clear();
				if (!file.open(QIODevice::ReadOnly))
				{ error(this, tr("Erreur inattendue lors de l'ouverture du fichier.\r\n%1").arg(path)); }
				while (!file.atEnd())
				{ this->d.append(file.readLine()); }
			this->image.loadFromData(this->d, (const char*)this->format.toAscii().data());
			this->loaded = true;
			this->update();
		}
		else
		{
			this->buttonSave->setText(tr("Enregistrer"));
			this->load();
		}
	}
	else if (reply->error() != QNetworkReply::OperationCanceledError)
	{ error(this, tr("Une erreur inattendue est survenue lors du chargement des tags.\r\n%1").arg(reply->url().toString())); }
}

void zoomWindow::replyFinishedZoom(QNetworkReply* reply)
{
	if (reply->error() == QNetworkReply::NoError)
	{
		this->d.append(reply->readAll());
		this->image.loadFromData(this->d, (const char*)this->format.toAscii().data());
		this->loaded = true;
		this->update();
	}
	else if (reply->error() != QNetworkReply::OperationCanceledError)
	{ error(this, tr("Une erreur inattendue est survenue lors du chargement de l'image.\r\n%1").arg(reply->url().toString())); }
}



void zoomWindow::update(bool onlysize)
{
	if (onlysize && (this->image.width() > this->labelImage->width() || this->image.height() > this->labelImage->height()))
	{ this->labelImage->setPixmap(this->image.scaled(this->labelImage->width(), this->labelImage->height(), Qt::KeepAspectRatio, Qt::FastTransformation)); }
	else if (this->loaded)
	{
		if (this->image.width() > this->labelImage->width() || this->image.height() > this->labelImage->height())
		{ this->labelImage->setPixmap(this->image.scaled(this->labelImage->width(), this->labelImage->height(), Qt::KeepAspectRatio, Qt::SmoothTransformation)); }
		else
		{ this->labelImage->setPixmap(this->image); }
	}
}

void zoomWindow::saveNQuit()
{
	this->saveImage();
	this->close();
}

void zoomWindow::saveImage()
{
	QSettings settings("settings.ini", QSettings::IniFormat);
	QString path = this->getSavePath();
	QFile f(settings.value("Save/path").toString()+"\\"+path);
	if (settings.value("Save/path").toString().isEmpty())
	{ error(this, tr("Vous n'avez pas précisé de dossier de sauvegarde !")); }
	else if (path.isEmpty())
	{ error(this, tr("Vous n'avez pas précisé de format de sauvegarde !")); }
	else if (!f.exists())
	{
		this->buttonSave->setText(tr("Sauvegarde..."));
		QDir dir(settings.value("Save/path").toString());
		if (!dir.mkpath(path.section('\\', 0, -2)))
		{ error(this, tr("Erreur lors de la sauvegarde de l'image.\r\n%1").arg(settings.value("Save/path").toString()+"\\"+path)); }
		f.open(QIODevice::WriteOnly);
		f.write(this->d);
		f.close();
		this->buttonSave->setText(tr("Sauvegardé !"));
	}
	else
	{ this->buttonSave->setText(tr("Fichier déjà existant")); }
}

void zoomWindow::saveImageAs()
{
	QString path = QFileDialog::getSaveFileName(this, tr("Enregistrer l'image"), this->url.section('/', -1), "Images (*.png *.gif *.jpg *.jpeg)");
	QFile f(path);
	f.open(QIODevice::WriteOnly);
	f.write(this->d);
	f.close();
}

QString zoomWindow::getSavePath()
{
	QSettings settings("settings.ini", QSettings::IniFormat);
	settings.beginGroup("Save");
	return settings.value("filename").toString()
	.replace("%artist%", (this->details["artists"].isEmpty() ? settings.value("artist_empty").toString() : (settings.value("artist_useall").toBool() || this->details["artists"].count() == 1 ? this->details["artists"].join(settings.value("artist_sep").toString()) : settings.value("artist_value").toString())))
	.replace("%general%", this->details["generals"].join(settings.value("separator").toString()))
	.replace("%copyright%", (this->details["copyrights"].isEmpty() ? settings.value("copyright_empty").toString() : (settings.value("copyright_useall").toBool() || this->details["copyrights"].count() == 1 ? this->details["copyrights"].join(settings.value("copyright_sep").toString()) : settings.value("copyright_value").toString())))
	.replace("%character%", (this->details["characters"].isEmpty() ? settings.value("character_empty").toString() : (settings.value("character_useall").toBool() || this->details["characters"].count() == 1 ? this->details["characters"].join(settings.value("character_sep").toString()) : settings.value("character_value").toString())))
	.replace("%model%", (this->details["models"].isEmpty() ? settings.value("model_empty").toString() : (settings.value("model_useall").toBool() || this->details["models"].count() == 1 ? this->details["models"].join(settings.value("model_sep").toString()) : settings.value("model_value").toString())))
	.replace("%model|artist%", (!this->details["models"].isEmpty() ? (settings.value("model_useall").toBool() || this->details["models"].count() == 1 ? this->details["models"].join(settings.value("model_sep").toString()) : settings.value("model_value").toString()) : (this->details["artists"].isEmpty() ? settings.value("artist_empty").toString() : (settings.value("artist_useall").toBool() || this->details["artists"].count() == 1 ? this->details["artists"].join(settings.value("artist_sep").toString()) : settings.value("artist_value").toString()))))
	.replace("%all%", this->details["alls"].join(settings.value("separator").toString()))
	.replace("%filename%", this->url.section('/', -1).section('.', 0, -2))
	.replace("%rating%", this->rating)
	.replace("%md5%", this->md5)
	.replace("%website%", this->site)
	.replace("%ext%", this->url.section('.', -1));
}



void zoomWindow::fullScreen()
{
	QAffiche *label = new QAffiche();
		label->setStyleSheet("background-color: black");
		label->setAlignment(Qt::AlignCenter);
		label->setPixmap(this->image.scaled(QApplication::desktop()->screenGeometry().size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
		label->showFullScreen();
	QShortcut *escape= new QShortcut(QKeySequence(Qt::Key_Escape), label);
		connect(escape, SIGNAL(activated()), label, SLOT(close()));
	connect(label, SIGNAL(doubleClicked()), label, SLOT(close()));
}



void zoomWindow::resizeEvent(QResizeEvent *e)
{
	if (this->loaded)
	{
		if (!this->timer->isActive())
		{
			this->timeout = this->image.width()*this->image.height()/100000;
			if (this->timeout < 50)		{ this->timeout = 50;	}
			if (this->timeout > 500)	{ this->timeout = 500;	}
		}
		this->timer->stop();
		this->timer->start(this->timeout);
		this->update(true);
	}
	QWidget::resizeEvent(e);
}

void zoomWindow::closeEvent(QCloseEvent *e)
{
	QSettings settings("settings.ini", QSettings::IniFormat);
		settings.beginGroup("Zoom");
		settings.setValue("state", int(this->windowState()));
		settings.setValue("size", this->size());
		settings.setValue("pos", this->pos());
	this->r->abort();
	QWidget::closeEvent(e);
}
