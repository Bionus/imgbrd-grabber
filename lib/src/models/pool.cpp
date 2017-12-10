#include "pool.h"


Pool::Pool(int id, const QString &name, int current, int next, int previous)
	: m_id(id), m_name(name), m_current(current), m_next(next), m_previous(previous)
{ }

int		Pool::id() const		{ return m_id;			}
QString	Pool::name() const		{ return m_name;		}
int		Pool::current() const	{ return m_current;		}
int		Pool::next() const		{ return m_next;		}
int		Pool::previous() const	{ return m_previous;	}
