#ifndef OPENMC_PARTICLE_H
#define OPENMC_PARTICLE_H

//! \file particle.h
//! \brief Particle type

#include <array>
#include <cstdint>
#include <sstream>
#include <string>

#include "openmc/capi.h"

namespace openmc {

//==============================================================================
// Constants
//==============================================================================

// Since cross section libraries come with different numbers of delayed groups
// (e.g. ENDF/B-VII.1 has 6 and JEFF 3.1.1 has 8 delayed groups) and we don't
// yet know what cross section library is being used when the tallies.xml file
// is read in, we want to have an upper bound on the size of the array we
// use to store the bins for delayed group tallies.
constexpr int MAX_DELAYED_GROUPS {8};

// Maximum number of secondary particles created
constexpr int MAX_SECONDARY {1000};

// Maximum number of lost particles
constexpr int MAX_LOST_PARTICLES {10};

// Maximum number of lost particles, relative to the total number of particles
constexpr double REL_MAX_LOST_PARTICLES {1.0e-6};

//! Particle types
enum class ParticleType {
  neutron, photon, electron, positron
};

struct LocalCoord {
  int cell {-1};
  int universe {-1};
  int lattice {-1};
  int lattice_x {-1};
  int lattice_y {-1};
  int lattice_z {-1};
  double xyz[3]; //!< particle position
  double uvw[3]; //!< particle direction
  bool rotated {false};  //!< Is the level rotated?

  //! clear data from a single coordinate level
  void reset();
};

//============================================================================
//! State of a particle being transported through geometry
//============================================================================

class Particle {
public:
  int64_t id_;  //!< Unique ID
  int type_;    //!< Particle type (n, p, e, etc.)

  int n_coord_;                  //!< number of current coordinate levels
  int cell_instance_;            //!< offset for distributed properties
  LocalCoord coord_[MAX_COORD];  //!< coordinates for all levels

  // Particle coordinates before crossing a surface
  int last_n_coord_;          //!< number of current coordinates
  int last_cell_[MAX_COORD];  //!< coordinates for all levels

  // Energy data
  double E_;       //!< post-collision energy in eV
  double last_E_;  //!< pre-collision energy in eV
  int g_;          //!< post-collision energy group (MG only)
  int last_g_;     //!< pre-collision energy group (MG only)

  // Other physical data
  double wgt_;     //!< particle weight
  double mu_;      //!< angle of scatter
  bool alive_;     //!< is particle alive?

  // Other physical data
  double last_xyz_current_[3];  //!< coordinates of the last collision or
                                //!< reflective/periodic surface crossing for
                                //!< current tallies
  double last_xyz_[3];          //!< previous coordinates
  double last_uvw_[3];          //!< previous direction coordinates
  double last_wgt_;             //!< pre-collision particle weight
  double absorb_wgt_;           //!< weight absorbed for survival biasing

  // What event took place
  bool fission_;       //!< did particle cause implicit fission
  int event_;          //!< scatter, absorption
  int event_nuclide_;  //!< index in nuclides array
  int event_mt_;       //!< reaction MT
  int delayed_group_;  //!< delayed group

  // Post-collision physical data
  int n_bank_;        //!< number of fission sites banked
  double wgt_bank_;   //!< weight of fission sites banked
  int n_delayed_bank_[MAX_DELAYED_GROUPS];  //!< number of delayed fission
                                            //!< sites banked

  // Indices for various arrays
  int surface_;        //!< index for surface particle is on
  int cell_born_;      //!< index for cell particle was born in
  int material_;       //!< index for current material
  int last_material_;  //!< index for last material

  // Temperature of current cell
  double sqrtkT_;       //!< sqrt(k_Boltzmann * temperature) in eV
  double last_sqrtkT_;  //!< last temperature

  // Statistical data
  int n_collision_;  //!< number of collisions

// Track output
  bool write_track_ {false};

  // Secondary particles created
  int64_t n_secondary_ {};
  Bank secondary_bank_[MAX_SECONDARY];

  //! resets all coordinate levels for the particle
  void clear();

  //! create a secondary particle
  //
  //! stores the current phase space attributes of the particle in the
  //! secondary bank and increments the number of sites in the secondary bank.
  //! \param uvw Direction of the secondary particle
  //! \param E Energy of the secondary particle in [eV]
  //! \param type Particle type
  //! \param run_CE Whether continuous-energy data is being used
  void create_secondary(const double* uvw, double E, int type, bool run_CE);

  //! sets default attributes for a particle
  void initialize();

  //! initialize from a source site
  //
  //! initializes a particle from data stored in a source site. The source
  //! site may have been produced from an external source, from fission, or
  //! simply as a secondary particle.
  //! \param src Source site data
  void from_source(const Bank* src);

  //! Transport a particle from birth to death
  void transport();

  //! Cross a surface and handle boundary conditions
  void cross_surface();

  //! mark a particle as lost and create a particle restart file
  //! \param message A warning message to display
  void mark_as_lost(const char* message);

  void mark_as_lost(const std::string& message)
  {mark_as_lost(message.c_str());}

  void mark_as_lost(const std::stringstream& message)
  {mark_as_lost(message.str());}

  //! create a particle restart HDF5 file
  void write_restart() const;
};

} // namespace openmc

#endif // OPENMC_PARTICLE_H
