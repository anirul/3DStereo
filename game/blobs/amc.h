#ifndef __MARCHINGCUBE_HEADER__
#define __MARCHINGCUBE_HEADER__

/*
	Polygonising a scalar field
	Also known as: "3D Contouring","Marching Cubes","Surface Reconstruction"
	Written by Paul Bourke
	Based on code by Cory Gene Bloyd
	May 1997 
*/

#define ABS(x) (((x) > 0) ? (x) : -(x))
 
struct XYZ {
	float x;
	float y;
	float z;
	float nx;
	float ny;
	float nz;
//	float tu;
//	float tv;
};

struct TRIANGLE {
   XYZ p[3];
};



struct GRIDCELL {
	XYZ p[8];
	double val[8];
};

/*
	Given a grid cell and an isolevel, calculate the triangular
	facets required to represent the isosurface through the cell.
	Return the number of triangular facets, the array "triangles"
	will be loaded up with the vertices at most 5 triangular facets.
	0 will be returned if the grid cell is either totally above
	of totally below the isolevel.
*/

int Polygonise(GRIDCELL grid,double isolevel,TRIANGLE* triangles);

/*
   Linearly interpolate the position where an isosurface cuts
   an edge between two vertices, each with their own scalar value
*/

XYZ VertexInterp(double isolevel,XYZ p1,XYZ p2,double valp1,double valp2);

#endif // __MARCHINGCUBE_HEADER__
