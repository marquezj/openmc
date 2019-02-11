#include "openmc/distribution_spatial.h"

#include "openmc/error.h"
#include "openmc/random_lcg.h"
#include "openmc/xml_interface.h"

namespace openmc {

//==============================================================================
// CartesianIndependent implementation
//==============================================================================

CartesianIndependent::CartesianIndependent(pugi::xml_node node)
{
  // Read distribution for x coordinate
  if (check_for_node(node, "x")) {
    pugi::xml_node node_dist = node.child("x");
    x_ = distribution_from_xml(node_dist);
  } else {
    // If no distribution was specified, default to a single point at x=0
    double x[] {0.0};
    double p[] {1.0};
    x_ = UPtrDist{new Discrete{x, p, 1}};
  }

  // Read distribution for y coordinate
  if (check_for_node(node, "y")) {
    pugi::xml_node node_dist = node.child("y");
    y_ = distribution_from_xml(node_dist);
  } else {
    // If no distribution was specified, default to a single point at y=0
    double x[] {0.0};
    double p[] {1.0};
    y_ = UPtrDist{new Discrete{x, p, 1}};
  }

  // Read distribution for z coordinate
  if (check_for_node(node, "z")) {
    pugi::xml_node node_dist = node.child("z");
    z_ = distribution_from_xml(node_dist);
  } else {
    // If no distribution was specified, default to a single point at z=0
    double x[] {0.0};
    double p[] {1.0};
    z_ = UPtrDist{new Discrete{x, p, 1}};
  }
}

Position CartesianIndependent::sample() const
{
  return {x_->sample(), y_->sample(), z_->sample()};
}

//==============================================================================
// CylindricalIndependent implementation
//==============================================================================

CylindricalIndependent::CylindricalIndependent(pugi::xml_node node)
{
  // Read distribution for r coordinate
  if (check_for_node(node, "r")) {
    pugi::xml_node node_dist = node.child("r");
    r_ = distribution_from_xml(node_dist);
  } else {
    // If no distribution was specified, default to a single point at r=0
    double x[] {0.0};
    double p[] {1.0};
    r_ = UPtrDist{new Discrete{x, p, 1}};
  }

  // Read distribution for theta coordinate
  if (check_for_node(node, "theta")) {
    pugi::xml_node node_dist = node.child("theta");
    theta_ = distribution_from_xml(node_dist);
  } else {
    // If no distribution was specified, default to a single point at theta=0
    double x[] {0.0};
    double p[] {1.0};
    theta_ = UPtrDist{new Discrete{x, p, 1}};
  }

  // Read distribution for z coordinate
  if (check_for_node(node, "z")) {
    pugi::xml_node node_dist = node.child("z");
    z_ = distribution_from_xml(node_dist);
  } else {
    // If no distribution was specified, default to a single point at z=0
    double x[] {0.0};
    double p[] {1.0};
    z_ = UPtrDist{new Discrete{x, p, 1}};
  }
}

Position CylindricalIndependent::sample() const
{
  double r;
  double theta;
  r = r_->sample();
  theta = theta_->sample();
  return {r*cos(theta), r*sin(theta), z_->sample()};
}

//==============================================================================
// SpatialBox implementation
//==============================================================================

SpatialBox::SpatialBox(pugi::xml_node node, bool fission)
  : only_fissionable_{fission}
{
  // Read lower-right/upper-left coordinates
  auto params = get_node_array<double>(node, "parameters");
  if (params.size() != 6)
    openmc::fatal_error("Box/fission spatial source must have six "
                        "parameters specified.");

  lower_left_ = Position{params[0], params[1], params[2]};
  upper_right_ = Position{params[3], params[4], params[5]};
}

Position SpatialBox::sample() const
{
  Position xi {prn(), prn(), prn()};
  return lower_left_ + xi*(upper_right_ - lower_left_);
}

//==============================================================================
// SpatialPoint implementation
//==============================================================================

SpatialPoint::SpatialPoint(pugi::xml_node node)
{
  // Read location of point source
  auto params = get_node_array<double>(node, "parameters");
  if (params.size() != 3)
    openmc::fatal_error("Point spatial source must have three "
                        "parameters specified.");

  // Set position
  r_ = Position{params.data()};
}

Position SpatialPoint::sample() const
{
  return r_;
}

} // namespace openmc
