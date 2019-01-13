#ifndef FLYWEIGHT_CACHE_H
#define FLYWEIGHT_CACHE_H

#include <QMap>
#include <QSharedPointer>


template <class K, class T>
class FlyweightCache
{
	public:
		static QSharedPointer<T> Get(const K &key);

	private:
		static QMap<K, QWeakPointer<T>> Map;
};


template <class K, class T>
QMap<K, QWeakPointer<T>> FlyweightCache<K, T>::Map;

template <class K, class T>
QSharedPointer<T> FlyweightCache<K, T>::Get(const K &key)
{
	auto it = Map.find(key);
	if (it != Map.end()) {
		return QSharedPointer<T>(it.value());
	}

	const QSharedPointer<T> shared(new T(key));
	const QWeakPointer<T> weak(shared);
	Map.insert(key, weak);

	return shared;
}

#endif // FLYWEIGHT_CACHE_H
