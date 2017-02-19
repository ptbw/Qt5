#include "animate.h"
#include "robot.h"
#include "i.h"
#include "speak.h"

Animate::Animate(Serial *serial) :
    QObject(0)
{
    _working = false;
    _abort = false;
    _serial = serial;

    text << "Good morning! How are you?"
         << "My brain is a Raspberry Pi"
         << "My muscles are an Arduino Microcontroller"
         << "It is a nice day!"
         << "Did you have your coffee yet?"
         << "My name is Fritz!"
         << "I am a robot! Are you a robot?"
         << "I was programmed by Phil"
         << "I have 13 movable parts"
         << "I am nearly 2 years old";
}

void Animate::doWork()
{
    Robot robot(_serial);
    QTime time = QTime::currentTime();
    qsrand((uint)time.msec());

    int dir = 1;
    int angle = 10;

    robot.SetCentre();
    robot.SetNeck(angle);

    bool running = true;
    while(running)
    {
        mutex.lock();
        bool abort = _abort;
        mutex.unlock();
        if (abort)
        {
            running = false;
            break;
        }

        int value = (int)(qrand() % 10);
        mutex.lock();
        double sonar = robot.GetSonar();
        qWarning() << "Sonar: " << sonar << endl;
        mutex.unlock();

//        if( sonar < 9999)
//        {
//            //qWarning() << QObject::tr("SonarValue %1").arg(sonar) << endl;
//            if(sonar < 50.0)
//            {
//                QString msg = text.at(value);
//                SpeakMessage(msg);
//                I::sleep(10);
//            }
//            if(sonar > 200)
//            {
//               angle = angle + (5 * dir);
//               if( angle >= 90 || angle <= 10 )
//               {
//                   dir = dir * -1;
//               }
//               //qWarning() << QObject::tr("Angle %1").arg(angle) << endl;
//               robot.SetNeck(angle);
//               I::msleep(500);
//            }
//        }
    }
    emit done();
    emit finished();
}

void Animate::SpeakMessage(QString msg)
{
    Speak speak;
    Robot robot(_serial);

    QStringList phons = speak.TextToPhon(msg);
    speak.TextToSpeech(msg);

    QStringListIterator iterator(phons);
    while (iterator.hasNext())
    {
        QString shape = speak.GetMouthShape(iterator.next());
        robot.SetMouth(shape);
        I::msleep(10);
    }
}

void Animate::requestWork()
{
    mutex.lock();
    _working = true;
    _abort = false;
    mutex.unlock();

    emit workRequested();
}

void Animate::abort()
{
    if (_working) {
        mutex.lock();
        _working = false;
        _abort = true;
        mutex.unlock();
        qDebug()<<"Request worker aborting in Thread "<<thread()->currentThreadId();
    }
}


