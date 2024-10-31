/*
    SPDX-FileCopyrightText: 2005 Koos Vriezen <koos.vriezen@xs4all.nl>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef _KMPLAYER_ASX_H_
#define _KMPLAYER_ASX_H_

#include <QString>

#include "kmplayerplaylist.h"

namespace KMPlayer {

namespace ASX {

const short id_node_asx = 400;
const short id_node_entry = 401;
const short id_node_ref = 402;
const short id_node_entryref = 403;
const short id_node_title = 404;
const short id_node_base = 405;
const short id_node_param = 406;
const short id_node_starttime = 407;
const short id_node_duration = 408;

/**
 * '<ASX>' tag
 */
class Asx : public Mrl
{
public:
    Asx (NodePtr & d) : Mrl (d, id_node_asx) {}
    Node *childFromTag (const QString & tag) override;
    void closed () override;
    const char * nodeName () const override { return "ASX"; }
    void *role (RoleType msg, void *content=nullptr) override;
};

/**
 * Entry tag as found in ASX for playlist item
 */
class Entry : public Mrl
{
public:
    Entry (NodePtr & d)
        : Mrl (d, id_node_entry), duration_timer (nullptr), ref_child_count (0) {}
    Node *childFromTag (const QString & tag) override;
    void message (MessageType msg, void *content=nullptr) override;
    const char * nodeName () const override { return "Entry"; }
    void closed () override;
    void activate () override;
    void deactivate () override;
    /**
     * False, but since we might have a 'base' child, we can have a rel. src
     */
    PlayType playType () override;
    void *role (RoleType msg, void *content=nullptr) override;

    Posting *duration_timer;
    int ref_child_count;
};

/**
 * Ref tag as found in ASX for URL item in playlist item
 */
class Ref : public Mrl
{
public:
    Ref (NodePtr & d) : Mrl (d, id_node_ref) {}
    //Node *childFromTag (const QString & tag);
    void opened () override;
    const char * nodeName () const override { return "Ref"; }
};

/**
 * EntryRef tag as found in ASX for shortcut of Entry plus Ref playlist item
 */
class EntryRef : public Mrl
{
public:
    EntryRef (NodePtr & d) : Mrl (d, id_node_entryref) {}
    //Node *childFromTag (const QString & tag);
    void opened () override;
    const char * nodeName () const override { return "EntryRef"; }
};

} //namespace ASX


} // namespace KMPlayer

#endif //_KMPLAYER_ASX_H_
