#include "udxinc.h"

using namespace nz::udx_ver2;

class diff_ : public Udtf {

private:
	float value;
	int valuelen;
	int i;

public:
	diff_(UdxInit *pInit) : Udtf(pInit) {}

	static Udtf* instantiate(UdxInit*);

virtual void newInputRow() {
	double valuesX = doubleArg(0);
	bool valuesXNull = isArgNull(0);
	
	if (valuesXNull)
		throwUdxException("Input values empty");
	value = valuesX;
	i = 0;
}

virtual DataAvailable nextOutputRow() {
	
	double rmean = doubleArg(1);
	int start = i;
	if(start==1)
		return Done;
	double *rk = doubleReturnColumn(0);
	*rk = value - rmean;
	i++;
	return MoreData;
}
};

Udtf* diff_::instantiate (UdxInit* pInit) {
        return new diff_(pInit);
}

