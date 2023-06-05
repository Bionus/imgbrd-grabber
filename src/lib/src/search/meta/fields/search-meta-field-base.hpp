#include <QString>
#include <utility>


template<typename T>
SearchMetaFieldBase<T>::SearchMetaFieldBase(QString label, T defaultValue)
	: SearchMetaField(std::move(label)), m_defaultValue(std::move(defaultValue))
{}


template<typename T>
T SearchMetaFieldBase<T>::defaultValue() const
{
	return m_defaultValue;
}
