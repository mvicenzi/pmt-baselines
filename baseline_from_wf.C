#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <fstream>

#include "TROOT.h"
#include "TLatex.h"
#include "TStyle.h"
#include "TGraph.h"
#include "TH1D.h"
#include "TCanvas.h"
#include "TTree.h"
#include "TFile.h"


//Funzione che calcola la media aritmetica
std::vector<double> mean_std(const vector<double> *data) { 
  double mean = 0., std = 0.;
  for(int i=0; i<data->size(); i++) {
    //cout << "data: " << (*data)[i] << endl; 
    mean += data->at(i);
    std += pow(data->at(i),2);
  }
  mean /= data->size(); // divide by number of data points  
  std = (std/data->size())-pow(mean,2);
  
  std::vector<double> mean_std = {mean, std};

  return mean_std;
}


// get baseline and standard deviation over <ns> samples
double compute_baseline(std::vector<double> waveform, int ns = 450)
{
    double bs, std_dev, min, max;

    int trial = 0;

    // try until you can a reliable value
    while (!(trial * ns > waveform.size()))
    {

        // compute mean on a <ns> window
        bs = 0.;
        for (int k = trial * ns; k < trial * ns + ns; k++){
            bs += waveform[k];
        }
        bs /= (ns);


        // standard deviation
        std_dev = 0;
        for (int k = trial * ns; k < trial * ns + ns; k++){
            std_dev += pow(waveform[k] - bs, 2);
        }
        std_dev /= (ns - 1);
        std_dev = std::sqrt(std_dev);

        
        // check the baseline
        min = *std::min_element(waveform.begin() + trial * ns, waveform.begin() + trial * ns + ns);
        max = *std::max_element(waveform.begin() + trial * ns, waveform.begin() + trial * ns + ns);
        if ((min > bs - 5 * std_dev) && (max < bs + 5 * std_dev))
            // got it
            break;

        // update
        ++trial;
    }

    if (trial * ns > waveform.size())
    {
        bs = -999;
        std::cout << "Cannot compute baseline." << std::endl;
    }

    // return result as a pair
  

    return bs;
}

// main analysis loop 
int print_baseline(std::string name, bool debug=false)
{

    // branch variables
    std::vector<double> *wf = new vector<double>;
    int ch, event;
    double tstart;

    // variables
    std::vector<double> w;
  
    // access tree
    TFile *ifile = new TFile(name.c_str());
    ifile->cd();
    TTree *itree = (TTree *)ifile->Get("wftree");
    itree->SetBranchAddress("wf", &wf);
    itree->SetBranchAddress("ch", &ch);
    itree->SetBranchAddress("tstart", &tstart);
    itree->SetBranchAddress("event", &event);
    int n = itree->GetEntries();

   
    //output files
    ofstream fout;

    int count=0;    
    // go over the tree
    for (int i = 0; i < n; i++)
    {
        // get the entry
        itree->GetEntry(i);

     
        // check the time to find the waveform on spill
        if (!((tstart < 1500) && (tstart + 50 > 1500))) continue;

        // get the waveform
        w = *wf;

        TString filename;
        filename.Form("results/baseline_ch%d.txt",ch);
        fout.open(filename, ios::out | ios::app);
        fout << compute_baseline(w)  << endl;
        fout.close();
       

        if(count==500){
           cout << "Analyzed: " << i << "/" << n << " entries" << endl;
           count=0;
        }
        count++;
    }
    ifile->Close();


    return 0;
}


int mean_baseline(){

  ifstream fin;
  ofstream fout;
  fout.open("baseline.csv", ios::out);

  std::vector<double> baseline, baseline2, mean_std_;  


  for(int i=0;i<360;i++){
    baseline.clear();
    fin.open(Form("results/baseline_ch%d.txt",i));
    for (; !fin.eof() ;){
       double temp;
       fin >> temp;

       if(temp != -999) baseline.push_back(temp);
    } 
    fin.close();

    mean_std_ = mean_std(&baseline);

    baseline2.clear();
    for (int j = 0; j< baseline.size(); j++){
      double mean = mean_std_[0], std = mean_std_[1], bs = baseline.at(j);
      if(bs > (mean-100) && bs < (mean+100)) 
        baseline2.push_back(baseline.at(j));

    }

    mean_std_ = mean_std(&baseline2);

    fout << i << "," << mean_std_[0] << "," << mean_std_[1] << endl;

  }

  fout.close();

  return 0;
}


