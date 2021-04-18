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

  LogComponentEnable("UdpEchoClientApplication", LOG_LEVEL_INFO);
  LogComponentEnable("UdpEchoServerApplication", LOG_LEVEL_INFO);

  LeoSatelliteConfig sat_network(n_planes, n_sats_per_plane, altitude);
  
  UdpEchoServerHelper echoServer (9);

  ApplicationContainer serverApps = echoServer.Install(sat_network.ground_stations.Get(1));
  serverApps.Start (Seconds (1.0));
  serverApps.Stop (Seconds (2000.0));

  UdpEchoClientHelper echoClient (sat_network.ground_station_interfaces[1].GetAddress(0), 9);
  echoClient.SetAttribute("MaxPackets", UintegerValue (20));
  echoClient.SetAttribute("Interval", TimeValue(Seconds(100.0)));
  echoClient.SetAttribute("PacketSize", UintegerValue (1024));

  ApplicationContainer clientApps = echoClient.Install (sat_network.ground_stations.Get(0));
  clientApps.Start (Seconds (2.0));
  clientApps.Stop (Seconds (2000.0));

  FlowMonitorHelper flowmonHelper;
  flowmonHelper.InstallAll();

  // for(uint32_t i=0; i<2; i++)
  // {
  //   Simulator::Stop(Seconds(100));
  //   Simulator::Run();
  //   sat_network.UpdateLinks();
  // }
  Simulator::Stop(Seconds(100));
  Simulator::Run();
 
  Simulator::Destroy ();



  int x1 = 6, y1 = 17, x2 = 12, y2 = 10;
  uint32_t source_access_id = sat_network.xyToaccessID.at(x1).at(y1);
  uint32_t destination_access_id = sat_network.xyToaccessID.at(x2).at(y2);
  std::vector<uint32_t> path;

  std::vector<std::vector<int>> adjList = sat_network.getSubGraph(x1, y1, x2, y2);

  std::vector<int> x_points, y_points;
  path = sat_network.bresenhemAlgo(x1, y1, x2, y2);
  std::cout<<">> Printing Bresenhem Path Coordinates";
  for (std::size_t i = 0; i < path.size(); ++i){
    std::vector<int> coord = sat_network.xyPosition[path[i]];
    std::cout<<" - > ("<<coord.at(0)<<" , "<<coord.at(1)<<") ";
  }
  std::cout<<std::endl;

  
  std::cout<<">> Source Access ID : "<<source_access_id<<", Destination Access Id : "<< destination_access_id<<std::endl;
  path = sat_network.DijkstraAlgorithm(source_access_id, destination_access_id, adjList, false, 0.85);
  std::cout<<">> Printing Dijkstra Path Coordinates";
  for (std::size_t i = 0; i < path.size(); ++i){
    std::vector<int> coord = sat_network.xyPosition[path[i]];
    std::cout<<" - > ("<<coord.at(0)<<" , "<<coord.at(1)<<") ";
  }
  std::cout<<std::endl;

  std::vector<uint32_t> edge_index;
  int bottleneckBW;
  double latency, PLR;

  std::tie(edge_index, bottleneckBW, latency, PLR) = sat_network.getPathParameters(path);
  std::cout<<">> Path Parameters are as follows :- BottleNeck BW : "<<bottleneckBW<<", End to end delay : "<<latency<<", Packet Loss rate : "<<PLR<<std::endl;
  sat_network.updateGraph(edge_index, 20);
  // sat_network.printGraph();

  // std::stringstream ss;
  // std::string file_title;
  // ss << "leo-satellite-example-"<<n_planes<<"-"<<n_sats_per_plane<<"-"<<altitude<<".flowmon";
  // ss >> file_title;

  // flowmonHelper.SerializeToXmlFile (file_title, false, false);
  return 0; 
}


