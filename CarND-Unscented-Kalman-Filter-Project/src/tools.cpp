#include <iostream>
#include "tools.h"

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