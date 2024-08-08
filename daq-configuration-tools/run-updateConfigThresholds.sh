export threshold=$1

#Clean the directory

rm -rf workdir/*

#Copy the basedir content 

scp -r basedir/* workdir 

export config=$( ls -d ./workdir/*/ )

echo "Applying ${threshold} to ${config}"

# Now edit the files and change the folder name
python updateConfigThresholds.py $config $threshold


