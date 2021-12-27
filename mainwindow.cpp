#include "mainwindow.hpp"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setCentralWidget(new QWidget(this));
    QGridLayout *mainLayout = new QGridLayout(centralWidget());
    mRows = 7, mCols = 7;
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
                if (j < mCols - 1)
                    mapLightLock.insert(QString::number(n), QString::number(n + 1));
                if (i > 0)
                    mapLightLock.insert(QString::number(n), QString::number(n - mRows));
                if (i < mRows - 1)
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

    // ============= add new Lock ===============
//    mLock[49] = new QCheckBox(QString::number(49), this);
//    connect(mLock[49], &QCheckBox::stateChanged, this, &MainWindow::lockClicked);
//    mainLayout->addWidget(mLock[49], 7, 8, Qt::AlignCenter);
//    mapLockLight.insert(QString::number(49), QString::number(0));
//    mapLockLight.insert(QString::number(49), QString::number(48));
//    mapLightLock.insert(QString::number(0), QString::number(49));
//    mapLightLock.insert(QString::number(48), QString::number(49));
    // ============= /add new Lock ==============

    mainLayout->setContentsMargins(30, 30, 30, 30);
    mainLayout->setHorizontalSpacing(30);
    mainLayout->setVerticalSpacing(30);
    setStyleSheet("QCheckBox::indicator::unchecked#lightStyle {image: url(C:/Users/Orbitvu/Documents/Qt/klodki/flashlight_off.svg)}"
                  "QCheckBox::indicator::checked#lightStyle {image: url(C:/Users/Orbitvu/Documents/Qt/klodki/flashlight_on.svg)}"
                  "QCheckBox::indicator::unchecked#lockStyle {image: url(C:/Users/Orbitvu/Documents/Qt/klodki/lock_open.svg)}"
                  "QCheckBox::indicator::checked#lockStyle {image: url(C:/Users/Orbitvu/Documents/Qt/klodki/lock_close.svg)}"
                  "QCheckBox {color: rgb(240, 240, 240)}");

    connect(this, &MainWindow::respondToLock, this, [=](QString value, QString choosenLock){
        QVector<QString> checkState = mapLockLight.values(choosenLock);
        bool lightState = false;

        for (int i = 0; i < checkState.size(); i++)
        {
            if (turnedOnBoxes.contains(checkState.at(i)))
                lightState = true;
        }
        mLight[value.toInt()]->setChecked(lightState);
    });

    connect(this, &MainWindow::respondToLight, this, [=](QString value, bool isLightOn){
        mLight[value.toInt()]->setChecked(isLightOn);
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
                        emit respondToLight(furtherValues.at(m), lightOn);
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
                    if (!turnedOnBoxes.contains(furtherValues.at(k)))
                    {
                        emit respondToLock(values.at(k), choosenLock);
                    }
                    else
                    {
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
