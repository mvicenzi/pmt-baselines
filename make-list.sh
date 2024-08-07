export run=$1
export limit=2
export listdir="/exp/icarus/data/users/mvicenzi/pmt-baselines/lists/"
export list="${listdir}/files-run${run}.txt"

mkdir -p ${listdir}

echo "creating file list $list"

if test -f "$list"; then
    echo "$list exists. Removing old list"
    rm $list
fi

echo "Creating new file list"
touch $list

DEFNAME="${USER}_PMTbaselines_run${run}_${limit}"
COND="run_number=${run} AND data_tier raw AND icarus_project.stage daq with limit ${limit}"

echo "Creating samweb definition ${DEFNAME}" 
#samweb delete-definition ${DEFNAME}
#samweb create-definition ${DEFNAME} ${COND}

prestage=0
for file in $( samweb list-files ${COND} )
do
	echo $file
        samwebLocFull=$(samweb locate-file $file | grep "enstore" | head -n 1)
        fileLocPart=${samwebLocFull#enstore:}
        fileLoc=${fileLocPart%(*}
 
	if [ -n "$fileLoc" ]; then
        	status=$(cat "${fileLoc}/.(get)($file)(locality)")
        	checkOnline=$(echo $status | grep "ONLINE")

		if [ -z "$checkOnline" ]; then
			echo "... not on disk -> prestage needed!"
                	((prestage++))
        	fi
    	fi
	echo $( samweb get-file-access-url --schema=root --location=enstore $file ) >> $list
  
done 

export njobs=$( wc -l < $list )
echo "Project has ${njobs} files"
echo "${prestage} files need prestaging!"

thr=$(echo "$njobs" | awk '{printf "%d", 0.01*$1}')

#if ((prestage > thr)); then
#	echo "Prestaging files..." 
#	echo "This can take a long time, but you can close this terminal & check status on webpage!"
#	samweb prestage-dataset --defname=${DEFNAME} --touch &
#fi

echo "ALL DONE!"
