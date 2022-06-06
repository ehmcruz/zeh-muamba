#ifndef __vectorheader__
#define __vectorheader__

#include "config.h"

class vector
{
	public:
	pixel i;
	pixel j;
	pixel k;
	
	vector();
	
	void copy (vector *v);
	void set (pixel ii, pixel jj, pixel kk);
	void set_module (pixel mod);
	void add (vector *v);
	void subtract (vector *v);
	void multiply (pixel n);
	void divide (pixel n);
	pixel module ();

	static pixel prod_int (vector *v1, vector *v2);
	static float cos_ang (vector *v1, vector *v2);
	void proj_vec (vector *v1, vector *v2);
	void prod_vec (vector *v1, vector *v2);
	
	/*pixel proj_vec_mod (vector *v1, vector *v2)
	{
		pixel p, m;
		p = vector::prod_int(v1, v2);
		if (p<0)
			p *= -1;
		m = v1->module();
		m *= m;
		p /= m;
		awnser->copy(v1);
		awnser->multiply(p);
	
		return awnser;	
	}*/

	// projeta o vetor 2 no 1	
};

#endif
