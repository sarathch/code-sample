#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <mpi.h>



/**********************CONSTANT-DEFINITIONS*******************/
#define ONE 1
#define UNITY 1.0
#define TWO 2.0
#define ITWO 2
#define HALF 0.5
#define ERROR 1
#define BILLION 1E9
#define TAG 13
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
Float option_price(Ivar,long,int,int,int,int,int);
TreeConst GetConst(Ivar,int,long);
Float Max(Float,Float);
/***************************************************************/
       
int main(int argc, char *argv[]) {
	Ivar data;
        int steps;     /*number of steps in tree*/
        Float price;
	int myrank,numnodes,chunkSize;
        double startTime,endTime,accum;
        struct timespec reqStart,reqEnd;

  	MPI_Init(&argc, &argv);

  	MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
  	MPI_Comm_size(MPI_COMM_WORLD, &numnodes);
	
	steps= atoi(argv[1]);

  	chunkSize = steps/numnodes;

 
	data.S = 20;		/*current spot price*/
	data.X = 30;		/*strike price of option*/
	data.T = 2.0;		/*time to expiration*/
	data.sigma=0.20;	/*volatility*/
	data.mu=0.05;		/*risk-free interest rate*/
        

	clock_gettime(CLOCK_REALTIME, &reqStart);
        price = option_price(data, steps, EUROPUT, CRR, myrank, chunkSize, numnodes);
        clock_gettime(CLOCK_REALTIME, &reqEnd);
        accum = ( reqEnd.tv_sec - reqStart.tv_sec )
    	+ ( reqEnd.tv_nsec - reqStart.tv_nsec )
      	/ BILLION;
        printf( "\n TIME TAKEN FOR BOPM Routine FOR node %d : %lf\n", myrank,accum );
        if(myrank==0){
                printf("EUROPUT WITH CRR\t%8.3f\n", price);
		printf( "\n Final time FOR node %d : %lf\n", myrank,accum );
	}
	MPI_Barrier(MPI_COMM_WORLD);

	 clock_gettime(CLOCK_REALTIME, &reqStart);
	price = option_price(data, steps, EUROCALL, CRR, myrank, chunkSize, numnodes);
	 clock_gettime(CLOCK_REALTIME, &reqEnd);
	if(myrank==0)
		printf("EUROCALL WITH CRR\t%8.3f\n", price);
        accum = ( reqEnd.tv_sec - reqStart.tv_sec )
    	+ ( reqEnd.tv_nsec - reqStart.tv_nsec )
      	/ BILLION;
         printf( "\n TIME TAKEN FOR BOPM Routine FOR node %d : %lf\n", myrank,accum );
	MPI_Barrier(MPI_COMM_WORLD);
	
	clock_gettime(CLOCK_REALTIME, &reqStart);
        price = option_price(data, steps, AMERPUT, CRR, myrank, chunkSize, numnodes);
        clock_gettime(CLOCK_REALTIME, &reqEnd);
        accum = ( reqEnd.tv_sec - reqStart.tv_sec )
    + ( reqEnd.tv_nsec - reqStart.tv_nsec )
      / BILLION;
        printf( "\n TIME TAKEN FOR BOPM Routine FOR node %d : %lf\n", myrank,accum );
        if(myrank==0){
                printf("AMERPUT WITH CRR\t%8.3f\n", price);
		printf( "\n Final time FOR node %d : %lf\n", myrank,accum );
	}
	MPI_Barrier(MPI_COMM_WORLD);
	
	clock_gettime(CLOCK_REALTIME, &reqStart);	
	price = option_price(data, steps, AMERCALL, CRR, myrank, chunkSize, numnodes);
	clock_gettime(CLOCK_REALTIME, &reqEnd);
        accum = ( reqEnd.tv_sec - reqStart.tv_sec )
    + ( reqEnd.tv_nsec - reqStart.tv_nsec )
      / BILLION;
	printf( "\n TIME TAKEN FOR BOPM Routine FOR node %d : %lf\n", myrank,accum );
	if(myrank==0){
		printf("AMERCALL WITH CRR\t%8.3f\n", price);
		printf( "\n Final time FOR node %d : %lf\n", myrank,accum );
	}
	   MPI_Finalize();
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
	int method,      /*Cox-Ross-Rubinstein or Equiprobability*/
	int myrank,
	int chunkSize,
	int numnodes
){

	Node *tree;
	TreeConst constants;    /*constants for CRR or EQUIPROB*/
	Float up, down;         /*constants for CRR or EQUIPROB*/
	Float up_prob;          /*constants for CRR or EQUIPROB*/
	Float down_prob;        /*constants for CRR or EQUIPROB*/
	Float *tmp_options,*tmp_spots,bcast_opt,bcast_spot;
	long StepPtr;           /*dummy: step pointer*/
	long iter;
	long SpotPtr;           /*dummy: price pointer*/ 
	Float discount;         /*discount factor for one time step*/
	Float price;            /*value to be returned*/
	Float dt;               /*time step*/
	int offset,ptr,bound;
	struct timespec reqStart,reqEnd,reqStart_loop,reqEnd_loop;
	double accum,accum_loop,ttime=0;
	int c1=0,c2=0;

	
	tmp_options = malloc(sizeof(Float)*steps);
        tmp_spots = malloc(sizeof(Float)*steps);
	

	offset = myrank*chunkSize;
	

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

	/*********FILL TREE WITH SPOT PRICES*********************/

	for(StepPtr = ONE; StepPtr < steps; StepPtr++){
		tree[StepPtr].spot = tree[StepPtr - ONE].spot * up;
		ptr = (int) StepPtr;
		 if(offset+chunkSize<StepPtr)
                        iter = offset+chunkSize;
                else
                        iter = StepPtr;

                for(SpotPtr = offset; SpotPtr < iter; SpotPtr++)
                        tree[SpotPtr].spot *= down;
		

		 if(StepPtr>0){
			 
			if((ptr+1) % chunkSize ==0){
				if(myrank+1 == ((ptr+1)/chunkSize)%numnodes){
                        	bcast_spot=tree[StepPtr].spot;
                                MPI_Send(&bcast_spot, 1, MPI_FLOAT,myrank+1,TAG,MPI_COMM_WORLD);
		 		}
    				else if(myrank == ((ptr+1)/chunkSize)%numnodes){
                                MPI_Recv(&bcast_spot,1,MPI_FLOAT,myrank-1,TAG,MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                                tree[offset-1].spot=bcast_spot;
                        	}
			}
		}

	}

        

	/******CALCULATE OPTION VALUES ON EXPIRATION DATE**********/
	if((EUROCALL == type)||(AMERCALL == type)){
		for(SpotPtr = offset; SpotPtr < ((myrank*chunkSize)+chunkSize); SpotPtr++){
			tree[SpotPtr].option =  
				MAX(0.0, tree[SpotPtr].spot - vars.X);
		}
		if(myrank>0){
			bcast_opt=tree[offset].option;
			bcast_spot=tree[offset].spot;
                                MPI_Send(&bcast_opt,1 , MPI_FLOAT,myrank-1,TAG,MPI_COMM_WORLD);
				if(AMERCALL == type)
					MPI_Send(&bcast_spot,1 , MPI_FLOAT,myrank-1,TAG,MPI_COMM_WORLD);
                }
		if(myrank<numnodes-1){
			MPI_Recv(&bcast_opt,1,MPI_FLOAT,myrank+1,TAG,MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			if(AMERCALL == type){
				MPI_Recv(&bcast_spot,1,MPI_FLOAT,myrank+1,TAG,MPI_COMM_WORLD, MPI_STATUS_IGNORE);
				tree[offset+chunkSize].spot=bcast_spot;
			}
			tree[offset+chunkSize].option=bcast_opt;
		}
		
	}
	else if((EUROPUT == type)||(AMERPUT == type)){
		for(SpotPtr = offset; SpotPtr < ((myrank*chunkSize)+chunkSize) ; SpotPtr++){
			tree[SpotPtr].option =
				MAX(0.0, vars.X - tree[SpotPtr].spot);
		}
		if(myrank>0){
                        bcast_opt=tree[offset].option;
                                MPI_Send(&bcast_opt,1 , MPI_FLOAT,myrank-1,TAG,MPI_COMM_WORLD);
                }
                if(myrank<numnodes-1){
                        MPI_Recv(&bcast_opt,1,MPI_FLOAT,myrank+1,TAG,MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                        tree[offset+chunkSize].option=bcast_opt;
                }
	}
	else{
		fprintf(stderr, "ERROR: invalid option type\n");
		exit(ERROR);
	}
	/***********************************************************/

	for(SpotPtr = 0; SpotPtr < steps; SpotPtr++){
                        tmp_options[SpotPtr]=tree[SpotPtr].option;
                        tmp_spots[SpotPtr]=tree[SpotPtr].spot;
	}


	/*WALK BACKWARDS THROUGH BINOMIAL TREE FROM EXPIRATION TO ORIGIN*/
clock_gettime(CLOCK_REALTIME, &reqStart);

	price=0.0;
	for(StepPtr = steps - ITWO; StepPtr >=0; StepPtr--){
		if(offset+chunkSize<StepPtr)
			iter = offset+(chunkSize-1);
		else
			iter = StepPtr;
		
		c1++;
		for(SpotPtr = offset; SpotPtr <= iter; SpotPtr++){
			Float early = 0.0;      /*value if exercised early*/

			/*calculate weighted average*/

			tmp_options[SpotPtr] = 
				down_prob * tmp_options[SpotPtr]
				+
				up_prob * tmp_options[SpotPtr + ONE];

			/*calculate spot price*/
				tmp_spots[SpotPtr] =
					tmp_spots[SpotPtr + ONE] / up;

			/*apply discount factor*/
			tmp_options[SpotPtr] *= discount;

			/*determine if early exercise is desirable*/
			if(AMERCALL == type){
				early = MAX(0.0, tmp_spots[SpotPtr] - vars.X);

			}
			else if(AMERPUT == type){
				early = MAX(0.0, vars.X - tmp_spots[SpotPtr]);
			}
			tmp_options[SpotPtr] = MAX(
				tmp_options[SpotPtr],
				early);
		 }
	clock_gettime(CLOCK_REALTIME, &reqStart_loop);
		 if(myrank>0){
                        bcast_opt=tmp_options[offset];
                        MPI_Send(&bcast_opt, 1, MPI_FLOAT,myrank-1,TAG,MPI_COMM_WORLD);
                 }
                 if(myrank<numnodes-1){
                        MPI_Recv(&bcast_opt,1,MPI_FLOAT,myrank+1,TAG,MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                        tmp_options[offset+chunkSize]=bcast_opt;
                 }
	clock_gettime(CLOCK_REALTIME, &reqEnd_loop);
		


		 if(AMERCALL == type || AMERPUT == type){
			
			if(myrank>0){
                        bcast_spot=tmp_spots[offset];
                                MPI_Send(&bcast_spot, 1, MPI_FLOAT,myrank-1,TAG,MPI_COMM_WORLD);
                 	}
                 	if(myrank<numnodes-1){
                        	MPI_Recv(&bcast_spot,1,MPI_FLOAT,myrank+1,TAG,MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                        	tmp_spots[offset+chunkSize]=bcast_spot;
                 	}
		 }
	accum_loop = ( reqEnd_loop.tv_sec - reqStart_loop.tv_sec )
    	+ ( reqEnd_loop.tv_nsec - reqStart_loop.tv_nsec )
      	/ BILLION;
	ttime+=accum_loop;

	}

	/*******************************************************************/

	clock_gettime(CLOCK_REALTIME, &reqEnd);
	
	if(myrank==0){
	/*price is...*/
		price =  tmp_options[0];
		accum = ( reqEnd.tv_sec - reqStart.tv_sec )
    		+ ( reqEnd.tv_nsec - reqStart.tv_nsec )
      		/ BILLION;
         	printf( "\n TIME TAKEN FOR WalkBack Routine : %lf::%lf\n", accum,ttime );
	}

	/*return the binomial tree to the memory pool and return the result*/
	free(tree);
	
	return price;
}
