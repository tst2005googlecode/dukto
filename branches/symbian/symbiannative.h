#ifndef SYMBIANNATIVE_H
#define SYMBIANNATIVE_H

#include <QString>

class SymbianNative
{
public:
    static QString getClipboardText();
    static void setClipboardText(QString text);

private:
    SymbianNative() {}
};

#endif // SYMBIANNATIVE_H
