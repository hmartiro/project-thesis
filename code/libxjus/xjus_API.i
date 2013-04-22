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

%apply ( int INPLACE_ARRAY2[ANY][ANY] ) {(int pvt[6][4])}

%include "xjus_API.hpp"