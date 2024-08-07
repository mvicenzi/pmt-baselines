#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <fstream>
#include <map>


double getMedian(std::vector<double> v){

	std::sort(v.begin(),v.end());
	if( v.size() % 2 == 0 ) //even
		return 0.5*(v[v.size()/2-1]+v[v.size()/2]); //average the two
	else
		return v[v.size()/2];
}


std::map<int, std::map<int,double>> ReduceToMeanPerChannel(std::map<int,std::map<int,std::vector<double>>> mp){

	std::map<int, std::map<int,double>> new_mp;

	for(auto it = mp.begin(); it != mp.end(); it++){
		std::map<int, double> channel_mp;
		for( auto jt = it->second.begin(); jt != it->second.end(); jt++) channel_mp.insert(std::make_pair(jt->first, getMedian(jt->second)));
		new_mp.insert(std::make_pair(it->first,channel_mp));			
	}

	return new_mp;
}


void printToCSV(std::string run_number, std::map<int,int> timestamps, std::map<int, std::map<int,double>> reduced_baselines){

	std::string filename = "/exp/icarus/data/users/mvicenzi/pmt-baselines/baselinesdb";
        int firstT = std::min_element(timestamps.begin(),timestamps.end())->second;     
        filename += "/" + std::to_string(firstT) + "_measChannelBaselines_run" + run_number + ".csv";
  	
	std::ofstream fout;
  	fout.open(filename, ios::out);

	fout << "event,timestamp,ch,baseline";
        fout << std::endl;
	
        for(auto it = reduced_baselines.begin(); it != reduced_baselines.end(); it++){

		for( auto jt = it->second.begin(); jt != it->second.end(); jt++){
                    fout << it->first << "," << timestamps[it->first];
 		    fout << "," << jt->first << "," << std::setprecision(10) << jt->second;
		    fout << std::endl;
                }
	}	

  	fout.close();
	std::cout << "Output file: " << filename << std::endl;
}

void check_baselines(int run_number)
{

	std::string run = std::to_string(run_number);
	std::string filename = "/exp/icarus/data/users/mvicenzi/pmt-baselines/runs/" + run + "/dumpwf_run" + run + ".root";  

        std::cout << "Opening " << filename << std::endl;

	TFile *ifile = new TFile(filename.c_str());
	TTree *ttree = (TTree *)ifile->Get("waveforms/wftree");

	int n = ttree->GetEntries();
	std::cout << filename << " " << n << std::endl;

	//variables
	int m_run;
	int m_event;
        int m_timestamp;
	int m_channel_id;
	double m_baseline;
	int m_nticks;

	ttree->SetBranchAddress("run", &m_run);
	ttree->SetBranchAddress("event", &m_event);
	ttree->SetBranchAddress("timestamp", &m_timestamp);
	ttree->SetBranchAddress("channel_id", &m_channel_id);
	ttree->SetBranchAddress("baseline", &m_baseline);
	ttree->SetBranchAddress("nsize", &m_nticks);

	std::map<int, std::map<int,std::vector<double>>> baselines;
	std::map<int,int> timestamps; //timestamps per event

	/// READING FROM FILE
	for(int i=0; i<n; i++){

		ttree->GetEntry(i);

		//std::cout << m_event << " -> ch" << m_channel_id << " " << m_baseline << std::endl; 
	
		if( baselines.find(m_event) == baselines.end() ){ //first time seeing this event
		
			//std::cout << "event: first time" << std::endl;	
			std::vector<double> bs;
			bs.push_back(m_baseline);

			std::map<int, std::vector<double>> mp;
			mp.insert(std::make_pair(m_channel_id, bs));

			baselines.insert(std::make_pair(m_event,mp));
			timestamps.insert(std::make_pair(m_event,m_timestamp));			
		}
		else { //this event is already in the map

			std::map mp = baselines[m_event];
			if( mp.find(m_channel_id) == mp.end() ){ //channel not seen yet
				
				//std::cout << "channel: first time" << std::endl;	
				std::vector<double> bs;
				bs.push_back(m_baseline);

				baselines[m_event].insert(std::make_pair(m_channel_id, bs));
			}
			else{ //channel already exists

				baselines[m_event][m_channel_id].push_back(m_baseline);

			}
		
		}
	}
	/// END FILE READING

	std::map<int, std::map<int,double>> baseline_per_ch_per_ev = ReduceToMeanPerChannel(baselines);
	printToCSV(run,timestamps,baseline_per_ch_per_ev);

        gApplication->Terminate(0);
}

