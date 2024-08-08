#############################################################
## File: extractConfigPedestalThreshold.py
## Author: Matteo Vicenzi (mvicenzi@bnl.gov)
## Description: 
##   This script is used to extract the pedestal, target baseline
##   and theshold values from a DAQ configutation and store it
##   in a .csv file 
## Usage:
##   python extractConfigPedestalThreshold.py <config-directory> 

import pandas as pd
import numpy as np
import sys, os
import sqlite3

#----------------------------------------------------------------
# helper functions to map board/digitizer channel to channel id

def readSqlitedb(database="/cvmfs/icarus.opensciencegrid.org/products/icarus/icarus_data/v09_79_01/icarus_data/database/ChannelMapICARUS_20230829.db", table="pmt_placements_29aug2023"):

    # Read sqlite query results into a pandas DataFrame
    con = sqlite3.connect(database)
    df = pd.read_sql_query("SELECT * from {}".format(table), con)

    con.close()

    return df

def get_channel_id(db, board, digitizer_channel):
    
    ch = db.loc[(db['digitizer_label']==board) & (db['digitizer_ch_number']==digitizer_channel), ['channel_id']]
    # print(board, ",", digitizer_channel, "-->", ch.values[0][0])

    return ch.values[0][0]

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
# Extract info to look for pedestals and thresholds

def extractInfo( filename ):
 
    readfile = open( filename, 'r' )
    board = get_board_label(filename)
    mapping = readSqlitedb()

    peds = {}
    thres = {}
    bs = {}

    for line in readfile:

        line = line.strip()
		
        if "BaselineCh" in line:
        
            argline = line.split(".")[-1]
            buff = argline.split(":")		
            digitizer_channel = int(buff[0].split("Ch")[-1]) #these go from 1 to 16
            bs[digitizer_channel-1] = int(buff[1])

        if "triggerThreshold" in line:
			                                
            argline = line.split(".")[-1]
            buff = argline.split(":")		
            digitizer_channel = int(buff[0].split("triggerThreshold")[-1]) #these go from 0 to 15
            threshold = int(buff[1])
            thres[digitizer_channel] = threshold

        if "channelPedestal" in line:
				
            argline = line.split(".")[-1]
            buff = argline.split(":")		
            digitizer_channel = int(buff[0].split("channelPedestal")[-1]) #these go from 0 to 15
            dc_offset = int(buff[1])
            peds[digitizer_channel] = dc_offset
	
    data = []

    if (len(peds) != len(thres)) or (len(peds) != len(bs)):
        print("ERROR: pedestals and thresholds don't match: is " + filename + "badly formatted?")
        return data

    for key, item in peds.items():
        data.append( [board, key, item, bs[key], thres[key]] )
    
    readfile.close()
    return data

### ------------------------------------------------

def main():

    foldername = sys.argv[1]
    configname = foldername.split("/")[-2].rstrip("_")
    pmt_components = [ filename for filename in os.listdir(foldername)  if "icaruspmt" in filename]

    df = pd.DataFrame(columns=['board','digitizer_channel', 'pedestal', 'target_baseline', 'threshold'])

    for pmt_component in pmt_components:	
        data = extractInfo( foldername+pmt_component )
        for row in data:
            df = df.append(pd.Series(row, index=df.columns), ignore_index=True)

    print("Extracted pedestals from pmt files in " + foldername + ".")
    
    outpath = "/exp/icarus/data/users/mvicenzi/pmt-baselines/configdb/"
    outname = configname + "_data.csv"
    df.to_csv(outpath+outname,sep=',',index=False)

    print("Output file: " + outpath+outname)

if __name__ == "__main__":
	main()
