
/* simple.h -- definition of the simple internal coordinate classes
 *
 *      THIS SOFTWARE FITS THE DESCRIPTION IN THE U.S. COPYRIGHT ACT OF A
 *      "UNITED STATES GOVERNMENT WORK".  IT WAS WRITTEN AS A PART OF THE
 *      AUTHOR'S OFFICIAL DUTIES AS A GOVERNMENT EMPLOYEE.  THIS MEANS IT
 *      CANNOT BE COPYRIGHTED.  THIS SOFTWARE IS FREELY AVAILABLE TO THE
 *      PUBLIC FOR USE WITHOUT A COPYRIGHT NOTICE, AND THERE ARE NO
 *      RESTRICTIONS ON ITS USE, NOW OR SUBSEQUENTLY.
 *
 *  Author:
 *      E. T. Seidl
 *      Bldg. 12A, Rm. 2033
 *      Computer Systems Laboratory
 *      Division of Computer Research and Technology
 *      National Institutes of Health
 *      Bethesda, Maryland 20892
 *      Internet: seidl@alw.nih.gov
 *      February, 1993
 */

#ifndef _intco_simple_h
#define _intco_simple_h

#ifdef __GNUC__
#pragma interface
#endif


#include <iostream.h>

#include <util/class/class.h>
#include <util/state/state.h>
#include <util/keyval/keyval.h>
#include <chemistry/molecule/molecule.h>
#include <chemistry/molecule/coor.h>

#include <math/scmat/vector3.h>

// ////////////////////////////////////////////////////////////////////////

/**
The SimpleCo abstract class describes a simple internal coordinate
of a molecule.  The number atoms involved can be 2, 3 or 4 and is
determined by the specialization of SimpleCo.

There are three ways to specify the atoms involved in the internal
coordinate.  The first way is a shorthand notation, just a vector of a
label followed by the atom numbers (starting at 1) is given.  For example,
a stretch between two atoms, 1 and 2, is given, in the
ParsedKeyVal format, as
\begin{verbatim}
  stretch<StreSimpleCo>: [ R12 1 2 ]
\end{verbatim}

The other two ways to specify the atoms are more general.  With them, it is
possible to give parameters for the IntCoor base class (and thus
give the value of the coordinate).  In the first of these input formats, a
vector associated with the keyword atoms gives the atom numbers.
The following specification for stretch is equivalent to that
above:
\begin{verbatim}
  stretch<StreSimpleCo>:( label = R12 atoms = [ 1 2 ] )
\end{verbatim}

In the second, a vector, atom_labels, is given along with a
Molecule object.  The atom labels are looked up in the
Molecule object to find the atom numbers.
The following specification for stretch is equivalent to those
above:
\begin{verbatim}
  molecule<Molecule>: (
    { atom_labels atoms   geometry      } = {
          H1         H   [ 1.0 0.0 0.0 ]
          H2         H   [-1.0 0.0 0.0 ] } )
  stretch<StreSimpleCo>:( label = R12
                          atom_labels = [ H1 H2 ]
                          molecule = $molecule )
\end{verbatim}
 */
class SimpleCo : public IntCoor {
#   define CLASSNAME SimpleCo
#   include <util/state/stated.h>
#   include <util/class/classd.h>
  protected:
    int natoms_;
    int *atoms;

  public:
    SimpleCo();
    /** This constructor takes an integer argument which is the number of
     atoms needed to describe the coordinate.  A second optional char*
     argument is a label for the coordinate.  This argument is passed on to
     the IntCoor constructor. */
    SimpleCo(int,const char* =0);
    /// The KeyVal constructor requires the number of atoms.
    SimpleCo(const RefKeyVal&,int natom);

    virtual ~SimpleCo();

    /// Returns the number of atoms in the coordinate.
    int natoms() const;
    /// Returns the index of the i'th atom in the coordinate.
    int operator[](int i) const;

    void save_data_state(StateOut&);
    SimpleCo(StateIn&);

    virtual int operator==(SimpleCo&);
    int operator!=(SimpleCo&u);

    // these IntCoor members are implemented in term of
    // the calc_force_con and calc_intco members.
    /// Returns an approximate force constant (a la Almlof).
    double force_constant(RefMolecule&);
    /** Recalculates the value of the coordinate based on the geometry
        in the Molecule. */
    void update_value(const RefMolecule&);
    /// Fill in a row of the B matrix.
    void bmat(const RefMolecule&,RefSCVector&bmat,double coef = 1.0);

    /// Calculates an approximate force constant and returns it's value.
    virtual double calc_force_con(Molecule&) = 0;
    /** Calculate the value of the coordinate based on what's in Molecule.
        If given a double*, fill in that part of the B matrix.  If the
        bmatrix is to be calculated, the third argument gives the
        coefficient. */
    virtual double calc_intco(Molecule&, double* =0, double =1) = 0;

    /// Print the coordinate.
    void print_details(const RefMolecule &, ostream& = cout) const;
    
    /** Tests to see if two coordinates are equivalent to each other.
        This is false if the atoms don't match. */
    int equivalent(RefIntCoor&);
  };
SavableState_REF_dec(SimpleCo);


// ///////////////////////////////////////////////////////////////////////

#define SimpleCo_DECLARE(classname)					      \
  public:								      \
    virtual classname& operator=(const classname&);			      \
    SimpleCo& operator=(const SimpleCo&);				      \
    double calc_force_con(Molecule&);					      \
    double calc_intco(Molecule&, double* =0, double =1);		      \
    classname(StateIn&);						      \
    void save_data_state(StateOut&);                                          \
  private:

#define SimpleCo_IMPL_eq(classname)					      \
SimpleCo& classname::operator=(const SimpleCo& c)			      \
{									      \
  classname *cp = classname::castdown((SimpleCo*)&c);			      \
  if(cp) {								      \
      *this=*cp;							      \
    }									      \
  else {								      \
      natoms_ = 0;							      \
      atoms = 0;							      \
    }									      \
									      \
  return *this;								      \
  }

#define SimpleCo_IMPL_StateIn(classname)				      \
classname::classname(StateIn&si):					      \
  SimpleCo(si)								      \
{									      \
}

#define SimpleCo_IMPL_save_data_state(classname)			      \
void classname::save_data_state(StateOut&so)				      \
{									      \
  SimpleCo::save_data_state(so);					      \
}

#define SimpleCo_IMPL(classname)		\
        SimpleCo_IMPL_eq(classname)		\
        SimpleCo_IMPL_StateIn(classname)	\
        SimpleCo_IMPL_save_data_state(classname)

// ///////////////////////////////////////////////////////////////////////

/**
The StreSimpleCo class describes an stretch internal coordinate of a
molecule.  The input is described in the documentation of its parent
class SimpleCo.

Designating the two atoms as $a$ and $b$ and their cartesian positions as
$\bar{r}_a$ and $\bar{r}_b$, the value of the coordinate, $r$, is
\[ r = \| \bar{r}_a - \bar{r}_b \| \]
 */
class StreSimpleCo : public SimpleCo {
#   define CLASSNAME StreSimpleCo
#   define HAVE_CTOR
#   define HAVE_KEYVAL_CTOR
#   define HAVE_STATEIN_CTOR
#   include <util/state/stated.h>
#   include <util/class/classd.h>
SimpleCo_DECLARE(StreSimpleCo)
  public:
    StreSimpleCo();
    StreSimpleCo(const StreSimpleCo&);
    /** This constructor takes a string containing a label, and two integers
      which are the indices of the atoms we're measuring the distance between.
      Atom numbering begins at atom 1, not atom 0. */
    StreSimpleCo(const char*, int, int);
    /** The KeyVal constructor.  This calls the SimpleCo keyval constructor
        with an integer argument of 2. */
    StreSimpleCo(const RefKeyVal&);

    ~StreSimpleCo();

    /// Always returns the string "STRE".
    const char * ctype() const;

    /// Returns the distance between the two atoms in atomic units.
    double bohr() const;
    /// Returns the distance between the two atoms in angstrom units.
    double angstrom() const;
    /// Returns the distance between the two atoms in angstrom units.
    double preferred_value() const;
  };

typedef StreSimpleCo Stre;

// ///////////////////////////////////////////////////////////////////////

static const double rtd = 180.0/3.14159265358979323846;

/** The BendSimpleCo class describes an bend internal coordinate of a
molecule.  The input is described in the documentation of its parent class
SimpleCo.

Designating the three atoms as $a$, $b$, and $c$ and their cartesian
positions as $\bar{r}_a$, $\bar{r}_b$, and $\bar{r}_c$, the value of the
coordinate, $\theta$, is given by

\[ \bar{u}_{ab} = \frac{\bar{r}_a - \bar{r}_b}{\| \bar{r}_a - \bar{r}_b \|} \]
\[ \bar{u}_{cb} = \frac{\bar{r}_c - \bar{r}_b}{\| \bar{r}_c - \bar{r}_b \|} \]
\[ \theta       = \arccos ( \bar{u}_{ab} \cdot \bar{u}_{cb} ) \]

*/
class BendSimpleCo : public SimpleCo { 
#   define CLASSNAME BendSimpleCo
#   define HAVE_CTOR
#   define HAVE_KEYVAL_CTOR
#   define HAVE_STATEIN_CTOR
#   include <util/state/stated.h>
#   include <util/class/classd.h>
SimpleCo_DECLARE(BendSimpleCo)
  public:
    BendSimpleCo();
    BendSimpleCo(const BendSimpleCo&);
    /** This constructor takes a string containing a label, and three
     integers a, b, and c which give the indices of the atoms involved in
     the angle abc. Atom numbering begins at atom 1, not atom 0. */
    BendSimpleCo(const char*, int, int, int);
    /** The KeyVal constructor.  This calls the SimpleCo keyval constructor
        with an integer argument of 3. */
    BendSimpleCo(const RefKeyVal&);

    ~BendSimpleCo();

    /// Always returns the string "BEND".
    const char * ctype() const;
    
    /// Returns the value of the angle abc in radians.
    double radians() const;
    /// Returns the value of the angle abc in degrees.
    double degrees() const;
    /// Returns the value of the angle abc in degrees.
    double preferred_value() const;
  };

typedef BendSimpleCo Bend;

// ///////////////////////////////////////////////////////////////////////

/**
   
The TorsSimpleCo class describes an torsion internal coordinate of a
molecule.  The input is described in the documentation of its parent
class SimpleCo.

Designating the four atoms as $a$, $b$, $c$, and $d$ and their cartesian
positions as $\bar{r}_a$, $\bar{r}_b$, $\bar{r}_c$, and $\bar{r}_d$, the
value of the coordinate, $\tau$, is given by

\[ \bar{u}_{ab} = \frac{\bar{r}_a - \bar{r}_b}{\| \bar{r}_a - \bar{r}_b \|} \]
\[ \bar{u}_{cb} = \frac{\bar{r}_c - \bar{r}_b}{\| \bar{r}_c - \bar{r}_b \|} \]
\[ \bar{u}_{cd} = \frac{\bar{r}_c - \bar{r}_d}{\| \bar{r}_c - \bar{r}_b \|} \]
\[ \bar{n}_{abc}= \frac{\bar{u}_{ab} \times \bar{u}_{cb}}
                       {\| \bar{u}_{ab} \times \bar{u}_{cb} \|} \]
\[ \bar{n}_{bcd}= \frac{\bar{u}_{cd} \times \bar{u}_{bc}}
                       {\| \bar{u}_{cd} \times \bar{u}_{bc} \|} \]
\[ s            = \cases{1, &if $(\bar{n}_{abc}\times\bar{n}_{bcd})
                                  \cdot \bar{u}_{cb} > 0$;\cr
                         -1, &otherwise.\cr} \]
\[ \tau         = s \arccos ( - \bar{n}_{abc} \cdot \bar{n}_{bcd} ) \]

*/
class TorsSimpleCo : public SimpleCo { 
#   define CLASSNAME TorsSimpleCo
#   define HAVE_CTOR
#   define HAVE_KEYVAL_CTOR
#   define HAVE_STATEIN_CTOR
#   include <util/state/stated.h>
#   include <util/class/classd.h>
SimpleCo_DECLARE(TorsSimpleCo)
  public:
    TorsSimpleCo();
    TorsSimpleCo(const TorsSimpleCo&);
    /** This constructor takes a string containing a label, and four
     integers a, b, c, and d which give the indices of the atoms involved in
     the torsion angle abcd. Atom numbering begins at atom 1, not atom 0. */
    TorsSimpleCo(const char *refr, int, int, int, int);
    /** The KeyVal constructor.  This calls the
        SimpleCo keyval constructor with an integer argument of 4. */
    TorsSimpleCo(const RefKeyVal&);

    ~TorsSimpleCo();

    /// Always returns the string "TORS".
    const char * ctype() const;
    
    /// Returns the value of the angle abc in radians.
    double radians() const;
    /// Returns the value of the angle abc in degrees.
    double degrees() const;
    /// Returns the value of the angle abc in degrees.
    double preferred_value() const;
  };

typedef TorsSimpleCo Tors;

// ///////////////////////////////////////////////////////////////////////

/**
The ScaledTorsSimpleCo class describes an scaled torsion internal
coordinate of a molecule.  The scaled torsion is more stable that ordinary
torsions (see the TorsSimpleCo class) in describing situations
where one of the torsions plane's is given by three near linear atoms.

Designating the four atoms as $a$, $b$, $c$, and $d$ and their cartesian
positions as $\bar{r}_a$, $\bar{r}_b$, $\bar{r}_c$, and $\bar{r}_d$, the
value of the coordinate, $\tau_s$, is given by

\[ \bar{u}_{ab} = \frac{\bar{r}_a - \bar{r}_b}{\| \bar{r}_a - \bar{r}_b \|}\]
\[ \bar{u}_{cb} = \frac{\bar{r}_c - \bar{r}_b}{\| \bar{r}_c - \bar{r}_b \|}\]
\[ \bar{u}_{cd} = \frac{\bar{r}_c - \bar{r}_d}{\| \bar{r}_c - \bar{r}_b \|}\]
\[ \bar{n}_{abc}= \frac{\bar{u}_{ab} \times \bar{u}_{cb}}
                       {\| \bar{u}_{ab} \times \bar{u}_{cb} \|}\]
\[ \bar{n}_{bcd}= \frac{\bar{u}_{cd} \times \bar{u}_{cb}}
                       {\| \bar{u}_{cd} \times \bar{u}_{cb} \|}\]
\[ s            = \cases{-1, &if $(\bar{n}_{abc}\times\bar{n}_{bcd})
                                  \cdot \bar{u}_{cb} > 0$;\cr
                         1, &otherwise.\cr}\]
\[ \tau_s       = s \sqrt{\left(1-(\bar{u}_{ab} \cdot \bar{u}_{cb}\right)^2)
                        \left(1-(\bar{u}_{cb} \cdot \bar{u}_{cd}\right)^2)}
                  \arccos ( - \bar{n}_{abc} \cdot \bar{n}_{bcd} )\]

 */
class ScaledTorsSimpleCo : public SimpleCo { 
#   define CLASSNAME ScaledTorsSimpleCo
#   define HAVE_CTOR
#   define HAVE_KEYVAL_CTOR
#   define HAVE_STATEIN_CTOR
#   include <util/state/stated.h>
#   include <util/class/classd.h>
SimpleCo_DECLARE(ScaledTorsSimpleCo)
  private:
    double old_torsion_;
  public:
    ScaledTorsSimpleCo();
    ScaledTorsSimpleCo(const ScaledTorsSimpleCo&);
    /** This constructor takes a string containing a label, and four
     integers a, b, c, and d which give the indices of the atoms involved in
     the torsion angle abcd. Atom numbering begins at atom 1, not atom 0. */
    ScaledTorsSimpleCo(const char *refr, int, int, int, int);
    /** The KeyVal constructor.  This calls the
        SimpleCo keyval constructor with an integer argument of 4. */
    ScaledTorsSimpleCo(const RefKeyVal&);

    ~ScaledTorsSimpleCo();

    /// Always returns the string "TORS".
    const char * ctype() const;
    
    /// Returns the value of the angle abc in radians.
    double radians() const;
    /// Returns the value of the angle abc in degrees.
    double degrees() const;
    /// Returns the value of the angle abc in degrees.
    double preferred_value() const;
  };

typedef ScaledTorsSimpleCo ScaledTors;

// ///////////////////////////////////////////////////////////////////////

/*
The OutSimpleCo class describes an out-of-plane internal coordinate
of a molecule.  The input is described in the documentation of its parent
class SimpleCo.

Designating the four atoms as $a$, $b$, $c$, and $d$ and their cartesian
positions as $\bar{r}_a$, $\bar{r}_b$, $\bar{r}_c$, and $\bar{r}_d$, the
value of the coordinate, $\tau$, is given by

\[ \bar{u}_{ab} = \frac{\bar{r}_a - \bar{r}_b}{\| \bar{r}_a - \bar{r}_b \|}\]
\[ \bar{u}_{cb} = \frac{\bar{r}_b - \bar{r}_c}{\| \bar{r}_c - \bar{r}_b \|}\]
\[ \bar{u}_{db} = \frac{\bar{r}_c - \bar{r}_d}{\| \bar{r}_c - \bar{r}_b \|}\]
\[ \bar{n}_{bcd}= \frac{\bar{u}_{cb} \times \bar{u}_{db}}
                     {\| \bar{u}_{cb} \times \bar{u}_{db} \|}\]
\[ \phi         = \arcsin ( \bar{u}_{ab} \cdot \bar{n}_{bcd} )\]

*/
class OutSimpleCo : public SimpleCo { 
#   define CLASSNAME OutSimpleCo
#   define HAVE_CTOR
#   define HAVE_KEYVAL_CTOR
#   define HAVE_STATEIN_CTOR
#   include <util/state/stated.h>
#   include <util/class/classd.h>
SimpleCo_DECLARE(OutSimpleCo)
  public:
    OutSimpleCo();
    OutSimpleCo(const OutSimpleCo&);
    /** This constructor takes a string containing a label, and four
     integers a, b, c, and d which give the indices of the atoms involved in
     the out-of-plane angle abcd. Atom numbering begins at atom 1, not
     atom 0. */
    OutSimpleCo(const char *refr, int, int, int, int);
    /** The KeyVal constructor.  This calls the SimpleCo keyval
        constructor with an integer argument of 4. */
    OutSimpleCo(const RefKeyVal&);

    ~OutSimpleCo();

    /// Always returns the string "OUT".
    const char * ctype() const;
    
    /// Returns the value of the angle abc in radians.
    double radians() const;
    /// Returns the value of the angle abc in degrees.
    double degrees() const;
    /// Returns the value of the angle abc in degrees.
    double preferred_value() const;
  };

typedef OutSimpleCo Out;

// ///////////////////////////////////////////////////////////////////////

/** The LinIPSimpleCo class describes an in-plane component of a linear
bend internal coordinate of a molecule.  The input is described in the
documentation of its parent class SimpleCo.  A vector, $\bar{u}$, given as
the keyword u, that is not colinear with either $\bar{r}_a -
\bar{r}_b$ or $\bar{r}_b - \bar{r}_c$ must be provided, where $\bar{r}_a$,
$\bar{r}_b$, and $\bar{r}_c$ are the positions of the first, second, and
third atoms, respectively.

  Usually, LinIPSimpleCo is used with a corresponding LinOPSimpleCo, which
is given exactly the same u.

Designating the three atoms as $a$, $b$, and $c$ and their cartesian
positions as $\bar{r}_a$, $\bar{r}_b$, and $\bar{r}_c$, the value of the
coordinate, $\theta_i$, is given by

\[  \bar{u}_{ab} = \frac{\bar{r}_a - \bar{r}_b}{\| \bar{r}_a - \bar{r}_b \|}\]
\[  \bar{u}_{cb} = \frac{\bar{r}_b - \bar{r}_c}{\| \bar{r}_c - \bar{r}_b \|}\]
\[  \theta_i     = \pi - \arccos ( \bar{u}_{ab} \cdot \bar{u} )
                    - \arccos ( \bar{u}_{cb} \cdot \bar{u} )\]

*/
class LinIPSimpleCo : public SimpleCo { 
#   define CLASSNAME LinIPSimpleCo
#   define HAVE_CTOR
#   define HAVE_KEYVAL_CTOR
#   define HAVE_STATEIN_CTOR
#   include <util/state/stated.h>
#   include <util/class/classd.h>
SimpleCo_DECLARE(LinIPSimpleCo)
  private:
    SCVector3 u2;
  public:
    LinIPSimpleCo();
    LinIPSimpleCo(const LinIPSimpleCo&);
    /** This constructor takes a string containing a label, and three
     integers a, b, and d which give the indices of the atoms involved in
     the linear angle abc.  The last argument, u, is a unit vector
     used to defined the direction in which distortion is measured.
     Atom numbering begins at atom 1, not atom 0. */
    LinIPSimpleCo(const char *refr, int, int, int, const SCVector3 &u);
    /** The KeyVal constructor.  This calls the SimpleCo keyval
        constructor with an integer argument of 3. */
    LinIPSimpleCo(const RefKeyVal&);

    ~LinIPSimpleCo();

    /// Always returns the string "LINIP".
    const char * ctype() const;

    /// Returns the value of the angle abc in radians.
    double radians() const;
    /// Returns the value of the angle abc in degrees.
    double degrees() const;
    /// Returns the value of the angle abc in degrees.
    double preferred_value() const;
  };

typedef LinIPSimpleCo LinIP;

// ///////////////////////////////////////////////////////////////////////

/** The LinOPSimpleCo class describes an out-of-plane component of a linear
bend internal coordinate of a molecule.  The input is described in the
documentation of its parent class SimpleCo.  A vector, $\bar{u}$, given as
the keyword u, that is not colinear with either $\bar{r}_a - \bar{r}_b$ or
$\bar{r}_b - \bar{r}_c$ must be provided, where $\bar{r}_a$, $\bar{r}_b$,
and $\bar{r}_c$ are the positions of the first, second, and third atoms,
respectively.

  Usually, LinOPSimpleCo is used with a corresponding LinIPSimpleCo, which
is given exactly the same u.

Designating the three atoms as $a$, $b$, and $c$ and their cartesian
positions as $\bar{r}_a$, $\bar{r}_b$, and $\bar{r}_c$, the value of the
coordinate, $\theta_o$, is given by


\[ \bar{u}_{ab} = \frac{\bar{r}_a - \bar{r}_b}{\| \bar{r}_a - \bar{r}_b \|}\]
\[ \bar{u}_{cb} = \frac{\bar{r}_b - \bar{r}_c}{\| \bar{r}_c - \bar{r}_b \|}\]
\[ \bar{n}      = \frac{\bar{u} \times \bar{u}_{ab}}
                       {\| \bar{u} \times \bar{u}_{ab} \|}\]
\[ \theta_o     = \pi - \arccos ( \bar{u}_{ab} \cdot \bar{n} )
                      - \arccos ( \bar{u}_{cb} \cdot \bar{n} )\]

*/
class LinOPSimpleCo : public SimpleCo { 
#   define CLASSNAME LinOPSimpleCo
#   define HAVE_CTOR
#   define HAVE_KEYVAL_CTOR
#   define HAVE_STATEIN_CTOR
#   include <util/state/stated.h>
#   include <util/class/classd.h>
SimpleCo_DECLARE(LinOPSimpleCo)
  private:
    SCVector3 u2;
  public:
    LinOPSimpleCo();
    LinOPSimpleCo(const LinOPSimpleCo&);
    /** This constructor takes a string containing a label, and three
     integers a, b, and c which give the indices of the atoms involved in
     the linear angle abc.  The last argument, u, is a unit vector used to
     defined the direction perpendicular to the direction in which
     distortion is measured.  Atom numbering begins at atom 1, not atom 0. */
    LinOPSimpleCo(const char *refr, int, int, int, const SCVector3 &u);
    /** The KeyVal constructor.  This calls the
        SimpleCo keyval constructor with an integer argument of 3. */
    LinOPSimpleCo(const RefKeyVal&);

    ~LinOPSimpleCo();

    /// Always returns the string "LINIP".
    const char * ctype() const;

    /// Returns the value of the angle abc in radians.
    double radians() const;
    /// Returns the value of the angle abc in degrees.
    double degrees() const;
    /// Returns the value of the angle abc in degrees.
    double preferred_value() const;
  };

typedef LinOPSimpleCo LinOP;

#endif /* _intco_simple_h */

// Local Variables:
// mode: c++
// c-file-style: "CLJ"
// End:
