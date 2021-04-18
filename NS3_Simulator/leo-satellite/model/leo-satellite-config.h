/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * LEO Satellite Constellation Config
 * Creates and maintains all satellites and links within a satellite communication network
 *
 * ENSC 427: Communication Networks
 * Spring 2020
 * Team 11
 */
#ifndef LEO_SATELLITE_CONFIG_H
#define LEO_SATELLITE_CONFIG_H

#include "ns3/vector.h"
#include "ns3/object.h"
#include "ns3/ptr.h"
#include "ns3/core-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/leo-satellite-mobility.h"
#include "ns3/ground-station-mobility.h"
#include <vector>
#include "ns3/mobility-module.h"
#include "ns3/csma-module.h"
#include <cmath>
#include "ns3/internet-module.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "ns3/applications-module.h"
#include <climits>
#include <stack>
#include <cstdlib>
#include <chrono>

using namespace std::chrono;


namespace ns3 {

class LeoSatelliteConfig : public Object
{
public:
  /**
   * Register this type with the TypeId system.
   * \return the object TypeId
   */
  static TypeId GetTypeId (void);

  LeoSatelliteConfig (uint32_t num_planes, uint32_t num_satellites_per_plane, double altitude);

  virtual ~LeoSatelliteConfig ();
  virtual TypeId GetInstanceTypeId (void) const;
  
  void UpdateLinks (); //update the intersatellite links
  void InitializeRoutes1();
  void addEdge(uint32_t node1_id, uint32_t node2_id, double params[], uint32_t type);
  double calculateCost(double params[], double maxCD);
  void printGraph();

  std::vector<uint32_t> bresenhemAlgo(int x1, int y1, int x2, int y2);
  std::tuple<std::vector<int>, std::vector<int>> bresenhemAlgoUtil(int x1, int y1, int x2, int y2);
  std::vector<std::vector<int>> getSubGraph(int x1, int y1, int x2, int y2);
  std::vector<uint32_t> DijkstraAlgorithm(uint32_t source_access_id, uint32_t destination_access_id, std::vector<std::vector<int>> AdjList, bool recomputeCost, double maxCD);
  uint32_t minDistance(double dist[], bool visited[], std::vector<uint32_t> Nodes);
  std::tuple<std::vector<uint32_t>, int, double, double> getPathParameters(std::vector<uint32_t> path);
  void updateGraph(std::vector<uint32_t> edge_index, int flow_BW);
  bool isSatisfying(int path_BW, double path_latency, double path_PLR, int flow_BW, double flow_latency, double flow_PLR);

  std::vector<uint32_t> planeDijkstraAlgorithm(uint32_t source_access_id, uint32_t destination_access_id);
  double getAvgCongestionDegree();

  void SDRA_DFS(int s, int d, std::vector<std::vector<int>> adjList);
  void getSDRAPath(int u, int d, bool visited[], int path[], int path_index, std::vector<std::vector<int>> adjList);
  std::vector<std::vector<int>> getSDRASubGraph(int x1, int y1, int x2, int y2);


  NodeContainer ground_stations; //node container to hold ground stations
  std::vector<Ipv4InterfaceContainer> ground_station_interfaces;

  uint32_t num_planes;
  uint32_t num_satellites_per_plane;
  double m_altitude;

  float K1, K2, K3;
  std::vector <std::vector<int>> adjList;

  std::vector <std::vector<int>> SDRAadjList;
  std::vector <std::vector<double>> edges;
  /*
    edges[i] = {Available_Bandwidth, delay, double PLR, CD, Bandwidth};
  */

  std::vector <std::vector<int>> xyPosition;
  std::vector <std::vector<int>> xyToaccessID;

  /*
    0 : 1 :: Lower : Upper
    2 : 3 :: LeftUpper : RightDown
    4 : 5 :: leftDown : RightUpper
  */

  std::vector<NodeContainer> plane; //node container for each plane
  std::vector<NetDeviceContainer> intra_plane_devices; //contains net devices for all P2P links for all planes
  std::vector<NetDeviceContainer> inter_plane_devices;
  std::vector<NetDeviceContainer> inter_plane_devices2;
  std::vector<Ptr<CsmaChannel>> inter_plane_channels;
  std::vector<Ptr<CsmaChannel>> inter_plane_channels2;
  std::vector<uint32_t> inter_plane_channel_tracker; //this will have the node from the adjacent plane that is currently connected
  std::vector<uint32_t> inter_plane_channel_tracker2; //this will have the node from the adjacent plane that is currently connected
  std::vector<NetDeviceContainer> ground_station_devices; 
  std::vector<Ptr<CsmaChannel>> ground_station_channels;
  std::vector<uint32_t> ground_station_channel_tracker;
  std::vector<Ipv4InterfaceContainer> intra_plane_interfaces;
  std::vector<Ipv4InterfaceContainer> inter_plane_interfaces;
  std::vector<Ipv4InterfaceContainer> inter_plane_interfaces2;
  
};
  
}

#endif /* LEO_SATELLITE_CONFIG_H */
