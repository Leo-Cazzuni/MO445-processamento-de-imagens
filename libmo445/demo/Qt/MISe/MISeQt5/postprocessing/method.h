#ifndef METHOD_H
#define METHOD_H

#include <QWidget>
#include <iftImage.h>

class Method : public QWidget
{
    Q_OBJECT
public:
    explicit Method(QWidget *parent = nullptr);
    Method(const Method &);
    Method &operator=(const Method &) = default;
    ~Method();

    QString toString() const;
    virtual iftImage *process(iftImage *img) = 0;
protected:
    QString _name;
};

Q_DECLARE_METATYPE(Method*)

#endif // METHOD_H
