
#include "udxinc.h"
#include <math.h>

using namespace nz::udx_ver2;

class cal_corr: public Uda{
	public:
	double sum,N,prod,count;
	double xSdev,ySdev;
	cal_corr(UdxInit *pInit):Uda(pInit){}
	static Uda* instantiate(UdxInit *pInit);
	virtual void initializeState();
	virtual void accumulate();
	virtual void merge();
	virtual ReturnValue finalResult();
};

Uda* cal_corr::instantiate(UdxInit *pInit){
	cal_corr *pcorr = new cal_corr(pInit);
	return (Uda *)pcorr;
}

void cal_corr::initializeState(){
	sum=0,N=0,prod=0,count=0;
	setStateNull(0, true);
	setStateNull(1, true);
}

void cal_corr::accumulate(){
	double *corr = doubleState(0);
	bool corrNull = isStateNull(0);
	double *div = doubleState(1);
	bool divNull = isStateNull(1);
	double xcurrVal = doubleArg(0);
	bool xcurrValNull = isArgNull(0);
	 double ycurrVal = doubleArg(1);
        bool ycurrValNull = isArgNull(1);
	double pxSdev = doubleArg(2);
        bool pxSdevNull = isArgNull(2);
        double pySdev = doubleArg(3);
        bool pySdevNull = isArgNull(3);
       
	
	if(!xcurrValNull && !ycurrValNull){
		if(corrNull ){
			setStateNull(0, false);
			*corr = xcurrVal * ycurrVal;
		}else{
				*corr = xcurrVal*ycurrVal;
		}
		if(divNull ){
			 setStateNull(1, false);
                        *div = pxSdev * pySdev;
                }

	}
}

void cal_corr::merge(){
	double *corr = doubleState(0);
	bool corrNull = isStateNull(0);

	 double *div = doubleState(1);
        bool divNull = isStateNull(1);

	double accVal = doubleArg(0);
	int accValNull = isArgNull(0);

	double accDiv = doubleArg(1);
	int accDivNull = isArgNull(1);
	if(!accValNull && ! accDivNull){
		if(corrNull){
			setStateNull(0, false);
			*corr = accVal;	
			sum=*corr;
		}else{
				*corr = accVal+sum;
				sum=*corr;
		}
		if(divNull ){
                         setStateNull(1, false);
                        *div = accDiv;
                }else{
                                *div = accDiv;
                }

		N=N+1;
	}
}

ReturnValue cal_corr::finalResult(){
	double corr = doubleArg(0);
	bool corrNull = isArgNull(0);

	double div = doubleArg(1);
        int divNull = isArgNull(1);

	if (corrNull || divNull) NZ_UDX_RETURN_NULL();
	setReturnNull(false);
	corr = corr / div;
	NZ_UDX_RETURN_DOUBLE(corr);
}	
