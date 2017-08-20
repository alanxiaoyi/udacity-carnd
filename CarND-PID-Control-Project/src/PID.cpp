#include "PID.h"
#include <cstdio>

using namespace std;

/*
* TODO: Complete the PID class.
*/

#define INIT_DP  0.01
#define INIT_DI 0.0001
#define INIT_DD 0.1
#define GRANU 400

#define TWIDDLE 0


enum CUR_STATE{
	ADD_PARAM = 0,
	SUB_PARAM
};

PID::PID() {}

PID::~PID() {}

void PID::Init(double Kp_, double Ki_, double Kd_) {

  K_param[0] = Kp_;
  K_param[1] = Ki_;
  K_param[2] = Kd_;


  p_error = 0.0;
  i_error = 0.0;
  d_error = 0.0;

  total_error = 0.0;

  best_error = 10000.0;

  counter = 1;

  d_param[0] = INIT_DP;
  d_param[1] = INIT_DI;
  d_param[2] = INIT_DD;

  cur_state = ADD_PARAM;
  cur_param = 0;

}

void PID::UpdateError(double cte) {

	d_error = cte - p_error;
	p_error = cte;

	/* at very begining when car start, since the speed is low,
	 * the I factor could be accumulated to very large and cause
	 * overshooting. This hacky work around can avoid the initial
	 * overshooting. In real case, we should consider speed when
	 * use I error.
	 */
	if(counter > 100)
		i_error = i_error + cte;

	total_error += cte * cte; //need absolute value;



	/**
	 * Twiddle implementation: a state machine
	 */
	double sum = d_param[0] + d_param[1] + d_param[2];
	if (counter % GRANU == 0 && sum > 0.0001 && TWIDDLE){
		if(total_error < best_error){
			best_error = total_error;
			d_param[cur_param] *= 1.1;
			cur_param = (cur_param + 1) % 3;
			cur_state = ADD_PARAM;
			K_param[cur_param] += d_param[cur_param];
		} else {
			if(cur_state == ADD_PARAM) {
				cur_state = SUB_PARAM;
				K_param[cur_param] -= 2 * d_param[cur_param];
			} else {
				K_param[cur_param] += d_param[cur_param];
				d_param[cur_param] *= 0.9;
				cur_param = (cur_param + 1) % 3;
				cur_state = ADD_PARAM;
			}
		}

		printf("k_param!!: %f, %f, %f\n", K_param[0], K_param[1], K_param[2]);
	}


	counter++;

}

double PID::TotalError() {
	return p_error + i_error + d_error;
}

