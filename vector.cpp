#include <math.h>
#include "config.h"
#include "vector.h"

vector::vector ()
{
	this->i = (pixel)0;
	this->j = (pixel)0;
	this->k = (pixel)0;
}

void vector::copy (vector *v)
{
	this->i = v->i;
	this->j = v->j;
	this->k = v->k;
}

void vector::set (pixel ii, pixel jj, pixel kk)
{
	this->i = ii;
	this->j = jj;
	this->k = kk;
}

void vector::set_module (pixel mod)
{
	this->divide( this->module() );
	this->multiply(mod);
}

void vector::add (vector *v)
{
	this->i += v->i;
	this->j += v->j;
	this->k += v->k;
}

void vector::subtract (vector *v)
{
	this->i -= v->i;
	this->j -= v->j;
	this->k -= v->k;
}

void vector::multiply (pixel n)
{
	this->i *= n;
	this->j *= n;
	this->k *= n;
}

void vector::divide (pixel n)
{
	this->i /= n;
	this->j /= n;
	this->k /= n;
}

pixel vector::module ()
{
	pixel m;
	m = sqrt( vector::prod_int(this, this) );	
	return m;
}

void vector::proj_vec (vector *v1, vector *v2)
{
	pixel p, m;
	
	p = vector::prod_int(v1, v2);
	if (p<0)
		p *= -1;
	m = v1->module();
	m *= m;
	p /= m;
	this->copy(v1);
	this->multiply(p);
}

void vector::prod_vec (vector *v1, vector *v2)
{
	/*
	      i j k
	   v1 i j k
	   v2 i j k
	*/
	
	this->i = v1->j*v2->k - v2->j*v1->k;
	this->j = v2->i*v1->k - v1->i*v2->k;
	this->k = v1->i*v2->j - v2->i*v1->j;
}

pixel vector::prod_int (vector *v1, vector *v2)
{
	pixel p;
	p = v1->i*v2->i + v1->j*v2->j + v1->k*v2->k;
	//p = v1->i*v2->i + v1->j*v2->j;
	return p;
}

float vector::cos_ang (vector *v1, vector *v2)
{
	pixel p;
	//	p = v1->i*v2->i + v1->j*v2->j + v1->z*v2->z;
	p = vector::prod_int(v1, v2);
	p /= v1->module();
	p /= v2->module();
	return p;
}
