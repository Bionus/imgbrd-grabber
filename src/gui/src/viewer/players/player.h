#ifndef PLAYER_H
#define PLAYER_H

#include <QObject>
#include <QWidget>


class Player : public QWidget
{
	Q_OBJECT

	public:
		explicit Player(QWidget *parent = nullptr);

		virtual bool supports(const QString &file) = 0;
		virtual void load(const QString &file) = 0;
		virtual void unload() = 0;
		virtual int duration() = 0; // In milliseconds
};

#endif // PLAYER_H
