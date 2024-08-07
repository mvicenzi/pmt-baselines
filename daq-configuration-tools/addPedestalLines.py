#############################################################
## File: addPedestalLines.py
## Author: Matteo Vicenzi (mvicenzi@bnl.gov)
## Description: 
##   This script is used to append channelPedestal lines
##   at the end of the PMT configuration files.
##   It is meant to be run only if the lines are completely
##   missing. If not, use the standard update scripts!  
## Usage:
##   python addPedestalLines.py <directory> <pedestal_value?> 

import sys, os

def addLines( filename, value ):
    
	ofile = open( filename, 'a' )
	ofile.write("\n") #first new line
	for digitizer_channel in range(0,16):
		
		line = "daq.fragment_receiver.channelPedestal"
		line += str(digitizer_channel)+": "
		line += value
		line += "\n\n"
		ofile.write(line)

	ofile.close()

def main():

	foldername = sys.argv[1] 
	value = sys.argv[2]
	pmt_components = [ filename for filename in os.listdir(foldername)  if "icaruspmt" in filename]
	for pmt_component in pmt_components:	
		addLines( foldername+pmt_component, value )
	print("Added pedestal lines with " + value + " to pmt files in " + foldername)

if __name__ == "__main__":
	main()

