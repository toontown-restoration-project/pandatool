// Filename: pathReplace.h
// Created by:  drose (07Feb03)
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

#ifndef PATHREPLACE_H
#define PATHREPLACE_H

#include "pandatoolbase.h"
#include "pathStore.h"
#include "referenceCount.h"
#include "globPattern.h"
#include "filename.h"
#include "dSearchPath.h"
#include "pvector.h"

////////////////////////////////////////////////////////////////////
//       Class : PathReplace
// Description : This encapsulates the user's command-line request to
//               replace existing, incorrect pathnames to models and
//               textures from a file with correct pathnames.  It
//               corresponds to a sequence of -pr command-line
//               options, as well as the -pp option.
//
//               This can also go the next step, which is to convert a
//               known file into a suitable form for storing in a
//               model file.  In this capacity, it corresponds to the
//               -ps and -pd options.
////////////////////////////////////////////////////////////////////
class PathReplace : public ReferenceCount {
public:
  PathReplace();
  ~PathReplace();

  INLINE void clear();
  INLINE void add_pattern(const string &orig_prefix, const string &replacement_prefix);

  INLINE int get_num_patterns() const;
  INLINE const string &get_orig_prefix(int n) const;
  INLINE const string &get_replacement_prefix(int n) const;

  INLINE bool is_empty() const;

  Filename match_path(const Filename &orig_filename, 
                      const DSearchPath &additional_path = DSearchPath());
  Filename store_path(const Filename &orig_filename);

  INLINE Filename convert_path(const Filename &orig_filename,
                               const DSearchPath &additional_path = DSearchPath());

  void write(ostream &out, int indent_level = 0) const;

public:
  // This is used (along with _entries) to support match_path().
  DSearchPath _path;

  // These are used to support store_path().
  PathStore _path_store;
  Filename _path_directory;

private:
  class Component {
  public:
    INLINE Component(const string &component);
    INLINE Component(const Component &copy);
    INLINE void operator = (const Component &copy);

    GlobPattern _orig_prefix;
    bool _double_star;
  };
  typedef pvector<Component> Components;

  class Entry {
  public:
    Entry(const string &orig_prefix, const string &replacement_prefix);
    INLINE Entry(const Entry &copy);
    INLINE void operator = (const Entry &copy);

    bool try_match(const Filename &filename, Filename &new_filename) const;
    size_t r_try_match(const vector_string &components, size_t oi, size_t ci) const;

    string _orig_prefix;
    Components _orig_components;
    bool _is_local;
    string _replacement_prefix;
  };

  typedef pvector<Entry> Entries;
  Entries _entries;
};

#include "pathReplace.I"

#endif

  
  