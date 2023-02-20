# Instructions
This folder contains the script `makeConfigurationWithBaselineThreshold.sh` that takes an existing configuration placed in the `./basedir` directory and creates another one in the `./workdir` directory, setting new channel pedestals (DC offsets),  baselines and thresholds for the PMT digitizers.

The script updates the `BaselineChXX`, `channelPedestalXX` and `triggerThresholdXX` parameters in the PMT configuration files. Note that `BaselineXX` values are only used to compute the `triggerThresholdXX` values, but they are not written directly in the board registers. The baselines are configured into the boards with the `channelPedestal` values that control the DC offsets. The relationship between `channelPedestalXX` and `BaselineChXX` is determined via a channel-by-channel calibration.

The spare channels which are used for trigger pulses are kept at fixed values (very high thresholds to avoid wrong majorities from them).

## Setting up
The code requires some specific python libraries, so the advice is to set up a python environment in which to install everything. To do so:

1. First, get a recent version of `icaruscode`: `setup icaruscode v09_66_02 -q e20:prof`
2. Create the environment at a destination of your preference:  `python3 -m venv /path/to/env/`
3. Do `source path/to/env/bin/activate`
4. Finally, install all requirements:  `python -m pip install -r requirement.txt`

The required libraries (and much more) are in the file [requirement.txt](../requirement.txt). 

After the first time, do step 1 and 3 to activate the environment each time.

## Inputs
The new baselines need to be provided in a `.csv` file.
The code expects at least three columns named "`channel_id`", "`baseline`" and "`DC_offset`".
The channel ID is the "LArSoft" channel number, not the PMT ID number.

The value of the baselines in the file are assumed to be the measured zero signal ADC value  when setting the corresponding DC offset of the DAC.
This code does not check for consistency: it is up to the user to check that the values in the input files are correct!
These are to be determined via calibration from data, channel-by-channel.

File structure example:
```
channel_id,baseline,DC_offset,
0,14500.2,7530,
1,18349.3,6543,
2,16574,4325,
...,
358,16273.2,3526,
359,19342.0,1746,
```

## How to run

The script can be run with
```
source makeConfigurationWithBaselineThreshold.sh new_threshold /path/to/baselines.csv
```

The script will wipe clean `./workdir`, copy there the old configuration and start modifying it.
At the end, it will attempt to rename it by changing the threshold value in the directory name.
Note that this step may fail if the naming convention is not what is expected.
