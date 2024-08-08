#############################################################
## File: updateConfigThresholds.py
## Author: Matteo Vicenzi (mvicenzi@bnl.gov)
## Description: 
##   This script updates the PMT relative threshold in a DAQ 
##   configuration. It assumes the pedestal and target baseline
##   values are still valid. If that's not the case, update
##   everything using `loadConfigPedestalThreshold.py`.
## Usage:
##   python updateConfigThresholds.py <path-to-config> <new-threshold>

import pandas as pd
import sys, os
import sqlite3

#-------------------------------------------------------------------
# editing the file --> change thresholds to new value according
# to the target baseline (which is ASSUMED to be correct).

def updateFile( filename, set_threshold ):
 
	readfile = open( filename, 'r' )
	baselines = {}
	new_file_content = ""

	for line in readfile:

		line = line.strip()

		### This is the target baseline: it it not the pedestal that we set, but
                ### rather what the final baseline ends up being given the pedestal we set.
                ### Historically, we have seen instabilities in the pedestal->baseline
                ### conversion than happens in the boards. This code ASSUMES the target
                ### baselines to be still valid and uses them to se the thresholds!
		if "BaselineCh" in line:
        
			argline = line.split(".")[-1]
			buff = argline.split(":")		
			digitizer_channel = int(buff[0].split("Ch")[-1]) #these go from 1 to 16
			bl = int(buff[1])

			baselines[digitizer_channel] = bl  #save for threshold computation
            
		if "triggerThreshold" in line:
			                                
			argline = line.split(".")[-1]
			buff = argline.split(":")		
			digitizer_channel = int(buff[0].split("triggerThreshold")[-1]) #these go from 0 to 15
			old_threshold = int(buff[1])
			new_threshold = 0
            
			if (digitizer_channel != 15): #regular pmt channels   
                
				baseline = baselines[digitizer_channel+1] #baselines are stored 1 to 16
				new_threshold = baseline - set_threshold
                
			else: #last spare digitizer channel has no pmt
				new_threshold = 100  #make sure it never triggers --> huge threshold
                                
			line = line.replace( ("triggerThreshold%d: %d" % (digitizer_channel, old_threshold) ), ("triggerThreshold%d: %d" % (digitizer_channel, new_threshold) )  )
            	
		new_file_content += line+"\n"        
	readfile.close()

	writing_file = open(filename, "w")
	writing_file.write(new_file_content)
	writing_file.close()

	return

def main():

    workdirpath = sys.argv[1]
    new_thres = sys.argv[2]
    
    if workdirpath[-1] != "/":
        workdirpath += "/"
    workdir = "/".join(workdirpath.split("/")[:-2])
    configname = workdirpath.split("/")[-2]

    pmt_components = [ filename for filename in os.listdir(workdirpath)  if "icaruspmt" in filename]

    for pmt_component in pmt_components:	
        updateFile( workdirpath+pmt_component, int(new_thres) )

    # Change name to the folder
    print("Updating thresholds to '{}' ADC for config '{}'".format(new_thres, configname))
    os.system( "mv %s %s" % (workdirpath, workdir+"/"+configname + "_" + new_thres + "ADC_" ) )

if __name__ == "__main__":
	main()
