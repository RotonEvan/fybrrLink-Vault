/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * LEO Satellite Example
 * Runs traffic through a configurable LEO satellite constellation
 *
 * ENSC 427: Communication Networks
 * Spring 2020
 * Team 11
 */

#include "ns3/core-module.h"
#include "ns3/leo-satellite-config.h"
#include "ns3/applications-module.h"
#include "ns3/flow-monitor-helper.h"
#include <string>
#include <sstream>

using namespace ns3;
std::tuple<std::vector<std::vector<uint32_t>>, std::vector<bool>, std::vector<std::vector<double>>> readCSV(std::string filename);


NS_LOG_COMPONENT_DEFINE("LeoSatelliteExample");

int main (int argc, char *argv[]) {
    uint32_t n_planes = 10;
    uint32_t n_sats_per_plane = 10 ;
    double altitude = 2000;

    CommandLine cmd;
    cmd.AddValue ("n_planes", "Number of planes in satellite constellation", n_planes);
    cmd.AddValue ("n_sats_per_plane", "Number of satellites per plane in the satellite constellation", n_sats_per_plane);
    cmd.AddValue ("altitude", "Altitude of satellites in constellation in kilometers ... must be between 500 and 2000", altitude);

    cmd.Parse (argc,argv);

    LeoSatelliteConfig sat_network(n_planes, n_sats_per_plane, altitude);

    // Simulator::Stop(Seconds(100));
    // Simulator::Run();
    
    // Simulator::Destroy ();
    std::vector<std::vector<uint32_t>> XYCoordinates;
    std::vector<bool> qosEnabled;
    std::vector<std::vector<double>> flowParams;
    int N;
    int x1, x2, y1, y2;
    uint32_t source_access_id, destination_access_id;
    std::vector<uint32_t> path;
    std::vector<std::vector<int>> adjList;
    std::vector<uint32_t> edge_index;
    int path_BW;
    double path_latency, path_PLR;
    int count = 0;

    std::ofstream myFile1("analysis.csv");

    std::tie(XYCoordinates, qosEnabled, flowParams) = readCSV("TestCases.csv");
    
    N = (int)qosEnabled.size();
    std::cout<<">> Running test cases"<<std::endl;
    for(int flow_index = 0; flow_index < N;flow_index++){
        // myFile1 << sat_network.getAvgCongestionDegree();
        // myFile1 << "\n";

        std::cout<<"["<<flow_index+1<<"/"<<N<<"] : Initialising routing..."<<std::endl; 

        x1 = XYCoordinates.at(flow_index).at(0);
        y1 = XYCoordinates.at(flow_index).at(1);
        x2 = XYCoordinates.at(flow_index).at(2);
        y2 = XYCoordinates.at(flow_index).at(3);

        std::cout<<"["<<flow_index+1<<"/"<<N<<"] : XY Coordinates are :- x1 = "<<x1<<", y1 = "<<y1<<", x2 = "<<x2<<", y2 = "<<y2<<std::endl;

        source_access_id = sat_network.xyToaccessID.at(x1).at(y1);
        destination_access_id = sat_network.xyToaccessID.at(x2).at(y2);

        std::cout<<"["<<flow_index+1<<"/"<<N<<"] : Source Access ID is "<<source_access_id<<" and Destination Access Id is "<< destination_access_id<<std::endl;
        
        auto start = high_resolution_clock::now();

        std::cout<<"["<<flow_index+1<<"/"<<N<<"] : Running Dijkstra ... "<<std::endl;
        path = sat_network.planeDijkstraAlgorithm(source_access_id, destination_access_id);
        std::tie(edge_index, path_BW, path_latency, path_PLR) = sat_network.getPathParameters(path);

        std::cout<<"["<<flow_index+1<<"/"<<N<<"] : Printing Dijkstra Path Coordinates";
            for (std::size_t i = 0; i < path.size(); ++i){
                std::vector<int> coord = sat_network.xyPosition[path[i]];
                std::cout<<" - > ("<<coord.at(0)<<" , "<<coord.at(1)<<") ";
            }
        std::cout<<std::endl;

        if(sat_network.isSatisfying(path_BW, path_latency, path_PLR, flowParams.at(flow_index).at(0), flowParams.at(flow_index).at(1), flowParams.at(flow_index).at(2))){ 
            std::cout<<"["<<flow_index+1<<"/"<<N<<"] : Flow successfully routed"<<std::endl;
            count++;
        }
        else{
            std::cout<<"["<<flow_index+1<<"/"<<N<<"] : Given Flow cannot be satisfied. Please relax some contraints. "<<std::endl;
        }
        sat_network.updateGraph(edge_index, flowParams.at(flow_index).at(0));
        auto stop = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(stop - start);

        myFile1 << path_BW<<","<<path_latency<<","<<path_PLR<<","<<(int)path.size()-1<<","<<duration.count()<<"\n";

    }
    myFile1.close();
    std::cout<<">> Successfull routing count is "<<count<<std::endl;
    return 0; 
}

std::tuple<std::vector<std::vector<uint32_t>>, std::vector<bool>, std::vector<std::vector<double>>> readCSV(std::string filename){
    std::vector<std::vector<uint32_t>> XYCoordinates;
    std::vector<bool> qosEnabled;
    std::vector<std::vector<double>> flowParams;

    // Create an input filestream
    std::ifstream myFile(filename);

    // Make sure the file is open
    if(!myFile.is_open()) throw std::runtime_error("Could not open file");

    // Helper vars
    std::string line;
    double val = 0;

    // Read the column names

    if(myFile.good()) {
        // Extract the first line in the file
        std::getline(myFile, line);
    }

    int rowIndex = 0;
    while(std::getline(myFile, line)) {
        // Create a stringstream of the current line
        std::stringstream ss(line);
        // std::cout<<ss<<std::endl;
        int colIdx = 0;

        
        // Extract each integer
        std::vector <uint32_t> v1(4, 1);
        XYCoordinates.push_back(v1);
        std::vector <double> v2(3, 0.00);
        flowParams.push_back(v2);
        qosEnabled.push_back(true);
        while(ss >> val){
            if (colIdx > 7){
                break;
            }
            // Add the current integer to the 'colIdx' column's values vector
            if(colIdx < 4){
                XYCoordinates.at(rowIndex).at(colIdx) = (uint32_t)val;
            }
            else if(colIdx == 4 && val == 0){
                qosEnabled.at(rowIndex) = false;
            }
            else if (colIdx > 4 && colIdx < 8 ){
                flowParams.at(rowIndex).at(colIdx-5) = (double)val;
            }            
            // If the next token is a comma, ignore it and move on
            if(ss.peek() == ',') ss.ignore();
            
            // Increment the column index
            colIdx++;
        }
        rowIndex++;
    }

    // Close file
    myFile.close();

    return std::make_tuple(XYCoordinates, qosEnabled, flowParams);
}