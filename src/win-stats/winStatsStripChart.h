// Filename: winStatsStripChart.h
// Created by:  drose (03Dec03)
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

#ifndef WINSTATSSTRIPCHART_H
#define WINSTATSSTRIPCHART_H

#include "pandatoolbase.h"

#include "winStatsGraph.h"
#include "pStatStripChart.h"
#include "pointerTo.h"

#include <windows.h>

class WinStatsMonitor;

////////////////////////////////////////////////////////////////////
//       Class : WinStatsStripChart
// Description : 
////////////////////////////////////////////////////////////////////
class WinStatsStripChart : public PStatStripChart, public WinStatsGraph {
public:
  WinStatsStripChart(WinStatsMonitor *monitor,
                     PStatView &view, int collector_index);
  virtual ~WinStatsStripChart();

  virtual void new_collector(int collector_index);
  virtual void new_data(int thread_index, int frame_number);

protected:
  virtual void update_labels();

  virtual void clear_region();
  virtual void copy_region(int start_x, int end_x, int dest_x);
  virtual void draw_slice(int x, int frame_number);
  virtual void draw_empty(int x);
  virtual void draw_cursor(int x);
  virtual void end_draw(int from_x, int to_x);

private:
  void create_window();
  static void register_window_class(HINSTANCE application);

  static LONG WINAPI static_window_proc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
  LONG WINAPI window_proc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

  int _brush_origin;

  static bool _window_class_registered;
  static const char * const _window_class_name;
};

#endif
