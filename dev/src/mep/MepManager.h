#ifndef MEPMANAGER_H
#define MEPMANAGER_H

#include <QObject>

class MepManager : public QObject
{
    Q_OBJECT
public:
    explicit MepManager(QObject *parent = nullptr);

signals:

};

#endif // MEPMANAGER_H
