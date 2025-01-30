Stereo Ground Estimation
=======
<b>Algorithm Summary:</b> Ground plane estimation is essentail in automotive applications such as obstacle and freespace detection. This function uses the stereo disparity map to estimate the ground plane, which can then be used to detect free space and obstacles on the ground. In the current implementation, we employ only the V-disparity map,computed using the disparity map from DMPAC SDE (Streo Depth Engine), to find the grond plane because it is simple yet effective. 

<b>Inputs/outputs of the function:</b>
    - [Input] Algorithm context 
         - This structure holds the context for the function. 
    - [Input] Right image 
         - Right image is the base image in SDE. It is just for debugging purposes and not used for ground estimation. 
    - [Input] Disparity map
         - Disparity map includes floating-point disparity values for all pixels in the image. 
    - [Input] Confidence map
         - Confidence map includes confidence values of all disparities.
    - [Output] Output disparity map
         - Output disparity map only has disparity values for pixels where the confidence value is larger than a threshold. It is an input to both the obstacle detection and freespace detection functions.
    - [Output] Ground plane model
         - This is the output ground plane model, which is an input to obstacle deteetion and freespace detection.


### Ground plane estimation based on V-disparity map

A V-disparity map is created by compiling histograms for every row of the disparity map. So its size is (disparity range)x(disparity map height). In the V-disparity map, objects appear as vertical line segments, while the ground plane appears as a slanted line segment. Hence, the ground plane can be estimated by simply modeling this slanted line segment. More details can be found <a href="../content_pages/supporting_docs/SDE_ground_plane_estimation.pdf" target="_blank"><b>here</b></a>.


