// Filename: pStatViewLevel.cxx
// Created by:  drose (11Jul00)
// 
////////////////////////////////////////////////////////////////////

#include "pStatViewLevel.h"
#include "pStatClientData.h"

#include <pStatCollectorDef.h>
#include <notify.h>

#include <algorithm>

////////////////////////////////////////////////////////////////////
//     Function: PStatViewLevel::get_net_value
//       Access: Public
//  Description: Returns the total level value (or elapsed time)
//               represented by this Collector, including all values
//               in its child Collectors.
////////////////////////////////////////////////////////////////////
float PStatViewLevel::
get_net_value() const {
  float net = _value_alone;

  Children::const_iterator ci;
  for (ci = _children.begin(); ci != _children.end(); ++ci) {
    net += (*ci)->get_net_value();
  }

  return net;
}


// STL function object for sorting children in order by the
// collector's sort index, used in sort_children(), below.
class SortCollectorLevels {
public:
  SortCollectorLevels(const PStatClientData *client_data) :
    _client_data(client_data) {
  }
  bool operator () (const PStatViewLevel *a, const PStatViewLevel *b) const {
    // By casting the sort numbers to unsigned ints, we cheat and make
    // -1 appear to be a very large positive integer, thus placing
    // collectors with a -1 sort value at the very end.
    return 
      (unsigned int)_client_data->get_collector_def(a->get_collector())._sort <
      (unsigned int)_client_data->get_collector_def(b->get_collector())._sort;
  }
  const PStatClientData *_client_data;
};

////////////////////////////////////////////////////////////////////
//     Function: PStatViewLevel::sort_children
//       Access: Public
//  Description: Sorts the children of this view level into order as
//               specified by the client's sort index.
////////////////////////////////////////////////////////////////////
void PStatViewLevel::
sort_children(const PStatClientData *client_data) {
  SortCollectorLevels sort_levels(client_data);

  sort(_children.begin(), _children.end(), sort_levels);
}

////////////////////////////////////////////////////////////////////
//     Function: PStatViewLevel::get_num_children
//       Access: Public
//  Description: Returns the number of children of this
//               Level/Collector.  These are the Collectors whose
//               value is considered to be part of the total value of
//               this level's Collector.
////////////////////////////////////////////////////////////////////
int PStatViewLevel::
get_num_children() const {
  return _children.size();
}

////////////////////////////////////////////////////////////////////
//     Function: PStatViewLevel::get_child
//       Access: Public
//  Description: Returns the nth child of this Level/Collector.
////////////////////////////////////////////////////////////////////
const PStatViewLevel *PStatViewLevel::
get_child(int n) const {
  nassertr(n >= 0 && n < (int)_children.size(), NULL);
  return _children[n];
}
