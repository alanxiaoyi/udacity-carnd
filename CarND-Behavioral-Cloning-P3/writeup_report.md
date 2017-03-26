#**Behavioral Cloning** 


---

**Behavioral Cloning Project**

[//]: # (Image References)

[image1]: ./examples/recover1.jpg "recover1"
[image2]: ./examples/recover2.jpg "recover2"
[image3]: ./examples/recover3.jpg "recover3"
[image4]: ./examples/recover4.jpg "recover4"


---
###Files Submitted


My project includes the following files:

* model.py containing the script to create and train the model
* drive.py for driving the car in autonomous mode. I modified this file to make the car drove more smoothly by itself
* model.h5 containing a trained convolution neural network 
* writeup_report.md  summarizing the results
* run1.mp4 contains the autonomous driving with my first data set
* run2.mp4 contains the autonomous driving with my newer training set

run2.mp4 is a much smoother driving after I collecting more data.

###Model Architecture

####1. The model being used

I basically use the NVDIA model that the lecture mentioned. The structure is like following:

lambda layer to normalize the image's pixels to be between -0.5-0.5
Cropping2D layer to crop top 70 and bottom 25 pixels
conv2d 5x5x24 step 2
relu
conv2d 5x5x36 step 2
relu
conv2d 5x5x48 step 2
relu
conv2d 3x3x64 step 1
relu
conv2d 3x3x64 step 1
relu
flatten
fully connection output 100
fully connection output 50
fully connection output 10
fully connection output 1

I tried inserting dropout layers. However, for the first track, I did not see much overfitting problem. In turn, if I add many dropout layers, the loss will become much higher with the same number of epochs. Another reason that I did not use dropout layers because I did not see much overfitting problems. So for first track. I eventually give up the dropout layers. 



####2. Attempts to reduce overfitting in the model

As I explained above, I did not see much overfitting problem of my data. Below is the 7 epoch fitting loss data for my last training:

loss: 0.0219 - val_loss: 0.0182
loss: 0.0186 - val_loss: 0.0182
loss: 0.0178 - val_loss: 0.0172
loss: 0.0169 - val_loss: 0.0172
loss: 0.0161 - val_loss: 0.0169
loss: 0.0153 - val_loss: 0.0168
loss: 0.0145 - val_loss: 0.0165

From the loss results, I found the validation data keep decreasing as the training data. I regard it as a good sign of not overfitting.


####3. Model Parameter Tuning

I fixed my epoch number to be 7 (I tried 15 but it takes too much time on my local machine). I tried different numbers of batch size (32, 64, 128), the loss rate tends to be better with the smaller batch size. So I eventually go with 32.

My most effort is put on collecting training data and adjusting the algorithm I use.

####4. Training Data Collection

I collected multiple sets of training data by driving the car for many laps and recovery laps. Following the suggestion of the lecture, I basically collect three types of data set:
1. Forwarding driving. Keeping the car in the middle of the road.
2. Reverse lap driving. 
3. Recovery. Record car recover from the side of the road.

Examples of recovering a car from side of road:

![alt text][image1]
![alt text][image2]
![alt text][image3]
![alt text][image4]

I start recording when the car stay on one side, and stop recording after the car in the middle of the track.  I found that using mouse is much better than using keyboard. By using keyboard for training, the autonomous driving is much less stable. 

The final data samples contains  11493 center camera images. I split them to have 80% as training data. Data augmentation makes the total sample point to be 11493*2. 

####5. Data Augmentation

I reverse the figure and the steering angle to have an additional data point. So the total sample count will double my original sample count.

####6. Modification of drive.py

The autonomous driving is pretty slow by default (10mph). To make it drive faster and also smoother, I changed the drive.py file. The first change is to set the speed to be faster when drive straight and slower when turning. The code is shown as below:
```
        if abs(steering_angle) < 0.1:
            controller.set_desired(20)
        else:
            controller.set_desired(10)
```

In this way, when car drive straight, it can be much faster and save time.

The second change is to stablize the steering angle. I use a very simple algorithm shown as below:
```
        if abs(steering_angle-prev_angle) < 0.1:
            steering_angle = 0.5*(steering_angle+prev_angle)
        prev_angle = steering_angle
```
The code means when the car drive straight, it will not suddently make a large turn. This is based on my finding that when the car drive straight, sometimes it is not very stable. Basically it does a lot of small left/right turns to make the car go straight. This is maybe because in my first training data set, I used keyboard to control the car, thus the training is no very smooth neither. After using this algorithm, the car is more stable when drive straight. 

### Solution Stratagy

My approach can be explained as following:

I first collect data as the lecture suggested. After collecting about 5k samples, I begin the first training trail. My image augmentation scheme include reverse the image and its steering angle. 

I found even with the first trial, the car can run pretty well. Although it is not very stable but most of time, the car can stay on track. I think this is because the NVDIA neural network model works very well.

Then I begin to use the right/left camera together with the center camera. When I use the left camera, I add a value to the steering angle, while for right camera, I substract a value from the steering angle. I tried different numbers (0.5, 1.5, 2.5), however, I found both validation and training loss become higher. The autonomous driving also becomes worse. For example, when on the bridge of the first track, the car keeps deviate from the center of the road. I tried to add more recovery data, but still not working very well. I think the reason is because simply add or substract a value from steering angle for left/right camera image is not a good methodology. The code I tried for left camera is shown below:
```
source_path = batch_sample[1]
image = cv2.imread(source_path)
images.append(image)
measurement = float(batch_sample[3])+0.15
measurements.append(measurement)
```

Since I did not find a better way to use left/right camera data, I finally gave up using left/right camera. I then collect more center camera data driving the car, and during the process I found the autonomous driving becomes better and better, although the validation loss is not necessarily reduced, there were less cases that the car deviate from the lane. 

One thing worths mentioning is that in the first set of data, I drive the car by using keyboard (sometimes mouse together). Afterward, I colect new data with mouse only and tried to make the car drive as smooth as possible. The autonomous driving shows much smoother driving by using my second set of data. This again shows the importance of the training data for this project. run1.mp4 shows the result from using my first set of data, run2.mp4 shows the result from using my second set of data.

I tried different numbers of dropout layers as well. However, I found by using dropout layers, I need to run more epochs to make the validation loss to be the same level with not using any dropout layer. Since I did not see very obvious overfitting, I eventually removed all the dropout layers I tried.

To make the car drive more stable in autonomous mode, I also edited the drive.py file. Please see previous section for details.

