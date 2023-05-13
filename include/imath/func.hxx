/***************************************************************************
  func.hxx  -  header file for class func, extending the GiNaC function class
                             -------------------
    begin                : Fri Feb 13 2004
    copyright            : (C) 2022 by Jan Rheinlaender
    email                : jrheinlaender@users.sourceforge.net
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef FUNC_H
#define FUNC_H

/**
 * @author Jan Rheinlaender
 * @short Extends the GiNaC function class
 */

#include <string>
#include <stdexcept>
#include "expression.hxx"
#include "printing.hxx"

namespace GiNaC {

#define FUNCHINT_NONE 1       // Default, no effect
#define FUNCHINT_LIB 2        // A library function, will not be deleted by clear()
#define FUNCHINT_TRIG 4       // For special printing of trigonometric functions
#define FUNCHINT_EXPAND 8     // Function is to be expanded immediately (e.g. square)
#define FUNCHINT_NOBRACKET 16 // Function does not need brackets when printed (e.g. sqrt{...})
#define FUNCHINT_DEFDIFF 32   // Differentiate definition, not function itself
#define FUNCHINT_PRINT 64     // Use hard-coded printing function

/// Extends GiNaC::function by runtime definition of functions
// Note: Making the whole class IMATH_DLLPUBLIC for MSVC results in lots of duplicate defined symbols from container<std::vector>
class
#ifndef _MSC_VER
IMATH_DLLPUBLIC
#endif
func : public exprseq {
  GINAC_DECLARE_REGISTERED_CLASS(func, exprseq)
public:
  /**
   * Construct a func with the given name
   * Important note: Only the Functionmanager should ever call these constructors directly
   * @param n A string with the name of the function
   * @param args An exvector containing the arguments of the function, e.g. (2*x; y^2)
   * @param s The serial number of the function
   * @param hc True if the function is hard-coded, false if it is user-defined
   * @param v The default variables of the function
   * @param d The definition of the user-defined function, or an empty expression if the function has no definition
   * @param h The function hints
   * @param pn The name to use when printing the function
   */
  func(const std::string &n, const exprseq &args, const unsigned s, const bool hc, const exvector& v, const expression& d, const unsigned h, const std::string& pn);
  func(const std::string &n, exprseq &&args, const unsigned s, const bool hc, const exvector& v, const expression& d, const unsigned h, const std::string& pn);

#ifdef DEBUG_CONSTR_DESTR
  func(const func& other);
  func& operator=(const func& other);
  ~func();
#endif

  /// Return pure func without arguments
  expression makepure() const;

  /// Print the function arguments
  void printseq(const print_context & c, const std::string &openbracket, const std::string& delim, const std::string &closebracket) const;

  /**
   * Print the function in a GiNaC print context.
   * @param c The print context (e.g., print_latex)
   * @param p A power argument for the function, important for printing sin^2 x instead of (sin x)^2
   * @param level Unused, for consistency with GiNaC print methods
   */
  void print_imath(const imathprint &c, const ex &p, unsigned level = 0) const;

  /**
   * Print the function in a GiNaC print context.
   * @param c The print context (e.g., print_latex)
   * @param level Unused, for consistency with GiNaC print methods
   */
  // *** changed name to do_print in 0.8
  void do_print(const print_context &c, unsigned level = 0) const;
  void do_print_imath(const imathprint &c, unsigned level = 0) const;

  /// Print function with difftype line
  void print_diff_line(const ex& g, const int gr, const print_context& c) const;

  // functions overriding virtual functions from base classes, see functions.hxx
  /// Set the precedence for functions
  inline unsigned precedence(void) const override { return 70; }

  /// expand the function, using its definition on the arguments
  static const unsigned no_expand_function_definition = 0x8000; // Don't expand the function definition. Extends GiNaC::expand_options
  ex expand(unsigned options = 0) const override;

  /// Override these functions because the default applies them to the arguments
  ex conjugate() const override;
  ex imag_part() const override;
  ex real_part() const override;

  /// symbolic integration (implemented for simple cases only). Returns empty expression if no integration is possible
  expression find_integral(const ex& sym) const;

  /// Evaluate the function to a numeric value, if possible
  ex eval() const override;

  /// Substitute the exmap in the function arguments and (for pure functions) in the variables
  ex subs(const exmap & m, unsigned options) const override;

  // Map the function arguments and (for pure functions) the variables
  ex map(map_function &f) const override;

  bool has(const ex & other, unsigned options = 0) const override;

  /**
   * Evaluate the function
   * @returns A GiNaC::ex with the result of the evaluation
   **/
  ex evalf() const override;

  /// Evaluate matrices in the function arguments
  ex evalm() const override;

  /// Calculate the hash value of the function
  unsigned calchash(void) const override;

  /// Create a series expansion of the function
  ex series(const relational &r, int order, unsigned options = 0) const override;

  /// Return this container with v
  // Required for container::subs() to work properly!
  ex thiscontainer(const exvector &v) const override;
  ex thiscontainer(exvector &&v) const override;

protected:
  /// Calculate the (total) derivative of the function with respect to s
  ex derivative(const symbol & s) const override;

  /**
   * Apply the function definition on the function arguments
   * An exception is thrown if the function is not registered, or has no definition
   * @exception runtime_error(Function is not registered)
   * @exception logic_error(Function has no definition)
   **/
  expression expand_definition() const;

  /// Equality checking functions required by GiNaC
  bool is_equal_same_type(const basic & other) const override;
  /// Type matching function required by GiNaC
  bool match_same_type(const basic & other) const override;
  /// Return type of the function
  unsigned return_type(void) const override;

  /**
   * Calculate the partial derivative of the function with respect to the nth dependant variable
   * @param diff_param The number of the dependant variable
   * @returns An expression containing the partial derivative
   * @exception logic_error(Parameter does not exist)
   **/
  ex pderivative(unsigned diff_param) const;

public:
// member functions-------------------------------------------
  /**
   * Return the name of the function
   * @returns A string with the name of the function - not the Latex name!
   */
  IMATH_DLLPUBLIC inline const std::string& get_name() const { return(name); }

  /// Return the number of arguments of the function
  inline size_t get_numargs() const { return vars.size(); }

  // Return arguments if there are any, else the variables
  exvector get_args() const;

  /// Return the serial number of the function
  IMATH_DLLPUBLIC inline unsigned get_serial() const { return serial; }

  /**
  * Return true if this is a trigonometric function, else false
  * @exception runtime_error(Function is not registered)
  **/
  inline bool is_trig() const { return (hints & FUNCHINT_TRIG); }

  /// Returns true if this function should be printed without brackets
  inline bool is_nobracket() const { return (hints & FUNCHINT_NOBRACKET); }

  /**
  * Return true if this is a library function, else false
  * @exception runtime_error(Function is not registered)
  **/
  inline bool is_lib() const { return (hints & FUNCHINT_LIB); }

  /// Return true if this function is expanded immediately when it is created
  inline bool is_expand() const { return (hints & FUNCHINT_EXPAND); }

  /// Return true if the function has no arguments (useful for printing)
  inline bool is_pure() const { return(seq.empty()); }

// member variables -------------------------------------------------
protected:
  /// The name of the function, this is used to look it up in the map of registered functions
  std::string name;

  /// The serial number of the function, if it is hard-coded into GiNaC
  unsigned serial;

  /// True if the function is hard-coded into GiNaC or iMath
  bool hard;

  /// dependant variables of the function (e.g. (x; y; z)
  exvector vars;

  /// The definition of the function (e.g. for cubic: x^3)
  expression definition;

  /// Contains hints for handling and printing of the function
  unsigned hints;

  /// The name for printing the function (optional)
  std::string printname;

private:
    using exprseq::printseq; // Shut up warning about our printseq() hiding the base classes' printseq()
};

// Fake the GiNaC unarchiver functionality because it breaks unopkg with error "Class ... is already registered" in ginac/archive.cpp
class IMATH_DLLPUBLIC func_unarchiver {
public:
  func_unarchiver();
  ~func_unarchiver();
};
static func_unarchiver func_unarchiver_instance;

}
#endif
