import pandas as pd
import numpy as np
import sys, os
import sqlite3

#------------------------------------------------------------------
# help function to parse the board name from the file

def get_board_label(filename):
    
    head, tail = os.path.split(filename)
    buf = tail.removesuffix(".fcl").removeprefix("icaruspmt")
    geo = buf[0:2]
    pos = buf[2:-2]
    slot = buf[-2:]
  
    if slot == "01":
        slot = "A"
    elif slot == "02":
        slot = "B"
    elif slot == "03":
        slot = "C"

    board_label = geo.upper() + "-" + pos.upper() + "-" + slot
    
    return board_label

#-------------------------------------------------------------------
# Load the new baseline/threshold configuration in the PMT fcl files 

def editFile( filename, dfconfig ):
 
	readfile = open( filename, 'r' )
	board_label = get_board_label(filename)
 
	_SELB = (dfconfig.board==board_label)
	new_file_content = ""  

	for line in readfile:

		line = line.strip()

		if "BaselineCh" in line:
        
			argline = line.split(".")[-1]
			buff = argline.split(":")		
			digitizer_channel = int(buff[0].split("Ch")[-1]) #these go from 1 to 16
			old_bl = int(buff[1])

			_SELCH = (dfconfig.digitizer_channel==digitizer_channel-1)	
			new_bl = dfconfig[_SELB&_SELCH].target_baseline.values[0]
            
            		# replace old baseline with new baseline
			# print( new_bl)
			line = line.replace( ("BaselineCh%d: %d" % (digitizer_channel, old_bl) ), 
					     ("BaselineCh%d: %d" % (digitizer_channel, new_bl) ))
            
		if "triggerThreshold" in line:
			                                
			argline = line.split(".")[-1]
			buff = argline.split(":")		
			digitizer_channel = int(buff[0].split("triggerThreshold")[-1]) #these go from 0 to 15
			old_threshold = int(buff[1])
			
			_SELCH = (dfconfig.digitizer_channel==digitizer_channel)	
			new_threshold = dfconfig[_SELB&_SELCH].threshold.values[0]
			

			line = line.replace( ("triggerThreshold%d: %d" % (digitizer_channel, old_threshold) ), 
					     ("triggerThreshold%d: %d" % (digitizer_channel, new_threshold) )  )
            	
		if "channelPedestal" in line:
				
			argline = line.split(".")[-1]
			buff = argline.split(":")		
			digitizer_channel = int(buff[0].split("channelPedestal")[-1]) #these go from 0 to 15
			dc_offset_old = int(buff[1])
			
			_SELCH = (dfconfig.digitizer_channel==digitizer_channel)	
			new_dc_offset = dfconfig[_SELB&_SELCH].pedestal.values[0]

			line = line.replace( ("channelPedestal%d: %d" % (digitizer_channel, dc_offset_old) ), 
					     ("channelPedestal%d: %d" % (digitizer_channel, new_dc_offset) )  )
		
		new_file_content += line+"\n"
                                               
	readfile.close()

	writing_file = open(filename, "w")
	writing_file.write(new_file_content)
	writing_file.close()

### ------------------------------------------------

def main():

    configfile = sys.argv[1]
    foldername = sys.argv[2]
    newfoldername = sys.argv[3]
    
    confname = configfile.split("/")[-1].rstrip(".csv")

    dfconfig = pd.read_csv(configfile,sep=',')

    pmt_components = [ filename for filename in os.listdir(foldername)  if "icaruspmt" in filename]

    for pmt_component in pmt_components:	
        editFile( foldername+pmt_component, dfconfig )

    # Change name to the folder
    print("Creating a configuration '{}' with {}".format(newfoldername, confname))
    os.system( "mv %s %s" % (foldername, "workdir/"+newfoldername) )

    
if __name__ == "__main__":
	main()
