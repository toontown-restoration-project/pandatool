// Filename: cLwoSurface.h
// Created by:  drose (25Apr01)
// 
////////////////////////////////////////////////////////////////////

#ifndef CLWOSURFACE_H
#define CLWOSURFACE_H

#include <pandatoolbase.h>

#include <lwoSurface.h>
#include <luse.h>
#include <eggTexture.h>
#include <vector_PT_EggVertex.h>

#include <map>

class CLwoSurfaceBlock;
class LwoToEggConverter;
class LwoSurfaceBlock;
class EggPrimitive;

////////////////////////////////////////////////////////////////////
// 	 Class : CLwoSurface
// Description : This class is a wrapper around LwoSurface and stores
//               additional information useful during the
//               conversion-to-egg process.
////////////////////////////////////////////////////////////////////
class CLwoSurface {
public:
  CLwoSurface(LwoToEggConverter *converter, const LwoSurface *surface);
  ~CLwoSurface();

  INLINE const string &get_name() const;

  void apply_properties(EggPrimitive *egg_prim,
			vector_PT_EggVertex &egg_vertices,
			float &smooth_angle);
  bool check_texture();

  INLINE bool has_uvs() const;

  enum Flags {
    F_color        = 0x0001,
    F_diffuse      = 0x0002,
    F_luminosity   = 0x0004,
    F_specular     = 0x0008,
    F_reflection   = 0x0010,
    F_transparency = 0x0020,
    F_translucency = 0x0040,
    F_smooth_angle = 0x0080,
    F_backface     = 0x0100,
  };
  
  int _flags;
  RGBColorf _color;
  float _diffuse;
  float _luminosity;
  float _specular;
  float _reflection;
  float _transparency;
  float _translucency;
  float _smooth_angle;
  bool _backface;

  LwoToEggConverter *_converter;
  CPT(LwoSurface) _surface;

  bool _checked_texture;
  PT(EggTexture) _egg_texture;
  bool _has_uvs;

  CLwoSurfaceBlock *_block;

private:
  LPoint2d get_uv(const LPoint3d &pos, const LPoint3d &centroid) const;
  void generate_uvs(vector_PT_EggVertex &egg_vertices);
};

#include "cLwoSurface.I"

#endif


