// Filename: lwoSurfaceBlock.h
// Created by:  drose (24Apr01)
// 
////////////////////////////////////////////////////////////////////

#ifndef LWOSURFACEBLOCK_H
#define LWOSURFACEBLOCK_H

#include <pandatoolbase.h>

#include "lwoGroupChunk.h"

////////////////////////////////////////////////////////////////////
// 	 Class : LwoSurfaceBlock
// Description : A texture layer or shader, part of a LwoSurface
//               chunk.
////////////////////////////////////////////////////////////////////
class LwoSurfaceBlock : public LwoGroupChunk {
public:

public:
  virtual bool read_iff(IffInputFile *in, size_t stop_at);
  virtual void write(ostream &out, int indent_level = 0) const;

  virtual IffChunk *make_new_chunk(IffInputFile *in, IffId id);

private:
  static TypeHandle _type_handle;
};

#endif

  

