#ifndef JAVACONVERTER_H
#define JAVACONVERTER_H

#include <QJsonValue>
#include <QJsonArray>
#include <QHash>
#include <QJsonObject>

// Java 类转换器
class JavaConverter{
private:
    QSet<QString> generatedClasses;

    QString toCamelCase(const QString &str) {
        if (str.isEmpty()) return str;
        QStringList parts = str.split(QRegularExpression("[^a-zA-Z0-9]"));
        QString result = parts[0].toLower();
        for (int i = 1; i < parts.size(); ++i) {
            result += parts[i].left(1).toUpper() + parts[i].mid(1).toLower();
        }
        return result;
    }

    QString getJavaType(const QJsonValue &value, const QString &fieldName) {
        switch (value.type()) {
        case QJsonValue::String:
            return "String";
        case QJsonValue::Double:
            return value.toDouble() == value.toInt() ? "int" : "double";
        case QJsonValue::Bool:
            return "boolean";
        case QJsonValue::Array: {
            QJsonArray array = value.toArray();
            if (!array.empty()) {
                QJsonValue firstElement = array[0];
                if (firstElement.isObject()) {
                    return QString("List<%1>").arg(capitalize(fieldName)+"Item");
                } else {
                    return QString("List<%1>").arg(getJavaType(firstElement, fieldName));
                }
            } else {
                return "List<Object>";
            }
        }
        case QJsonValue::Object:
            return capitalize(fieldName);
        default:
            return "Object";
        }
    }

    QString capitalize(const QString &str) {
        if (str.isEmpty()) return str;
        QString result = str;
        result[0] = result[0].toUpper();
        return result;
    }

    void generateJavaClass(QString &code, const QJsonObject &object, const QString &className) {
        if (generatedClasses.contains(className)) {
            return;
        }
        generatedClasses.insert(className);

        code += QString("public class %1 {\n").arg(className);

        QHash<QString, QString> fieldTypes;
        for (auto it = object.begin(); it != object.end(); ++it) {
            QString fieldName = toCamelCase(it.key());
            QString fieldType = getJavaType(it.value(), fieldName);
            fieldTypes[fieldName] = fieldType;
        }

        for (auto it = fieldTypes.begin(); it != fieldTypes.end(); ++it) {
            code += QString("    private %1 %2;\n").arg(it.value(), it.key());
        }

        code += QString("\n    public %1(").arg(className);
        QString params;
        for (auto it = fieldTypes.begin(); it != fieldTypes.end(); ++it) {
            params += QString("%1 %2, ").arg(it.value(), it.key());
        }
        if (!params.isEmpty()) params.chop(2);
        code += params + ") {\n";
        for (auto it = fieldTypes.begin(); it != fieldTypes.end(); ++it) {
            code += QString("        this.%1 = %1;\n").arg(it.key());
        }
        code += "    }\n";

        for (auto it = fieldTypes.begin(); it != fieldTypes.end(); ++it) {
            QString fieldName = it.key();
            QString fieldType = it.value();
            QString capitalizedFieldName = fieldName.left(1).toUpper() + fieldName.mid(1);

            code += QString("\n    public %1 get%2() {\n").arg(fieldType, capitalizedFieldName);
            code += QString("        return this.%1;\n").arg(fieldName);
            code += "    }\n";

            code += QString("    public void set%2(%1 %3) {\n").arg(fieldType, capitalizedFieldName, fieldName);
            code += QString("        this.%1 = %1;\n").arg(fieldName);
            code += "    }\n";
        }

        code += "}\n\n";

        for (auto it = object.begin(); it != object.end(); ++it) {
            if (it.value().isObject()) {
                QString nestedClassName = capitalize(it.key());
                generateJavaClass(code, it.value().toObject(), nestedClassName);
            } else if (it.value().isArray()) {
                QJsonArray array = it.value().toArray();
                if (!array.empty() && array[0].isObject()) {
                    QString nestedClassName = capitalize(it.key())+"Item";
                    generateJavaClass(code, array[0].toObject(), nestedClassName);
                }
            }
        }
    }

public:
    QString convert(const QJsonValue &value, const QString &className){
        QString code;
        code += "import java.util.List;\n\n";
        if (value.isObject()) {
            generateJavaClass(code, value.toObject(), className);
        }
        generatedClasses.clear();
        return code;
    }
};

#endif // JAVACONVERTER_H
