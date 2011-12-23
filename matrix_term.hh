#ifndef _MATRIX_TERM_HH
#define _MATRIX_TERM_HH

#include <gsl/gsl_matrix.h>

#include "enum_elements.hh"

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
  virtual gsl_matrix_complex *matrix(double min, double max, size_t num);

  // Getters and setters for dielectric constant and inverse effective
  // Bohr radius
  double get_inv_radius();
  double set_inv_radius(double r);
  double get_dielectric_constant();
  double set_dielectric_constant(double k);
protected:
  // Callbacks to perform additional actions when parameters are set.
  // These are called before setting the new value, but the new value
  // is passed as an argument.
  virtual void on_set_inv_radius(double r);
  virtual void on_set_dielectric_constant(double k);

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

// wurtzite crystal term
class exp_wz : public crystal_term
{
public:
  exp_wz(double A1, double A2, double A3, double A4, double A5, double A6,
	 double d1, double d2, double d3, double dielectric);
protected:
  gsl_matrix_complex *matrix_block(double a1, double a2);
  double A1;
  double A2;
  double A3;
  double A4;
  double A5;
  double A6;
  double d1;
  double d2;
  double d3;
};

// generalized wurtzite crystal term
class exp_gwz : public crystal_term
{
public:
  exp_gwz(double A1, double A2, double A3, double B1, double B2, double B3,
	  double C1, double C2, double C3, double D1, double D2, double D3,
	  double d1c, double d2c, double d1so, double d2so, double d3so,
	  double dielectric);
protected:
  gsl_matrix_complex *matrix_block(double a1, double a2);
  double A1;
  double A2;
  double A3;
  double B1;
  double B2;
  double B3;
  double C1;
  double C2;
  double C3;
  double D1;
  double D2;
  double D3;
  double d1c;
  double d2c;
  double d1so;
  double d2so;
  double d3so;
};

// coulomb impurity term
class exp_coulomb : public impurity_term
{
public:
  exp_coulomb();
protected:
  gsl_matrix_complex *matrix_block(double a1, double a2);
};

// wang_chen impurity term
class exp_wang : public impurity_term
{
public:
  exp_wang(double V, double ra, double rb, double r1);
protected:
  gsl_matrix_complex *matrix_block(double a1, double a2);
  double V;
  double ra;
  double rb;
  double r1;
};

class exp_LCZ_atom;

// Lam-Cohen-Zunger pseudopotential impurity term
class exp_LCZ : public impurity_term
{
public:
  exp_LCZ(elements_t host, elements_t impurity);
  gsl_matrix_complex *matrix(double min, double max, size_t num);
protected:
  gsl_matrix_complex *matrix_block(double a1, double a2);
  void on_set_inv_radius(double r);
  void on_set_dielectric_constant(double k);
  void on_delete();
  exp_LCZ_atom *host;
  exp_LCZ_atom *impurity;
  exp_coulomb *coulomb;
};

// Lam-Cohen-Zunger pseudopotential atomic impurity term
class exp_LCZ_atom : public impurity_term
{
public:
  exp_LCZ_atom(elements_t atom);
protected:
  // set atom with d electrons treated as core when in full energy
  // level
  void set_d_core(elements_t atom);
  gsl_matrix_complex *matrix_block(double a1, double a2);
  int Zc;
  double C1;
  double C2;
  double C3;
};

// overlap term
class exp_overlap : public overlap_term
{
protected:
  gsl_matrix_complex *matrix_block(double a1, double a2);
};


#endif // _MATRIX_TERM_HH
