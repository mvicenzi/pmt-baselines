export run=$1
export list="/exp/icarus/data/users/mvicenzi/pmt-baselines/lists/files-run${run}.txt"
export out="/exp/icarus/data/users/mvicenzi/pmt-baselines/runs/${run}"
export fcl="baseline_dump.fcl"

export njobs=$( wc -l < $list )
echo "Project has ${njobs} files"

mkdir -p ${out}

if test -f "${out}/dumpwf_run${run}.root"; then 
	
	echo "file ${out}/dumpwf_run${run}.root already exists!"
	echo "File will be replaced!"
        rm ${out}/dumpwf_run${run}.root
fi 

lar -c ${fcl} -S ${list} -T ${out}/dumpwf_run${run}.root 

echo "ALL DONE!"
