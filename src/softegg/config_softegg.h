// Filename: config_softegg.h
// Created by:  masad (25Sep03)
//
////////////////////////////////////////////////////////////////////
//
// PANDA 3D SOFTWARE
// Copyright (c) 2001, Disney Enterprises, Inc.  All rights reserved
//
// All use of this software is subject to the terms of the Panda 3d
// Software license.  You should have received a copy of this license
// along with this source code; you will also find a current copy of
// the license at http://www.panda3d.org/license.txt .
//
// To contact the maintainers of this program write to
// panda3d@yahoogroups.com .
//
////////////////////////////////////////////////////////////////////

#ifndef CONFIG_SOFTEGG_H
#define CONFIG_SOFTEGG_H

#include "pandatoolbase.h"
#include "notifyCategoryProxy.h"

NotifyCategoryDeclNoExport(softegg);

extern const bool soft_default_double_sided;
extern const bool soft_default_vertex_color;

extern void init_libsoftegg();

#endif