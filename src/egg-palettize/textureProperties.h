// Filename: textureProperties.h
// Created by:  drose (28Nov00)
// 
////////////////////////////////////////////////////////////////////

#ifndef TEXTUREPROPERTIES_H
#define TEXTUREPROPERTIES_H

#include <pandatoolbase.h>

#include <eggTexture.h>
#include <typedWriteable.h>

class PNMFileType;

////////////////////////////////////////////////////////////////////
// 	 Class : TextureProperties
// Description : This is the set of characteristics of a texture that,
//               if different from another texture, prevent the two
//               textures from sharing a PaletteImage.  It includes
//               properties such as mipmapping, number of channels,
//               etc.
////////////////////////////////////////////////////////////////////
class TextureProperties : public TypedWriteable {
public:
  TextureProperties();
  TextureProperties(const TextureProperties &copy);
  void operator = (const TextureProperties &copy);

  bool has_num_channels() const;
  int get_num_channels() const;
  bool uses_alpha() const;

  string get_string() const;
  void update_properties(const TextureProperties &other);
  void fully_define();

  void update_egg_tex(EggTexture *egg_tex) const;

  bool operator < (const TextureProperties &other) const;
  bool operator == (const TextureProperties &other) const;
  bool operator != (const TextureProperties &other) const;

  bool _got_num_channels;
  int _num_channels;
  EggTexture::Format _format;
  EggTexture::FilterType _minfilter, _magfilter;
  PNMFileType *_color_type;
  PNMFileType *_alpha_type;

private:
  static string get_format_string(EggTexture::Format format);
  static string get_filter_string(EggTexture::FilterType filter_type);
  static string get_type_string(PNMFileType *color_type, 
				PNMFileType *alpha_type);

  static EggTexture::Format union_format(EggTexture::Format a,
					 EggTexture::Format b);
  static EggTexture::FilterType union_filter(EggTexture::FilterType a,
					     EggTexture::FilterType b);
  // The TypedWriteable interface follows.
public:
  static void register_with_read_factory();
  virtual void write_datagram(BamWriter *writer, Datagram &datagram); 
  virtual int complete_pointers(vector_typedWriteable &plist, 
				BamReader *manager);

protected:
  static TypedWriteable *make_TextureProperties(const FactoryParams &params);

public:
  void fillin(DatagramIterator &scan, BamReader *manager);

public:
  static TypeHandle get_class_type() {
    return _type_handle;
  }
  static void init_type() {
    TypedWriteable::init_type();
    register_type(_type_handle, "TextureProperties",
		  TypedWriteable::get_class_type());
  }
  virtual TypeHandle get_type() const {
    return get_class_type();
  }

private:
  static TypeHandle _type_handle;
};

#endif
