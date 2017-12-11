#ifndef _DPMLL_
#define _DPMLL_

#include "DPM.h"
#include <array>
#include <algorithm> //to use max and min functions

class DPMLL : public DPM {
	public:
	
	unsigned int n; //mesh size
	//define Cell as vector of Particle pointers
	typedef std::vector<Particle*> Cell; 
	std::vector<Cell> ll; //linked list

	void computeForce(Particle* p, Particle* q) {
		double distance = (p->r-q->r).getLength();
		double overlap = p->a+q->a-distance;
		if (overlap>=0) {
			Vec3D normal = (p->r-q->r)/distance;
			double vreln = Vec3D::dot(p->v-q->v,normal);
			double normalForce = k*overlap+gam*vreln; 
			p->f += normal*normalForce;
			q->f += normal*(-normalForce);
		}		
	}

	void computeForces(Cell& cell1, Cell& cell2) {
		for (Particle* p : cell1)
		for (Particle* q : cell2)
			computeForce(p,q);
	}	

	void computeForces(Cell& cell) {
		for (auto p = cell.begin();  p!=cell.end(); ++p)
		for (auto q = p+1;  q!=cell.end(); ++q)
			computeForce(*p,*q);
	}	

	//solve contains the time stepping algorithm and the output routines 
	void solve () {
		//LL: determine size of the linked list cells
		double aMax = 0;
		for (Particle p : particles) {
			if (p.a>aMax) aMax=p.a;
		}
		n = ceil(0.5/aMax);
		ll.resize(n*n*n); 
		//initialise time, and counter/ofstream for data output
		double t=0;
		unsigned int counter = 0;
		std::array<Cell,13> neighbors;
		std::ofstream file("data");
		while (t<tMax) {
			//integrate position and velocity with Velocity Verlet
			for (Particle& p : particles) {
				p.v += p.f*(0.5*dt/p.m);
			}
			//set gravitational body force for all particles
			for (Particle& p : particles) {
				p.f = gravity * p.m;
			}
			//loop over all wall-particle contacts to evaluate forces
			for (Particle& p : particles)
			for (Wall& w : walls) {
				double distance = Vec3D::dot((w.p-p.r),w.n);
				double overlap = p.a-distance;
				if (overlap>=0) {
					double vreln = Vec3D::dot(p.v,w.n);
					double normalForce = k*overlap+gam*vreln; 
					p.f += w.n*(-normalForce);
				}
			}
			//LL: remove old linked list
			for (Cell& cell : ll) cell.resize(0); // ll.resize(0); does the same function ? \
			//LL: re-initialise linked list
			for (Particle& p : particles) {
				std::array<unsigned int,3> i = {
					std::max<unsigned int> (0,std::min<unsigned int>(n-1,p.r.getComponent(0)*n)), 
					std::max<unsigned int> (0,std::min<unsigned int>(n-1,p.r.getComponent(1)*n)), 
					std::max<unsigned int> (0,std::min<unsigned int>(n-1,p.r.getComponent(2)*n))};
				unsigned int ix = i[0]+(i[1]+i[2]*n)*n;
				ll[ix].push_back(&p);
			}	
			//LL: loop over all contacts p=1..N, q=p+1..N to evaluate forces
			std::array<int,3> i={0,0,0};
			for (unsigned int i=0; i<n; ++i)
			for (unsigned int j=0; j<n; ++j)
			for (unsigned int k=0; k<n; ++k) {
				unsigned int ix = i+(j+k*n)*n;
				Cell& cell = ll[ix];
				if (cell.size()==0) continue;
				computeForces(cell);		//compute forces between the particle pairs within a cell
				if (i>0) {
					if (j<n-2) {
						if (k<n-2) computeForces(cell,ll[ix-1+n+n*n]);
						           computeForces(cell,ll[ix-1+n    ]);
						if (k>0)   computeForces(cell,ll[ix-1+n-n*n]);
					}
					if (k<n-2)     computeForces(cell,ll[ix-1  +n*n]);
					               computeForces(cell,ll[ix-1      ]);
					if (k>0)       computeForces(cell,ll[ix-1  -n*n]);
					if (j>0) {
						if (k<n-2) computeForces(cell,ll[ix-1-n+n*n]);
						           computeForces(cell,ll[ix-1-n    ]);
						if (k>0)   computeForces(cell,ll[ix-1-n-n*n]);
					}
				} // compute forces with neighbours
				if (j>0) {
					if (k<n-2)     computeForces(cell,ll[ix  -n+n*n]);
					               computeForces(cell,ll[ix  -n    ]);
					if (k>0)       computeForces(cell,ll[ix  -n-n*n]);
				}
				if (k>0)           computeForces(cell,ll[ix    -n*n]);
			}
			//integrate position and velocity with Velocity Verlet
			for (Particle& p : particles) {
				p.r += p.v*dt;
				p.v += p.f*(0.5*dt/p.m);
			}
			t += dt;
			//write output file in the .data format every 10th time step
			if (++counter>=saveCount) {
				//reset the counter, write time to terminal
				counter = 0;
				std::cout << "t=" << t << std::endl; 
				//write global parameters (time, number of particles, domain size)
				file << particles.size() << " " << t << " 0 0 0 1 1 1\n"; 
				for (Particle& p : particles) {
					//write particle data (position, velocity, radius)
					file << p.r << " " << p.v << " " << p.a << " 0 0 0 0 0 0 0\n";
				}
			}
		} //end time loop
		file.close();
	} //end solve
};
#endif
