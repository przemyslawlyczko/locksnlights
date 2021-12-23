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

    connect(this, &MainWindow::respondToHLock, this, [=](QString value){
        if (mLight[value.toInt() + 1]->isChecked())
        {
            mLight[value.toInt() - 1]->setChecked(true);

//            mLight[value.toInt() + 1]->setEnabled(false);
//            mLight[value.toInt() - 1]->setEnabled(false);
        }
        if (mLight[value.toInt() -1]->isChecked())
        {
            mLight[value.toInt() + 1]->setChecked(true);

//            mLight[value.toInt() - 1]->setEnabled(false);
//            mLight[value.toInt() + 1]->setEnabled(false);
        }
    });
    connect(this, &MainWindow::respondToVLock, this, [=](QString value){
        if (mLight[value.toInt() + mRows]->isChecked())
        {
            mLight[value.toInt() - mRows]->setChecked(true);
        }
        if (mLight[value.toInt() - mRows]->isChecked())
        {
            mLight[value.toInt() + mRows]->setChecked(true);
        }
    });
}

MainWindow::~MainWindow()
{
}

void MainWindow::lightClicked(int state)
{
    bool lightOn;
    QCheckBox *mLight = (QCheckBox *)sender();

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

//    if (mapLightLock.contains(mLight->text()))
//    {
//        QList<QString> values = mapLightLock.values(mLight->text());
//        for (int k = 0; k < values.size(); ++k)
//            qDebug() << values.at(k);
//    }
}

void MainWindow::lockClicked(int state)
{
    bool lockOn;
    QCheckBox *mLock = (QCheckBox *)sender();
    QString choosenLock = mLock->text();

    state == 2 ? lockOn = true : lockOn = false;

    if (lockOn)
    {
        lockedBoxes.append(mLock->text());
        qDebug()<<"1";

        auto recurency = [=](QString choosenLock, auto &&recurency)->QString{
            qDebug()<<"2";
            if (mapLockLight.contains(choosenLock)) // 34
            {
                QList<QString> values = mapLockLight.values(choosenLock); // (24, 44)
                qDebug()<<values<<turnedOnBoxes;
                for (int k = 0; k < values.size(); ++k)
                {
                    qDebug()<<values.at(k);
                    if (turnedOnBoxes.contains(values.at(k)))
                    {
                        qDebug()<<"3";
                        if (values.at(k).toInt() == choosenLock.toInt() - 1
                                || values.at(k).toInt() == choosenLock.toInt() + 1)
                            emit respondToHLock(choosenLock);
                        else
                            emit respondToVLock(choosenLock);
                    }
//                    if (values.at(k) != choosenLock
//                            && lockedBoxes.contains(values.at(k)))
//                    {
//                        qDebug()<<values.at(k);
//                        return recurency(values.at(k), recurency);
//                    }

                    QList<QString> furtherValues = mapLightLock.values(values.at(k)); // (34, 43, 45, 54), (14, 23, 25, 34)
                    for (int m = 0; m < furtherValues.size(); ++m)
                    {
                        qDebug()<<furtherValues.at(m);
                        if (furtherValues.at(m) != choosenLock
                                && lockedBoxes.contains(furtherValues.at(m)))
                        {
                            qDebug()<<furtherValues.at(m); // >43< 34
                            return recurency(furtherValues.at(m), recurency);
                        }
                    }
                }
            }
        };
        recurency(choosenLock, recurency);
    }
    else
    {
        if (lockedBoxes.contains(mLock->text()))
            lockedBoxes.removeOne(mLock->text()); // usuwa gdy odznaczamy
    }
}
