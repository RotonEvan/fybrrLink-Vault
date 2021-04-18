/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * LEO Satellite Constellation Config
 * Creates and maintains all satellites and links within a satellite communication network
 *
 * ENSC 427: Communication Networks
 * Spring 2020
 * Team 11
 */

#include "leo-satellite-config.h"

namespace ns3 {

  NS_OBJECT_ENSURE_REGISTERED (LeoSatelliteConfig);
  NS_LOG_COMPONENT_DEFINE ("LeoSatelliteConfig");

  extern double CalculateDistanceGroundToSat (const Vector &a, const Vector &b);

  double speed_of_light = 299792458; //in m/s

  //typeid
  TypeId LeoSatelliteConfig::GetTypeId (void) {
    static TypeId tid = TypeId ("ns3::LeoSatelliteConfig").SetParent<Object>().SetGroupName("LeoSatellite");
    return tid;
  }

  LeoSatelliteConfig::~LeoSatelliteConfig (){
  }

  TypeId LeoSatelliteConfig::GetInstanceTypeId (void) const {
    TypeId tid = this->GetTypeId();
    return tid;
  }

  //constructor
  LeoSatelliteConfig::LeoSatelliteConfig (uint32_t num_planes, uint32_t num_satellites_per_plane, double altitude) {
    this->num_planes = num_planes;
    this->num_satellites_per_plane = num_satellites_per_plane;
    this->m_altitude = altitude;
    this->K1 = 0.55;
    this->K2 = 0.30;
    this->K3 = 0.15;

    uint32_t total_num_satellites = num_planes*num_satellites_per_plane;

    for (uint32_t i = 0;i < total_num_satellites;i++){
      std::vector <int> v1(12, -1);
      this->adjList.push_back(v1);
      std::vector <int> v2(2, -1);
      this->xyPosition.push_back(v2);
    }
    for (uint32_t i = 0; i < 2*num_satellites_per_plane;i++){
      std::vector <int> v3(2*num_planes, -1);
      this->xyToaccessID.push_back(v3);
    }

    NodeContainer temp;
    temp.Create(total_num_satellites);

    //assign mobility model to all satellites
    MobilityHelper mobility;
    mobility.SetMobilityModel ("ns3::LeoSatelliteMobilityModel",
                              "NPerPlane", IntegerValue (num_satellites_per_plane),
                              "NumberofPlanes", IntegerValue (num_planes),
                              "Altitude", DoubleValue(altitude),
                              "Time", DoubleValue(Simulator::Now().GetSeconds()));
    mobility.Install(temp);
    
    for (NodeContainer::Iterator j = temp.Begin ();
        j != temp.End (); ++j)
      {
        Ptr<Node> object = *j;
        Ptr<MobilityModel> position = object->GetObject<MobilityModel> ();
        Vector null = Vector(0.0, 0.0, 0.0);
        position->SetPosition(null); // needs to be done to initialize
        NS_ASSERT (position != 0);
      }

    //assigning nodes to e/ plane's node container as necessary
    for (uint32_t i=0; i<num_planes; i++)
    {
      NodeContainer temp_plane;
      for(uint32_t j=0; j<num_satellites_per_plane/2; j++)
      {
        //  Vector pos = temp.Get(i*num_satellites_per_plane/2 + j)->GetObject<MobilityModel> ()->GetPosition();
        //  std::cout << Simulator::Now().GetSeconds() << ": plane # "<< i << " node # " <<j<< ": x = " << pos.x << ", y = " << pos.y << ", z = " << pos.z << std::endl;
        temp_plane.Add(temp.Get(i*num_satellites_per_plane/2 + j));
      }
      for(uint32_t j=num_satellites_per_plane/2; j> 0; j--)
      {
        //  Vector pos = temp.Get(total_num_satellites/2 + i*num_satellites_per_plane/2 + j - 1)->GetObject<MobilityModel> ()->GetPosition();
        //  std::cout << Simulator::Now().GetSeconds() << ": plane # "<< i << " node # " <<num_satellites_per_plane - j<< ": x = " << pos.x << ", y = " << pos.y << ", z = " << pos.z << std::endl;
        temp_plane.Add(temp.Get(total_num_satellites/2 + i*num_satellites_per_plane/2 + j - 1));
      }
      InternetStackHelper stack;
      stack.Install(temp_plane);
      this->plane.push_back(temp_plane);
    }

    //setting up all intraplane links
    Vector nodeAPosition = this->plane[0].Get(0)->GetObject<MobilityModel>()->GetPosition();
    Vector nodeBPosition = this->plane[0].Get(1)->GetObject<MobilityModel>()->GetPosition();
    double distance = CalculateDistance(nodeAPosition, nodeBPosition);
    double delay = (distance * 1000)/speed_of_light; //should get delay in seconds
    PointToPointHelper intraplane_link_helper;
    intraplane_link_helper.SetDeviceAttribute ("DataRate", StringValue ("5.36Gbps"));
    intraplane_link_helper.SetChannelAttribute ("Delay", TimeValue(Seconds (delay)));

    std::cout<<"Setting up intra-plane links with distance of "<<distance<<" km and delay of "<<delay<<" seconds."<<std::endl;

    srand(2);
    double CD, PLR;
    int AB;
    for (uint32_t i=0; i<num_planes; i++)
    {
      for (uint32_t j=0; j<num_satellites_per_plane; j++)
      {
        CD = (double)(60 + (rand()%20))/100;
        PLR = (double)(1 + (rand()%20))/10000;
        AB = (int)4160*(1-CD);
        double param[5] = {(double)AB, delay, PLR, CD, 4160}; // 4.16 Gbps

        this->intra_plane_devices.push_back(intraplane_link_helper.Install(plane[i].Get(j), plane[i].Get((j+1)%num_satellites_per_plane)));
        uint32_t access_idx, adj_node_idx; 
        access_idx = i*num_satellites_per_plane + j;
        adj_node_idx = i*num_satellites_per_plane + (j+1)%num_satellites_per_plane;
        this->addEdge(access_idx, adj_node_idx, param, 0);
        // std::cout<<" Params : "<<param[0]<<" : "<<param[1]<<" : "<<param[2]<<" : "<<param[3]<<" : "<<param[4]<<std::endl;
        // std::cout<<"Intraplane link established between satellite["<<i<<"]["<<j<<"] and satellite["<<i<<"]["<<(j+1)%num_satellites_per_plane<<"]"<<std::endl;
      }
    } 
    int val = 2*(num_satellites_per_plane-1);
    for (uint32_t j=0; j<num_satellites_per_plane; j++){
      this->xyPosition.at(j).at(0) = 0;
      this->xyPosition.at(j).at(1) = val;
      this->xyToaccessID.at(0).at(val) = j;
      val = val - 2;
    }
    
    double minLat = INT_MAX;
    double maxLat = 0;

    //setting up interplane links
    std::cout<<"Setting up inter-plane links"<<std::endl;
    for (uint32_t i=0; i<num_planes; i++)
    {
      // int upper = 0;
      // int lower = 0;
      for (uint32_t j=0; j<num_satellites_per_plane; j++)
      {
        uint32_t nodeBIndex,nodeCIndex;
        (i == num_planes - 1) ? nodeBIndex = num_satellites_per_plane - j - 1: nodeBIndex = j;
        nodeCIndex = (num_satellites_per_plane + nodeBIndex-1)%num_satellites_per_plane;
        // nodeBIndex is Right Lower Link and nodeCIndex is Right Upper Link 
        Vector nodeAPos = this->plane[i].Get(j)->GetObject<MobilityModel>()->GetPosition();
        Vector nodeBPos = this->plane[(i+1)%num_planes].Get(nodeBIndex)->GetObject<MobilityModel>()->GetPosition();
        Vector nodeCPos = this->plane[(i+1)%num_planes].Get(nodeCIndex)->GetObject<MobilityModel>()->GetPosition();

        double distanceAB = CalculateDistance(nodeAPos, nodeBPos);
        double distanceAC = CalculateDistance(nodeAPos, nodeCPos);

        double delayAB = (distanceAB*1000)/speed_of_light;
        CsmaHelper interplane_link_helperAB;
        interplane_link_helperAB.SetChannelAttribute("DataRate", StringValue ("5.36Gbps"));
        interplane_link_helperAB.SetChannelAttribute("Delay", TimeValue(Seconds(delayAB)));

        double delayAC = (distanceAC*1000)/speed_of_light;
        CsmaHelper interplane_link_helperAC;
        interplane_link_helperAC.SetChannelAttribute("DataRate", StringValue ("5.56Gbps"));
        interplane_link_helperAC.SetChannelAttribute("Delay", TimeValue(Seconds(delayAC)));

        // std::cout<<"Interplane link established between satellite["<<i<<"]["<<j<<"] and satellite["<<(i+1)%num_planes<<"]["<<nodeBIndex<< "] with distance "<<distanceAB<< "km and delay of "<<delayAB<<" seconds"<<std::endl;
        // std::cout<<"Interplane link established between satellite["<<i<<"]["<<j<<"] and satellite["<<(i+1)%num_planes<<"]["<<nodeCIndex<< "] with distance "<<distanceAC<< "km and delay of "<<delayAC<<" seconds"<<std::endl;
        CD = (double)(60 + (rand()%20))/100;
        PLR = (double)(1 + (rand()%20))/10000;
        int BW = 3100 + (rand()%700);
        int AB = (int)BW*(1-CD);
        double param[5] = {(double)AB, delayAB, PLR, CD, (double)BW};

        uint32_t access_idx, right_down_idx, right_up_idx;
        access_idx = i*num_satellites_per_plane + j;
        right_down_idx = ((i+1)%num_planes)*num_satellites_per_plane + nodeBIndex;
        right_up_idx = ((i+1)%num_planes)*num_satellites_per_plane + nodeCIndex;

        this->addEdge(access_idx, right_down_idx, param, 3);
        // std::cout<<" Params : "<<param[0]<<" : "<<param[1]<<" : "<<param[2]<<" : "<<param[3]<<std::endl;

        minLat = std::min(delayAB, minLat);
        maxLat = std::max(delayAB, maxLat);

        CD = (double)(60 + (rand()%20))/100;
        BW = 3100 + (rand()%700);
        PLR = (double)(1 + (rand()%20))/10000;
        AB = (int)BW*(1-CD);
        param[0] = (double)AB;
        param[1] = delayAC;
        param[2] = PLR;
        param[3] = CD;
        param[4] = (double)BW;

        this->addEdge(access_idx, right_up_idx, param, 5);
        // std::cout<<" Params : "<<param[0]<<" : "<<param[1]<<" : "<<param[2]<<" : "<<param[3]<<std::endl;

        minLat = std::min(delayAC, minLat);
        maxLat = std::max(delayAC, maxLat);


        NodeContainer temp_node_container;
        temp_node_container.Add(this->plane[i].Get(j));
        temp_node_container.Add(this->plane[(i+1)%num_planes]);
        NetDeviceContainer temp_netdevice_container;

        temp_netdevice_container = interplane_link_helperAB.Install(temp_node_container);
        Ptr<CsmaChannel> csma_channel;
        Ptr<Channel> channel;
        channel = temp_netdevice_container.Get(0)->GetChannel();
        csma_channel = channel->GetObject<CsmaChannel> ();
        
        for (uint32_t k=0; k<num_satellites_per_plane; k++)
        {
          if (nodeBIndex != k)
          {
            csma_channel->Detach(temp_netdevice_container.Get(k+1)->GetObject<CsmaNetDevice> ());
          }
        }
        this->inter_plane_devices.push_back(temp_netdevice_container);
        this->inter_plane_channels.push_back(csma_channel);
        this->inter_plane_channel_tracker.push_back(nodeBIndex);

        temp_netdevice_container = interplane_link_helperAC.Install(temp_node_container);
        channel = temp_netdevice_container.Get(0)->GetChannel();
        csma_channel = channel->GetObject<CsmaChannel> ();

        for (uint32_t k=0; k<num_satellites_per_plane; k++)
        {
          if (nodeCIndex != k)
          {
            csma_channel->Detach(temp_netdevice_container.Get(k+1)->GetObject<CsmaNetDevice> ());
          }
        }
          
        this->inter_plane_devices2.push_back(temp_netdevice_container);
        this->inter_plane_channels2.push_back(csma_channel);
        this->inter_plane_channel_tracker2.push_back(nodeCIndex);

        // Setting the positions of the satellites.
        if (i != num_planes-1){
          this->xyPosition.at(right_down_idx).at(0) = this->xyPosition.at(access_idx).at(0) + 2;
          this->xyPosition.at(right_down_idx).at(1) = this->xyPosition.at(access_idx).at(1) - 1;
          if (this->xyPosition.at(right_down_idx).at(1) < 0){
            this->xyPosition.at(right_down_idx).at(1) = this->xyPosition.at(right_down_idx).at(1) + 2*num_satellites_per_plane;
          }
          // std::cout<<this->xyPosition.at(access_idx).at(0)<<" : "<<this->xyPosition.at(access_idx).at(1)<<" : "<<access_idx<<" : "<<this->xyPosition.at(right_down_idx).at(0)<<" : "<<this->xyPosition.at(right_down_idx).at(1)<<" : "<<right_down_idx<<std::endl;
          this->xyToaccessID.at(this->xyPosition.at(right_down_idx).at(0)).at(this->xyPosition.at(right_down_idx).at(1)) = right_down_idx;
        }
        
        
      }
      // std::cout<<"Plane "<<i<<" Upper : "<<upper<<" and Lower : "<< lower <<std::endl;

    }
    std::cout<<"Min Latency : "<<minLat<<", Max Latency : "<<maxLat<<std::endl;

    //setting up two ground stations for now
    std::cout << "Setting up two ground stations" << std::endl;
    ground_stations.Create(2);
    //assign mobility model to ground stations
    MobilityHelper groundMobility;
    groundMobility.SetMobilityModel ("ns3::GroundStationMobilityModel",
                              "NPerPlane", IntegerValue (num_satellites_per_plane),
                              "NumberofPlanes", IntegerValue (num_planes));
    groundMobility.Install(ground_stations);
    //Install IP stack
    InternetStackHelper stack;
    stack.Install(ground_stations);
    for (int j = 0; j<2; j++)
    {
      Vector temp = ground_stations.Get(j)->GetObject<MobilityModel> ()->GetPosition();
      std::cout << Simulator::Now().GetSeconds() << ": ground station # " << j << ": x = " << temp.x << ", y = " << temp.y << std::endl;
    }
    //setting up links between ground stations and their closest satellites
    std::cout<<"Setting links between ground stations and satellites"<<std::endl;
    for (uint32_t i=0; i<2; i++)
    {
      Vector gndPos = ground_stations.Get(i)->GetObject<MobilityModel> ()->GetPosition();
      uint32_t closestAdjSat = 0;
      uint32_t closestAdjSatDist = 0;
      uint32_t planeIndex;
      if (i == 0)
        planeIndex = 0;
      else
        planeIndex = floor(3*num_planes/7);
      //find closest adjacent satellite for ground station
      for (uint32_t j=0; j<this->num_satellites_per_plane; j++)
      {
        Vector pos = this->plane[planeIndex].Get(j)->GetObject<MobilityModel>()->GetPosition();
        double temp_dist = CalculateDistanceGroundToSat(gndPos,pos);
        if((temp_dist < closestAdjSatDist) || (j==0))
        {
          closestAdjSatDist = temp_dist;
          closestAdjSat = j;
        }
      }
      double delay = (closestAdjSatDist*1000)/speed_of_light;
      CsmaHelper ground_station_link_helper;
      ground_station_link_helper.SetChannelAttribute("DataRate", StringValue ("5.36Gbps"));
      ground_station_link_helper.SetChannelAttribute("Delay", TimeValue(Seconds(delay)));

      // std::cout<<"Channel open between ground station " << i << " and plane " << planeIndex << " satellite "<<closestAdjSat<<" with distance "<<closestAdjSatDist<< "km and delay of "<<delay<<" seconds"<<std::endl;

      NodeContainer temp_node_container;
      temp_node_container.Add(ground_stations.Get(i));
      temp_node_container.Add(this->plane[planeIndex]);
      NetDeviceContainer temp_netdevice_container;
      temp_netdevice_container = ground_station_link_helper.Install(temp_node_container);
      Ptr<CsmaChannel> csma_channel;
      Ptr<Channel> channel;
      channel = temp_netdevice_container.Get(0)->GetChannel();
      csma_channel = channel->GetObject<CsmaChannel> ();

      for (uint32_t k=0; k<num_satellites_per_plane; k++)
      {
        if (closestAdjSat != k)
        {
          csma_channel->Detach(temp_netdevice_container.Get(k+1)->GetObject<CsmaNetDevice> ());
        }
      }
          
      this->ground_station_devices.push_back(temp_netdevice_container);
      this->ground_station_channels.push_back(csma_channel);
      this->ground_station_channel_tracker.push_back(closestAdjSat);
    }

    //Configure IP Addresses for all NetDevices
    Ipv4AddressHelper address;
    address.SetBase ("10.1.0.0", "255.255.255.0");

    //configuring IP Addresses for IntraPlane devices
    for(uint32_t i=0; i< this->intra_plane_devices.size(); i++)
    {
      address.NewNetwork();
      this->intra_plane_interfaces.push_back(address.Assign(this->intra_plane_devices[i]));
    }
    
    //configuring IP Addresses for InterPlane devices
    for(uint32_t i=0; i< this->inter_plane_devices.size(); i++)
    {
      address.NewNetwork();
      this->inter_plane_interfaces.push_back(address.Assign(this->inter_plane_devices[i]));
      for(uint32_t j=1; j<= this->num_satellites_per_plane; j++)
      {
        if(j != this->inter_plane_channel_tracker[i] + 1)
        {
          std::pair< Ptr< Ipv4 >, uint32_t > interface = this->inter_plane_interfaces[i].Get(j);
          interface.first->SetDown(interface.second);
        }
      }
    }

    for(uint32_t i=0; i< this->inter_plane_devices2.size(); i++)
    {
      address.NewNetwork();
      this->inter_plane_interfaces2.push_back(address.Assign(this->inter_plane_devices2[i]));
      for(uint32_t j=1; j<= this->num_satellites_per_plane; j++)
      {
        if(j != this->inter_plane_channel_tracker2[i] + 1)
        {
          std::pair< Ptr< Ipv4 >, uint32_t > interface = this->inter_plane_interfaces2[i].Get(j);
          interface.first->SetDown(interface.second);
        }
      }
    }

    //configuring IP Addresses for Ground devices
    for(uint32_t i=0; i< this->ground_station_devices.size(); i++)
    {
      address.NewNetwork();
      this->ground_station_interfaces.push_back(address.Assign(this->ground_station_devices[i]));
      for(uint32_t j=1; j<= this->num_satellites_per_plane; j++)
      {
        if(j != this->ground_station_channel_tracker[i] + 1)
        {
          std::pair< Ptr< Ipv4 >, uint32_t > interface = this->ground_station_interfaces[i].Get(j);
          interface.first->SetDown(interface.second);
        }
      }
    }

    //Populate Routing Tables
    // std::cout<<"Populating Routing Tables"<<std::endl;
    // Ipv4GlobalRoutingHelper::PopulateRoutingTables ();
    // std::cout<<"Finished Populating Routing Tables"<<std::endl;

    // Set up packet sniffing for entire network
    /*CsmaHelper csma;
    for (uint32_t i=0; i< this->inter_plane_devices.size(); i++)
    {
      csma.EnablePcap("inter-sniff", this->inter_plane_devices[i].Get(1), true);
    }
    PointToPointHelper p2p;
    for(uint32_t i=0; i< this->intra_plane_devices.size(); i++)
    {
      p2p.EnablePcap("intra-sniff", this->intra_plane_devices[i].Get(1), true);
    }
    for(uint32_t i=0; i< this->ground_station_devices.size(); i++)
    {
      p2p.EnablePcap("ground-sniff", this->ground_station_devices[i].Get(1), true);
    }*/
  }

  void LeoSatelliteConfig::UpdateLinks() {
    std::cout<<std::endl<<std::endl<<std::endl<<"Updating Links"<<std::endl;

    // update_links_plane will store all planes
    std::vector<NodeContainer> update_links_plane = this->plane;
    std::cout<<update_links_plane.size()<<std::endl;
    NodeContainer final_plane;
    for(uint32_t j=0; j<num_satellites_per_plane; j++)
    {
      final_plane.Add(this->plane[0].Get(num_satellites_per_plane - j - 1));
    }
    // Add the first plane satellites once again (Satellites in reverse order)
    update_links_plane.push_back(final_plane);
    
    for (uint32_t i=0; i<this->num_planes; i++)
    {
      Vector refSatPos;
      uint32_t refSat = 0;
      //find reference satellite (closest to equator)
      for (uint32_t j=0; j<this->num_satellites_per_plane; j++)
      {
        Vector pos = update_links_plane[i].Get(j)->GetObject<MobilityModel>()->GetPosition();
        if ((std::abs(pos.x) < std::abs(refSatPos.x)) || j == 0)
        {
          refSatPos = pos;
          refSat = j;
        }
      }

      //find the closest adjacent satellite to the reference satellite
      uint32_t closestAdjSat = 0;
      double closestAdjSatDist = 0;
      Vector adjPos; //debug
      for (uint32_t j=0; j<this->num_satellites_per_plane; j++)
      {
        Vector pos = update_links_plane[i+1].Get(j)->GetObject<MobilityModel>()->GetPosition();
        double temp_dist = CalculateDistance(refSatPos,pos);
        if((temp_dist < closestAdjSatDist) || (j==0))
        {
          closestAdjSatDist = temp_dist;
          closestAdjSat = j;
          adjPos = pos; //debug
        }
      }

      //calculate the reference increment factor for adjacent satellites in a plane
      uint32_t ref_incr;
      (refSat <= closestAdjSat) ? (ref_incr = closestAdjSat - refSat) : (ref_incr = this->num_satellites_per_plane - refSat + closestAdjSat);

      // int upper = 0;
      // int lower = 0;
      //update all adjacent satellites for this plane
      for (uint32_t j=0; j<this->num_satellites_per_plane; j++)
      {
        uint32_t access_idx = i*(this->num_satellites_per_plane) + j;
        uint32_t currAdjNodeID = this->inter_plane_channel_tracker[access_idx];
        uint32_t currAdjNodeID2 = this->inter_plane_channel_tracker2[access_idx];
        uint32_t nextAdjNodeID = (j + ref_incr)%(this->num_satellites_per_plane);

        double nextAdjNodeDist,nextAdjNodeDist2;

        Vector constNodePos = update_links_plane[i].Get(j)->GetObject<MobilityModel>()->GetPosition();
        Vector nextAdjNodePos = update_links_plane[(i+1)/*%(this->num_planes)*/].Get(nextAdjNodeID)->GetObject<MobilityModel>()->GetPosition();

        if (i == this->num_planes - 1)
          nextAdjNodeID = num_satellites_per_plane - nextAdjNodeID - 1;
        
        uint32_t nextAdjNodeID2 =  (num_satellites_per_plane + nextAdjNodeID-1)%num_satellites_per_plane;

        Vector nextAdjNodePos2 = update_links_plane[(i+1)/*%(this->num_planes)*/].Get(nextAdjNodeID2)->GetObject<MobilityModel>()->GetPosition();
        nextAdjNodeDist = CalculateDistance(constNodePos, nextAdjNodePos);
        nextAdjNodeDist2 = CalculateDistance(constNodePos, nextAdjNodePos2);
        double new_delay;

        uint32_t right_down_idx, right_up_idx;
        right_down_idx = ((i+1)%num_planes)*num_satellites_per_plane + nextAdjNodeID;
        right_up_idx = ((i+1)%num_planes)*num_satellites_per_plane + nextAdjNodeID2;

        if(currAdjNodeID == nextAdjNodeID)
        {
          new_delay = (nextAdjNodeDist*1000)/speed_of_light;
          this->inter_plane_channels[access_idx]->SetAttribute("Delay", TimeValue(Seconds(new_delay)));
          // std::cout<<"Interplane link updated between satellite["<<i<<"]["<<j<<"] and satellite["<<(i+1)%num_planes<<"]["<<nextAdjNodeID<< "] with distance "<<nextAdjNodeDist<< "km and delay of "<<new_delay<<" seconds"<<std::endl;

          new_delay = (nextAdjNodeDist2*1000)/speed_of_light;
          this->inter_plane_channels2[access_idx]->SetAttribute("Delay", TimeValue(Seconds(new_delay)));
          // std::cout<<"Interplane link updated between satellite["<<i<<"]["<<j<<"] and satellite["<<(i+1)%num_planes<<"]["<<nextAdjNodeID2<< "] with distance "<<nextAdjNodeDist2<< "km and delay of "<<new_delay<<" seconds"<<std::endl;
        }
        else
        {

          this->inter_plane_channels[access_idx]->Detach(this->inter_plane_devices[access_idx].Get(currAdjNodeID+1)->GetObject<CsmaNetDevice> ());
          std::pair< Ptr< Ipv4 >, uint32_t> interface = this->inter_plane_interfaces[access_idx].Get(currAdjNodeID+1);
          interface.first->SetDown(interface.second);
          this->inter_plane_channels[access_idx]->Reattach(this->inter_plane_devices[access_idx].Get(nextAdjNodeID+1)->GetObject<CsmaNetDevice> ());
          interface = this->inter_plane_interfaces[access_idx].Get(nextAdjNodeID+1);
          interface.first->SetUp(interface.second);
          this->inter_plane_channel_tracker[access_idx] = nextAdjNodeID;
          new_delay = (nextAdjNodeDist*1000)/speed_of_light;
        
          this->inter_plane_channels[access_idx]->SetAttribute("Delay", TimeValue(Seconds(new_delay)));

          double param[] = {5.36, new_delay, 0.23, 0.76};
          this->addEdge(access_idx, right_down_idx, param, 3);

          // std::cout<<"New interplane link established between satellite["<<i<<"]["<<j<<"] and satellite["<<(i+1)%num_planes<<"]["<<nextAdjNodeID<< "] with distance "<<nextAdjNodeDist<< "km and delay of "<<new_delay<<" seconds"<<std::endl;
        
          this->inter_plane_channels2[access_idx]->Detach(this->inter_plane_devices2[access_idx].Get(currAdjNodeID2+1)->GetObject<CsmaNetDevice> ());
          interface = this->inter_plane_interfaces2[access_idx].Get(nextAdjNodeID2+1);
          interface.first->SetDown(interface.second);
          this->inter_plane_channels2[access_idx]->Reattach(this->inter_plane_devices2[access_idx].Get(nextAdjNodeID2+1)->GetObject<CsmaNetDevice> ());
          interface = this->inter_plane_interfaces2[access_idx].Get(nextAdjNodeID2+1);
          interface.first->SetUp(interface.second);
          this->inter_plane_channel_tracker2[access_idx] = nextAdjNodeID2;
          new_delay = (nextAdjNodeDist2*1000)/speed_of_light;

          param[1] = new_delay;
          this->addEdge(access_idx, right_up_idx, param, 5);

          this->inter_plane_channels2[access_idx]->SetAttribute("Delay", TimeValue(Seconds(new_delay)));
          // std::cout<<"New interplane link established between satellite["<<i<<"]["<<j<<"] and satellite["<<(i+1)%num_planes<<"]["<<nextAdjNodeID2<< "] with distance "<<nextAdjNodeDist2<< "km and delay of "<<new_delay<<" seconds"<<std::endl;
        }
        if (i != num_planes-1){
          this->xyPosition.at(right_down_idx).at(0) = this->xyPosition.at(access_idx).at(0) + 2;
          this->xyPosition.at(right_down_idx).at(1) = this->xyPosition.at(access_idx).at(1) - 1;
          if (this->xyPosition.at(right_down_idx).at(1) < 0){
            this->xyPosition.at(right_down_idx).at(1) = this->xyPosition.at(right_down_idx).at(1) + 2*num_satellites_per_plane;
          }
          // std::cout<<this->xyPosition.at(access_idx).at(0)<<" : "<<this->xyPosition.at(access_idx).at(1)<<" : "<<access_idx<<" : "<<this->xyPosition.at(right_down_idx).at(0)<<" : "<<this->xyPosition.at(right_down_idx).at(1)<<" : "<<right_down_idx<<std::endl;
          this->xyToaccessID.at(this->xyPosition.at(right_down_idx).at(0)).at(this->xyPosition.at(right_down_idx).at(1)) = right_down_idx;
        }
      }
        // std::cout<<"Plane "<<i<<" Upper : "<<upper<<" and Lower : "<< lower <<std::endl;

    }

    //updating links between ground stations and their closest satellites
    for (uint32_t i=0; i<2; i++)
    {
      Vector gndPos = ground_stations.Get(i)->GetObject<MobilityModel> ()->GetPosition();
      uint32_t closestAdjSat = 0;
      uint32_t closestAdjSatDist = 0;
      uint32_t planeIndex;
      if (i == 0)
        planeIndex = 0;
      else
        planeIndex = floor(3*num_planes/7);
      //find closest adjacent satellite for ground station
      for (uint32_t j=0; j<this->num_satellites_per_plane; j++)
      {
        Vector pos = this->plane[planeIndex].Get(j)->GetObject<MobilityModel>()->GetPosition();
        double temp_dist = CalculateDistanceGroundToSat(gndPos,pos);
        if((temp_dist < closestAdjSatDist) || (j==0))
        {
          closestAdjSatDist = temp_dist;
          closestAdjSat = j;
        }
      }

      uint32_t currAdjNodeID = this->ground_station_channel_tracker[i];
      if(currAdjNodeID == closestAdjSat)
      {
        double new_delay = (closestAdjSatDist*1000)/speed_of_light;
        this->ground_station_channels[i]->SetAttribute("Delay", TimeValue(Seconds(new_delay)));
        // std::cout<<"Channel updated between ground station "<<i<<" and plane "<<planeIndex<<" satellite "<<closestAdjSat<< " with distance "<<closestAdjSatDist<< "km and delay of "<<new_delay<<" seconds"<<std::endl;
        }
        else
        {
          this->ground_station_channels[i]->Detach(this->ground_station_devices[i].Get(currAdjNodeID+1)->GetObject<CsmaNetDevice> ());
          std::pair< Ptr< Ipv4 >, uint32_t> interface = this->ground_station_interfaces[i].Get(currAdjNodeID+1);
          interface.first->SetDown(interface.second);
          this->ground_station_channels[i]->Reattach(this->ground_station_devices[i].Get(closestAdjSat+1)->GetObject<CsmaNetDevice> ());
          interface = this->ground_station_interfaces[i].Get(closestAdjSat+1);
          interface.first->SetUp(interface.second);
          this->ground_station_channel_tracker[i] = closestAdjSat;
          double new_delay = (closestAdjSatDist*1000)/speed_of_light;
          this->ground_station_channels[i]->SetAttribute("Delay", TimeValue(Seconds(new_delay)));
          // std::cout<<"New channel between ground station "<<i<<" and plane "<<planeIndex<<" satellite "<<closestAdjSat<< " with distance "<<closestAdjSatDist<< "km and delay of "<<new_delay<<" seconds"<<std::endl;
        }

    }
    
    //Recompute Routing Tables
    std::cout<<"Recomputing Routing Tables"<<std::endl;
    Ipv4GlobalRoutingHelper::RecomputeRoutingTables ();
    std::cout<<"Finished Recomputing Routing Tables"<<std::endl;
  }

  void LeoSatelliteConfig:: printGraph(){

    std::cout<<">> Printing links of each satellite."<<std::endl;
    for (std::size_t i = 0; i < this->adjList.size(); ++i) {
      uint32_t plane_no = i/this->num_satellites_per_plane;
      uint32_t satellit_no = i%this->num_satellites_per_plane;
      std::cout<<"Node ["<< plane_no<<"]["<<satellit_no<<"] : ";
      for (uint32_t j = 0; j < 6; j++){
        std::cout<<this->adjList.at(i).at(j)<<" ";
      }
      std::cout<<std::endl;
    }

    std::cout<<">> Printing Position of each satellite."<<std::endl;
    for (std::size_t i = 0; i < this->xyPosition.size(); ++i) {
      uint32_t plane_no = i/this->num_satellites_per_plane;
      uint32_t satellit_no = i%this->num_satellites_per_plane;
      std::cout<<"Node ["<< plane_no<<"]["<<satellit_no<<"] is at ("<<this->xyPosition.at(i).at(0)<< "," << this->xyPosition.at(i).at(1) << ") "<<std::endl;
    }

    std::cout<<">> Printing link parameters of each link."<<std::endl;
    for (std::size_t i = 0; i < this->edges.size(); ++i) {
      std::cout<<"Edge no. "<< i<<", Parameters are : ";
      for (uint32_t j = 0; j < 5; j++){
        std::cout<<this->edges.at(i).at(j)<<" ";
      }
      std::cout<<std::endl;
    }

  }
  void LeoSatelliteConfig:: addEdge(uint32_t node1_id, uint32_t node2_id, double params[], uint32_t type){
        int edge_index;
        double cost = this->calculateCost(params, 0.85);

        if (this->adjList.at(node1_id).at(type) == -1){
          std::vector<double> e = {params[0], params[1], params[2], params[3], params[4]};
          e.push_back(cost);

          edge_index = this->edges.size();

          this->edges.push_back(e);

          this->adjList.at(node1_id).at(type) = edge_index;
          this->adjList.at(node1_id).at(type+6) = node2_id;

          if (type%2 == 0){
            type = type + 1;
          }
          else{
            type = type - 1;
          }
          this->adjList.at(node2_id).at(type) = edge_index;
          this->adjList.at(node2_id).at(type+6) = node1_id;

        }
        else{
          edge_index = this->adjList.at(node1_id).at(type);
          this->edges.at(edge_index).at(5) = cost;
        }    
  }



  double LeoSatelliteConfig::calculateCost(double params[], double maxCD){
    int AB = (int)params[0];
    if(params[3] < 0.97){
        if (params[3] >= maxCD){
          AB = 0;
        }
      double score = this->K1*AB + this->K2/params[1] + this->K3*(1 - params[2]);
      return 1/score;
    }
    else{
      return INT_MAX;
    } 
  }

  std::vector<uint32_t> LeoSatelliteConfig::bresenhemAlgo(int x1, int y1, int x2, int y2){

    std::vector<int> x_points, y_points;
    std::vector<uint32_t> shortestPath;
    std::tie(x_points, y_points) = this->bresenhemAlgoUtil(y1, x1 , y2, x2); // Reversing because algorithm is designed for the horizontal orbits.

    for (std::size_t i = 0; i < x_points.size(); ++i) {
      shortestPath.push_back(this->xyToaccessID.at(y_points.at(i)).at(x_points.at(i)));
    }
    return shortestPath;
  }

  std::tuple<std::vector<int>, std::vector<int>> LeoSatelliteConfig::bresenhemAlgoUtil(int x1, int y1, int x2, int y2){
    int dx = abs(x2 - x1);
	  int dy = abs(y2 - y1);

	  bool is_steep = dy > dx;

	  bool swapped = false;
	  if (x1 > x2){
      std::swap(x1, x2);
      std::swap(y1, y2);
      swapped = true;
    }
    dx = abs(x2 - x1);
    dy = abs(y2 - y1);

	  int decision_var = 2*dy - dx;

	  bool is_source_up = false;

    int ystep = 2;

	  if (y1 >= y2){
      is_source_up = true;
		  ystep = -2;
    }

		int x, y;
    x = x1;
    y = y1;

	  std::vector<int> point_x;
    std::vector<int> point_y;

    if (is_steep){
      while(x <= x2+1){
        if(!is_source_up && y > y2){
          break;
        }
        else if (is_source_up && y < y2){
          break;
        }
        point_x.push_back(x);
        point_y.push_back(y);
        if (decision_var < 0){
          decision_var = decision_var + 4*dy;
          x++;
        }
        else{
          decision_var = decision_var + 2*(dy-2*dx);
          y = y + ystep;
        }
        x++;
      }
    }
    else{
      while(x <= x2){
        point_x.push_back(x);
        point_y.push_back(y);

        if(decision_var < 0){
          decision_var = decision_var + 4*dy;
          x++;
        }
        else{
          decision_var = decision_var + 2*(dy-2*dx);
          y = y + ystep;
        } 
        x++;
      }
    }
  // for (std::size_t i = 0; i < point_x.size(); ++i){
  //   std::cout<<" - > ("<<point_x.at(i)<<" , "<<point_y.at(i)<<") ";
  // }

	if (point_x.back() != x2 || point_y.back() != y2){
    std::vector<int> extra_x_points, extra_y_points;
    std::tie(extra_x_points, extra_y_points) = this->bresenhemAlgoUtil(point_x.back(), point_y.back(), x2, y2);
    for (std::size_t i = 1; i < extra_x_points.size(); ++i) {
      point_x.push_back(extra_x_points[i]);
      point_y.push_back(extra_y_points[i]);
    }
  }

	if (swapped){
    reverse(point_x.begin(), point_x.end());
    reverse(point_y.begin(), point_y.end());
  }
  
	return std::make_tuple(point_x, point_y);
  }

  std::vector<std::vector<int>> LeoSatelliteConfig::getSubGraph(int x1, int y1, int x2, int y2){

    std::vector<std::vector<int>> subAdjList = this->adjList;

    uint32_t link[5] = {4, 2, 1, 2, 5 };
    uint32_t vertical_steps, diagonal_steps;

    if (x1 > x2){
      std::swap(x1, x2);
      std::swap(y1, y2);
    }
    diagonal_steps = abs(x2 - x1)/2;
    vertical_steps = abs(abs(y1 - y2) - diagonal_steps)/2;
    // std::cout<<">> Diagonal Steps : "<<diagonal_steps<<", Vertical Steps : "<<vertical_steps<<std::endl;
    
	  if (y1 >= y2){
      link[4] = 3;
      link[3] = 4;
      link[2] = 0;
    }

    uint32_t i;
    uint32_t access_idx = this->xyToaccessID.at(x1).at(y1);

    for (i = 0; i < vertical_steps+1; i++){
      // std::cout<<"Hello Node ["<< this->xyPosition.at(access_idx).at(0)<<"]["<<this->xyPosition.at(access_idx).at(1)<<"] are : "<<std::endl;
      subAdjList.at(access_idx).at(link[0]) = -1;
      subAdjList.at(access_idx).at(link[1]) = -1;
      if (i != vertical_steps)
        access_idx = subAdjList.at(access_idx).at(link[2]+6);
    }


    for (i = 0; i < diagonal_steps+1; i++){
      subAdjList.at(access_idx).at(link[2]) = -1;
      subAdjList.at(access_idx).at(link[3]) = -1;
      if (i != diagonal_steps)
        access_idx = subAdjList.at(access_idx).at(link[4]+6);
    }

    link[2] = 1 - link[2];
    if (y1 >= y2){
      link[4] = 2;
      link[3] = 5;
    }
    else{
      link[4] = 4;
      link[3] = 3;
    }
    link[0]++;
    link[1]++;

    for (i = 0; i < vertical_steps+1; i++){
      subAdjList.at(access_idx).at(link[0]) = -1;
      subAdjList.at(access_idx).at(link[1]) = -1;
      if (i != vertical_steps)
        access_idx = subAdjList.at(access_idx).at(link[2]+6);
    }
    for (i = 0; i < diagonal_steps+1; i++){
      subAdjList.at(access_idx).at(link[2]) = -1;
      subAdjList.at(access_idx).at(link[3]) = -1;
      if (i != diagonal_steps)
        access_idx = subAdjList.at(access_idx).at(link[4]+6);
    }

    // for (std::size_t i = 0; i < subAdjList.size(); ++i) {
    //   for (uint32_t j = 0; j < 12; j++)
    //     std::cout<<subAdjList.at(i).at(j)<<" ";
    //   std::cout<<std::endl;
    // }
    return subAdjList;
  }

  uint32_t LeoSatelliteConfig::minDistance(double dist[], bool visited[], std::vector<uint32_t> Nodes){
    double min = INT_MAX;
    int min_index = 0;
    for (std::size_t i = 0; i < Nodes.size(); i++){
      uint32_t v = Nodes.at(i);
      if (visited[v] == false && dist[v] <= min)
          min = dist[v], min_index = v;
    }
    return min_index;
  }

  std::vector<uint32_t> LeoSatelliteConfig::DijkstraAlgorithm(uint32_t source_access_id, uint32_t destination_access_id, std::vector<std::vector<int>> AdjList, bool recomputeCost = false, double maxCD = 0.85) {
    int V = this->num_satellites_per_plane*this->num_planes;

    std::vector<uint32_t> Nodes;
    bool visited[V];
    for (int i = 0; i < V; i++)
      visited[i] = false;
    

    std::stack<int> stack_arr;
    stack_arr.push(source_access_id);
    uint32_t access_idx;

    while (!stack_arr.empty()) {
        access_idx = stack_arr.top();
        stack_arr.pop();
        if (!visited[access_idx]){
          Nodes.push_back(access_idx);
          visited[access_idx] = true;
        }
        for (uint32_t i = 0;i < 6;i++){
          if (AdjList.at(access_idx).at(i) != -1 && !visited[AdjList.at(access_idx).at(i+6)]){
              stack_arr.push(AdjList.at(access_idx).at(i+6));
              // std::cout<<"Node "<< access_idx<<"] is at ("<<this->xyPosition.at(access_idx).at(0)<< "," << this->xyPosition.at(access_idx).at(1) << ") and "<<"Node "<< AdjList.at(access_idx).at(i+6)<<"] is at ("<<this->xyPosition.at(AdjList.at(access_idx).at(i+6)).at(0)<< "," << this->xyPosition.at(AdjList.at(access_idx).at(i+6)).at(1) << ")"<<std::endl;
          }
        }
    }
        
    int parent[V];
    double dist[V];
    double cost;

    for (int i = 0; i < V; i++)
      dist[i] = INT_MAX, visited[i] = false, parent[i] = -1;
        
    dist[source_access_id] = 0;

    for (std::size_t count = 0; count < Nodes.size(); ++count) {
        // std::cout<<Nodes.at(count)<<std::endl;
        uint32_t u = this->minDistance(dist, visited, Nodes);
        visited[u] = true;
        for (int i = 0; i < 6; i++){
          int edge_index = AdjList.at(u).at(i);
          int v_id = AdjList.at(u).at(i+6);
          if (edge_index == -1){
            continue;
          }
          if (!recomputeCost){
            cost = this->edges.at(edge_index).at(5);
          }
          else{
            double param[] = {this->edges.at(edge_index).at(0), this->edges.at(edge_index).at(1), this->edges.at(edge_index).at(2), this->edges.at(edge_index).at(3)};
            cost = this->calculateCost(param, maxCD);
            // std::cout<<u<<" : "<<cost<<std::endl;
          }
          if (!visited[v_id] && dist[u] + cost < dist[v_id]){
            dist[v_id] = dist[u] + cost;
            parent[v_id] = u;
          }
        }         
    }
    // std::cout<<">> Dijkstra algorithm executed."<<std::endl;

    access_idx = destination_access_id;
    std::vector<uint32_t> path;
    // for (int i = 0; i < V; i++) {
    //   std::cout<<"i : "<<i<<" ,parent[i] : "<<parent[i]<<visited[i]<<std::endl;
    // }
    if(!visited[destination_access_id]){
      std::cout<<">> Destination Unreachable"<<std::endl;
      return path;
    }
    while(access_idx != source_access_id){
      path.push_back(access_idx);
      access_idx = parent[access_idx];
    }
    path.push_back(source_access_id);

    reverse(path.begin(), path.end());

    return path;
  }

  std::tuple<std::vector<uint32_t>, int, double, double> LeoSatelliteConfig::getPathParameters(std::vector<uint32_t> path){
    int n = (int)path.size();
    std::vector<uint32_t> edge_index;
    uint32_t index = 0;
    double latency = 0;
    int bottleneckBW = 0;
    double PLR = 0;

    for (int i = 0; i < n-1; i++){
      for (int j = 0;j < 6; j++){
        if ((uint32_t)this->adjList.at(path.at(i)).at(j+6) == path.at(i+1)){
          index = this->adjList.at(path.at(i)).at(j);
          edge_index.push_back(index);

          latency += this->edges.at(index).at(1);
          if (i == 0){
              PLR = (1-this->edges.at(index).at(2));
              bottleneckBW = this->edges.at(index).at(0);
              break;
          }
          PLR = PLR*(1-this->edges.at(index).at(2));
          if (bottleneckBW > this->edges.at(index).at(0)){
            bottleneckBW = this->edges.at(index).at(0);
          }
          break;
        }
      }
    }
    PLR = 1 - PLR;
    return std::make_tuple(edge_index, bottleneckBW, latency*1000, PLR);
  }

  void LeoSatelliteConfig::updateGraph(std::vector<uint32_t> edge_index, int flow_BW){
    int n = (int)edge_index.size();
    uint32_t index;
    
    for (int i = 0; i < n; i++){
      index = edge_index.at(i);
      this->edges.at(index).at(0) = this->edges.at(index).at(0) - flow_BW;
      this->edges.at(index).at(3) = std::min((double)(this->edges.at(index).at(4) - this->edges.at(index).at(0))/this->edges.at(index).at(4), 1.0);
      double param[] = {this->edges.at(index).at(0), this->edges.at(index).at(1), this->edges.at(index).at(2), this->edges.at(index).at(3)};
      this->edges.at(index).at(5) = this->calculateCost(param, 1);
      // std::cout<<this->edges.at(index).at(0)<<" : "<<this->edges.at(index).at(1)<<" : "<<this->edges.at(index).at(2)<<" : "<<this->edges.at(index).at(3)<<" : "<<this->edges.at(index).at(5)<<std::endl;
    }
  }

  bool LeoSatelliteConfig::isSatisfying(int path_BW, double path_latency, double path_PLR, int flow_BW, double flow_latency, double flow_PLR){
    // std::cout<<path_latency<<" : "<<flow_latency<<" : "<<path_BW<<" : "<<flow_BW<<" : "<<path_PLR<<" : "<<flow_PLR<<std::endl;
    if (path_latency <= flow_latency && path_BW >= flow_BW && path_PLR <= flow_PLR ){
      return true;
    }
	  return false;
  }
  
  std::vector<uint32_t> LeoSatelliteConfig::planeDijkstraAlgorithm(uint32_t source_access_id, uint32_t destination_access_id) {

  int V = this->num_satellites_per_plane*this->num_planes;

    std::vector<uint32_t> Nodes;
    bool visited[V];
    for (int i = 0; i < V; i++){
      visited[i] = false;
      Nodes.push_back(i);
    }
    
    int parent[V];
    double dist[V];
    double cost;

    for (int i = 0; i < V; i++)
      dist[i] = INT_MAX, visited[i] = false, parent[i] = -1;
        
    dist[source_access_id] = 0;

    for (std::size_t count = 0; count < Nodes.size(); ++count) {
        // std::cout<<Nodes.at(count)<<std::endl;
        uint32_t u = this->minDistance(dist, visited, Nodes);
        visited[u] = true;
        for (int i = 0; i < 6; i++){
          int edge_index = this->adjList.at(u).at(i);
          int v_id = this->adjList.at(u).at(i+6);
          if (edge_index == -1){
            continue;
          }
          if(this->edges.at(edge_index).at(3) > 0.99){
            cost = INT_MAX;
          }
          else{
            cost = (double)1/(this->edges.at(edge_index).at(1));
          }
          if (!visited[v_id] && dist[u] + cost < dist[v_id]){
            dist[v_id] = dist[u] + cost;
            parent[v_id] = u;
          }
        }         
    }
    // std::cout<<">> Dijkstra algorithm executed."<<std::endl;

    uint32_t access_idx = destination_access_id;
    std::vector<uint32_t> path;
    // for (int i = 0; i < V; i++) {
    //   std::cout<<"i : "<<i<<" ,parent[i] : "<<parent[i]<<std::endl;
    // }
    while(access_idx != source_access_id){
      path.push_back(access_idx);
      access_idx = parent[access_idx];
    }
    path.push_back(source_access_id);

    reverse(path.begin(), path.end());

    return path;
  }

  double LeoSatelliteConfig::getAvgCongestionDegree(){
    double avg = 0;
    int n = this->edges.size();
    for(int i = 0;i < n;i++){
      avg = avg + this->edges.at(i).at(3);
    }
    avg = avg/n;
    return avg;
  }
  std::vector<std::vector<int>> LeoSatelliteConfig::getSDRASubGraph(int x1, int y1, int x2, int y2){

    std::vector<std::vector<int>> subAdjList = this->adjList;

    uint32_t link[5] = {4, 2, 1, 2, 3 };
    uint32_t vertical_steps, diagonal_steps;

    if (x1 > x2){
      std::swap(x1, x2);
      std::swap(y1, y2);
    }
    diagonal_steps = abs(x2 - x1)/2;
    vertical_steps = abs(y1 - y2);
    // std::cout<<">> Diagonal Steps : "<<diagonal_steps<<", Vertical Steps : "<<vertical_steps<<std::endl;
    
	  if (y1 >= y2){
      link[4] = 3;
      link[3] = 4;
      link[2] = 0;
    }

    uint32_t i;
    uint32_t access_idx = this->xyToaccessID.at(x1).at(y1);

    for (i = 0; i < vertical_steps+1; i++){
      std::cout<<"Hello Node ["<< this->xyPosition.at(access_idx).at(0)<<"]["<<this->xyPosition.at(access_idx).at(1)<<"] are : "<<std::endl;
      subAdjList.at(access_idx).at(link[0]) = -1;
      subAdjList.at(access_idx).at(link[1]) = -1;
      if (i != vertical_steps)
        access_idx = subAdjList.at(access_idx).at(link[2]+6);
    }


    for (i = 0; i < diagonal_steps+1; i++){
            std::cout<<"Hello Node ["<< this->xyPosition.at(access_idx).at(0)<<"]["<<this->xyPosition.at(access_idx).at(1)<<"] are : "<<std::endl;

      subAdjList.at(access_idx).at(link[2]) = -1;
      subAdjList.at(access_idx).at(link[3]) = -1;
      if (i != diagonal_steps)
        access_idx = subAdjList.at(access_idx).at(link[4]+6);
    }

    link[2] = 1 - link[2];
    if (y1 >= y2){
      link[4] = 2;
      link[3] = 5;
    }
    else{
      link[4] = 2;
      link[3] = 3;
    }
    link[0]++;
    link[1]++;

    for (i = 0; i < vertical_steps+1; i++){   
         std::cout<<"Hello Node ["<< this->xyPosition.at(access_idx).at(0)<<"]["<<this->xyPosition.at(access_idx).at(1)<<"] are : "<<std::endl;

      subAdjList.at(access_idx).at(link[0]) = -1;
      subAdjList.at(access_idx).at(link[1]) = -1;
      if (i != vertical_steps)
        access_idx = subAdjList.at(access_idx).at(link[2]+6);
    }
    for (i = 0; i < diagonal_steps+1; i++){
            std::cout<<"Hello Node ["<< this->xyPosition.at(access_idx).at(0)<<"]["<<this->xyPosition.at(access_idx).at(1)<<"] are : "<<std::endl;

      subAdjList.at(access_idx).at(link[2]) = -1;
      subAdjList.at(access_idx).at(link[3]) = -1;
      if (i != diagonal_steps)
        access_idx = subAdjList.at(access_idx).at(link[4]+6);
    }
    return subAdjList;
  }

  void LeoSatelliteConfig::SDRA_DFS(int s, int d, std::vector<std::vector<int>> AdjList) {
    int V = this->num_planes*this->num_satellites_per_plane;
    bool* visited = new bool[V];
  
    // Create an array to store paths
    int* path = new int[V];
    int path_index = 0;
  
    for (int i = 0; i < V; i++)
        visited[i] = false;

    // std::vector<uint32_t> Nodes;
    // bool visited[V];
    // for (int i = 0; i < V; i++)
    //   visited[i] = false;
    

    // std::stack<int> stack_arr;
    // stack_arr.push(s);
    // uint32_t access_idx;

    // while (!stack_arr.empty()) {
    //     access_idx = stack_arr.top();
    //     stack_arr.pop();
    //     if (!visited[access_idx]){
    //       Nodes.push_back(access_idx);
    //       visited[access_idx] = true;
    //     }
    //     for (uint32_t i = 0;i < 6;i++){
    //       if (AdjList.at(access_idx).at(i) != -1 && !visited[AdjList.at(access_idx).at(i+6)]){
    //           stack_arr.push(AdjList.at(access_idx).at(i+6));
    //           std::cout<<"Node "<< access_idx<<"] is at ("<<this->xyPosition.at(access_idx).at(0)<< "," << this->xyPosition.at(access_idx).at(1) << ") and "<<"Node "<< AdjList.at(access_idx).at(i+6)<<"] is at ("<<this->xyPosition.at(AdjList.at(access_idx).at(i+6)).at(0)<< "," << this->xyPosition.at(AdjList.at(access_idx).at(i+6)).at(1) << ")"<<std::endl;
    //       }
    //     }
    // }
    this->getSDRAPath(s, d, visited, path, path_index, AdjList);

    for (int i = 0; i < V; i++)
        std::cout<<path[i]<<std::endl;
  }
    
  void LeoSatelliteConfig::getSDRAPath(int u, int d, bool visited[], int path[], int path_index, std::vector<std::vector<int>> adjList) {
      // Mark the current node and store it in path[]
      visited[u] = true;
      path[path_index] = u;
      path_index++;

      if (u == d) {
          for (int i = 0; i < path_index; i++)
              std::cout << path[i] << " ";
          std::cout << std::endl;
      }
      else
      {
          for (int index = 0; index < 6;index++){
            // std::cout<<adjList.at(u).at(index)<<std::endl
            if (adjList.at(u).at(index) != -1 && !visited[adjList.at(u).at(index+6)])
              this->getSDRAPath(index, d, visited, path, path_index, adjList);
          }
          // for (i = adj[u].begin(); i != adj[u].end(); ++i)
          //     if (!visited[*i])
          //         getSDRAPath(*i, d, visited, path, path_index, adjList);
      }
    
      // Remove current vertex from path[] and mark it as unvisited
      path_index--;
      visited[u] = false;
  }

}