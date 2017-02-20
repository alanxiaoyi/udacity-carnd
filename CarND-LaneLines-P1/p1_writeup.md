[//]: # (Image References)


---

### Reflection

###1. Describe your pipeline. As part of the description, explain how you modified the draw_lines() function.

My pipeline has steps described as below:
1. convert to grey scale.
2. filter the noise by gaussian blur.
3. find the edge by using canny method.
4. get the interest area using four indexes.
5. get hough lines.
6. extrapolate the hough lines.
7. write the image back to disk.


To extrapolate the hough lines, I used two different methods. 
1. Avergae the hough lines: 
I use the slope of lines to recognize lines that should belone to either left or right lane. Then I average the slopes of all the lines. I also average all the begining points of the lines belonging to one lane (the beginning point start from the bottom edge of the figure), and use the avergaed point as the starting point as my single extrapolated lines.

2. Using linear regression:
I recognize points blonging to left lanes or right lanes. I use np.polyfit to find fit the single extrapolated line.


I found the first method is less stable, so I eventually stick to the second method.



###2. Identify potential shortcomings with your current pipeline

The pipeline can finish its basic job but I find the grey scaled figure may not work well when the yellow line covered by a shadow (like the one in the challenge video). 

For the average method to extrapolating lines, the line is not very stable in the video. The averaged line could shake a lot.

For the regression method, one stain on the road could make the single line shifting a lot. 

###3. Suggest possible improvements to your pipeline

Fundamentally, one improvement should be not using the greyscale it is color agnostic to some extent. A better way is to transfer single color images to grey scale and then use canning.

Then I think for video, currently we did not consider the relationships between frames. In realistic usages, lines in one frame should relate to the lines in last frame. This could help us filter out some noises.

For the regression method I am using, I think I should add another filtering method before doing the regression, to filter out outliners such as stains on the road.