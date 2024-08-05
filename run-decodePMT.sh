export run=$1
export list="${run}/files-run${run}.txt"
export limit=200
export fcl="decodePMT_icarus.fcl"

export njobs=$( wc -l < $list )
echo "Project has ${njobs} files"


# This part does the fit
if test -f "${run}/dumpLightInfo_run${run}.root"; then 
	
	echo "file ${run}/dumpLightInfo_run${run}.root already exists!"
	echo "File will be replaced!"
fi 

lar -c ${fcl} -S ${list} -T ${run}/decodePMT_run${run}.root


echo "ALL DONE!"
