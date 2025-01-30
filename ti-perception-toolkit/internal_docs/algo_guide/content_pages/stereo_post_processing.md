Stereo Post-Processing
=======
<b>Algorithm Summary:</b> Stereo post-processings are applied to the disparity map from SDE HWA to improve its robustness. Three different post-processing methods are supported in PTK, disparity merge, 1-D hole filling filter and 2-D median filter. 
    - Disparity merge algorithm combines a low-resolution disparity map with a high-resolution disparity map. The low-resolution disparity map is up-sampled before being combined with the hig-resolution disparity map. For every pixel, if the differene between two disparities is less than a threshold, a high-resolution diaprity is selected, otherwise, a disparity is set to 0, i.g. invalid disparity.
    - Hole filling algorithm fills holes, which are corresponding to invalid disparities. The holes are interpolated from the closest valid left and right disparities. 
    - Median filter algorithm applies 2-D median filter on every disaprity.


<b>Inputs/outputs of disparity merge function:</b>
    - [Input] Algorithm context 
         - This structure holds the context for the function. 
    - [Input] Low resolution disparity
         - Input low-resolution disparity map to be upsampled. 
    - [Input] High resolution disparity
         - Input high-resolution disparity map to be combined with the up-sampled low-resolution disparity map. Its width and height are twice the width adn the height of the low-resolution disparity map.
    - [Output] Output disparity
         - Output combined disparity map. Its resolution is the same as the high-resolution disparity map.
    - [Input] Stride of low-resolution disparity map
         - Stride of the input low-resolution disparity map
    - [Input] Stride of high-resolution disparity map
         - Stride of the input high-resolution disparity map and the output disparity map

<b>Inputs/outputs of 1-D hole filling function:</b>
    - [Input] Algorithm context 
         - This structure holds the context for the function. 
    - [Input/Output] Disparity 
         - Input disparity map to be filetered and updated.
    - [Input] Stride 
         - Stride of the disparity map

<b>Inputs/outputs of median filter function:</b>
    - [Input] Algorithm context 
         - This structure holds the context for the function. 
    - [Input] Input disparity Disparity 
         - Input disparity map to be filetered.
    - [Output] Output disparity Disparity 
         - Output disparity map to be updated after filtering.
    - [Input] Stride 
         - Stride of the disparity maps


