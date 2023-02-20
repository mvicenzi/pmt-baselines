#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <fstream>
#include <map>


UShort_t getMedian(std::vector<UShort_t> v){

	std::sort(v.begin(),v.end());
	if( v.size() % 2 == 0 ) //even
		return 0.5*(v[v.size()/2-1]+v[v.size()/2]); //average the two
	else
		return v[v.size()/2];
}


std::map<int, std::map<int,UShort_t>> ReduceToMeanPerChannel(std::map<int,std::map<int,std::vector<UShort_t>>> mp){

	std::map<int, std::map<int,UShort_t>> new_mp;

	for(auto it = mp.begin(); it != mp.end(); it++){
		std::map<int, UShort_t> channel_mp;
		for( auto jt = it->second.begin(); jt != it->second.end(); jt++) channel_mp.insert(std::make_pair(jt->first, getMedian(jt->second)));
		new_mp.insert(std::make_pair(it->first,channel_mp));			
	}

	return new_mp;
}


void printToCSV(std::string run_number, std::map<int,int> timestamps, std::map<int, std::map<int,UShort_t>> reduced_baselines){

	std::string filename = run_number + "/measChannelBaselines_run" + run_number + ".csv";
  	
	std::ofstream fout;
  	fout.open(filename, ios::out);

	fout << "event,timestamp";
	for(int i=0;i<360;i++) fout << ",baseline_ch" << i;
	fout << std::endl;

	for(auto it = reduced_baselines.begin(); it != reduced_baselines.end(); it++){

		fout << it->first << "," << timestamps[it->first];
		for( auto jt = it->second.begin(); jt != it->second.end(); jt++) fout << "," << jt->second;
		fout << std::endl;
	}	

  	fout.close();
	std::cout << "Output file: " << filename << std::endl;
}

void check_baselines(int run_number)
{

	std::string run = std::to_string(run_number);
	std::string filename = run + "/dumpLightInfo_run" + run + ".root";  

	TFile *ifile = new TFile(filename.c_str());
	TDirectory *dir = (TDirectory *)ifile->Get("simpleLightAna");
	TTree *ttree = (TTree *)dir->Get("daqPMTwfttree");

	int n = ttree->GetEntries();
	std::cout << filename << " " << n << std::endl;

	//variables
	int m_run;
	int m_event;
	int m_timestamp;
	int m_channel_id;
	UShort_t m_baseline;
	UShort_t m_chargesum;
	int m_nticks;

	ttree->SetBranchAddress("run", &m_run);
	ttree->SetBranchAddress("event", &m_event);
	ttree->SetBranchAddress("timestamp", &m_timestamp);
	ttree->SetBranchAddress("channel_id", &m_channel_id);
	ttree->SetBranchAddress("baseline", &m_baseline);
	ttree->SetBranchAddress("chargesum", &m_chargesum);
	ttree->SetBranchAddress("nticks", &m_nticks);

	std::map<int, std::map<int,std::vector<UShort_t>>> baselines;
	std::map<int,int> timestamps; //timestamps per event

	/// READING FROM FILE
	for(int i=0; i<n; i++){

		ttree->GetEntry(i);

		//std::cout << m_event << " -> ch" << m_channel_id << " " << m_baseline << std::endl; 
	
		if( baselines.find(m_event) == baselines.end() ){ //first time seeing this event
		
			//std::cout << "event: first time" << std::endl;	
			std::vector<UShort_t> bs;
			bs.push_back(m_baseline);

			std::map<int, std::vector<UShort_t>> mp;
			mp.insert(std::make_pair(m_channel_id, bs));

			baselines.insert(std::make_pair(m_event,mp));
			timestamps.insert(std::make_pair(m_event,m_timestamp));			
		}
		else { //this even is already in the map

			std::map mp = baselines[m_event];
			if( mp.find(m_channel_id) == mp.end() ){ //channel not seen yet
				
				//std::cout << "channel: first time" << std::endl;	
				std::vector<UShort_t> bs;
				bs.push_back(m_baseline);

				baselines[m_event].insert(std::make_pair(m_channel_id, bs));
			}
			else{ //channel already exists

				baselines[m_event][m_channel_id].push_back(m_baseline);

			}
		
		}
	}
	/// END FILE READING

	std::map<int, std::map<int,UShort_t>> baseline_per_ch_per_ev = ReduceToMeanPerChannel(baselines);
	printToCSV(run,timestamps,baseline_per_ch_per_ev);

}

