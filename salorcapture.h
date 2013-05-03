#ifndef SALORCAPTURE_H
#define SALORCAPTURE_H

#include "salorpage.h"
#include "salorprocess.h"

class SalorPage;

class SalorCapture : public QObject
{
    Q_OBJECT

public:
    SalorCapture( SalorPage* page,
                  const QString& output,
                  int delay,
                  const QString& scriptProp,
                  const QString& scriptCode
                 );

signals:
  void done();

public slots:
  void DocumentComplete(bool ok);
  void DocumentPrint();
  void InitialLayoutCompleted();
  void JavaScriptWindowObjectCleared();
  void Timeout();
  void Delayed();

private:
  void TryDelayedRender();
  void saveSnapshot();
  bool mSawInitialLayout;
  bool mSawDocumentComplete;

protected:
  int          mDelay;
  SalorPage*   mPage;
  QObject*     mScriptObj;
  QString      mScriptProp;
  QString      mScriptCode;
  QString      mOutput;
};

#endif // SALORCAPTURE_H
