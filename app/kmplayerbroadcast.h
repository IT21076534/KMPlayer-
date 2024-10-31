/*
    This file is part of the KMPlayer application
    SPDX-FileCopyrightText: 2003 Koos Vriezen <koos.vriezen@xs4all.nl>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef _KMPLAYER_BROADCAST_SOURCE_H_
#define _KMPLAYER_BROADCAST_SOURCE_H_

#include <list>
#include <vector>

#include <QFrame>
#include <QPointer>

#include "kmplayerappsource.h"
#include "kmplayerconfig.h"

class KMPlayerPrefBroadcastPage;        // broadcast
class KMPlayerPrefBroadcastFormatPage;  // broadcast format
class QListBox;
class QComboBox;
class QLineEdit;
class QTable;
class QPushButton;
class KLed;
class KProcess;

namespace KMPlayer {
    class FFMpeg;
}

class FFServerSetting
{
public:
    FFServerSetting () {}
    FFServerSetting (int i, const QString & n, const QString & f, const QString & ac, int abr, int asr, const QString & vc, int vbr, int q, int fr, int gs, int w, int h);
    FFServerSetting (const QStringList & sl) { *this = sl; }
    ~FFServerSetting () {}
    int index;
    QString name;
    QString format;
    QString audiocodec;
    QString audiobitrate;
    QString audiosamplerate;
    QString videocodec;
    QString videobitrate;
    QString quality;
    QString framerate;
    QString gopsize;
    QString width;
    QString height;
    QStringList acl;
    FFServerSetting & operator = (const QStringList &);
    FFServerSetting & operator = (const FFServerSetting & fs);
    const QStringList list ();
    QString & ffconfig (QString & buf);
};

typedef std::vector <FFServerSetting *> FFServerSettingList;


class KMPlayerPrefBroadcastPage : public QFrame
{
    Q_OBJECT
public:
    KMPlayerPrefBroadcastPage (QWidget * parent);
    ~KMPlayerPrefBroadcastPage () {}

    QLineEdit * bindaddress;
    QLineEdit * port;
    QLineEdit * maxclients;
    QLineEdit * maxbandwidth;
    QLineEdit * feedfile;
    QLineEdit * feedfilesize;
};

class KMPlayerPrefBroadcastFormatPage : public QFrame
{
    Q_OBJECT
public:
    KMPlayerPrefBroadcastFormatPage (QWidget * parent, FFServerSettingList &);
    ~KMPlayerPrefBroadcastFormatPage () {}

    QListBox * profilelist;
    QComboBox * format;
    QLineEdit * audiocodec;
    QLineEdit * audiobitrate;
    QLineEdit * audiosamplerate;
    QLineEdit * videocodec;
    QLineEdit * videobitrate;
    QLineEdit * quality;
    QLineEdit * framerate;
    QLineEdit * gopsize;
    QLineEdit * moviewidth;
    QLineEdit * movieheight;
    QLineEdit * profile;
    QPushButton * startbutton;
    KLed * serverled;
    KLed * feedled;
    void setSettings (const FFServerSetting &);
    void getSettings (FFServerSetting &);
private Q_SLOTS:
    void slotIndexChanged (int index);
    void slotItemHighlighted (int index);
    void slotTextChanged (const QString &);
    void slotLoad ();
    void slotSave ();
    void slotDelete ();
private:
    QTable * accesslist;
    QPushButton * load;
    QPushButton * save;
    QPushButton * del;
    FFServerSettingList & profiles;
};


/*
 * Preference page for ffmpeg commandline arguments
 */
class KMPlayerFFServerConfig : public KMPlayer::PreferencesPage
{
public:
    KMPlayerFFServerConfig ();
    ~KMPlayerFFServerConfig () {}
    virtual void write (KConfig *);
    virtual void read (KConfig *);
    virtual void sync (bool fromUI);
    virtual void prefLocation (QString & item, QString & icon, QString & tab);
    virtual QFrame * prefPage (QWidget * parent);
    int ffserverport;
    int maxclients;
    int maxbandwidth;
    QString feedfile;
    int feedfilesize;
    QString bindaddress;
private:
    QPointer <KMPlayerPrefBroadcastPage> m_configpage;
};

/*
 * Preference page for ffserver
 */
class KMPlayerBroadcastConfig : public QObject, public KMPlayer::PreferencesPage
{
    Q_OBJECT
public:
    KMPlayerBroadcastConfig (KMPlayer::PartBase * player, KMPlayerFFServerConfig * fsc);
    ~KMPlayerBroadcastConfig ();

    virtual void write (KConfig *);
    virtual void read (KConfig *);
    virtual void sync (bool fromUI);
    virtual void prefLocation (QString & item, QString & icon, QString & tab);
    virtual QFrame * prefPage (QWidget * parent);

    bool broadcasting () const;
    void stopServer ();
    const QString & serverURL () const { return m_ffserver_url; }

    FFServerSetting ffserversettings;
    FFServerSettingList ffserversettingprofiles;
Q_SIGNALS:
    void broadcastStarted ();
    void broadcastStopped ();
private Q_SLOTS:
    void processOutput (KProcess *, char *, int);
    void processStopped (KProcess * process);
    void startServer ();
    void startFeed ();
    void sourceChanged (KMPlayer::Source *, KMPlayer::Source *);
private:
    KMPlayer::PartBase * m_player;
    KMPlayerFFServerConfig * m_ffserverconfig;
    QPointer <KMPlayerPrefBroadcastFormatPage> m_configpage;
    KMPlayer::FFMpeg * m_ffmpeg_process;
    KProcess * m_ffserver_process;
    bool m_endserver;
    QString m_ffserver_out;
    QString m_ffserver_url;
};


#endif //_KMPLAYER_BROADCAST_SOURCE_H_
