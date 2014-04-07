#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>



/**********************CONSTANT-DEFINITIONS*******************/
#define ONE 1
#define UNITY 1.0
#define TWO 2.0
#define ITWO 2
#define HALF 0.5
#define ERROR 1
#define BILLION 1E9
/*************************************************************/



/***********************ARGUMENT MACROS***********************/
#define MAX(x,y) ((x)>(y)) ? (x) : (y)
#define SQR(x) ((x)*(x))
/*************************************************************/




/**********************ENUMERATION CONSTANTS******************/
typedef enum {
	CRR,            /*use Cox-Ross-Rubinstein formula for probabilities*/
	EQUIPROB,       /*use equiprobability formula*/
};

typedef enum {
	EUROCALL,       /*European call*/
	EUROPUT,        /*European put*/
	AMERCALL,       /*American call*/
	AMERPUT,        /*American put*/
};
/**************************************************************/




/**************************DATA TYPES**************************/
/*Set precision for floating-point calcuations*/
typedef float Float;

/*Node: data to be stored at each node in the binary tree*/
typedef struct{
	Float spot;     /*hypothetical spot price*/
	Float prob;     /*computed probability*/
	Float option;   /*computed option value*/
} Node;

/*TreeConst: parameters needed for building binomial tree*/
typedef struct{
	Float up;       /*multiplicative upward movement*/
	Float down;     /*multiplicatiive downward movement*/
	Float prob;     /*probability of upward movement*/
} TreeConst;

/*Ivar: structure containing five floating-point vars for option pricing*/
typedef struct{
	Float S;        /*spot price*/
	Float X;        /*strike price*/
	Float T;        /*time to expiration*/
	Float sigma;    /*volatility*/
	Float mu;       /*risk-free interest rate*/
} Ivar;
/***************************************************************/




/************************FUNCTION PROTOTYPES********************/
Float option_price(Ivar,long,int,int);
TreeConst GetConst(Ivar,int,long);
Float Max(Float,Float);
/***************************************************************/


int main(int argc, char *argv[]){
	Ivar data;
	long steps; 	/*number of steps in tree*/
	Float price;
	double startTime,endTime,accum;
	struct timespec reqStart,reqEnd;       

 
	data.S = 20;		/*current spot price*/
	data.X = 30;		/*strike price of option*/
	data.T = 2.0;		/*time to expiration*/
	data.sigma=0.20;	/*volatility*/
	data.mu=0.05;		/*risk-free interest rate*/
        
        steps = atoi(argv[1]);
	
	clock_gettime(CLOCK_REALTIME, &reqStart);
	price = option_price(data, steps, EUROPUT, CRR);
	printf("EUROPUT WITH CRR:\t%8.3f\n", price);
	clock_gettime(CLOCK_REALTIME, &reqEnd);

	accum = ( reqEnd.tv_sec - reqStart.tv_sec )
    	+ ( reqEnd.tv_nsec - reqStart.tv_nsec )
      	/ BILLION;
     	printf( "\n TIME TAKEN FOR BOPM Routine : %lf\n", accum );


	clock_gettime(CLOCK_REALTIME, &reqStart);
	price = option_price(data, steps, EUROCALL, CRR);
	clock_gettime(CLOCK_REALTIME, &reqEnd);
	printf("EUROCALL WITH CRR\t%8.3f\n", price);

        accum = ( reqEnd.tv_sec - reqStart.tv_sec )
    	+ ( reqEnd.tv_nsec - reqStart.tv_nsec )
      	/ BILLION;
     	printf( "\n TIME TAKEN FOR BOPM Routine : %lf\n", accum );


	clock_gettime(CLOCK_REALTIME, &reqStart);
	price = option_price(data, steps, AMERPUT, CRR);
	printf("AMERPUT WITH CRR\t%8.3f\n", price);
	clock_gettime(CLOCK_REALTIME, &reqEnd);

        accum = ( reqEnd.tv_sec - reqStart.tv_sec )
    	+ ( reqEnd.tv_nsec - reqStart.tv_nsec )
      	/ BILLION;
     	printf( "\n TIME TAKEN FOR BOPM Routine : %lf\n", accum );

	
	clock_gettime(CLOCK_REALTIME, &reqStart);	
	price = option_price(data, steps, AMERCALL, CRR);
	printf("AMERCALL WITH CRR\t%8.3f\n", price);
	clock_gettime(CLOCK_REALTIME, &reqEnd);

        accum = ( reqEnd.tv_sec - reqStart.tv_sec )
    	+ ( reqEnd.tv_nsec - reqStart.tv_nsec )
      	/ BILLION;
     	printf( "\n TIME TAKEN FOR BOPM Routine : %lf\n", accum );

	return 0;
}

/*calculate up, down, and probability based on method used*/
TreeConst GetConst(Ivar option_data, int method, long steps){
	TreeConst constants;
	Float mu, sigma, dt;


	/******DATA FOR DETERMINATION OF UP, DOWN, AND PROB******/
	mu = option_data.mu;
	sigma = option_data.sigma;
	dt = option_data.T / (steps - ONE);
	/********************************************************/
	

	/******COX-ROSS-RUBINSTEIN FORMULAE**********************/
	if(CRR == method){
		constants.up =   exp(sigma * sqrt(dt));
		constants.down = UNITY / constants.up;
		constants.prob = (exp(mu*dt) - constants.down) / 
			(constants.up - constants.down);
	}
	
	/********************************************************/


	/**********FAULTY VALUE FOR METHOD***********************/
	else{
		fprintf(stderr, "ERROR: invalid method specified\n");
		exit(ERROR);
	}
	/*********************************************************/

	return constants;
}




/*return option price*/
Float option_price(
	Ivar vars,      /*five floating vars defined in Ivar type*/
	long steps,     /*number of steps in the binomial tree*/
	int type,       /*call / put  -- American/European*/
	int method      /*Cox-Ross-Rubinstein or Equiprobability*/
){

	Node *tree;
	TreeConst constants;    /*constants for CRR or EQUIPROB*/
	Float up, down;         /*constants for CRR or EQUIPROB*/
	Float up_prob;          /*constants for CRR or EQUIPROB*/
	Float down_prob;        /*constants for CRR or EQUIPROB*/
	long StepPtr;           /*dummy: step pointer*/
	int count=0;
	long SpotPtr;           /*dummy: price pointer*/ 
	Float discount;         /*discount factor for one time step*/
	Float price;            /*value to be returned*/
	Float dt;               /*time step*/



	/******CALCULATE DISCOUNT FACTOR FOR ONE TIME STEP********/
	dt = vars.T / (steps - ONE);
	discount = exp(-vars.mu * dt);
	/*********************************************************/


	/***CALCULATE APPROPRIATE CONSTANTS FOR SPECIFIED METHOD**/
	constants = GetConst(vars, method, steps);
	up = constants.up;
	down = constants.down;
	up_prob = constants.prob;
	down_prob = UNITY - up_prob;
	/*********************************************************/
	


	/******ALLOCATE SPACE FOR ONE STEP IN BINOMIAL TREE*******/
	tree = (Node*)malloc((steps + ONE)*sizeof(Node));
	if(NULL == tree){
		fprintf(stderr, "ERROR: failure of malloc()\n");
		exit(ERROR);
	}
	/********************************************************/



	/*********FIRST INITIALIZE ZEROTH NODE*******************/
	tree[0].spot = vars.S;
	tree[0].prob = UNITY;
	/*********************************************************/



	/*********FILL TREE WITH SPOT PRICES**********************/
	for(StepPtr = ONE; StepPtr < steps; StepPtr++){
		tree[StepPtr].spot = tree[StepPtr - ONE].spot * up;
		for(SpotPtr = 0; SpotPtr < StepPtr; SpotPtr++){
			tree[SpotPtr].spot *= down; 
		}
	}

	/**********************************************************/



	/********FILL TREE WITH PROBABILITIES**********************/
	for(StepPtr = ONE; StepPtr < steps; StepPtr++){

		/*calculate prob for highest spot price*/
		tree[StepPtr].prob =
			tree[StepPtr - ONE].prob * up_prob;

		/*calculate prob for spot price in between*/
		for(SpotPtr = StepPtr - ONE; SpotPtr > 0; SpotPtr--){
			tree[SpotPtr].prob = 
				tree[SpotPtr].prob * down_prob
				+
				tree[SpotPtr - ONE].prob * up_prob
			;
		}

		/*calculate prob for lowest spot price*/
		tree[0].prob *= down_prob;
	}
	/**********************************************************/



	/******CALCULATE OPTION VALUES ON EXPIRATION DATE**********/
	if((EUROCALL == type)||(AMERCALL == type)){
		for(SpotPtr = 0; SpotPtr < steps; SpotPtr++){
			tree[SpotPtr].option =  
				MAX(0.0, tree[SpotPtr].spot - vars.X);
		}
	}
	else if((EUROPUT == type)||(AMERPUT == type)){
		for(SpotPtr = 0; SpotPtr < steps; SpotPtr++){
			tree[SpotPtr].option =
				MAX(0.0, vars.X - tree[SpotPtr].spot);
		}
	}
	else{
		fprintf(stderr, "ERROR: invalid option type\n");
		exit(ERROR);
	}
	/***********************************************************/


	/*WALK BACKWARDS THROUGH BINOMIAL TREE FROM EXPIRATION TO ORIGIN*/
	price=0.0;
	for(StepPtr = steps - ITWO; StepPtr >=0; StepPtr--){
		for(SpotPtr = 0; SpotPtr <= StepPtr; SpotPtr++){
			Float early = 0.0;      /*value if exercised early*/
			count++;
			/*calculate weighted average*/
			tree[SpotPtr].option = 
				down_prob * tree[SpotPtr].option 
				+
				up_prob * tree[SpotPtr + ONE].option;

			/*calculate spot price*/
			tree[SpotPtr].spot =
				tree[SpotPtr+ONE].spot / up;

			/*apply discount factor*/
			tree[SpotPtr].option *= discount;


			/*determine if early exercise is desirable*/
			if(AMERCALL == type){
				early = MAX(0.0, tree[SpotPtr].spot - vars.X);

			}
			else if(AMERPUT == type){
				early = MAX(0.0, vars.X - tree[SpotPtr].spot);
			}
			
			tree[SpotPtr].option = MAX(
				tree[SpotPtr].option,
				early
			);


		}
	}


	/*******************************************************************/

	

	/*price is...*/
	price =  tree[0].option;

	/*return the binomial tree to the memory pool and return the result*/
	free(tree);
	
	return price;
}
