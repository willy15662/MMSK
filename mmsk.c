#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#define Idle 0
#define Busy 1
#define MODLUS 2147483647
#define MULT1       24112
#define MULT2       26143

//math
//階乘 
int factorial(int n){
	if((n==0)||(n==1)) return 1;
	else return n*factorial(n-1);
} 
//p0 equation  page42
double P0(float lamda, float mu, int s, int k){
	double sum = 1.0;
	int i;
	for(i=1;i<s;i++)
		sum += pow((lamda/mu),i) / factorial(i);
	sum += (pow((lamda/mu), s) / factorial(s) * ((1-pow((lamda/(s*mu)), (k-s+1))) / (1-(lamda/(s*mu)))));
	return 1/sum;
}

double Pn(float lamda, float mu, int s, int k, int n){
	if(n<s)
		return ((pow((lamda/mu), n)) / factorial(n)) * P0(lamda, mu, s, k);
	return ((pow((lamda/mu), n)) / (factorial(s) * pow(s, (n-s)))) * P0(lamda, mu, s, k);
}

double Lq_math(float lamda, float mu, int s, int k){
	double result = 0;
	int i;
	for(i=s;i<k+1;i++)
		result += (i-s)*Pn(lamda, mu, s, k, i);
	return result;
}

double L_math(float lamda, float mu, int s, int k){
	double sum1 = 0, sum2 = 0;
	int i;
	for(i=0;i<s;i++){
		double temp = Pn(lamda, mu, s, k, i);
		sum1 += i*temp;
		sum2 += temp; 
	}
	sum2 = s * (1- sum2);
	return (Lq_math(lamda, mu, s, k) + sum1 + sum2);
}

double lamdaEff(float lamda , float mu, int s, int k){
	return lamda*(1-Pn(lamda, mu, s, k, k));
}

double W_math(float lamda, float mu, int s, int k){
	return (L_math(lamda, mu, s, k) / lamdaEff(lamda, mu, s, k));
} 
	    
double Wq_math(float lamda, float mu, int s, int k){
    return (Lq_math(lamda, mu, s, k) / lamdaEff(lamda, mu, s, k));
}
//*****************找隨機數**************************
int Server_num = 0;
static long zrng = 903;

float lcgrand()
{
    long zi, lowprd, hi31;

    zi     = zrng;
    lowprd = (zi & 65535) * MULT1;
    hi31   = (zi >> 16) * MULT1 + (lowprd >> 16);
    zi     = ((lowprd & 65535) - MODLUS) +
             ((hi31 & 32767) << 16) + (hi31 >> 15);
    if (zi < 0) zi += MODLUS;
    lowprd = (zi & 65535) * MULT2;
    hi31   = (zi >> 16) * MULT2 + (lowprd >> 16);
    zi     = ((lowprd & 65535) - MODLUS) +
             ((hi31 & 32767) << 16) + (hi31 >> 15);
    if (zi < 0) zi += MODLUS;
    zrng = zi;
    return (zi >> 7 | 1) / 16777216.0;
}

//用來找隨機數 
float expon (float mean){
    return -mean* log(lcgrand());
}
//**************************simulation***************************************
//找最快被服務完的 
double find_min(double *next_dept_time, int S){
    double min = next_dept_time[1];
    Server_num = 1;
	int i;
    for (i=2;i<=S;i++){
        if (min > next_dept_time[i]){
            min=next_dept_time[i];
            Server_num = i;
        }
    }
    return min;
}

void arrival (int S, double *Time_Arrival, int *server_status, double *next_dept_time, double* Time_arrival_sys, int * Q_limit);
void depature (int S, double *Time_Arrival, int *server_status, double *next_dept_time, double* Time_arrival_sys);

//被服務完陣列向前移 
void move (double* Time_arrival_sys, int S){
	int i;
    for (i = 0; i < S ;i++){
        Time_arrival_sys[i] = Time_arrival_sys[i+1];
    }
}

double sim_time = 0,next_arr_time = 0,Area_Under_Q = 0,Area_Under_System = 0,Last_Event_Time = 0,sys_delay = 0,total_sys_delay = 0;
double Total_q_Delay = 0,q_Delay = 0,Time_Since_Last_Event = 0, interarrival_time = 0,service_time = 0;
int Num_In_Queue = 0, aleady_fin_Custs = 0, index = -1;

int main()
{
// Initiailize
	FILE *fp;
	fp = fopen("mmsk.csv", "w"); 
    double W_ = 0, Wq_ =0, Lq_ = 0, L_ = 0;
	int S = 0, K = 0, Number_of_customer = 0, i;
       
    //初值輸入 
	float lamda = 2;
	interarrival_time = 60.0/lamda;
    next_arr_time = expon(interarrival_time);
	printf("%f\n", next_arr_time);
	service_time = 40;
	float mu = (1.0/service_time)*60.0;
	
	Number_of_customer = 10000;
	S = 1;
	K = 10;
	
	double next_dept_time [S+1];
    int server_status [S+1];
    double Time_arrival_sys[S];

    int Q_limit = K - S;
    double Time_Arrival[Q_limit+1];

    for (i=1;i<Q_limit+1;i++){
        Time_Arrival[i] = 0;
    }
	for (i=1;i<=S;i++){ // 設定一個很大的離開值 
        next_dept_time[i] = pow(10,30);
        server_status[i] = Idle;
    }
// 程式執行 
	// while 還有客人 
   while (aleady_fin_Custs < Number_of_customer){
   	     //當下個人還沒到 先執行離開 
        if (next_arr_time > find_min(next_dept_time, S)){  
            depature (S, Time_Arrival, server_status, next_dept_time, Time_arrival_sys);
        }
		//下個人到 讓他進入 
        else {  
            arrival (S,Time_Arrival, server_status, next_dept_time, Time_arrival_sys, &Q_limit);
        }
    }
	//輸出 
	printf("simulation\n");
	L_ = Area_Under_System/sim_time;         //所有人在系統中的時間總和 / 系統總時間 
	Lq_ = Area_Under_Q/sim_time;             //所有人在Queue中的時間總和 / 系統總時間  
	W_ = total_sys_delay/aleady_fin_Custs;   //系統中總Delay總和 / 被服務人數   
	Wq_ = Total_q_Delay/aleady_fin_Custs;    //Queue中總Delay總和 / 被服務人數 
    printf("L_ = %.8f ", L_);
    printf("Lq_= %.8f ",Lq_);  
    printf ("W_= %.8f ", W_/60);
    printf("Wq_= %.8f \n",Wq_/60);
	printf("L = %.8lf ", L_math(lamda, mu, S, K));
	printf("Lq = %.8lf ", Lq_math(lamda, mu, S, K));
	printf("W = %.8lf ", W_math(lamda, mu, S, K));
	printf("Wq = %.8lf ", Wq_math(lamda, mu, S, K));
	
//	fprintf(fp, "%.8lf,", L_math(lamda, mu, S, K));
//	fprintf(fp, "%.8lf,", Lq_math(lamda, mu, S, K));
//	fprintf(fp, "%.8lf,", W_math(lamda, mu, S, K));
//	fprintf(fp, "%.8lf,", Wq_math(lamda, mu, S, K));
//	fprintf(fp, "%.8f,", L_);
//	fprintf(fp, "%.8f,",Lq_);
//	fprintf(fp, "%.8f,", W_/60);
//	fprintf(fp, "%.8f,\n",Wq_/60);

    fclose(fp);
    return 0;
}

void arrival (int S, double *Time_Arrival, int *server_status, double *next_dept_time, double* Time_arrival_sys, int * Q_limit){

	sim_time = next_arr_time;      //更新時間 
	next_arr_time = sim_time + expon(interarrival_time);   //更新下個人到的時間 
	
	Time_Since_Last_Event = sim_time - Last_Event_Time;
	Last_Event_Time = sim_time;
	Area_Under_Q += (Num_In_Queue) * (Time_Since_Last_Event);
	if(server_status[1] == Idle){
	    Area_Under_System += (Num_In_Queue) * (Time_Since_Last_Event);          
	}else{
	    Area_Under_System += (Num_In_Queue + 1) * (Time_Since_Last_Event);
	}
	int i;
	//若空閒 讓他進入 
	for (i=1;i<=S;i++){
	    if (server_status[i] == Idle){
	        (aleady_fin_Custs)++;
	        server_status[i]=Busy;
	        next_dept_time[i]= sim_time +  expon(service_time);
	        (index) ++;
	        Time_arrival_sys[index] = sim_time;
	        return;
	    }
	}
	//若busy將時間加進總時間內 
	for(i=2;i<=S;i++){
	    if(server_status[i] == Busy){
	        Area_Under_System += (Time_Since_Last_Event);
	    }
	}
	    (Num_In_Queue)++;
	//若滿人 請他離開 
    if (Num_In_Queue> *Q_limit){
    	(Num_In_Queue)--;
    }
    else {
        Time_Arrival[Num_In_Queue] = sim_time;
    }
}

void depature (int S, double *Time_Arrival, int *server_status, double *next_dept_time, double* Time_arrival_sys){

	sim_time = next_dept_time[Server_num];
	Time_Since_Last_Event = sim_time - Last_Event_Time;
	Last_Event_Time = sim_time;
	Area_Under_Q += (Num_In_Queue)*(Time_Since_Last_Event);
	if(server_status[1] == Idle){
	    Area_Under_System += (Num_In_Queue) * (Time_Since_Last_Event);
	}else{
	    Area_Under_System += (Num_In_Queue + 1) * (Time_Since_Last_Event);
	}
	int i;
	for(i=2;i<=S;i++){
	    if(server_status[i] == Busy){
	        Area_Under_System += (Time_Since_Last_Event);
	    }
	}
	//沒人在等 
	if (Num_In_Queue ==0){
	    server_status[Server_num] = Idle;
	    next_dept_time [Server_num] = pow(10,30);
	    sys_delay = sim_time - Time_arrival_sys[0];
	    total_sys_delay += sys_delay;
	    (index)--;
	    move(Time_arrival_sys, S);
	}
	//有人在等 
	else{
		(Num_In_Queue)--;
		q_Delay=sim_time-Time_Arrival[1];
		sys_delay = sim_time-Time_arrival_sys[0];
		Total_q_Delay += q_Delay;
		total_sys_delay += sys_delay;
		(index)--;
		
	    move(Time_arrival_sys, S);
	    (index) ++;
	    Time_arrival_sys[index] = Time_Arrival[1];
	
	    (aleady_fin_Custs)++;
	    next_dept_time[Server_num] = sim_time + expon(service_time);
	
	    for (i=1; i<=Num_In_Queue; i++){
	        Time_Arrival[i]=Time_Arrival[i+1];
    	}
	}
}


