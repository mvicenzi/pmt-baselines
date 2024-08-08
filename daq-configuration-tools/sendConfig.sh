## Send configuration to gateway path 
## (output of `pwd`) from workdir directory.

export path=${1}
export workdir="/exp/icarus/app/users/mvicenzi/pmt-baselines/daq-configuration-tools/workdir"
export gateway="icarus-gateway01.fnal.gov"

echo "Workdir : ${workdir}"
echo "Pointing: icarus@${gateway}:${path}/"

scp -r ${workdir}/* "icarus@${gateway}:${path}/"
