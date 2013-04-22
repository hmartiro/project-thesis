 /* example.i */

 %module xjus_API

 %{
 	#define SWIG_FILE_WITH_INIT
	#include "xjus_API.hpp"
 %}

%include "numpy.i"
%init %{
	import_array();
%}

%apply (int DIM1, float* IN_ARRAY1) {(int N, float value[])};
%apply (int DIM1, int DIM2, float* IN_ARRAY2) {(int N, int M, float value[][4])};
%apply ( float IN_ARRAY2[ANY][ANY] ) {(float value[6][4])}
%apply ( int INPLACE_ARRAY2[ANY][ANY] ) {(int data[6][4])}
%apply ( int INPLACE_ARRAY2[ANY][ANY] ) {(int data[6*5][4])}

/*
%typemap(in) float value[ANY] (float temp[$1_dim0]) {
  int i;
  if (!PySequence_Check($input)) {
    PyErr_SetString(PyExc_ValueError,"Expected a sequence");
    return NULL;
  }
  if (PySequence_Length($input) != $1_dim0) {
    PyErr_SetString(PyExc_ValueError,"Size mismatch. Expected $1_dim0 elements");
    return NULL;
  }
  for (i = 0; i < $1_dim0; i++) {
    PyObject *o = PySequence_GetItem($input,i);
    if (PyNumber_Check(o)) {
      temp[i] = (float) PyFloat_AsDouble(o);
    } else {
      PyErr_SetString(PyExc_ValueError,"Sequence elements must be numbers");      
      return NULL;
    }
  }
  $1 = temp;
}

%typemap(in) int nodeA[ANY] {
  int i;
  if (!PySequence_Check($input)) {
    PyErr_SetString(PyExc_ValueError,"Expected a sequence");
    return NULL;
  }
  if (PySequence_Length($input) != $1_dim0) {
    PyErr_SetString(PyExc_ValueError,"Size mismatch. Expected $1_dim0 elements");
    return NULL;
  }
  $1 = (float *) malloc($1_dim0*sizeof(float));
  for (i = 0; i < $1_dim0; i++) {
    PyObject *o = PySequence_GetItem($input,i);
    if (PyNumber_Check(o)) {
      $1[i] = (float) PyInt_AsLong(o);
    } else {
      PyErr_SetString(PyExc_ValueError,"Sequence elements must be numbers");      
      free($1);
      return NULL;
    }
  }
}
%typemap(freearg) int nodeA[ANY] {
   if ($1) free($1);
}

%typemap(in) int nodeA[ANY] (int temp[$1_dim0]) {
  int i;
  if (!PySequence_Check($input)) {
    PyErr_SetString(PyExc_ValueError,"Expected a sequence");
    return NULL;
  }
  if (PySequence_Length($input) != $1_dim0) {
    PyErr_SetString(PyExc_ValueError,"Size mismatch. Expected $1_dim0 elements");
    return NULL;
  }
  for (i = 0; i < $1_dim0; i++) {
    PyObject *o = PySequence_GetItem($input,i);
    if (PyNumber_Check(o)) {
      temp[i] = (int) PyInt_AsLong(o);
    } else {
      PyErr_SetString(PyExc_ValueError,"Sequence elements must be numbers");      
      return NULL;
    }
  }
  $1 = temp;
}
*/

%include "xjus_API.hpp"