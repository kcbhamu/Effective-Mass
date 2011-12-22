#ifndef _MATRIX_TERM_HH
#define _MATRIX_TERM_HH

#include <gsl/gsl_matrix.h>

// Base class for matrix terms.  It needs to be able to generate a
// matrix based on the scale granularity parameters.
class matrix_term
{
public:
  matrix_term();
  ~matrix_term();

  // Build a matrix out of implementation-defined blocks.
  // Deallocating this matrix is the caller's responsibility.  Min and
  // max set minimum and maximum scales in multiples of the inverse
  // effective Bohr radius.  Num basis states will be used scaling
  // between these values in a geometric progression.
  gsl_matrix_complex *matrix(double min, double max, size_t num);

  // Getters and setters for dielectric constant and inverse effective
  // Bohr radius
  double get_inv_radius();
  double set_inv_radius(double r);
  double get_dielectric_constant();
  double set_dielectric_constant(double k);
protected:
  // Return a matrix block based on scaling arguments and internal state.
  virtual gsl_matrix_complex *matrix_block(double a1, double a2) = 0;

  // Inheriting classes should override this with clean-up code.
  virtual void on_delete();

  // Inverse effective Bohr radius, in units of 1 / Bohr radius.  The
  // crystal term should calculate this in the constructor, and the
  // caller is responsible for copying it to the other terms.
  double inv_radius;

  // Dielectric constant.  This should be passed into the crystal term
  // through the constructor, then passed to the impurity term by the
  // caller.
  double dielectric;
};

// the basic types of matrix terms
class crystal_term   : public matrix_term {};
class impurity_term : public matrix_term {};
class overlap_term   : public matrix_term {};

// terms using basis functions with radial exponential decay

// zinc-blend crystal term
class exp_zb : public crystal_term
{
public:
  exp_zb(double g1, double g2, double g3, double d0, double dielectric);
protected:
  gsl_matrix_complex *matrix_block(double a1, double a2);
  double g1;
  double g2;
  double g3;
  double d0;
};

// exponential overlap term
class exp_overlap : public overlap_term
{
protected:
  gsl_matrix_complex *matrix_block(double a1, double a2);
};


#endif // _MATRIX_TERM_HH
