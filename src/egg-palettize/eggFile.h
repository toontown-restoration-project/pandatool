// Filename: eggFile.h
// Created by:  drose (28Nov00)
// 
////////////////////////////////////////////////////////////////////

#ifndef EGGFILE_H
#define EGGFILE_H

#include <pandatoolbase.h>

#include "paletteGroups.h"
#include "textureReference.h"

#include <filename.h>
#include <namable.h>
#include <typedWriteable.h>

#include <set>

class SourceTextureImage;
class EggData;
class TextureImage;

////////////////////////////////////////////////////////////////////
// 	 Class : EggFile
// Description : This represents a single egg file known to the
//               palettizer.  It may reference a number of textures,
//               and may also be assigned to a number of groups.  All
//               of its textures will try to assign themselves to one
//               of its groups.
////////////////////////////////////////////////////////////////////
class EggFile : public TypedWriteable, public Namable {
public:
  EggFile();

  void from_command_line(EggData *data,
			 const Filename &source_filename, 
			 const Filename &dest_filename);

  void scan_textures();
  void get_textures(set<TextureImage *> &result) const;

  void post_txa_file();

  const PaletteGroups &get_groups() const;

  void build_cross_links();
  void choose_placements();

  void update_egg();

  void write_texture_refs(ostream &out, int indent_level = 0) const;

private:
  EggData *_data;
  Filename _source_filename;
  Filename _dest_filename;

  typedef vector<TextureReference *> Textures;
  Textures _textures;

  PaletteGroups _assigned_groups;


  // The TypedWriteable interface follows.
public:
  static void register_with_read_factory();
  virtual void write_datagram(BamWriter *writer, Datagram &datagram); 
  virtual int complete_pointers(vector_typedWriteable &plist, 
				BamReader *manager);

protected:
  static TypedWriteable *make_EggFile(const FactoryParams &params);
  void fillin(DatagramIterator &scan, BamReader *manager);

private:
  // This value is only filled in while reading from the bam file;
  // don't use it otherwise.
  int _num_textures;

public:
  static TypeHandle get_class_type() {
    return _type_handle;
  }
  static void init_type() {
    TypedWriteable::init_type();
    Namable::init_type();
    register_type(_type_handle, "EggFile",
		  TypedWriteable::get_class_type(),
		  Namable::get_class_type());
  }
  virtual TypeHandle get_type() const {
    return get_class_type();
  }

private:
  static TypeHandle _type_handle;

  friend class TxaLine;
};

#endif
