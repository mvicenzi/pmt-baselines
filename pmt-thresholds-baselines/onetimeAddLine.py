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

