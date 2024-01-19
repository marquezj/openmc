#include "openmc/tallies/filter_adjointmesh.h"

#include "openmc/capi.h"
#include "openmc/mesh.h"

namespace openmc {

void AdjointMeshFilter::get_all_bins(
  const Particle& p, TallyEstimator estimator, FilterMatch& match) const
{
  for (auto r: p.r_history()){
      // apply translation if present
      if (translated_) r -= translation();
      auto bin = model::meshes[mesh_]->get_bin(r);
      if (bin >= 0) {
        match.bins_.push_back(bin);
        match.weights_.push_back(1.0);
      }
  }
}

//==============================================================================
// C-API functions
//==============================================================================

extern "C" int openmc_adjointmesh_filter_get_mesh(int32_t index, int32_t* index_mesh)
{
  if (!index_mesh) {
    set_errmsg("Mesh index argument is a null pointer.");
    return OPENMC_E_INVALID_ARGUMENT;
  }

  // Make sure this is a valid index to an allocated filter.
  if (int err = verify_filter(index))
    return err;

  // Get a pointer to the filter and downcast.
  const auto& filt_base = model::tally_filters[index].get();
  auto* filt = dynamic_cast<AdjointMeshFilter*>(filt_base);

  // Check the filter type.
  if (!filt) {
    set_errmsg("Tried to get mesh on a non-mesh filter.");
    return OPENMC_E_INVALID_TYPE;
  }

  // Output the mesh.
  *index_mesh = filt->mesh();
  return 0;
}

extern "C" int openmc_adjointmesh_filter_set_mesh(int32_t index, int32_t index_mesh)
{
  // Make sure this is a valid index to an allocated filter.
  if (int err = verify_filter(index))
    return err;

  // Get a pointer to the filter and downcast.
  const auto& filt_base = model::tally_filters[index].get();
  auto* filt = dynamic_cast<AdjointMeshFilter*>(filt_base);

  // Check the filter type.
  if (!filt) {
    set_errmsg("Tried to set mesh on a non-mesh filter.");
    return OPENMC_E_INVALID_TYPE;
  }

  // Check the mesh index.
  if (index_mesh < 0 || index_mesh >= model::meshes.size()) {
    set_errmsg("Index in 'meshes' array is out of bounds.");
    return OPENMC_E_OUT_OF_BOUNDS;
  }

  // Update the filter.
  filt->set_mesh(index_mesh);
  return 0;
}

extern "C" int openmc_adjointmesh_filter_get_translation(
  int32_t index, double translation[3])
{
  // Make sure this is a valid index to an allocated filter
  if (int err = verify_filter(index))
    return err;

  // Check the filter type
  const auto& filter = model::tally_filters[index];
  if (filter->type() != FilterType::MESH &&
      filter->type() != FilterType::MESH_SURFACE &&
      filter->type() != FilterType::ADJOINTMESH) {
    set_errmsg("Tried to get a translation from a non-mesh-based filter.");
    return OPENMC_E_INVALID_TYPE;
  }

  // Get translation from the mesh filter and set value
  auto mesh_filter = dynamic_cast<AdjointMeshFilter*>(filter.get());
  const auto& t = mesh_filter->translation();
  for (int i = 0; i < 3; i++) {
    translation[i] = t[i];
  }

  return 0;
}

extern "C" int openmc_adjointmesh_filter_set_translation(
  int32_t index, double translation[3])
{
  // Make sure this is a valid index to an allocated filter
  if (int err = verify_filter(index))
    return err;

  const auto& filter = model::tally_filters[index];
  // Check the filter type
  if (filter->type() != FilterType::MESH &&
      filter->type() != FilterType::MESH_SURFACE&&
      filter->type() != FilterType::ADJOINTMESH) {
    set_errmsg("Tried to set mesh on a non-mesh-based filter.");
    return OPENMC_E_INVALID_TYPE;
  }

  // Get a pointer to the filter and downcast
  auto mesh_filter = dynamic_cast<AdjointMeshFilter*>(filter.get());

  // Set the translation
  mesh_filter->set_translation(translation);

  return 0;
}

} // namespace openmc
