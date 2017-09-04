# CarND-Controls-MPC
Self-Driving Car Engineer Nanodegree Program

---
1. Student describes their model in detail. This includes the state, actuators and update equations.
---
The model we should use is:

x1 =  (x0 + v0 * CppAD::cos(psi0) * dt);

y1 = (y0 + v0 * CppAD::sin(psi0) * dt);

psi1 =  (psi0 + v0 * delta0 / Lf * dt);

v1 =  (v0 + a0 * dt);

cte1 = ((f0 - y0) + (v0 * CppAD::sin(epsi0) * dt));

epsi1 =  ((psi0 - psides0) + v0 * delta0 / Lf * dt);


We use delta and a as the steering and throttle actuators. 

2. Choose of N and delta t:
---
I tried different pairs of values. I will use 10/0.1, 10/0.2, and 20/0.1 as example
to describe the difference.

My car can run on track with 10/0.1 well. When I changed to 10/0.2, the next fitting
point of the green curve will be farther ahead of the car. This causes the car
to run on the curb when the track has a sharp curve. Because the fitting point is
farther ahead, the car tries to shortcut to that point. 

For N, it will affect how far ahead in total we calculate. In my test, it will
not reduce the stability of the car, however, it could increase the calculation cost
a lot since we fit more points.


3. Cost function:
---

We need to tune the cost function to make the car stable. My initial try failed to keep
car on track. It moves in a zig-zag way.
Following this post:https://discussions.udacity.com/t/mpc-cost-paramter-tuning-question/354670
I add the cost function as:

fg[0] += 1000*CppAD::pow(vars[delta_start + t] * vars[v_start+t], 2);

This helps a lot to maintain the car on track.

4. Considering the latency:
---

I found latency is an important factor to maintain the car on track. Basically I followed
https://discussions.udacity.com/t/how-to-incorporate-latency-into-the-model/257391/63 to 
implement my code.

Basically, we need to consider the "future" state of the car after the latency, before we
feed the values into the mpc.solve. The formula I use is (after transform to car's local
coordinates):

px = v*latency;

py = 0;

psi = -v*delta*latency/Lf;

epsi = -atan(coeffs[1]) + psi; 

cte= polyeval(coeffs,0)+v*sin(epsi)*latency;

v += a*latency;


By using these formulas before feeding car's state into mpc.solve, we can take the latency
into consideration.
