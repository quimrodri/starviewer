#include "patientbrowsermenulist.h"

#include <QLabel>
#include <QGridLayout>
#include "patientbrowsermenugroup.h"

#include "patientbrowsermenubasicitem.h"
#include "patientbrowsermenufusionitem.h"

#include <QDeclarativeItem>
#include <QDeclarativeContext>

namespace udg {

PatientBrowserMenuList::PatientBrowserMenuList(QWidget *parent)
 : QWidget(parent)
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setMargin(10);
    m_qmlView = new QDeclarativeView(this);
    m_qmlView->rootContext()->setContextProperty("browserModel", QVariant::fromValue(m_groups));

    layout->addWidget(m_qmlView);

    m_qmlView->setSource(QUrl("qrc:///qmlpatientbrowsermenu.qml"));
    QDeclarativeItem *object = qobject_cast<QDeclarativeItem*>(m_qmlView->rootObject());
    object->setProperty("fusionLabelText", QVariant::fromValue(tr("Fusion")));

    connect(object, SIGNAL(isActive(QString)), this, SIGNAL(isActive(QString)));
    connect(object, SIGNAL(selectedItem(QString)), this, SIGNAL(selectedItem(QString)));
    connect(object, SIGNAL(sizeChanged()), this, SLOT(updateSize()));
}

PatientBrowserMenuList::~PatientBrowserMenuList()
{
}

void PatientBrowserMenuList::addItemsGroup(const QString &caption, const QList<QPair<QString, QString> > &itemsList)
{
    QList<PatientBrowserMenuBasicItem*> elements;
    QList<PatientBrowserMenuBasicItem*> fusionElements;
    typedef QPair<QString, QString> MyPair;
    foreach (MyPair itemPair, itemsList)
    {
        if (itemPair.second.contains("+"))
        {
            PatientBrowserMenuFusionItem *item = new PatientBrowserMenuFusionItem(this);
            item->setText(itemPair.first);
            item->setIdentifier(itemPair.second);

            fusionElements << item;
        }
        else
        {
            PatientBrowserMenuBasicItem *item = new PatientBrowserMenuBasicItem(this);
            item->setText(itemPair.first);
            item->setIdentifier(itemPair.second);

            elements << item;
        }
    }

    PatientBrowserMenuGroup *group = new PatientBrowserMenuGroup(this);
    group->setCaption(caption);
    group->setElements(elements);
    group->setFusionElements(fusionElements);
    m_groups.append(group);

    m_qmlView->rootContext()->setContextProperty("browserModel", QVariant::fromValue(m_groups));
}

void PatientBrowserMenuList::markItem(const QString &identifier)
{
    m_markedItem = identifier;
    m_qmlView->rootObject()->setProperty("markedItem", QVariant::fromValue(m_markedItem));
}

QString PatientBrowserMenuList::getMarkedItem() const
{
    return m_markedItem;
}

bool PatientBrowserMenuList::event(QEvent *event)
{
    // Si s'ha pulsat l'escape
    if (event->type() == QEvent::Close)
    {
        emit closed();
    }

    return QWidget::event(event);
}

QSize PatientBrowserMenuList::sizeHint() const {
    int width = m_qmlView->rootObject()->property("width").toInt() +  2 * this->layout()->margin();
    int height = m_qmlView->rootObject()->property("height").toInt() +  2 * this->layout()->margin();
    return QSize(width, height);
}

void PatientBrowserMenuList::setMaximumWidth(int width)
{
    QWidget::setMaximumWidth(width);
    m_qmlView->rootObject()->setProperty("maxWidth", QVariant::fromValue((width - 2 * this->layout()->margin())));
    updateSize();
}

void PatientBrowserMenuList::setMaximumHeight(int maxh)
{
    QWidget::setMaximumHeight(maxh);
    m_qmlView->rootObject()->setProperty("maxHeight", QVariant::fromValue((maxh - 2 * this->layout()->margin())));

}

void PatientBrowserMenuList::updateSize()
{
    this->setFixedHeight(m_qmlView->rootObject()->property("height").toInt() +  2 * this->layout()->margin());
    this->setFixedWidth(m_qmlView->rootObject()->property("width").toInt() +  2 * this->layout()->margin());
}

}
