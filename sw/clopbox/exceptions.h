#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include <QException>
#include <QString>
#include <iostream>

typedef enum {
  error = 0,
  warning = 10,
  notice = 20,
  info = 30,
  verboseInfo = 40,
  debug = 50,
  allLogs = 9999,
} tLoglevel;

#define emitSException(msg)    \
    emit CustomException(msg); \
    std::cerr << msg.toStdString() << "\n";

typedef QHash<QString, tLoglevel> LogLevelMap;

class LogException : public QException {
public:
    LogException(tLoglevel loglevel, QString message)
        : QException()
        , className("BasicControlException")
    {
        //    this->log = Logs::getInstance("exception");
        this->message = message;
        this->loglevel = loglevel;
        //    this->log->write(loglevel, message);
    }
    const QString className;
    void raise() const { throw *this; }
    LogException* clone() const { return new LogException(*this); }
    virtual const char *getExceptionClassName() const { return "BasicControlException"; }
    QString getMessage() const { return QString("%1(%2): %3").arg(this->getExceptionClassName()).arg(this->getLogLevelStr()).arg(this->message); }
    tLoglevel getLoglevel() const { return this->loglevel; }
    QString getLogLevelStr() const {
        return LogLevelMap({
            {"error", error}, /**< unrecoverable error, application will crash */
            {"warning",
             warning}, /**< unrecoverable error, method will exit with error */
            {"notice", notice}, /**< something went wrong, but it's recoverable */
            {"info", info},     /**< debug message */
            {"verbose-info", verboseInfo}, /**< very often/huge debug message */
            {"debug", debug},
            {"all", allLogs},
        }).key(this->loglevel); }

private:
    tLoglevel loglevel;
    QString message;
    //  Logs *log;

    // QException interface
};


#endif // EXCEPTIONS_H
