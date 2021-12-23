#include "mainwindow.hpp"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setCentralWidget(new QWidget(this));
    QGridLayout *mainLayout = new QGridLayout(centralWidget());
    QCheckBox *mLock[mRows * mCols];
    QCheckBox *mLight[49];
    int n = 0;

    for (int i = 0; i < mRows; i++)
    {
        for (int j = 0; j < mCols; j++)
        {
            if (i % 2 == 0 && j % 2 == 0)
            {
                mLight[n] = new QCheckBox(QString::number(n), this);
                mLight[n]->setObjectName("lightStyle");
                connect(mLight[n], &QCheckBox::stateChanged, this, &MainWindow::lightClicked);

                mainLayout->addWidget(mLight[n], i, j);

                if (j > 0)
                    mapLightLock.insert(QString::number(n), QString::number(n - 1));
                if (j < 6)
                    mapLightLock.insert(QString::number(n), QString::number(n + 1));
                if (i > 0)
                    mapLightLock.insert(QString::number(n), QString::number(n - mRows));
                if (i < 6)
                    mapLightLock.insert(QString::number(n), QString::number(n + mRows));
            }
            else if ((i % 2 == 0 && j % 2 != 0) || (i % 2 != 0 && j % 2 == 0))
            {
                mLock[n] = new QCheckBox(QString::number(n), this);
                mLock[n]->setObjectName("lockStyle");
                connect(mLock[n], &QCheckBox::stateChanged, this, &MainWindow::lockClicked);

                mainLayout->addWidget(mLock[n], i, j, Qt::AlignCenter);

                if (i % 2 == 0 && j % 2 != 0) // nieparzyste rzedzy
                {
                    mapLockLight.insert(QString::number(n), QString::number(n - 1));
                    mapLockLight.insert(QString::number(n), QString::number(n + 1));
                }
                else // parzyste rzedy
                {
                    mapLockLight.insert(QString::number(n), QString::number(n - mRows));
                    mapLockLight.insert(QString::number(n), QString::number(n + mRows));
                }
            }
            n++;
        }
    }

//    setStyleSheet("QCheckBox::indicator::unchecked#lightStyle {image: url(C:/Users/Orbitvu/Documents/Qt/klodki/flashlight_off.svg)}"
//                  "QCheckBox::indicator::checked#lightStyle {image: url(C:/Users/Orbitvu/Documents/Qt/klodki/flashlight_on.svg)}"
//                  "QCheckBox::indicator::unchecked#lockStyle {image: url(C:/Users/Orbitvu/Documents/Qt/klodki/lock_open.svg)}"
//                  "QCheckBox::indicator::checked#lockStyle {image: url(C:/Users/Orbitvu/Documents/Qt/klodki/lock_close.svg)}");

    connect(this, &MainWindow::respondToHLock, this, [=](QString value){
        if (mLight[value.toInt() + 1]->isChecked())
        {
            mLight[value.toInt() - 1]->setChecked(true);
            if (!turnedOnBoxes.contains(QString::number(value.toInt() - 1)))
                turnedOnBoxes.append(QString::number(value.toInt() - 1));
        }
        if (mLight[value.toInt() -1]->isChecked())
        {
            mLight[value.toInt() + 1]->setChecked(true);
            if (!turnedOnBoxes.contains(QString::number(value.toInt() + 1)))
                turnedOnBoxes.append(QString::number(value.toInt() + 1));
        }
    });
    connect(this, &MainWindow::respondToVLock, this, [=](QString value){
        if (mLight[value.toInt() + mRows]->isChecked())
        {
            mLight[value.toInt() - mRows]->setChecked(true);
            if (!turnedOnBoxes.contains(QString::number(value.toInt() + 1)))
                turnedOnBoxes.append(QString::number(value.toInt() + 1));
        }
        if (mLight[value.toInt() - mRows]->isChecked())
        {
            mLight[value.toInt() + mRows]->setChecked(true);
            if (!turnedOnBoxes.contains(QString::number(value.toInt() - 1)))
                turnedOnBoxes.append(QString::number(value.toInt() - 1));
        }
    });

    connect(this, &MainWindow::respondToTurnOnLight, this, [=](QString value){
        mLight[value.toInt()]->setChecked(true);
    });
    connect(this, &MainWindow::respondToTurnOffLight, this, [=](QString value){
        mLight[value.toInt()]->setChecked(false);
    });
}

MainWindow::~MainWindow()
{
}

void MainWindow::lightClicked(int state)
{
    bool lightOn;
    QCheckBox *mLight = (QCheckBox *)sender();
    QString choosenLight = mLight->text();
    QVector<QString> furtherValues = {};

    state == 2 ? lightOn = true : lightOn = false;

    if (lightOn)
    {
        turnedOnBoxes.append(mLight->text());
    }
    else
    {
        if (turnedOnBoxes.contains(mLight->text()))
            turnedOnBoxes.removeOne(mLight->text()); // usuwa gdy odznaczamy
    }

    if (mapLightLock.contains(choosenLight))
    {
        QVector<QString> values = mapLightLock.values(choosenLight);
        for (int k = 0; k < values.size(); ++k)
        {
            if (lockedBoxes.contains(values.at(k)))
            {
                furtherValues.append(mapLockLight.values(values.at(k)));
                for (int m = 0; m < furtherValues.size(); ++m)
                {
                    if (furtherValues.at(m) != choosenLight)
                    {
                        if (lightOn)
                        {
                            emit respondToTurnOnLight(furtherValues.at(m));
                        }
                        else
                        {
                            emit respondToTurnOffLight(furtherValues.at(m));
                        }
                    }
                }
            }
        }
    }
}

void MainWindow::lockClicked(int state)
{
    bool lockOn;
    QCheckBox *mLock = (QCheckBox *)sender();
    QString choosenLock = mLock->text();
    QVector<QString> alreadyUsedLocks = {}, furtherValues = {};

    state == 2 ? lockOn = true : lockOn = false;

    if (lockOn)
    {
        lockedBoxes.append(mLock->text());

        auto lockRecurency = [&](QString choosenLock, auto &&lockRecurency)->void
        {
            if (mapLockLight.contains(choosenLock))
            {
                QVector<QString> values = mapLockLight.values(choosenLock);
                for (int k = 0; k < values.size(); ++k)
                {
                    furtherValues.append(mapLightLock.values(values.at(k)));
                    if (turnedOnBoxes.contains(values.at(k)))
                    {
                        if (values.at(k).toInt() == choosenLock.toInt() - 1
                                || values.at(k).toInt() == choosenLock.toInt() + 1)
                            emit respondToHLock(choosenLock);
                        else
                            emit respondToVLock(choosenLock);

                        for (int m = 0; m < furtherValues.size(); ++m)
                        {
                            if (furtherValues.at(m) != choosenLock
                                    && lockedBoxes.contains(furtherValues.at(m))
                                    && !alreadyUsedLocks.contains(furtherValues.at(m)))
                            {
                                alreadyUsedLocks.append(choosenLock);
                                return lockRecurency(furtherValues.at(m), lockRecurency);
                            }
                        }
                    }
                }
            }
        };
        lockRecurency(choosenLock, lockRecurency);
    }
    else
    {
        if (lockedBoxes.contains(mLock->text()))
            lockedBoxes.removeOne(mLock->text()); // usuwa gdy odznaczamy
    }
}
