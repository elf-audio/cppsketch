//
//  Was called - CFxRbjFilter.h
//  Someone's implementation of Robert Bristow-Johnson's biquad eqns.
//
//  Created by Marek Bereza on 02/10/2012.
// (found it on musicdsp.org)
//  Modified 04/04/19 by Marek Bereza

#pragma once
#include <cmath>
class Biquad {
public:
	

	constexpr static int LOWPASS = 0;
	constexpr static int HIGHPASS = 1;
	constexpr static int BANDPASS_CSG = 2;
	constexpr static int BANDPASS_CZPG = 3;
	constexpr static int NOTCH = 4;
	constexpr static int ALLPASS = 5;
	constexpr static int PEAKING = 6;
	constexpr static int LOWSHELF = 7;
	constexpr static int HIGHSHELF = 8;
	
	
	
	
	float filter(float in0) {
		// filter
		float const yn = b0a0*in0 + b1a0*in1 + b2a0*in2 - a1a0*ou1 - a2a0*ou2;
		
		// push in/out buffers
		in2=in1;
		in1=in0;
		ou2=ou1;
		ou1=yn;
		
		// return output
		return yn;
	};
	
	
	void calc(int const type,double const frequency,double const sample_rate,double const q,double const db_gain,bool q_is_bandwidth) {
		// temp pi
		double const temp_pi=3.1415926535897932384626433832795;
		
		// temp coef vars
		double alpha,a0,a1,a2,b0,b1,b2;
		
		// peaking, lowshelf and hishelf
		if(type>=6)
		{
			double const A		=	pow(10.0,(db_gain/40.0));
			double const omega	=	2.0*temp_pi*frequency/sample_rate;
			double const tsin	=	sin(omega);
			double const tcos	=	cos(omega);
			
			if(q_is_bandwidth)
				alpha=tsin*sinh(std::log(2.0)/2.0*q*omega/tsin);
			else
				alpha=tsin/(2.0*q);
			
			double const beta	=	sqrt(A)/q;
			
			
			// peaking
			if(type==PEAKING) {
				b0=float(1.0+alpha*A);
				b1=float(-2.0*tcos);
				b2=float(1.0-alpha*A);
				a0=float(1.0+alpha/A);
				a1=float(-2.0*tcos);
				a2=float(1.0-alpha/A);
			}
			
			// lowshelf
			else if(type==LOWSHELF)
			{
				b0=float(A*((A+1.0)-(A-1.0)*tcos+beta*tsin));
				b1=float(2.0*A*((A-1.0)-(A+1.0)*tcos));
				b2=float(A*((A+1.0)-(A-1.0)*tcos-beta*tsin));
				a0=float((A+1.0)+(A-1.0)*tcos+beta*tsin);
				a1=float(-2.0*((A-1.0)+(A+1.0)*tcos));
				a2=float((A+1.0)+(A-1.0)*tcos-beta*tsin);
			}
			
			// hishelf
			else if(type==HIGHSHELF)
			{
				b0=float(A*((A+1.0)+(A-1.0)*tcos+beta*tsin));
				b1=float(-2.0*A*((A-1.0)+(A+1.0)*tcos));
				b2=float(A*((A+1.0)+(A-1.0)*tcos-beta*tsin));
				a0=float((A+1.0)-(A-1.0)*tcos+beta*tsin);
				a1=float(2.0*((A-1.0)-(A+1.0)*tcos));
				a2=float((A+1.0)-(A-1.0)*tcos-beta*tsin);
			} else {
				// stop compiler warning
				a0 = 0;
				b1 = 0;
				b2 = 0;
				a1 = 0;
				a2 = 0;
			}
		}
		else
		{
			// other filters
			double const omega	=	2.0*temp_pi*frequency/sample_rate;
			double const tsin	=	sin(omega);
			double const tcos	=	cos(omega);
			
			if(q_is_bandwidth)
				alpha=tsin*sinh(std::log(2.0)/2.0*q*omega/tsin);
			else
				alpha=tsin/(2.0*q);
			
			
			// lowpass
			if(type==LOWPASS)
			{
				b0=(1.0-tcos)/2.0;
				b1=1.0-tcos;
				b2=(1.0-tcos)/2.0;
				a0=1.0+alpha;
				a1=-2.0*tcos;
				a2=1.0-alpha;
			}
			
			// hipass
			else if(type==HIGHPASS)
			{
				b0=(1.0+tcos)/2.0;
				b1=-(1.0+tcos);
				b2=(1.0+tcos)/2.0;
				a0=1.0+ alpha;
				a1=-2.0*tcos;
				a2=1.0-alpha;
			}
			
			// bandpass csg
			else if(type==BANDPASS_CSG)
			{
				b0=tsin/2.0;
				b1=0.0;
			    b2=-tsin/2;
				a0=1.0+alpha;
				a1=-2.0*tcos;
				a2=1.0-alpha;
			}
			
			// bandpass czpg
			else if(type==BANDPASS_CZPG)
			{
				b0=alpha;
				b1=0.0;
				b2=-alpha;
				a0=1.0+alpha;
				a1=-2.0*tcos;
				a2=1.0-alpha;
			}
			
			// notch
			else if(type==NOTCH)
			{
				b0=1.0;
				b1=-2.0*tcos;
				b2=1.0;
				a0=1.0+alpha;
				a1=-2.0*tcos;
				a2=1.0-alpha;
			}
			
			// allpass
			else if(type==ALLPASS)
			{
				b0=1.0-alpha;
				b1=-2.0*tcos;
				b2=1.0+alpha;
				a0=1.0+alpha;
				a1=-2.0*tcos;
				a2=1.0-alpha;
			} else {
				// stop compiler warning
				a0 = 0;
				b1 = 0;
				b2 = 0;
				a1 = 0;
				a2 = 0;
			}
		}
		
		// set filter coeffs
		b0a0=float(b0/a0);
		b1a0=float(b1/a0);
		b2a0=float(b2/a0);
		a1a0=float(a1/a0);
		a2a0=float(a2/a0);
	};
	
	void copyCoeffsFrom(const Biquad &b) {
		b0a0 = b.b0a0;
		b1a0 = b.b1a0;
		b2a0 = b.b2a0;
		a1a0 = b.a1a0;
		a2a0 = b.a2a0;
	}
private:
	
	// filter coeffs
	float b0a0 = 0,b1a0 = 0,b2a0 = 0,a1a0 = 0,a2a0 = 0;
	
	// in/out history
	float ou1 = 0,ou2 = 0,in1 = 0,in2 = 0;
};
