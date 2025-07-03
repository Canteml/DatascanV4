#include "TestCore.h"

#include <QDebug>


TestCore::TestCore() : Core("TestCore")
{
    Logger::addFatalError("TEST FATAL");
    fLogFatal("fLogFatal");
    fLogError("fLogError");
    fLogWarning("fLogWarning");
    fLogInfo("fLogInfo");
    fLogDebug1("fLogDebug1");
    fLogDebug2("fLogDebug2");




}

TestCore::~TestCore()
{

}

QString TestCore::getVersion() const
{
    return "1.2.3";
}


// -- Core
QString TestCore::getApplicationName() const
{
    return "TestCore";
}

bool TestCore::initApp()
{
    m_t1 = new QTimer();
    m_t1->setSingleShot(true);
    connect(m_t1, &QTimer::timeout, this, &TestCore::onT1);

    m_t2 = new QTimer();
    m_t2->setSingleShot(true);
    connect(m_t2, &QTimer::timeout, this, &TestCore::onT2);

    return true;
}

bool TestCore::startApp()
{
    m_t1->start(100);
    return true;
}

void TestCore::stopApp()
{

}

void TestCore::deleteApp()
{

}

// -- Timer

void TestCore::onT1()
{
    for(int i=0; i<10000; i++) {
        fLogInfo(QString("Log message %1").arg(i));
    }

    m_t2->start(100);
}

void TestCore::onT2()
{
    // Normal exit
    // qApp->quit();

    // Error exit
    QString * pS = new QString();
    delete pS;
    pS->append("123");
    //raise( SIGFPE);
}
