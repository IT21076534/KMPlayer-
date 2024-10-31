/*
    This file belong to the KMPlayer project, a movie player plugin for Konqueror
    SPDX-FileCopyrightText: 2009 Koos Vriezen <koos.vriezen@gmail.com>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef _KMPLAYER_LISTS_H_
#define _KMPLAYER_LISTS_H_

#include <config-kmplayer.h>

#include <QProcess>

#include "kmplayerplaylist.h"
#include "kmplayerpartbase.h"

static const short id_node_recent_document = 31;
static const short id_node_recent_node = 32;
static const short id_node_disk_document = 33;
static const short id_node_disk_node = 34;
static const short id_node_gen_generator = 36;
static const short id_node_gen_input = 37;
static const short id_node_gen_uri = 38;
static const short id_node_gen_literal = 39;
static const short id_node_gen_ask = 40;
static const short id_node_gen_title = 41;
static const short id_node_gen_description = 42;
static const short id_node_gen_process = 43;
static const short id_node_gen_program = 44;
static const short id_node_gen_argument = 45;
static const short id_node_gen_predefined = 46;
static const short id_node_gen_document = 47;
static const short id_node_gen_http_get = 48;
static const short id_node_gen_http_key_value = 49;
static const short id_node_gen_sequence = 50;

class QTextStream;
class KMPlayerApp;

class ListsSource : public KMPlayer::URLSource
{
public:
    ListsSource (KMPlayer::PartBase * p)
        : KMPlayer::URLSource (p, QUrl ("lists://")) {}
    void play (KMPlayer::Mrl *) override;
    void activate () override;
    QString prettyName () override;
};

class FileDocument : public KMPlayer::SourceDocument
{
public:
    FileDocument (short id, const QString&, KMPlayer::Source *source = nullptr);
    KMPlayer::Node *childFromTag (const QString &tag) override;
    void readFromFile (const QString &file);
    void writeToFile (const QString &file);
    void sync (const QString & file);
    unsigned int load_tree_version;
};

class Recents : public FileDocument
{
public:
    Recents (KMPlayerApp *a);
    void defer () override;
    void activate () override;
    void message (KMPlayer::MessageType msg, void *content=nullptr) override;
    KMPlayer::Node *childFromTag (const QString &tag) override;
    const char *nodeName () const override { return "playlist"; }
    KMPlayerApp *app;
};

class Recent : public KMPlayer::Mrl
{
public:
    Recent (KMPlayer::NodePtr & doc, KMPlayerApp *a, const QString &url = QString());
    void activate () override;
    void closed () override;
    const char *nodeName () const override { return "item"; }
    KMPlayerApp *app;
};

class Group
 : public KMPlayer::Element, public KMPlayer::PlaylistRole
{
public:
    Group (KMPlayer::NodePtr &doc, KMPlayerApp *a, const QString &pn=QString());
    KMPlayer::Node *childFromTag (const QString &tag) override;
    void defer () override {} // TODO lazy loading of largish sub trees
    void closed () override;
    void *role (KMPlayer::RoleType msg, void *content=nullptr) override;
    const char *nodeName () const override { return "group"; }
    KMPlayerApp *app;
};

class Playlist : public FileDocument
{
public:
    Playlist (KMPlayerApp *a, KMPlayer::Source *s, bool plmod = false);
    void message (KMPlayer::MessageType msg, void *content=nullptr) override;
    void defer () override;
    void activate () override;
    KMPlayer::Node *childFromTag (const QString &tag) override;
    const char * nodeName () const override { return "playlist"; }
    KMPlayerApp *app;
    bool playmode;
};

class PlaylistItemBase : public KMPlayer::Mrl
{
public:
    PlaylistItemBase (KMPlayer::NodePtr &d, short id, KMPlayerApp *a, bool pm);
    void activate () override;
    void closed () override;
    KMPlayerApp *app;
    bool playmode;
};

class PlaylistItem : public PlaylistItemBase
{
public:
    PlaylistItem (KMPlayer::NodePtr & doc, KMPlayerApp *a, bool playmode, const QString &url = QString());
    void closed () override;
    void begin () override;
    void setNodeName (const QString&) override;
    const char *nodeName () const override { return "item"; }
};

class PlaylistGroup
 : public KMPlayer::Element, public KMPlayer::PlaylistRole
{
public:
    PlaylistGroup (KMPlayer::NodePtr &doc, KMPlayerApp *a, const QString &pn);
    PlaylistGroup (KMPlayer::NodePtr &doc, KMPlayerApp *a, bool plmode=false);
    KMPlayer::Node *childFromTag (const QString &tag) override;
    void closed () override;
    void *role (KMPlayer::RoleType msg, void *content=nullptr) override;
    void setNodeName (const QString&) override;
    const char *nodeName () const override { return "group"; }
    KMPlayerApp *app;
    bool playmode;
};

class HtmlObject : public PlaylistItemBase
{
public:
    HtmlObject (KMPlayer::NodePtr &doc, KMPlayerApp *a, bool playmode);
    void activate () override;
    void closed () override;
    KMPlayer::Node *childFromTag (const QString &tag) override;
    const char *nodeName () const override { return "object"; }
};

class Generator : public QObject, public FileDocument
{
    Q_OBJECT
public:
    Generator (KMPlayerApp *a);
    void activate () override;
    void begin () override;
    void deactivate () override;
    void message (KMPlayer::MessageType msg, void *content=nullptr) override;
    KMPlayer::Node *childFromTag (const QString &tag) override;
    const char *nodeName () const override { return "generator"; }

private Q_SLOTS:
    void started ();
    void error (QProcess::ProcessError err);
    void readyRead ();
    void finished ();

private:
    struct ProgramCmd
    {
        QString program;
        QStringList args;
        bool isEmpty() const { return program.isEmpty(); }
        QString toString() const;
    };
    ProgramCmd genReadProgramCmd (KMPlayer::Node *n);
    QString genReadInput (KMPlayer::Node *n);
    QString genReadString (KMPlayer::Node *n);
    QString genReadUriGet (KMPlayer::Node *n);
    QString genReadAsk (KMPlayer::Node *n);

    KMPlayerApp *app;
    QProcess *qprocess;
    QTextStream *data;
    ProgramCmd process;
    QString buffer;
    bool canceled;
    bool quote;
};

class GeneratorElement : public KMPlayer::Element
{
public:
    GeneratorElement (KMPlayer::NodePtr &doc, const QString &t, short id)
        : KMPlayer::Element (doc, id), tag (t.toUtf8 ()) {}
    KMPlayer::Node *childFromTag (const QString &tag) override;
    const char *nodeName () const override { return tag.constData (); }
    QByteArray tag;
};

#endif
