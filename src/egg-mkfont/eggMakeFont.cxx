// Filename: eggMakeFont.cxx
// Created by:  drose (16Feb01)
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

#include "eggMakeFont.h"
#include "rangeIterator.h"
#include "pnmTextMaker.h"
#include "pnmTextGlyph.h"
#include "eggGroup.h"
#include "eggPoint.h"
#include "eggPolygon.h"
#include "eggTexture.h"
#include "eggVertexPool.h"
#include "eggVertex.h"
#include "string_utils.h"

////////////////////////////////////////////////////////////////////
//     Function: EggMakeFont::Constructor
//       Access: Public
//  Description:
////////////////////////////////////////////////////////////////////
EggMakeFont::
EggMakeFont() : EggWriter(true, false) {
  set_program_description
    ("egg-mkfont uses the FreeType library to generate an egg file "
     "and a series of texture images from a font file "
     "input, such as a TTF file.  The resulting egg file "
     "can be loaded in Panda as a StaticTextFont object for "
     "rendering text, even if FreeType is not compiled into "
     "the executing Panda.\n\n"

     "It is strongly recommended that the resulting egg file "
     "be subsequently passed through egg-palettize to consolidate the many "
     "generated texture images into a single texture image to "
     "improve rendering performance.  This can also reduce the "
     "texture images to achieve antialiasing.");

  clear_runlines();
  add_runline("[opts] -o output.egg font");
  add_runline("[opts] font output.egg");

  add_option
    ("i", "pattern", 0,
     "The pattern to be used to generate the texture images.  This string "
     "will be passed to sprintf to generate the actual file name; it "
     "should contain the string %d or %x (or some variant such as %03d) "
     "which will be filled in with the Unicode number of each symbol.  "
     "If it is omitted, the default is based on the name of the egg file.",
     &EggMakeFont::dispatch_string, NULL, &_output_image_pattern);

  add_option
    ("fg", "r,g,b[,a]", 0,
     "Specifies the foreground color of the generated texture map.  The "
     "default is white: 1,1,1,1, which leads to the most flexibility "
     "as the color can be modulated at runtime to any suitable color.",
     &EggMakeFont::dispatch_color, NULL, &_fg[0]);

  add_option
    ("bg", "r,g,b[,a]", 0,
     "Specifies the background color of the generated texture map.  The "
     "default is transparent: 1,1,1,0, which allows the text to be "
     "visible against any color background by placing a polygon of a "
     "suitable color behind it.  If the alpha component of either -fg "
     "or -bg is not 1, the generated texture images will include an "
     "alpha component; if both colors specify an alpha component of 1 "
     "(or do not specify an alpha compenent), then the generated images "
     "will not include an alpha component.",
     &EggMakeFont::dispatch_color, NULL, &_bg[0]);

  add_option
    ("interior", "r,g,b[,a]", 0,
     "Specifies the color to render the interior part of a hollow font.  "
     "This is a special effect that involves analysis of the bitmap after "
     "the font has been rendered, and so is more effective when the pixel "
     "size is large.  It also implies -noaa (but you can use a scale "
     "factor with -sf to achieve antialiasing).",
     &EggMakeFont::dispatch_color, &_got_interior, &_interior[0]);

  add_option
    ("chars", "range", 0,
     "Specifies the characters of the font that are used.  The range "
     "specification may include combinations of decimal or hex unicode "
     "values (where hex values are identified with a leading 0x), separated "
     "by commas and hyphens to indicate ranges, e.g. '32-126,0xfa0-0xfff'.  "
     "It also may specify ranges of ASCII characters by enclosing them "
     "within square brackets, e.g. '[A-Za-z0-9]'.  If this is not specified, "
     "the default is the set of ASCII characters.",
     &EggMakeFont::dispatch_range, NULL, &_range);

  add_option
    ("ppu", "pixels", 0,
     "Specify the pixels per unit.  This is the number of pixels in the "
     "generated texture map that are used for each onscreen unit (or each "
     "10 points of font; see -ps).  Setting this number larger results in "
     "an easier-to-read font, but at the cost of more texture memory.",
     &EggMakeFont::dispatch_double, NULL, &_pixels_per_unit);

  add_option
    ("ps", "size", 0,
     "Specify the point size of the resulting font.  This controls the "
     "apparent size of the font when it is rendered onscreen.  By convention, "
     "a 10 point font is 1 screen unit high.",
     &EggMakeFont::dispatch_double, NULL, &_point_size);

  add_option
    ("pm", "n", 0,
     "The number of extra pixels around a single character in the "
     "generated polygon.  This may be a floating-point number.",
     &EggMakeFont::dispatch_double, NULL, &_poly_margin);

  add_option
    ("tm", "n", 0,
     "The number of extra pixels around each character in the texture map.  "
     "This may only be an integer.",
     &EggMakeFont::dispatch_int, NULL, &_tex_margin);

  add_option
    ("sf", "factor", 0,
     "The scale factor of the generated image.  This is the factor by which "
     "the font image is generated oversized, then reduced to its final size, "
     "to improve antialiasing.  If the specified font contains one "
     "or more fixed-size fonts instead of a scalable font, the scale factor "
     "may be automatically adjusted as necessary to scale the closest-"
     "matching font to the desired pixel size.",
     &EggMakeFont::dispatch_double, NULL, &_scale_factor);

  add_option
    ("nr", "", 0,
     "Don't actually reduce the images after applying the scale factor, but "
     "leave them at their inflated sizes.  Presumably you will reduce "
     "them later, for instance with egg-palettize.",
     &EggMakeFont::dispatch_none, &_no_reduce);

  add_option
    ("noaa", "", 0,
     "Disable low-level antialiasing by the Freetype library.  "
     "This is unrelated to the antialiasing that is applied due to the "
     "scale factor specified by -sf; you may have either one, neither, or "
     "both kinds of antialiasing enabled.",
     &EggMakeFont::dispatch_none, &_no_native_aa);

  add_option
    ("face", "index", 0,
     "Specify the face index of the particular face within the font file "
     "to use.  Some font files contain multiple faces, indexed beginning "
     "at 0.  The default is face 0.",
     &EggMakeFont::dispatch_int, NULL, &_face_index);

  _fg.set(1.0, 1.0, 1.0, 1.0);
  _bg.set(1.0, 1.0, 1.0, 0.0);
  _interior.set(1.0, 1.0, 1.0, 0.0);
  _pixels_per_unit = 30.0;
  _point_size = 10.0;
  _poly_margin = 1.0;
  _tex_margin = 2;
  _scale_factor = 2.0;
  _face_index = 0;

  _text_maker = NULL;
  _vpool = NULL;
  _group = NULL;
}


////////////////////////////////////////////////////////////////////
//     Function: EggMakeFont::handle_args
//       Access: Protected, Virtual
//  Description: Does something with the additional arguments on the
//               command line (after all the -options have been
//               parsed).  Returns true if the arguments are good,
//               false otherwise.
////////////////////////////////////////////////////////////////////
bool EggMakeFont::
handle_args(ProgramBase::Args &args) {
  if (args.empty()) {
    nout << "Must specify name of font file on command line.\n";
    return false;
  }

  _input_font_filename = args[0];
  args.pop_front();
  return EggWriter::handle_args(args);
}

////////////////////////////////////////////////////////////////////
//     Function: EggMakeFont::run
//       Access: Public
//  Description:
////////////////////////////////////////////////////////////////////
void EggMakeFont::
run() {
  _text_maker = new PNMTextMaker(_input_font_filename, _face_index);
  if (!_text_maker->is_valid()) {
    exit(1);
  }

  if (_no_reduce) {
    _tex_margin *= _scale_factor;
    _poly_margin *= _scale_factor;
    _pixels_per_unit *= _scale_factor;
    _scale_factor = 1.0;
  }

  _text_maker->set_point_size(_point_size);
  _text_maker->set_native_antialias(!_no_native_aa && !_got_interior);
  _text_maker->set_interior_flag(_got_interior);
  _text_maker->set_pixels_per_unit(_pixels_per_unit);
  _text_maker->set_scale_factor(_scale_factor);
  
  if (_range.is_empty()) {
    // If there's no specified range, the default is the entire ASCII
    // set.
    _range.add_range(0x20, 0x7e);
  }
  if (_output_image_pattern.empty()) {
    // Create a default texture filename pattern.
    _output_image_pattern = get_output_filename().get_fullpath_wo_extension() + "%03d.rgb";
  }

  // Figure out how many channels we need based on the foreground and
  // background colors.
  bool needs_alpha = (_fg[3] != 1.0 || _bg[3] != 1.0 || _interior[3] != 1.0);
  bool needs_color = (_fg[0] != _fg[1] || _fg[1] != _fg[2] ||
                      _bg[0] != _bg[1] || _bg[1] != _bg[2] ||
                      _interior[0] != _interior[1] || _interior[1] != _interior[2]);

  if (needs_alpha) {
    if (needs_color) {
      _num_channels = 4;
      _format = EggTexture::F_rgba;
    } else {
      if (_fg[0] == 1.0 && _bg[0] == 1.0 && _interior[0] == 1.0) {
        // A special case: we only need an alpha channel.  Copy the
        // alpha data into the color channels so we can write out a
        // one-channel image.
        _fg[0] = _fg[1] = _fg[2] = _fg[3];
        _bg[0] = _bg[1] = _bg[2] = _bg[3];
        _num_channels = 1;
        _format = EggTexture::F_alpha;
      } else {
        _num_channels = 2;
        _format = EggTexture::F_luminance_alpha;
      }
    }
  } else {
    if (needs_color) {
      _num_channels = 3;
      _format = EggTexture::F_rgb;
    } else {
      _num_channels = 1;
      _format = EggTexture::F_luminance;
    }
  }      

  _group = new EggGroup();
  _data.add_child(_group);

  _vpool = new EggVertexPool("vpool");
  _group->add_child(_vpool);

  // Make the group a sequence, as a convenience.  If we view the
  // egg file directly we can see all the characters one at a time.
  _group->set_switch_flag(true);
  _group->set_switch_fps(2.0);

  // Also create an egg group indicating the font's design size.
  EggGroup *ds_group = new EggGroup("ds");
  _group->add_child(ds_group);
  EggVertex *vtx = make_vertex(LPoint2d(0.0, _text_maker->get_line_height()));
  EggPoint *point = new EggPoint;
  ds_group->add_child(point);
  point->add_vertex(vtx);

  // Finally, add the characters, one at a time.
  RangeIterator ri(_range);
  do {
    add_character(ri.get_code());
  } while (ri.next());

  write_egg_file();
}

////////////////////////////////////////////////////////////////////
//     Function: EggMakeFont::dispatch_range
//       Access: Private, Static
//  Description: 
////////////////////////////////////////////////////////////////////
bool EggMakeFont::
dispatch_range(const string &, const string &arg, void *var) {
  RangeDescription *ip = (RangeDescription *)var;
  return ip->parse_parameter(arg);
}

////////////////////////////////////////////////////////////////////
//     Function: EggMakeFont::make_vertex
//       Access: Private
//  Description: Allocates and returns a new vertex from the vertex
//               pool representing the indicated 2-d coordinates.
////////////////////////////////////////////////////////////////////
EggVertex *EggMakeFont::
make_vertex(const LPoint2d &xy) {
  return
    _vpool->make_new_vertex(LPoint3d::origin(_coordinate_system) +
                            LVector3d::rfu(xy[0], 0.0, xy[1], _coordinate_system));
}

////////////////////////////////////////////////////////////////////
//     Function: EggMakeFont::add_character
//       Access: Private
//  Description: Generates the indicated character and adds it to the
//               font description.
////////////////////////////////////////////////////////////////////
void EggMakeFont::
add_character(int code) {
  PNMTextGlyph *glyph = _text_maker->get_glyph(code);
  if (glyph == (PNMTextGlyph *)NULL) {
    nout << "No definition in font for character " << code << ".\n";
    return;
  }

  make_geom(glyph, code);
}


////////////////////////////////////////////////////////////////////
//     Function: EggMakeFont::make_geom
//       Access: Private
//  Description: Creates the actual geometry for the glyph.
////////////////////////////////////////////////////////////////////
void EggMakeFont::
make_geom(PNMTextGlyph *glyph, int character) {
  // Create an egg group to hold the polygon.
  string group_name = format_string(character);
  EggGroup *group = new EggGroup(group_name);
  _group->add_child(group);

  if (glyph->get_width() != 0 && glyph->get_height() != 0) {
    int bitmap_top = glyph->get_top();
    int bitmap_left = glyph->get_left();
    double tex_x_size = glyph->get_width();
    double tex_y_size = glyph->get_height();
    
    double poly_margin = _poly_margin;
    double x_origin = _tex_margin;
    double y_origin = _tex_margin;
    double page_y_size = tex_y_size + _tex_margin * 2;
    double page_x_size = tex_x_size + _tex_margin * 2;
    
    // Determine the corners of the rectangle in geometric units.
    double tex_poly_margin = poly_margin / _pixels_per_unit;
    double origin_y = bitmap_top / _pixels_per_unit;
    double origin_x = bitmap_left / _pixels_per_unit;
    double top = origin_y + tex_poly_margin;
    double left = origin_x - tex_poly_margin;
    double bottom = origin_y - tex_y_size / _pixels_per_unit - tex_poly_margin;
    double right = origin_x + tex_x_size / _pixels_per_unit + tex_poly_margin;
    
    // And the corresponding corners in UV units.
    double uv_top = 1.0f - (double)(y_origin - poly_margin) / page_y_size;
    double uv_left = (double)(x_origin - poly_margin) / page_x_size;
    double uv_bottom = 1.0f - (double)(y_origin + poly_margin + tex_y_size) / page_y_size;
    double uv_right = (double)(x_origin + poly_margin + tex_x_size) / page_x_size;
    
    // Create the vertices for the polygon.
    EggVertex *v1 = make_vertex(LPoint2d(left, bottom));
    EggVertex *v2 = make_vertex(LPoint2d(right, bottom));
    EggVertex *v3 = make_vertex(LPoint2d(right, top));
    EggVertex *v4 = make_vertex(LPoint2d(left, top));
    
    v1->set_uv(TexCoordd(uv_left, uv_bottom));
    v2->set_uv(TexCoordd(uv_right, uv_bottom));
    v3->set_uv(TexCoordd(uv_right, uv_top));
    v4->set_uv(TexCoordd(uv_left, uv_top));
    
    EggPolygon *poly = new EggPolygon();
    group->add_child(poly);
    poly->set_texture(get_tref(glyph, character));

    poly->add_vertex(v1);
    poly->add_vertex(v2);
    poly->add_vertex(v3);
    poly->add_vertex(v4);
  }

  // Now create a single point where the origin of the next character
  // will be.

  EggVertex *v0 = make_vertex(LPoint2d(glyph->get_advance() / _pixels_per_unit, 0.0));
  EggPoint *point = new EggPoint;
  group->add_child(point);
  point->add_vertex(v0);
}

////////////////////////////////////////////////////////////////////
//     Function: EggMakeFont::get_tref
//       Access: Private
//  Description: Returns the egg texture reference for a particular
//               glyph, creating it if it has not already been
//               created.
////////////////////////////////////////////////////////////////////
EggTexture *EggMakeFont::
get_tref(PNMTextGlyph *glyph, int character) {
  TRefs::iterator ti = _trefs.find(glyph);
  if (ti != _trefs.end()) {
    return (*ti).second;
  }

  EggTexture *tref = make_tref(glyph, character);
  _trefs[glyph] = tref;
  return tref;
}

////////////////////////////////////////////////////////////////////
//     Function: EggMakeFont::make_tref
//       Access: Private
//  Description: Writes out the texture image for an indicated glyph,
//               and returns its egg reference.
////////////////////////////////////////////////////////////////////
EggTexture *EggMakeFont::
make_tref(PNMTextGlyph *glyph, int character) {
  char buffer[1024];
  sprintf(buffer, _output_image_pattern.c_str(), character);

  Filename texture_filename = buffer;
  PNMImage image(glyph->get_width() + _tex_margin * 2,
                 glyph->get_height() + _tex_margin * 2, _num_channels);
  image.fill(_bg[0], _bg[1], _bg[2]);
  if (image.has_alpha()) {
    image.alpha_fill(_bg[3]);
  }
  if (_got_interior) {
    glyph->place(image, -glyph->get_left() + _tex_margin, 
                 glyph->get_top() + _tex_margin, _fg, _interior);
  } else {
    glyph->place(image, -glyph->get_left() + _tex_margin, 
                 glyph->get_top() + _tex_margin, _fg);
  }

  if (!image.write(texture_filename)) {
    nout << "Unable to write " << texture_filename << "\n";
  }

  EggTexture *tref = 
    new EggTexture(texture_filename.get_basename_wo_extension(),
                   texture_filename);
  tref->set_format(_format);
  tref->set_wrap_mode(EggTexture::WM_clamp);
  tref->set_minfilter(EggTexture::FT_linear_mipmap_linear);
  tref->set_magfilter(EggTexture::FT_linear);

  return tref;
}

int main(int argc, char *argv[]) {
  EggMakeFont prog;
  prog.parse_command_line(argc, argv);
  prog.run();
  return 0;
}
