#include <iostream>
#include "tools.h"

#define EPSLON 0.001
using Eigen::VectorXd;
using Eigen::MatrixXd;
using std::vector;

Tools::Tools() {}

Tools::~Tools() {}

VectorXd Tools::CalculateRMSE(const vector<VectorXd> &estimations,
                              const vector<VectorXd> &ground_truth) {
  /**
  TODO:
    * Calculate the RMSE here.
  */
	float px_tot=0;
	float py_tot=0;
	float vx_tot=0;
	float vy_tot=0;
	int i;
   for(i = 0; i < estimations.size(); i++){
   	   px_tot += pow((estimations[i](0) - ground_truth[i](0)), 2);
   	   py_tot += pow((estimations[i](1) - ground_truth[i](1)), 2);
   	   vx_tot += pow((estimations[i](2) - ground_truth[i](2)), 2);
   	   vy_tot += pow((estimations[i](3) - ground_truth[i](3)), 2);
   }

   VectorXd ret  = VectorXd(4);

   ret << sqrt(px_tot/i), sqrt(py_tot/i), sqrt(vx_tot/i), sqrt(vy_tot/i);
   return ret;

}

MatrixXd Tools::CalculateJacobian(const VectorXd& x_state) {
  /**
  TODO:
    * Calculate a Jacobian here.
  */

	MatrixXd ret = MatrixXd(3, 4);

	float px = x_state(0);
	float py = x_state(1);
	float vx = x_state(2);
	float vy = x_state(3);

	float x2y2 = pow(px,2) + pow(py,2);

	if(x2y2 < EPSLON) x2y2 = EPSLON;

	ret << px/(sqrt(x2y2)), py/(sqrt(x2y2)), 0, 0,
		 -py/x2y2, px/x2y2, 0, 0,
py*(vx*py-vy*px)/pow(x2y2, 1.5), px*(vy*px-vx*py)/pow(x2y2, 1.5), px/(sqrt(x2y2)), py/(sqrt(x2y2));

	return ret;

}
