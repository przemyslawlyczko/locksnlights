#pragma once

#include <QMainWindow>
#include <QGridLayout>
#include <QCheckBox>
#include <QDebug>
#include <QString>
#include <QMultiHash>
#include <QVector>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void lightClicked(int state);
    void lockClicked(int state);

signals:
    void respondToLock(QString value, QString choosenLock);
    void respondToLight(QString value, bool isLightOn);

private:
    int mRows, mCols;
    bool mIsButtonClicked;
    QMultiHash<QString, QString> mapLightLock, mapLockLight;
    QVector<QString> lockedBoxes, turnedOnBoxes;
};
