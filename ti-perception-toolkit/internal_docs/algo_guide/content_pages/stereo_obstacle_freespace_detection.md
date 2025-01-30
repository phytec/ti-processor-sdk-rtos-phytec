Stereo Obstacle / Freespace Detection
=======
<b>Algorithm Summary:</b> Stereo based obstacle / freespace detection is applied on a disparity map using an estimated ground plane model. In this function, we assume that obstacles stand on ground and make use of the fact that
    - Obstacle has disparities larger than or equal to the disparity of ground plane where it stands on
    - Freespace has disparities smaller than or equal to the disarpity of an object which stands on that freespace.
Therefore the accurate estimation of ground plane along with high-quality disparity map is very critice to these algorithms. To reduce computational complexity, we downsample the disparity map before applying algoithms. 

<b>Inputs/outputs of obstacle detection function:</b>
    - [Input] Algorithm context 
         - This structure holds the context for the function. 
    - [Input] Right image 
         - Right image is the base image in SDE. It is just for debugging purposes and not used for obstacle detection.
    - [Input] Ground plane model 
         - This is the estimated ground plane model. We can use any type of ground plane model. But we only support V-disparity map based ground plane model for now.
    - [Input] Disparity map
         - This disparity map only has disparity values for pixels where the confidence value is larger than a threshold.
    - [Output] Number of detected obstacles
         - This is the number of detected obstacles from the algorithm. 
    - [Output] Obstacle bounding boxes
         - Structure that holds the bounding boxes coordinates for all detected obstacles. 

<b>Inputs/outputs of freespace detection function:</b>
    - [Input] Algorithm context 
         - This structure holds the context for the function. 
    - [Input] Right image 
         - Rig Right image is the base image in SDE. It is just for debugging purposes and not used for obstacle detetion.
    - [Input] Ground plane model 
         - This is the estimated ground plane model. We only support V-disparity map based ground plane model for now.
    - [Input] Disparity map
         - This disparity map only has disparity values for pixels where the confidence value is larger than a threshold.
    - [Output] Freespace boundary
         - This is an array that contains the pixel positions of where freespace ends for every column of an image.
    - [Output] Drivable space
         - This indicates the maximum clear distance in front of the ego-vehicle


### Obstacle / Freespace detection 

Summary of the algorithm:
    - Compute the probable obstacle pixels, which have larger disparity than the ground plane, in a (0.5 m)x(2.5 m) window. Note that the window size in pixels depends on the depth of the pixel of interest.
    - Find the closest obstacle boundary in every column. Obstacle should have n ( > threshold) consecutive probable pixels in a column.
    - For freespace detection, a freespace boundary is formed from the obstacle boundaries in all columns.
    - For obstacle detection, ground the neighboring obstacle pixels, then merge and split obstacles depending on their depths and perspectives, etc. 

More details about the algorithms can be found <a href="../content_pages/supporting_docs/SDE_obstacle_freespace_detection.pdf" target="_blank"><b>here</b></a>.
