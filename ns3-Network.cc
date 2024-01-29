#include <string>
#include "ns3/applications-module.h"
#include "ns3/core-module.h"
#include "ns3/csma-module.h"
#include "ns3/internet-module.h"
#include "ns3/mobility-module.h"
#include "ns3/network-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/ssid.h"
#include "ns3/yans-wifi-helper.h"
#include "ns3/wifi-helper.h"
#include "ns3/wifi-helper.h"
#include "ns3/yans-wifi-helper.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("task_2012670");

/*

da implementare nell'application

TCP burst traffic of 1617 B for each packet starting at 0.49 s
	Sender: Node 10	Receiver: Server 0
TCP burst traffic of 1673 B for each packet starting at 3.32 s
	Sender: Node 8	Receiver: Server 1
TCP burst traffic of 1517 B for each packet starting at 3.24 s
	Sender: Node 6	Receiver: Server 0

*/

/*

Wi-Fi operating in Infrastructure mode, AP is stationary, Stations nodes move with random walk mobility pattern over a bounded square of 30 meters for each side
UDP Echo application with Client 11 and Server 3
	Size of packet: 1759 Bytes	Periodicity:20ms	MaxPackets: 250

*/

int main( int argc, char* argv[]){
    
    //Parameters -> run ./ns3 run task_2012670 -- --studentId=2012670 --tracing=1
    std::string studentId;
    bool enableRtsCts = false;
    bool tracing = false;

    CommandLine cmd(__FILE__);
    cmd.AddValue("studentId", "Student Id", studentId);
    cmd.AddValue("enableRtsCts", "Enable RTS/CTS", enableRtsCts);
    cmd.AddValue("tracing", "Enable pcap tracing", tracing);
    cmd.Parse(argc, argv);

    if(enableRtsCts){
        Config::SetDefault("ns3::WifiRemoteStationManager::RtsCtsThreshold", StringValue("0"));
    } else{
        /*
        serve per disabilitare il meccanismo di RTS/CTS poiche sto impostalndo l'RtsCtsThreshold ad un numero tale che solo i pacchetti
        di dimensione maggiore a 999999 bytes potranno usare rts/cts
        */
        Config::SetDefault("ns3::WifiRemoteStationManager::RtsCtsThreshold", StringValue("999999")); 
    }

    if(studentId.empty()){
        NS_LOG_UNCOND("Settare lo studentId con l'opzione --studentId=2012670");
        return 0;
    }


    /*
    Settiamo inizialmente tutti i collegamenti PTP e csma in base all'immagine png fornitaci 
    successivamente li installeremo
    */
    PointToPointHelper p2p_10Mbps_200ms;
    p2p_10Mbps_200ms.SetDeviceAttribute("DataRate", StringValue("10Mbps"));
    p2p_10Mbps_200ms.SetChannelAttribute("Delay", TimeValue(MilliSeconds(200)));

    PointToPointHelper p2p_100Mbps_20ms;
    p2p_100Mbps_20ms.SetDeviceAttribute("DataRate", StringValue("100Mbps"));
    p2p_100Mbps_20ms.SetChannelAttribute("Delay", TimeValue(MilliSeconds(20)));

    CsmaHelper csma_5Mbps_20ms;
    csma_5Mbps_20ms.SetChannelAttribute("DataRate", StringValue("5Mbps"));
    csma_5Mbps_20ms.SetChannelAttribute("Delay", TimeValue(MilliSeconds(20)));

    CsmaHelper csma_10Mbps_200ms;
    csma_10Mbps_200ms.SetChannelAttribute("DataRate", StringValue("10Mbps"));
    csma_10Mbps_200ms.SetChannelAttribute("Delay", TimeValue(MilliSeconds(200)));

    //creation network B (p2p server)  

    NodeContainer networkB, routerB;
    networkB.Create(2);
    routerB.Create(1);
    NodeContainer allNodesB = NodeContainer(networkB, routerB);

    //NetDeviceContainer devicesB;
    NetDeviceContainer device0, device1;

    device0 = p2p_10Mbps_200ms.Install(networkB.Get(0), routerB.Get(0));
    device1 = p2p_10Mbps_200ms.Install(networkB.Get(1), routerB.Get(0));
    //devicesB.Add(device0);
    //devicesB.Add(device1); 
    /******************************************************/    

    //declaration of the stack and the address
    InternetStackHelper stack;
    Ipv4AddressHelper address;

    /****************IP addressing network B **************/


    stack.Install(allNodesB);
    address.SetBase("192.168.1.0", "255.255.255.252");
    Ipv4InterfaceContainer interfacesB0 = address.Assign(device0);

    address.SetBase("192.168.1.4", "255.255.255.252");
    Ipv4InterfaceContainer interfacesB1 = address.Assign(device1);
    
    /******************************************************/    

    //MIGLIORARE
    /*per ora non faccio i collegamenti tra il server e i router del networkA e networkC (multihoming non trattato a lezione)*/
    //li ho provati a fare ma non funzionano  -> da rivedere 
    /********Creation of Network D server-router csma_10Mbps_200ms**************/

    NodeContainer networkD, routerD;
    networkD.Create(1);
    routerD.Create(1);

    NodeContainer allNodesD = NodeContainer(networkD, routerD);

    NetDeviceContainer devicesD;
    devicesD = csma_10Mbps_200ms.Install(allNodesD);

    /*********************************************/

    //MIGLIORARE
    /****************IP addressing network D******************/   

    //NodeContainer allNodesTuttoB = NodeContainer(allNodesD, nodoDC, nodoDA);


    stack.Install(allNodesD);

    address.SetBase("192.168.1.8", "255.255.255.252");
    //address.SetBase("192.168.2.0", "255.255.255.252");
    Ipv4InterfaceContainer interfacesD = address.Assign(devicesD);

    /*********************************************/

    /*******creation of the network C csma_5Mbps_20ms*************/

    NodeContainer networkC, routerC;

    routerC.Create(1);
    networkC.Create(3);
    NodeContainer allNodesC = NodeContainer(networkC, routerC);

    NetDeviceContainer devicesC;
    devicesC = csma_5Mbps_20ms.Install(allNodesC);

    /******************************************************/    


    /****************IP addressing network C **************/

    stack.Install(allNodesC);
    address.SetBase("192.168.1.24", "255.255.255.248");
    //address.SetBase("192.168.3.0", "255.255.255.248");
    Ipv4InterfaceContainer interfacesC = address.Assign(devicesC);

    /********************************************************/



    //Creation of Network A (wi-fi infrastructure)

    NodeContainer networkA, APNode, wifiNodes;

    APNode.Create(1);
    wifiNodes.Create(7);
    networkA.Add(APNode);
    networkA.Add(wifiNodes);

    YansWifiChannelHelper channel = YansWifiChannelHelper::Default();
    YansWifiPhyHelper phy;
    phy.SetChannel(channel.Create());

    WifiMacHelper mac;
    Ssid ssid = Ssid("2012670");

    WifiHelper wifi; 
    wifi.SetStandard(WIFI_STANDARD_80211g); 
    wifi.SetRemoteStationManager("ns3::AarfWifiManager");

    NetDeviceContainer devicesA;

    mac.SetType("ns3::StaWifiMac", "Ssid", SsidValue(ssid), "ActiveProbing", BooleanValue(false));
    NetDeviceContainer staDevices;
    staDevices = wifi.Install(phy, mac, wifiNodes);

    mac.SetType("ns3::ApWifiMac", "Ssid", SsidValue(ssid));
    NetDeviceContainer apDevices;
    apDevices = wifi.Install(phy, mac, APNode);

    devicesA.Add(staDevices);
    devicesA.Add(apDevices);

    /*********************************************/

    /****************IP addressing network A******************/    

    stack.Install(networkA);
    address.SetBase("192.168.1.32", "255.255.255.240");
    //address.SetBase("192.168.4.0", "255.255.255.240");
    Ipv4InterfaceContainer interfacesA = address.Assign(devicesA);

    /******************************************************/    


    /***********COLLEGAMENTI******************/
    
    /*****Collegamento P2P tra il networkD,l'APNode e il routerC******/

    /*
    NetDeviceContainer devicesD0, devicesD1;
    devicesD0 = p2p_100Mbps_20ms.Install(APNode.Get(0), networkD.Get(0));
    devicesD1 = p2p_100Mbps_20ms.Install(routerC.Get(0), networkD.Get(0));

    address.SetBase("192.168.2.4", "255.255.255.252");
    Ipv4InterfaceContainer interfacesD0 = address.Assign(devicesD0);

    address.SetBase("192.168.2.8", "255.255.255.252");
    Ipv4InterfaceContainer interfacesD1 = address.Assign(devicesD1);
    */

    /*********************************************/

   

    /*****Create links between routers****/
    
    //L1 collega il routerB -> routerD (ancora da definire routerD)
    NodeContainer L1;
    L1.Add(routerB.Get(0));
    L1.Add(routerD.Get(0));

    NetDeviceContainer devicesL1 = p2p_100Mbps_20ms.Install(L1);

    stack.Install(L1);
    address.SetBase("192.168.1.48", "255.255.255.252");
    //address.SetBase("192.168.5.0", "255.255.255.252");
    Ipv4InterfaceContainer interfacesL1 = address.Assign(devicesL1);

    //L2 collega il routerA -> routerD
    NodeContainer L2;
    L2.Add(APNode.Get(0));
    L2.Add(routerD.Get(0));

    NetDeviceContainer devicesL2 = p2p_100Mbps_20ms.Install(L2);

    stack.Install(L2);
    address.SetBase("192.168.1.52", "255.255.255.252");
    //address.SetBase("192.168.6.0", "255.255.255.252");
    Ipv4InterfaceContainer interfacesL2 = address.Assign(devicesL2);

    //L3 collega il routerC -> routerD
    NodeContainer L3;
    L3.Add(routerC.Get(0));
    L3.Add(routerD.Get(0));

    NetDeviceContainer devicesL3 = p2p_100Mbps_20ms.Install(L3);
    stack.Install(L3);
    address.SetBase("192.168.1.56", "255.255.255.252");
    //address.SetBase("192.168.7.0", "255.255.255.252");
    Ipv4InterfaceContainer interfacesL3 = address.Assign(devicesL3);

    /*********************************************/

    /******************Mobility*********************/


    //per nodi stazionario
    MobilityHelper mobilityAP;
    mobilityAP.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    mobilityAP.Install(APNode);
    mobilityAP.Install(networkB);
    mobilityAP.Install(routerB);
    mobilityAP.Install(networkC);
    mobilityAP.Install(routerC);
    mobilityAP.Install(networkD);
    mobilityAP.Install(routerD);
    mobilityAP.Install(L1);
    mobilityAP.Install(L2);
    mobilityAP.Install(L3);

    //per i device wifi --- random walk mobility pattern over a bounded square of 30 meters for each side
   
    MobilityHelper mobilityStations;
    mobilityStations.SetPositionAllocator("ns3::RandomRectanglePositionAllocator",
                                          "X", StringValue("ns3::UniformRandomVariable[Min=0.0|Max=30.0]"),
                                          "Y", StringValue("ns3::UniformRandomVariable[Min=0.0|Max=30.0]"));

    mobilityStations.SetMobilityModel("ns3::RandomWalk2dMobilityModel",
                                      "Bounds", RectangleValue(Rectangle(0,30,0,30)));
    mobilityStations.Install(wifiNodes);
    
    /*********************************************/


    /**************Applications****************/

    /*
    TCP burst traffic of 1617 B for each packet starting at 0.49 s
    	Sender: Node 10	Receiver: Server 0
    TCP burst traffic of 1673 B for each packet starting at 3.32 s
    	Sender: Node 8	Receiver: Server 1
    TCP burst traffic of 1517 B for each packet starting at 3.24 s
    	Sender: Node 6	Receiver: Server 0
    */

    //burst traffic of 1617 B for each packet starting at 0.49 s Sender: Node 10 Receiver: Server 0
    uint16_t port1 = 1025;
    uint32_t payloadSize1 = 1617; // bytes
    double startTime1 = 0.49; // seconds
    
    //receiver
    PacketSinkHelper sink1("ns3::TcpSocketFactory", InetSocketAddress(Ipv4Address::GetAny(), port1));
    ApplicationContainer sinkApps1 = sink1.Install(allNodesB.Get(0));
    sinkApps1.Start(Seconds(0.0));

    //sender
    OnOffHelper source1("ns3::TcpSocketFactory", InetSocketAddress(interfacesB0.GetAddress(0), port1));
    source1.SetAttribute("OnTime", StringValue("ns3::ConstantRandomVariable[Constant=1]")); 
    source1.SetAttribute("OffTime", StringValue("ns3::ConstantRandomVariable[Constant=1]"));
    source1.SetAttribute("PacketSize", UintegerValue(payloadSize1)); 
    ApplicationContainer sourceApps1 = source1.Install(networkA.Get(1));
    sourceApps1.Start(Seconds(startTime1));
    

    //burst traffic of 1673 B for each packet starting at 3.32 s Sender: Node 8 Receiver: Server 1
    uint16_t port2 = 1026;
    uint32_t payloadSize2 = 1673; // bytes
    double startTime2 = 3.32; // seconds


    //receiver
    PacketSinkHelper sink2("ns3::TcpSocketFactory", InetSocketAddress(Ipv4Address::GetAny(), port2));
    ApplicationContainer sinkApps2 = sink2.Install(allNodesB.Get(1));
    sinkApps2.Start(Seconds(0.0));

    //sender 
    OnOffHelper source2("ns3::TcpSocketFactory", InetSocketAddress(interfacesB1.GetAddress(0), port2));
    source2.SetAttribute("OnTime", StringValue("ns3::ConstantRandomVariable[Constant=1]"));
    source2.SetAttribute("OffTime", StringValue("ns3::ConstantRandomVariable[Constant=1]"));
    source2.SetAttribute("PacketSize", UintegerValue(payloadSize2));
    //ApplicationContainer sourceApps2 = source2.Install(allNodesC.Get(3));
    ApplicationContainer sourceApps2 = source2.Install(networkC.Get(2));
    sourceApps2.Start(Seconds(startTime2));


    //burst traffic of 1517 B for each packet starting at 3.24 s Sender: Node 6 Receiver: Server 0
    uint16_t port3 = 1027;
    uint32_t payloadSize3 = 1517; // bytes
    double startTime3 = 3.24; // seconds

    
    //receiver
    PacketSinkHelper sink3("ns3::TcpSocketFactory", InetSocketAddress(Ipv4Address::GetAny(), port3));
    ApplicationContainer sinkApps3 = sink3.Install(allNodesB.Get(0));
    sinkApps3.Start(Seconds(0.0));

    //sender
    OnOffHelper source3("ns3::TcpSocketFactory", InetSocketAddress(interfacesB0.GetAddress(0), port3));
    source3.SetAttribute("OnTime", StringValue("ns3::ConstantRandomVariable[Constant=1]"));
    source3.SetAttribute("OffTime", StringValue("ns3::ConstantRandomVariable[Constant=1]"));
    source3.SetAttribute("PacketSize", UintegerValue(payloadSize3));
    ApplicationContainer sourceApps3 = source3.Install(networkC.Get(0));
    sourceApps3.Start(Seconds(startTime3));
    
    /***************************************************/
    
    /* UDP Echo application with Client 11 and Server 3*/
    /* Size of packet: 1759 Bytes	Periodicity:20ms	MaxPackets: 250*/
    uint16_t port4 = 1028;
    UdpEchoServerHelper echoServer(port4);
    ApplicationContainer serverApps = echoServer.Install(allNodesD.Get(0));
    serverApps.Start(Seconds(0.0));

    UdpEchoClientHelper echoClient(interfacesD.GetAddress(0), port4);
    echoClient.SetAttribute("MaxPackets", UintegerValue(250));
    echoClient.SetAttribute("Interval", TimeValue(MilliSeconds(20)));
    echoClient.SetAttribute("PacketSize", UintegerValue(1759));
    
    ApplicationContainer clientApps = echoClient.Install(networkA.Get(2)); 
    clientApps.Start(Seconds(0.0));
    
    Ptr<UdpEchoClient> Echoclient = DynamicCast<UdpEchoClient>(clientApps.Get(0));
    Echoclient->SetFill("Julian,Panait,Luca,Tam,Francesco,Macri,Giulia,Pelorossi");

    /**************************************/

    //debugging
    /*
    //what's inside allNodesD?
    
    std::cout << "allNodesD\n"<< std::endl;
    for (uint32_t i = 0; i < allNodesD.GetN(); ++i){
        Ptr<Node> node = allNodesD.Get(i);
        std::cout << "Node " << node->GetId() << " is in allNodesD" << std::endl;
    }

    //what's inside interfaceD?
    std::cout << "interfaceD\n"<< std::endl;
    for (uint32_t i = 0; i < interfacesD.GetN(); ++i){
        Ipv4Address address = interfacesD.GetAddress(i);
        std::cout << "Interface " << i << " has address " << address << std::endl;
    }

    //what's inside networkA?
    std::cout << "networkA\n"<< std::endl;
    for (uint32_t i = 0; i < networkA.GetN(); ++i){
        Ptr<Node> node = networkA.Get(i);
        std::cout << "Node " << node->GetId() << " is in networkA" << std::endl;
    }
    */

    //Routing Table help debugging and understanding the network topology 
    Ipv4GlobalRoutingHelper::PopulateRoutingTables();
    Ipv4GlobalRoutingHelper g;
    Ptr<OutputStreamWrapper> routingStream = Create<OutputStreamWrapper>("routing-table.log", std::ios::out);
    g.PrintRoutingTableAllAt(Seconds(2), routingStream);

    
    /**********pcap-files******************/

    if(tracing){   
        if (enableRtsCts){
            phy.SetPcapDataLinkType(WifiPhyHelper::DLT_IEEE802_11_RADIO);
            p2p_100Mbps_20ms.EnablePcapAll("task_RtsCts");
            p2p_10Mbps_200ms.EnablePcapAll("task_RtsCts");
            for (uint32_t i = 0; i < staDevices.GetN(); ++i){
                phy.EnablePcap("task_RtsCts", staDevices.Get(i));
            }
            phy.EnablePcap("task_RtsCts", apDevices.Get(0));
            csma_5Mbps_20ms.EnablePcap("task_RtsCts", devicesC.Get(0), true);
            csma_5Mbps_20ms.EnablePcap("task_RtsCts", devicesC.Get(1), true);    
            csma_5Mbps_20ms.EnablePcap("task_RtsCts", devicesC.Get(2), true);  
            csma_5Mbps_20ms.EnablePcap("task_RtsCts", devicesC.Get(3), true);  
            csma_10Mbps_200ms.EnablePcap("task_RtsCts", devicesD.Get(0), true); 
            csma_10Mbps_200ms.EnablePcap("task_RtsCts", devicesD.Get(1), true); 
        }else{
            phy.SetPcapDataLinkType(WifiPhyHelper::DLT_IEEE802_11_RADIO);
            p2p_100Mbps_20ms.EnablePcapAll("task");
            p2p_10Mbps_200ms.EnablePcapAll("task");
            for (uint32_t i = 0; i < staDevices.GetN(); ++i){
                phy.EnablePcap("task", staDevices.Get(i));
            }
            phy.EnablePcap("task", apDevices.Get(0));
            csma_5Mbps_20ms.EnablePcap("task", devicesC.Get(0), true);
            csma_5Mbps_20ms.EnablePcap("task", devicesC.Get(1), true);    
            csma_5Mbps_20ms.EnablePcap("task", devicesC.Get(2), true);  
            csma_5Mbps_20ms.EnablePcap("task", devicesC.Get(3), true);  
            csma_10Mbps_200ms.EnablePcap("task", devicesD.Get(0), true); 
            csma_10Mbps_200ms.EnablePcap("task", devicesD.Get(1), true); 
        }
    } 

    /****************************************/



    //start simulation
    Simulator::Stop(Seconds(15.0));
    Simulator::Run();
    Simulator::Destroy();

}
   
