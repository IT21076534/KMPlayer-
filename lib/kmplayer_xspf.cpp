/*
    SPDX-FileCopyrightText: 2006 Koos Vriezen <koos.vriezen@gmail.com>

    SPDX-License-Identifier: LGPL-2.0-only
*/

#include "config-kmplayer.h"

#include "kmplayercommon_log.h"
#include "kmplayer_xspf.h"

using namespace KMPlayer;


Node *XSPF::Playlist::childFromTag (const QString & tag) {
    QByteArray ba = tag.toLatin1();
    const char * name = ba.constData ();
    if (!strcasecmp (name, "tracklist"))
        return new Tracklist (m_doc);
    else if (!strcasecmp (name, "creator"))
        return new DarkNode (m_doc, name, id_node_creator);
    else if (!strcasecmp (name, "title"))
        return new DarkNode (m_doc, name, id_node_title);
    else if (!strcasecmp (name, "annotation"))
        return new DarkNode (m_doc, name, id_node_annotation);
    else if (!strcasecmp (name, "info"))
        return new DarkNode (m_doc, name, id_node_info);
    else if (!strcasecmp (name, "location"))
        return new DarkNode (m_doc, name, id_node_location);
    else if (!strcasecmp (name, "identifier"))
        return new DarkNode (m_doc, name, id_node_identifier);
    else if (!strcasecmp (name, "image"))
        return new DarkNode (m_doc, name, id_node_image);
    else if (!strcasecmp (name, "date"))
        return new DarkNode (m_doc, name, id_node_date);
    else if (!strcasecmp (name, "license"))
        return new DarkNode (m_doc, name, id_node_license);
    else if (!strcasecmp (name, "attribution"))
        return new DarkNode (m_doc, name, id_node_attribution);
    else if (!strcasecmp (name, "link"))
        return new DarkNode (m_doc, name, id_node_link);
    else if (!strcasecmp (name, "meta"))
        return new DarkNode (m_doc, name, id_node_meta);
    else if (!strcasecmp (name, "extension"))
        return new DarkNode (m_doc, name, id_node_extension);
    return nullptr;
}

void XSPF::Playlist::closed () {
    for (Node *e = firstChild (); e; e = e->nextSibling ()) {
        if (e->id == id_node_title)
            title = e->innerText ().simplified ();
        else if (e->id == id_node_location)
            src = e->innerText ().trimmed ();
    }
    Mrl::closed ();
}

void *XSPF::Playlist::role (RoleType msg, void *content)
{
    if (RolePlaylist == msg)
        return !title.isEmpty () ? (PlaylistRole *) this : nullptr;
    return Mrl::role (msg, content);
}

//-----------------------------------------------------------------------------

Node *XSPF::Tracklist::childFromTag (const QString & tag) {
    QByteArray ba = tag.toLatin1 ();
    const char *name = ba.constData ();
    if (!strcasecmp (name, "track"))
        return new XSPF::Track (m_doc);
    return nullptr;
}

//-----------------------------------------------------------------------------

Node *XSPF::Track::childFromTag (const QString & tag) {
    QByteArray ba = tag.toLatin1 ();
    const char *name = ba.constData ();
    if (!strcasecmp (name, "location"))
        return new Location (m_doc);
    else if (!strcasecmp (name, "creator"))
        return new DarkNode (m_doc, name, id_node_creator);
    else if (!strcasecmp (name, "title"))
        return new DarkNode (m_doc, name, id_node_title);
    else if (!strcasecmp (name, "annotation"))
        return new DarkNode (m_doc, name, id_node_annotation);
    else if (!strcasecmp (name, "info"))
        return new DarkNode (m_doc, name, id_node_info);
    else if (!strcasecmp (name, "identifier"))
        return new DarkNode (m_doc, name, id_node_identifier);
    else if (!strcasecmp (name, "album"))
        return new DarkNode (m_doc, name, id_node_album);
    else if (!strcasecmp (name, "image"))
        return new DarkNode (m_doc, name, id_node_image);
    else if (!strcasecmp (name, "trackNum"))
        return new DarkNode (m_doc, name, id_node_tracknum);
    else if (!strcasecmp (name, "duration"))
        return new DarkNode (m_doc, name, id_node_duration);
    else if (!strcasecmp (name, "link"))
        return new DarkNode (m_doc, name, id_node_link);
    else if (!strcasecmp (name, "meta"))
        return new DarkNode (m_doc, name, id_node_meta);
    else if (!strcasecmp (name, "extension"))
        return new DarkNode (m_doc, name, id_node_extension);
    return nullptr;
}

void XSPF::Track::closed () {
    Location *location = nullptr;
    QString title;
    for (Node *e = firstChild (); e; e = e->nextSibling ()) {
        switch (e->id) {
            case id_node_title:
                title = e->innerText ();
                break;
            case id_node_location:
                location = static_cast <Location *> (e);
                break;
        }
    }
    if (location && !title.isEmpty ())
        location->setCaption (title);
    Element::closed ();
}

void XSPF::Track::activate () {
    for (Node *e = firstChild (); e; e = e->nextSibling ())
        if (e->id == id_node_annotation) {
            QString inf = e->innerText ().trimmed ();
            document ()->message (MsgInfoString, &inf);
            break;
        }
    Element::activate ();
}

void XSPF::Location::closed () {
    src = innerText ().trimmed ();
    Mrl::closed ();
}
