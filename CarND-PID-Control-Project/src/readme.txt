#1. Describe the effect each of the P, I, D components had in your implementation.
------------------


PD control: Car can still keep in lane. As lecture told, I is used to
solve systematic biases. If the car does not have systematic biases, I is 
not very useful.

PI control: Without D, the car overshoot at very beginning. This is because
D factor is used to reduce overshooting. It considers how fast the error changes, and
make sure it is not too fast to incur overshooting.

ID control: Car overshooting at very beginning. Without P, the algorithm
cannot directly recognize the location of the car. I and D cannot rectify
the steering angle fast enough to make the car back to track.


#2. Describe how the final hyperparameters were chosen.
---------------

I use twiddle algorithm to find the best parameters. Every 400 points, I compare
the total error and the best error. According to the result, the algorithm changes
the parameter according to the algorithm (time 1.1 or 0.9) from the lecture. When
the sum of delta is smaller than 0.001 I stopped the iteration.

I start the parameter by manually chose P=0.2, I=0.01, D=0.3 and based on these
initial factors, the twiddle algorithm found the local optimization of
P=0.231476, I=0.011, D=3.10.
