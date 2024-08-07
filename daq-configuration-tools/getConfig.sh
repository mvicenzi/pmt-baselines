## Import configuration from gateway path 
## (output of `pwd`) into basedir directory.

export path=${1}
export basedir="/exp/icarus/app/users/mvicenzi/pmt-baselines/daq-configuration-tools/basedir"
export gateway="icarus-gateway01.fnal.gov"

echo "Basedir : ${basedir}"
rm -rf ${basedir}/*

echo "Pointing: icarus@${gateway}:${path}/*"
scp -r "icarus@${gateway}:${path}/*" ${basedir}
