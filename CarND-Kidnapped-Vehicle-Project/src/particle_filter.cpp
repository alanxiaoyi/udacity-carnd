/*
 * particle_filter.cpp
 *
 *  Created on: Dec 12, 2016
 *      Author: Tiffany Huang
 */

#include <random>
#include <algorithm>
#include <iostream>
#include <numeric>
#include <math.h> 
#include <iostream>
#include <sstream>
#include <string>
#include <iterator>
#include <iostream>
#include <chrono>
#include <random>
#include "particle_filter.h"

using namespace std;

#define NUM_PARTICLE 100
#define EPS 0.001

void ParticleFilter::init(double x, double y, double theta, double std[]) {
	// TODO: Set the number of particles. Initialize all particles to first position (based on estimates of 
	//   x, y, theta and their uncertainties from GPS) and all weights to 1. 
	// Add random Gaussian noise to each particle.
	// NOTE: Consult particle_filter.h for more information about this method (and others in this file).
	unsigned seed1 = std::chrono::system_clock::now().time_since_epoch().count();
	static default_random_engine gen(seed1);
	// This line creates a normal (Gaussian) distribution for x
	normal_distribution<double> dist_x(x, std[0]);
	
	// TODO: Create normal distributions for y and psi
	normal_distribution<double> dist_y(y, std[1]);
	normal_distribution<double> dist_psi(theta, std[2]);

	for (int i = 0; i < NUM_PARTICLE; i++) {
		Particle p;
		p.id = i;
		p.x = dist_x(gen);
		p.y = dist_y(gen);
		p.theta = dist_psi(gen);
		p.weight = 1.0;
		particles.push_back(p);

	}
	is_initialized = true;
	num_particles = NUM_PARTICLE;

}

void ParticleFilter::prediction(double delta_t, double std_pos[], double velocity, double yaw_rate) {
	// TODO: Add measurements to each particle and add random Gaussian noise.
	// NOTE: When adding noise you may find std::normal_distribution and std::default_random_engine useful.
	//  http://en.cppreference.com/w/cpp/numeric/random/normal_distribution
	//  http://www.cplusplus.com/reference/random/default_random_engine/
	unsigned seed1 = std::chrono::system_clock::now().time_since_epoch().count();
	static default_random_engine gen(seed1);
	// This line creates a normal (Gaussian) distribution for x
	normal_distribution<double> dist_x(0, std_pos[0]);
	
	// TODO: Create normal distributions for y and psi
	normal_distribution<double> dist_y(0, std_pos[1]);
	normal_distribution<double> dist_psi(0, std_pos[2]);

	for (int i=0; i < num_particles; i++){
		double noise_x = dist_x(gen);
		double noise_y = dist_y(gen);
		double noise_theta = dist_psi(gen);

		if(yaw_rate < -EPS || yaw_rate > EPS){
			particles[i].x = particles[i].x + velocity/yaw_rate *
							(sin(particles[i].theta + yaw_rate * delta_t) -
							sin(particles[i].theta));

			particles[i].y = particles[i].y + velocity/yaw_rate *
							(-cos(particles[i].theta + yaw_rate*delta_t) +
							cos(particles[i].theta));
		}
		else{
			particles[i].x += velocity * delta_t * cos(particles[i].theta);
			particles[i].y += velocity * delta_t * sin(particles[i].theta);
		}


		particles[i].theta += yaw_rate * delta_t;

		particles[i].x += noise_x;
		particles[i].y += noise_y;
		particles[i].theta += noise_theta;
	}

}

void ParticleFilter::dataAssociation(std::vector<LandmarkObs> predicted, std::vector<LandmarkObs>& observations) {
	// TODO: Find the predicted measurement that is closest to each observed measurement and assign the 
	//   observed measurement to this particular landmark.
	// NOTE: this method will NOT be called by the grading code. But you will probably find it useful to 
	//   implement this method and use it as a helper during the updateWeights phase.
	for (int i = 0; i < observations.size(); i++) {
		double min = std::numeric_limits<double>::max();
		for(int j=0; j < predicted.size(); j++) {
			double dist_v = dist(observations[i].x, observations[i].y,
								predicted[j].x, predicted[j].y);
			if(dist_v < min){
				min = dist_v;
				observations[i].id = predicted[j].id;
			}
		}
	}
}

void ParticleFilter::updateWeights(double sensor_range, double std_landmark[], 
		std::vector<LandmarkObs> observations, Map map_landmarks) {
	// TODO: Update the weights of each particle using a mult-variate Gaussian distribution. You can read
	//   more about this distribution here: https://en.wikipedia.org/wiki/Multivariate_normal_distribution
	// NOTE: The observations are given in the VEHICLE'S coordinate system. Your particles are located
	//   according to the MAP'S coordinate system. You will need to transform between the two systems.
	//   Keep in mind that this transformation requires both rotation AND translation (but no scaling).
	//   The following is a good resource for the theory:
	//   https://www.willamette.edu/~gorr/classes/GeneralGraphics/Transforms/transforms2d.htm
	//   and the following is a good resource for the actual equation to implement (look at equation 
	//   3.33
	//   http://planning.cs.uiuc.edu/node99.html


	for (int i = 0; i < num_particles; i++){
		std::vector<LandmarkObs> tobs;
		std::vector<LandmarkObs> land;

		/* dont forget to reset the weights */
		particles[i].weight = 1.0;
		for(int k = 0; k < map_landmarks.landmark_list.size(); k++) {
			Map::single_landmark_s sl = map_landmarks.landmark_list[k];
			double t_dist = dist(sl.x_f, sl.y_f, particles[i].x, particles[i].y);
			if(t_dist <= sensor_range){
				LandmarkObs tmp;
				tmp.id = sl.id_i;
				tmp.x = sl.x_f;
				tmp.y = sl.y_f;
				land.push_back(tmp);
			}
		}


		for(int j = 0; j < observations.size(); j++) {
			double new_x = particles[i].x +
						cos(particles[i].theta) * observations[j].x -
						sin(particles[i].theta) * observations[j].y;
			double new_y = particles[i].y +
						cos(particles[i].theta) * observations[j].y +
						sin(particles[i].theta) * observations[j].x;

			LandmarkObs trans;
			trans.x = new_x;
			trans.y = new_y;
			tobs.push_back(trans);
		}
		dataAssociation(land, tobs);


		double tot_weight = 1;
		for (int j = 0; j < tobs.size(); j++) {
			LandmarkObs l;
			for(int k = 0; k < land.size(); k++) {
				if(land[k].id == tobs[j].id){
					l = land[k];
					break;
				}
			}

			double delta_x = tobs[j].x - l.x;
			double delta_y = tobs[j].y - l.y;
			double omega_x = std_landmark[0] * std_landmark[0];
			double omega_y = std_landmark[1] * std_landmark[1];
			double e_idx = -0.5 * (delta_x) * (delta_x) / omega_x -
						   0.5 * (delta_y) * (delta_y) / omega_y;

			double divider = 2 * M_PI * std_landmark[0] * std_landmark[1];

			tot_weight *= exp(e_idx) / divider;
		}

		particles[i].weight = tot_weight;

	}

}

void ParticleFilter::resample() {
	// TODO: Resample particles with replacement with probability proportional to their weight. 
	// NOTE: You may find std::discrete_distribution helpful here.
	//   http://en.cppreference.com/w/cpp/numeric/random/discrete_distribution
	unsigned seed1 = std::chrono::system_clock::now().time_since_epoch().count();
	static default_random_engine gen(seed1);
	vector<Particle> np;
	vector<double> weights;
	double sum_w = 0;
	for (int i = 0; i < num_particles; i++){
		sum_w += particles[i].weight;
	}
	for (int i = 0; i < num_particles; i++){
		weights.push_back(particles[i].weight/sum_w);
	}
	discrete_distribution<> d(weights.begin(), weights.end());
	for (int i = 0; i < num_particles; i++){
		np.push_back(particles[d(gen)]);
	}
	particles = np;
}

Particle ParticleFilter::SetAssociations(Particle particle, std::vector<int> associations, std::vector<double> sense_x, std::vector<double> sense_y)
{
	//particle: the particle to assign each listed association, and association's (x,y) world coordinates mapping to
	// associations: The landmark id that goes along with each listed association
	// sense_x: the associations x mapping already converted to world coordinates
	// sense_y: the associations y mapping already converted to world coordinates

	//Clear the previous associations
	particle.associations.clear();
	particle.sense_x.clear();
	particle.sense_y.clear();

	particle.associations= associations;
 	particle.sense_x = sense_x;
 	particle.sense_y = sense_y;

 	return particle;
}

string ParticleFilter::getAssociations(Particle best)
{
	vector<int> v = best.associations;
	stringstream ss;
    copy( v.begin(), v.end(), ostream_iterator<int>(ss, " "));
    string s = ss.str();
    s = s.substr(0, s.length()-1);  // get rid of the trailing space
    return s;
}
string ParticleFilter::getSenseX(Particle best)
{
	vector<double> v = best.sense_x;
	stringstream ss;
    copy( v.begin(), v.end(), ostream_iterator<float>(ss, " "));
    string s = ss.str();
    s = s.substr(0, s.length()-1);  // get rid of the trailing space
    return s;
}
string ParticleFilter::getSenseY(Particle best)
{
	vector<double> v = best.sense_y;
	stringstream ss;
    copy( v.begin(), v.end(), ostream_iterator<float>(ss, " "));
    string s = ss.str();
    s = s.substr(0, s.length()-1);  // get rid of the trailing space
    return s;
}
