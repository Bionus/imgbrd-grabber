#include "search-meta-field-factory.h"
#include <QJSValue>
#include <QMap>
#include <QString>
#include "js-helpers.h"
#include "search-meta-field-boolean.h"
#include "search-meta-field-input.h"
#include "search-meta-field-options.h"


SearchMetaField *SearchMetaFieldFactory::build(const QJSValue &jsValue)
{
	const QString type = getPropertyOr(jsValue, "type", QString());
	const QString label = getPropertyOr(jsValue, "label", QString());
	const QJSValue defaultValue = jsValue.property("default");

	// Boolean
	if (type == QLatin1String("bool")) {
		const bool def = defaultValue.isBool() && defaultValue.toBool();
		return new SearchMetaFieldBoolean(label, def);
	}

	// Input
	if (type == QLatin1String("input")) {
		const QString def = defaultValue.isString() ? defaultValue.toString() : QString();
		return new SearchMetaFieldInput(label, def);
	}

	// Options
	if (type == QLatin1String("options")) {
		QMap<QString, QString> options;
		const QJSValue jsOptions = jsValue.property("options");
		const quint32 length = jsOptions.property("length").toUInt();
		for (quint32 i = 0; i < length; ++i) {
			const QJSValue val = jsOptions.property(i);
			if (val.isObject()) {
				const QJSValue valLabel = val.property("label");
				const QJSValue valValue = val.property("value");
				options.insert((!valLabel.isUndefined() ? valLabel : valValue).toString(), valValue.toString());
			} else {
				options.insert(val.toString(), val.toString());
			}
		}

		const QString def = defaultValue.isString() ? defaultValue.toString() : QString();
		return new SearchMetaFieldOptions(label, def, options);
	}

	return nullptr;
}
