#pragma once

#include <QMainWindow>
#include <QGridLayout>
#include <QCheckBox>
#include <QPushButton>
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

    const int mRows = 7, mCols = 7;
    bool mIsButtonClicked;
    QMultiHash<QString, QString> mapLightLock, mapLockLight;
    QVector<QString> lockedBoxes, turnedOnBoxes;

    void lightClicked(int state);
    void lockClicked(int state);

signals:
    void respondToHLock(QString value);
    void respondToVLock(QString value);
    void respondToTurnOnLight(QString value);
    void respondToTurnOffLight(QString value);
};
