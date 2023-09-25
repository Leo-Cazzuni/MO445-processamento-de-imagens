#include "mycombobox.h"

MyComboBox::MyComboBox(QWidget *parent):
    QComboBox (parent)
{
    view()->installEventFilter(this);
    view()->window()->installEventFilter(this);
    view()->viewport()->installEventFilter(this);
}

MyComboBox::~MyComboBox()
{

}

bool MyComboBox::eventFilter(QObject *object, QEvent *event)
{
    if (event->type() == QEvent::MouseButtonRelease){
        int index = view()->currentIndex().row();

        if (index == 0)
            // propagate to parent class
            return QObject::eventFilter(object, event);

        if (itemData(index, Qt::CheckStateRole) == Qt::Checked) {
            setItemData(index, Qt::Unchecked, Qt::CheckStateRole);
        } else {
            setItemData(index, Qt::Checked, Qt::CheckStateRole);
        }

        setItemText(0,"");
        for (int i = 2; i < count(); i++)
            if (itemData(i, Qt::CheckStateRole) == Qt::Checked)
                setItemText(0,itemText(0)+itemText(i)+"; ");

        emit itemClicked(index);

        return true;
    } else {
        // propagate to parent class
        return QObject::eventFilter(object, event);
    }
}
