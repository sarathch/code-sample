

#include "udxinc.h"
#include <math.h>

using namespace nz::udx_ver2;

class cal_dev: public Uda{
	public:
	double sum;
	double N;
	cal_dev(UdxInit *pInit):Uda(pInit){}
	static Uda* instantiate(UdxInit *pInit);
	virtual void initializeState();
	virtual void accumulate();
	virtual void merge();
	virtual ReturnValue finalResult();
};

Uda* cal_dev::instantiate(UdxInit *pInit){
	cal_dev *psDev = new cal_dev(pInit);
	return (Uda *)psDev;
}

void cal_dev::initializeState(){
	sum=0;
	N=0;
	setStateNull(0, true);
	setStateNull(1, true);
}

void cal_dev::accumulate(){
	double *sDev = doubleState(0);
	bool sDevNull = isStateNull(0);
	double currVal = doubleArg(0);
	bool currValNull = isArgNull(0);
	
	if(!currValNull){
		if(sDevNull){
			setStateNull(0, false);
			*sDev = currVal*currVal;
		}else
			*sDev = (currVal*currVal);
	}
}

void cal_dev::merge(){
	double *sDev = doubleState(0);
	bool sDevNull = isStateNull(0);
	
	double accVal = doubleArg(0);
	int accValNull = isArgNull(0);
	
	
	if(!accValNull){
		if(sDevNull){
			setStateNull(0, false);
			*sDev = accVal;
			sum=*sDev;
		}else{
				*sDev = accVal+sum;
				sum=*sDev;
		}
	 N=N+1;
	}
}

ReturnValue cal_dev::finalResult(){
	double sDev = doubleArg(0);
	bool sDevNull = isArgNull(0);
	if (sDevNull) NZ_UDX_RETURN_NULL();
	setReturnNull(false);
	sDev = sqrt (sDev);
	NZ_UDX_RETURN_DOUBLE(sDev);
}	
