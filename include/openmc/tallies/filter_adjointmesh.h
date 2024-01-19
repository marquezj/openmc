#ifndef OPENMC_TALLIES_FILTER_ADJOINTMESH_H
#define OPENMC_TALLIES_FILTER_ADJOINTMESH_H

#include <cstdint>

#include "openmc/position.h"
#include "openmc/tallies/filter_mesh.h"

namespace openmc {

//==============================================================================
//! Indexes the location of particle events to a regular mesh.  For tracklength
//! tallies, it will produce multiple valid bins and the bin weight will
//! correspond to the fraction of the track length that lies in that bin.
//==============================================================================

class AdjointMeshFilter : public MeshFilter {
public:
  //----------------------------------------------------------------------------
  // Constructors, destructors

  ~AdjointMeshFilter() = default;

  //----------------------------------------------------------------------------
  // Methods

  std::string type_str() const override { return "mesh"; }
  FilterType type() const override { return FilterType::ADJOINTMESH; }

  void get_all_bins(const Particle& p, TallyEstimator estimator,
    FilterMatch& match) const override;

protected:
  //----------------------------------------------------------------------------
  // Data members

  int32_t mesh_;            //!< Index of the mesh
  bool translated_ {false}; //!< Whether or not the filter is translated
  Position translation_ {0.0, 0.0, 0.0}; //!< Filter translation
};

} // namespace openmc
#endif // OPENMC_TALLIES_FILTER_ADJOINTMESH_H
