/*
  swig -python -c++ xxx.i   
*/
%module POLYHEDRON
%include "std_string.i"
%include "std_vector.i"
%include "carrays.i"

%array_class(float,  ArrayF);
%array_class(double, ArrayD);
%array_class(int,    ArrayI);


//%include "typemaps.i"
//%apply int *OUTPUT { int *result01 };


%{
  #include "Point.h"
  #include "Vector.h"
  #include "Polyhedron.h"
%}


%include "Point.h"
%include "Vector.h"
%include "Polyhedron.h"


namespace ccmc
{
  %template(PT)      Point<double>;
  %template(VC)     Vector<double>;
  %template(VI)     Vector<   int>;
  %template(PL) Polyhedron<double>;
}


namespace std
{
  %template(VectorI) vector<int>;
  %template(VectorD) vector<double>;
  %template(VectorV) vector< ccmc::Vector<double> >;
}
