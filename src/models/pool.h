#ifndef POOL_H
#define POOL_H

#include <QString>



class Pool
{
	public:
		explicit Pool(int id, QString name, int current, int next = 0, int previous = 0);
		~Pool();

		// Getters
		int		id() const;
		QString name() const;
		int		current() const;
		int		next() const;
		int		previous() const;

	private:
		int		m_id;
		QString	m_name;
		int		m_current;
		int		m_next;
		int		m_previous;
};

#endif // POOL_H
