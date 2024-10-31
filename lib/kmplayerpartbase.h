/*
    SPDX-FileCopyrightText: 2002-2003 Koos Vriezen <koos.vriezen@gmail.com>

    SPDX-License-Identifier: LGPL-2.0-only
*/

#ifndef KMPLAYERPARTBASE_H
#define KMPLAYERPARTBASE_H

#include "config-kmplayer.h"

#include <QObject>
#include <QList>
#include <QPointer>
#include <QUrl>
#include <QStringList>
#include <QMap>
#include <QModelIndex>

#include <KMediaPlayer/Player>
#include <KSharedConfig>
#include <KConfigGroup>

#include "kmplayercommon_export.h"
#include "kmplayerview.h"
#include "kmplayerplaylist.h"

class QMenu;

class KAboutData;
class KActionCollection;
class KBookmarkMenu;
class KBookmarkManager;


namespace KMPlayer {

class PartBase;
class Process;
class MPlayer;
class BookmarkOwner;
class PlayModel;
class Settings;
class MediaManager;


/**
 * Class for a certain media, like URL, DVD, TV etc
 */
class KMPLAYERCOMMON_EXPORT Source : public QObject, public PlayListNotify
{
    Q_OBJECT
public:
    struct LangInfo {
        LangInfo (int i, const QString & n) : id (i), name (n) {}
        int id; QString name; SharedPtr <LangInfo> next;
    };
    typedef SharedPtr <LangInfo> LangInfoPtr;

    Source (const QString & name, PartBase * player, const char * src);
    ~Source () override;
    virtual void init ();
    virtual bool processOutput (const QString & line);

    bool identified () const { return m_identified; }
    virtual bool hasLength ();
    virtual bool isSeekable ();

    KMPLAYERCOMMON_NO_EXPORT int width () const { return m_width; }
    KMPLAYERCOMMON_NO_EXPORT int height () const { return m_height; }
    virtual void dimensions (int & w, int & h) { w = m_width; h = m_height; }
    /* length () returns length in deci-seconds */
    KMPLAYERCOMMON_NO_EXPORT int length () const { return m_length; }
    /* position () returns position in deci-seconds */
    KMPLAYERCOMMON_NO_EXPORT int position () const { return m_position; }
    KMPLAYERCOMMON_NO_EXPORT float aspect () const { return m_aspect; }
    KMPLAYERCOMMON_NO_EXPORT const QUrl & url () const { return m_url; }
    KMPLAYERCOMMON_NO_EXPORT const QUrl & subUrl () const { return m_sub_url; }
    PartBase * player () { return m_player; }
    virtual void reset ();
    KMPLAYERCOMMON_NO_EXPORT const QString & audioDevice () const { return m_audiodevice; }
    KMPLAYERCOMMON_NO_EXPORT const QString & videoDevice () const { return m_videodevice; }
    KMPLAYERCOMMON_NO_EXPORT const QString & videoNorm () const { return m_videonorm; }
    /* frequency() if set, returns frequency in kHz */
    KMPLAYERCOMMON_NO_EXPORT int frequency () const { return m_frequency; }
    KMPLAYERCOMMON_NO_EXPORT int xvPort () const { return m_xvport; }
    KMPLAYERCOMMON_NO_EXPORT int xvEncoding () const { return m_xvencoding; }
    KMPLAYERCOMMON_NO_EXPORT int audioLangId () const { return m_audio_id; }
    KMPLAYERCOMMON_NO_EXPORT int subTitleId () const { return m_subtitle_id; }
    KMPLAYERCOMMON_NO_EXPORT const QString & pipeCmd () const { return m_pipecmd; }
    KMPLAYERCOMMON_NO_EXPORT const QString & options () const { return m_options; }
    KMPLAYERCOMMON_NO_EXPORT const QString & recordCmd () const { return m_recordcmd; }
    KMPLAYERCOMMON_NO_EXPORT const QString & tuner () const { return m_tuner; }
    KMPLAYERCOMMON_NO_EXPORT const char* name() const { return m_name; }
    KMPLAYERCOMMON_NO_EXPORT Mrl *current() { return m_current ? m_current->mrl() : nullptr;}
    virtual void setCurrent (Mrl *mrl);
    QString plugin (const QString &mime) const;
    virtual NodePtr document ();
    void setDocument (KMPlayer::NodePtr doc, KMPlayer::NodePtr cur);
    virtual NodePtr root ();
    virtual QString filterOptions ();
    virtual bool authoriseUrl (const QString &url);

    virtual void setUrl (const QString &url);
    void insertURL (NodePtr mrl, const QString & url, const QString & title=QString());
    KMPLAYERCOMMON_NO_EXPORT void setSubURL (const QUrl & url) { m_sub_url = url; }
    void setLanguages (LangInfoPtr alang, LangInfoPtr slang) KMPLAYERCOMMON_NO_EXPORT;
    KMPLAYERCOMMON_NO_EXPORT void setWidth (int w) { m_width = w; }
    KMPLAYERCOMMON_NO_EXPORT void setHeight (int h) { m_height = h; }
    virtual void setDimensions (NodePtr, int w, int h);
    virtual void setAspect (NodePtr, float a);
    /* setLength (len) set length in deci-seconds */
    void setLength (NodePtr, int len);
    /* setPosition (pos) set position in deci-seconds */
    void setPosition (int pos) KMPLAYERCOMMON_NO_EXPORT;
    virtual void setIdentified (bool b = true);
    KMPLAYERCOMMON_NO_EXPORT void setAutoPlay (bool b) { m_auto_play = b; }
    KMPLAYERCOMMON_NO_EXPORT bool autoPlay () const { return m_auto_play; }
    KMPLAYERCOMMON_NO_EXPORT void setAvoidRedirects (bool b) { m_avoid_redirects = b; }
    KMPLAYERCOMMON_NO_EXPORT bool avoidRedirects () const { return m_avoid_redirects; }
    void setTitle (const QString & title);
    void setLoading (int percentage) KMPLAYERCOMMON_NO_EXPORT;

    virtual QString prettyName ();
    virtual void activate () = 0;
    virtual void deactivate () = 0;
    virtual void forward ();
    virtual void backward ();
    /**
     * Play at node position
     */
    virtual void play (Mrl *);
Q_SIGNALS:
    void startPlaying ();
    void stopPlaying ();
    /**
     * Signal for notifying this source is at the end of play items
     */
    void endOfPlayItems ();
    void dimensionsChanged ();
    void titleChanged (const QString & title);
public Q_SLOTS:
    void slotActivate ();
    void setAudioLang (int) KMPLAYERCOMMON_NO_EXPORT;
    void setSubtitle (int) KMPLAYERCOMMON_NO_EXPORT;
protected:
    void timerEvent (QTimerEvent *) override;
    /**
     * PlayListNotify implementation
     */
    void stateElementChanged (Node * element, Node::State os, Node::State ns) override;
    void bitRates (int & preferred, int & maximal) override;
    void setTimeout (int ms) override;
    void openUrl (const QUrl &url, const QString &target, const QString &srv) override;
    void enableRepaintUpdaters (bool enable, unsigned int off_time) override;

    NodePtr m_document;
    NodePtrW m_current;
    const char* m_name;
    PartBase * m_player;
    QString m_recordcmd;
    bool m_identified;
    bool m_auto_play;
    bool m_avoid_redirects;
    QUrl m_url;
    QUrl m_sub_url;
    QString m_audiodevice;
    QString m_videodevice;
    QString m_videonorm;
    QString m_tuner;
    int m_frequency;
    int m_xvport;
    int m_xvencoding;
    int m_audio_id;
    int m_subtitle_id;
    QString m_pipecmd;
    QString m_options;
    QString m_plugin;
    LangInfoPtr m_audio_infos;
    LangInfoPtr m_subtitle_infos;
private:
    int m_width;
    int m_height;
    float m_aspect;
    int m_length;
    int m_position;
    int m_doc_timer;
private Q_SLOTS:
    void changedUrl();
};

class KMPLAYERCOMMON_EXPORT SourceDocument : public Document
{
public:
    SourceDocument (Source *s, const QString &url);

    void message (MessageType msg, void *data=nullptr) override;
    void *role (RoleType msg, void *data=nullptr) override;
protected:
    Source *m_source;
    ConnectionList m_KeyListeners;
};
/*
 * Source from URLs
 */
class KMPLAYERCOMMON_EXPORT URLSource : public Source
{
    Q_OBJECT
public:
    URLSource (PartBase * player, const QUrl & url = QUrl ());
    ~URLSource () override;

    void dimensions (int & w, int & h) override;
    bool hasLength () override;
    QString prettyName () override;
    void reset () override;
    void setUrl (const QString &url) override;
    bool authoriseUrl (const QString &url) override;
    void init () override;
    void activate () override KMPLAYERCOMMON_NO_EXPORT;
    void deactivate () override;
    void forward () override;
    void backward () override;
    void play (Mrl *) override;
protected:
    bool activated; // 'solve' an singleShot race w/ cmdline url's
};

/*
 * KDE's KMediaPlayer::Player implementation and base for KMPlayerPart
 */
class KMPLAYERCOMMON_EXPORT PartBase : public KMediaPlayer::Player
{
    Q_OBJECT
public:
    PartBase (QWidget *parent, QObject *objParent, KSharedConfigPtr);
    ~PartBase () override;
    void init (KActionCollection *ac, const QString &objname, bool transparent);
    KMediaPlayer::View* view () override;
    View* viewWidget () const { return m_view; }
    static KAboutData* createAboutData ();

    Settings * settings () const { return m_settings; }
    void keepMovieAspect (bool);
    QUrl url () const { return m_sources ["urlsource"]->url (); }
    void setUrl (const QString &url) { m_sources ["urlsource"]->setUrl (url); }
    QUrl docBase () const { return m_docbase; }

    /* Changes the backend process */
    QString processName (Mrl *mrl);

    /* Changes the source,
     * calls init() and reschedules an activate() on the source
     * */
    void setSource (Source * source);
    void createBookmarkMenu(QMenu *owner, KActionCollection *ac);
    void connectPanel (ControlPanel * panel);
    void connectPlaylist (PlayListView * playlist);
    void connectInfoPanel (InfoWindow * infopanel);
    void connectSource (Source * old_source, Source * source);
    MediaManager *mediaManager () const { return m_media_manager; }
    PlayModel *playModel () const { return m_play_model; }
    Source * source () const { return m_source; }
    QMap <QString, Source *> & sources () { return m_sources; }
    KSharedConfigPtr config () const { return m_config; }
    bool mayResize () const { return !m_noresize; }
    void updatePlayerMenu (ControlPanel *, const QString &backend=QString ());
    void updateInfo (const QString & msg);
    void updateStatus (const QString & msg);

    // these are called from Process
    void changeURL (const QString & url) KMPLAYERCOMMON_NO_EXPORT;
    void updateTree (bool full=true, bool force=false);
    void setLanguages (const QStringList & alang, const QStringList & slang);
    void recorderPlaying ();
    void recorderStopped ();
    void stopRecording ();
    bool isRecording ();
public Q_SLOTS:
    bool openUrl (const QUrl & url) override;
    virtual void openUrl (const QUrl &, const QString &t, const QString &srv);
    virtual bool openUrl(const QList<QUrl>& urls);
    bool closeUrl () override;
    void pause (void) override;
    void play (void) override;
    void stop (void) override;
    void record ();
    void record (const QString &src, const QString &file,
                 const QString &recorder, int auto_start);
    void adjustVolume (int incdec);
    bool playing () const;
    void showConfigDialog ();
    void showPlayListWindow ();
    void slotPlayerMenu (QAction*) KMPLAYERCOMMON_NO_EXPORT;
    void back ();
    void forward ();
    void addBookMark (const QString & title, const QString & url) KMPLAYERCOMMON_NO_EXPORT;
    void volumeChanged (int) KMPLAYERCOMMON_NO_EXPORT;
    void increaseVolume ();
    void decreaseVolume ();
    void setPosition (int position, int length) KMPLAYERCOMMON_NO_EXPORT;
    virtual void setLoaded (int percentage);
    virtual void processCreated (Process *);
public:
    bool isSeekable (void) const override;
    qlonglong position (void) const override;
    bool hasLength (void) const override;
    qlonglong length (void) const override;
    void seek (qlonglong) override;
    void toggleFullScreen () KMPLAYERCOMMON_NO_EXPORT;
    bool isPlaying () KMPLAYERCOMMON_NO_EXPORT;

    virtual QString doEvaluate (const QString &script);
    void showControls (bool show) KMPLAYERCOMMON_NO_EXPORT;
    QString getStatus ();
Q_SIGNALS:
    void sourceChanged (KMPlayer::Source * old, KMPlayer::Source * nw);
    void sourceDimensionChanged ();
    void loading (int percentage);
    void urlAdded (const QString & url);
    void urlChanged (const QString & url);
    void processChanged (const char *);
    void treeChanged (int id, NodePtr root, NodePtr, bool select, bool open);
    void infoUpdated (const QString & msg);
    void statusUpdated (const QString & msg);
    void languagesUpdated(const QStringList & alang, const QStringList & slang);
    void audioIsSelected (int id);
    void subtitleIsSelected (int id);
    void positioned (int pos, int length);
    void panelActionToggled(QAction*);
    void recording (bool);
protected:
    bool openFile() override;
    void timerEvent (QTimerEvent *) override;
    virtual void playingStarted ();
    virtual void playingStopped ();
protected Q_SLOTS:
    void posSliderPressed () KMPLAYERCOMMON_NO_EXPORT;
    void posSliderReleased () KMPLAYERCOMMON_NO_EXPORT;
    void positionValueChanged (int val) KMPLAYERCOMMON_NO_EXPORT;
    void contrastValueChanged (int val) KMPLAYERCOMMON_NO_EXPORT;
    void brightnessValueChanged (int val) KMPLAYERCOMMON_NO_EXPORT;
    void hueValueChanged (int val) KMPLAYERCOMMON_NO_EXPORT;
    void saturationValueChanged (int val) KMPLAYERCOMMON_NO_EXPORT;
    void sourceHasChangedAspects () KMPLAYERCOMMON_NO_EXPORT;
    void fullScreen () KMPLAYERCOMMON_NO_EXPORT;
    void playListItemClicked (const QModelIndex &) KMPLAYERCOMMON_NO_EXPORT;
    void playListItemActivated (const QModelIndex &) KMPLAYERCOMMON_NO_EXPORT;
    void slotPlayingStarted ();
    void slotPlayingStopped ();
    void settingsChanged ();
    void audioSelected (QAction*) KMPLAYERCOMMON_NO_EXPORT;
    void subtitleSelected (QAction*) KMPLAYERCOMMON_NO_EXPORT;
protected:
    QUrl m_docbase;
    NodePtr m_record_doc;
    KSharedConfigPtr m_config;
    QPointer <View> m_view;
    QMap <QString, QString> temp_backends;
    Settings *m_settings;
    MediaManager *m_media_manager;
    PlayModel *m_play_model;
    Source * m_source;
    QMap <QString, Source *> m_sources;
    KBookmarkManager * m_bookmark_manager;
    BookmarkOwner * m_bookmark_owner;
    KBookmarkMenu * m_bookmark_menu;
    int m_update_tree_timer;
    int m_rec_timer;
    bool m_noresize : 1;
    bool m_auto_controls : 1;
    bool m_use_agent : 1;
    bool m_bPosSliderPressed : 1;
    bool m_in_update_tree : 1;
    bool m_update_tree_full : 1;
};

} // namespace

#endif
