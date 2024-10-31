/**
    This file belong to the KMPlayer project, a movie player plugin for Konqueror
    SPDX-FileCopyrightText: 2008 Koos Vriezen <koos.vriezen@gmail.com>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "config-kmplayer.h"

#ifdef KMPLAYER_WITH_CAIRO
# include <cairo.h>
#endif

#include <QWidget>

#include "kmplayercommon_log.h"
#include "surface.h"
#include "viewarea.h"

using namespace KMPlayer;


Surface::Surface (ViewArea *widget)
  : bounds(SRect(0, 0,
                 widget->width() * widget->devicePixelRatioF(),
                 widget->height() * widget->devicePixelRatioF()
    )),
    xscale (1.0), yscale (1.0),
    background_color (0),
#ifdef KMPLAYER_WITH_CAIRO
    surface (nullptr),
#endif
    dirty (false),
    scroll (false),
    has_mouse (false),
    view_widget (widget)
{}

Surface::~Surface() {
#ifdef KMPLAYER_WITH_CAIRO
    if (surface)
        cairo_surface_destroy (surface);
#endif
}

template <> void TreeNode<Surface>::appendChild (Surface *c) {
    appendChildImpl (c);
}

template <> void TreeNode<Surface>::insertBefore (Surface *c, Surface *b) {
    insertBeforeImpl (c, b);
}

template <> void TreeNode<Surface>::removeChild (SurfacePtr c) {
    removeChildImpl (c);
}

void Surface::clear () {
    m_first_child = nullptr;
    background_color = 0;
}

void Surface::remove () {
    Surface *sp = parentNode ();
    if (sp) {
        sp->markDirty ();
        sp->removeChild (this);
    }
}

void Surface::resize (const SRect &rect, bool parent_resized) {
    SRect old_bounds = bounds;
    bounds = rect;
    if (parent_resized || old_bounds != rect) {

        if (parent_resized || old_bounds.size != rect.size) {
            virtual_size = SSize (); //FIXME try to preserve scroll on resize
            markDirty ();
#ifdef KMPLAYER_WITH_CAIRO
            if (surface) {
                cairo_surface_destroy (surface);
                surface = nullptr;
            }
#endif
            updateChildren (true);
        } else if (parentNode ()) {
            parentNode ()->markDirty ();
        }
        if (parentNode ())
            parentNode ()->repaint (old_bounds.unite (rect));
        else
            repaint ();
    }
}

void Surface::markDirty () {
    for (Surface *s = this; s && !s->dirty; s = s->parentNode ())
        s->dirty = true;
}

void Surface::updateChildren (bool parent_resized) {
    for (Surface *c = firstChild (); c; c = c->nextSibling ())
        if (c->node)
            c->node->message (MsgSurfaceBoundsUpdate, (void *) parent_resized);
        else
            qCCritical(LOG_KMPLAYER_COMMON) << "Surface without node";
}

Surface *Surface::createSurface (NodePtr owner, const SRect & rect) {
    Surface *surface = new Surface (view_widget);
    surface->node = owner;
    surface->bounds = rect;
    appendChild (surface);
    return surface;
}

IRect Surface::toScreen (const SSize &size) {
    //FIXME: handle scroll
    Matrix matrix (0, 0, xscale, yscale);
    matrix.translate (bounds.x (), bounds.y ());
    for (Surface *s = parentNode(); s; s = s->parentNode()) {
        matrix.transform(Matrix (0, 0, s->xscale, s->yscale));
        matrix.translate (s->bounds.x (), s->bounds.y ());
    }
    return matrix.toScreen (SRect (0, 0, size));
}

void Surface::setBackgroundColor (unsigned int argb) {
#ifdef KMPLAYER_WITH_CAIRO
    if (surface &&
            ((background_color & 0xff000000) < 0xff000000) !=
            ((argb & 0xff000000) < 0xff000000)) {
        cairo_surface_destroy (surface);
        surface = nullptr;
    }
#endif
    background_color = argb;
}

static void clipToScreen (Surface *s, Matrix &m, IRect &clip) {
    Surface *ps = s->parentNode ();
    if (!ps) {
        clip = IRect (s->bounds.x (), s->bounds.y (),
                s->bounds.width (), s->bounds.height ());
        m = Matrix (s->bounds.x (), s->bounds.y (), s->xscale, s->yscale);
    } else {
        clipToScreen (ps, m, clip);
        IRect scr = m.toScreen (s->bounds);
        clip = clip.intersect (scr);
        Matrix m1 = m;
        m = Matrix (s->bounds.x (), s->bounds.y (), s->xscale, s->yscale);
        m.transform (m1);
        if (!s->virtual_size.isEmpty ())
            m.translate (-s->x_scroll, -s->y_scroll);
    }
}


void Surface::repaint (const SRect &rect) {
    Matrix matrix;
    IRect clip;
    clipToScreen (this, matrix, clip);
    IRect scr = matrix.toScreen (rect);
    clip = clip.intersect (scr);
    if (!clip.isEmpty ())
        view_widget->scheduleRepaint (clip);
}

void Surface::repaint () {
    Surface *ps = parentNode ();
    if (ps)
        ps->repaint (bounds);
    else
        view_widget->scheduleRepaint (IRect (bounds.x (), bounds.y (),
                bounds.width (), bounds.height ()));
}

