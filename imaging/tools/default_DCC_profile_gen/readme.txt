Usage:
1. Go to scripts folder
2. 'generate_IMX390_cfg.sh' OR python ctt_def_xml_gen.py <sensor properties file>

For e.g. 
python ctt_def_xml_gen.py ../configs/imx390_properties.txt

For other sensors, create a new script using (generate_IMX390_cfg.sh) for reference.


Default DCC XML files will be created in the folder PRJ_DIR as defined in the sensor properties file. 
Existing files in the folder with same file names will be overwritten. 


Dependencies
1. Ubuntu 18.04
2. python3 and numpy

Note
The parameters like width,height, DCC ID, sensor name etc must match the values defined in sensor driver. 

 