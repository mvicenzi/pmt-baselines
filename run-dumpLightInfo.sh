export run=$1
export list="runs/${run}/files-run${run}.txt"
export limit=2
export fcl="dumpLightInfo.fcl"


export njobs=$( wc -l < $list )
echo "Project has ${njobs} files"


# This part does the fit
if test -f "runs/${run}/dumpLightInfo_run${run}.root"; then 
	
	echo "file runs/${run}/dumpLightInfo_run${run}.root already exists!"
	echo "File will be replaced!"
fi 

lar -c ${fcl} -S ${list} -T runs/${run}/dumpLightInfo_run${run}.root


echo "ALL DONE!"
