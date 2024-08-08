export path="/exp/icarus/data/users/mvicenzi/pmt-baselines/configdb"
export config=${1}
export newname=${2}

#Clean the directory
rm -rf workdir/*

#Copy the basedir content 
scp -r basedir/* workdir 

export folder=$( ls -d ./workdir/*/ )

echo "Applying ${path}/${config} to ${folder}"
echo "New config name: ${newname}"

# Now edit the files and change the folder name
python loadConfigPedestalThreshold.py ${path}/${config} ${folder} ${newname}


