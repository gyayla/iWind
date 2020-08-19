/************************************************************/
/* orig: NAME: Mohamed Saad IBN SEDDIK                      */
/* changes: Gokay Yayla, KU Leuven                          */
/* ORGN: ENSTA Bretagne                                     */
/*    FILE: WIND.cpp                                        */
/*    DATE: 2020-08-13                                      */
/************************************************************/

#include <iterator>
#include "MBUtils.h"
#include "WIND.h"

using namespace std;

//---------------------------------------------------------
// Constructor

WIND::WIND()
{
  std::cout << "Wind Sensor construction" << std::endl;
  m_iterations = 0;
  m_timewarp   = 1;
  m_prefix = "WIND";
}

//---------------------------------------------------------
// Destructor

WIND::~WIND()
{
}

//---------------------------------------------------------
// Procedure: OnNewMail

bool WIND::OnNewMail(MOOSMSG_LIST &NewMail)
{
   return UpdateMOOSVariables(NewMail);
}

//---------------------------------------------------------
// Procedure: OnConnectToServer

bool WIND::OnConnectToServer()
{
   RegisterVariables();
   return(true);
}

//---------------------------------------------------------
// Procedure: Iterate()
//            happens AppTick times per second

bool WIND::Iterate()
{
  m_iterations++;
  std::cout << "new iteration" << std::endl;

  if(GetData()){
    std::cout << "Receiving data from WindSense" << std::endl;
    PublishData();
  }
  else{
    std::cout << "No data from WindSense" << std::endl;
  }
  return(true);
}

//---------------------------------------------------------
// Procedure: OnStartUp()
//            happens before connection is open

bool WIND::OnStartUp()
{
  CMOOSInstrument::OnStartUp();

  int max_retries = 5;
  double dWINDPeriod = 1.0;

  list<string> sParams;
  m_MissionReader.EnableVerbatimQuoting(false);
  if(m_MissionReader.GetConfiguration(GetAppName(), sParams)) {
    list<string>::iterator p;
    for(p=sParams.begin(); p!=sParams.end(); p++) {
      string original_line = *p;
      string param = stripBlankEnds(toupper(biteString(*p, '=')));
      string value = stripBlankEnds(*p);

      if(param == "WIND_PERIOD") {
        dWINDPeriod = atof(value.c_str());
      }
      else if(param == "MAX_RETRIES") {
        max_retries = atoi(value.c_str());
      }
      else if((param == "PREFIX") && !strContainsWhite(value)) {
      m_prefix = value;
      }
    }
  }

  m_timewarp = GetMOOSTimeWarp();

  AddMOOSVariable("angle", "", "WIND_REL_DIRECTION", dWINDPeriod);
  AddMOOSVariable("speed", "", "WIND_SPEED", dWINDPeriod);
  AddMOOSVariable("raw", "", "WIND_RAW", dWINDPeriod);

  RegisterMOOSVariables();
  RegisterVariables();

  if (!SetupPort())
  {
    return false;
  }
  if (!(InitialiseSensorN(max_retries,"WIND")))
  {
    return false;
  }
  return(true);
}

//---------------------------------------------------------
// Procedure: RegisterVariables

void WIND::RegisterVariables()
{
  // m_Comms.Register("FOOBAR", 0);
}

bool WIND::InitialiseSensor()
{
  MOOSPause(1000);
  if (m_Port.Flush()==-1)
  {
    return false;
  } else return true;
}

bool WIND::GetData()
{
  string sMessage;
  double dTime;

  if(m_Port.IsStreaming())
  {
    if (!m_Port.GetLatest(sMessage,dTime))
    {
      return false;
    }
  } else {
    if (!m_Port.GetTelegram(sMessage,0.5))
    {
      return false;
    }
  }
  if (PublishRaw())
  {
    SetMOOSVar("Raw",sMessage,MOOSTime());
  }

  return ParseNMEAString(sMessage);
}

bool WIND::PublishData()
{
  return PublishFreshMOOSVariables();
}

bool WIND::ParseNMEAString(string &sNMEAString)
{
/*
  if(!DoNMEACheckSum(sNMEAString))
  {
    MOOSDebugWrite("NMEA checksum failed!");
    return false;
  }
*/
  string sCopy = sNMEAString;
  string sHeader = MOOSChomp(sNMEAString,",");
  bool bQuality = true;

  if (sHeader == "$WIMWV")
  {
    double dTimeMOOS = MOOSTime();
    string sTmp = MOOSChomp(sNMEAString,","); // Direction
    double dDirection = atof(sTmp.c_str());
    sTmp = MOOSChomp(sNMEAString, ","); // Reference R:Relative T:Actual
    sTmp = MOOSChomp(sNMEAString, ","); // Speed
    double dSpeed = atof(sTmp.c_str());
    sTmp = MOOSChomp(sNMEAString, ","); // Speed Units N:Knots
    sTmp = MOOSChomp(sNMEAString, ","); // Validity
    if (sTmp == "V") //if data is invalid
      bQuality = false;

    if (bQuality)
    {
      dSpeed = Knots2MPS(dSpeed); //meter per second
      SetMOOSVar(m_prefix+"SPEED",dSpeed,dTimeMOOS);
      SetMOOSVar(m_prefix+"DIRECTION",dDirection,dTimeMOOS);
      Notify("WIND_DIRECTION", dDirection);
      Notify("WIND_SPEED", dSpeed);
    }

  }
  return true;
}

double WIND::Knots2MPS(double speed)
{
  return speed*KNOTS2MPS;
}
