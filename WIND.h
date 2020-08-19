/************************************************************/
/*    NAME: Mohamed Saad IBN SEDDIK                         */
/*    ORGN: ENSTA Bretagne
      Modified: Gokay Yayla                                  */
/*    FILE: WIND.h                                           */
/*    DATE: 2015-12-17     2020-08-13                        */
/************************************************************/

#ifndef WIND_HEADER
#define WIND_HEADER

#include "MOOS/libMOOS/MOOSLib.h"
#include "MOOS/libMOOSGeodesy/MOOSGeodesy.h"

#define KNOTS2MPS 0.5144444444

class WIND : public CMOOSInstrument
{
 public:
   WIND();
   ~WIND();

 protected:
   bool OnNewMail(MOOSMSG_LIST &NewMail);
   bool Iterate();
   bool OnConnectToServer();
   bool OnStartUp();
   void RegisterVariables();

   bool InitialiseSensor();

   bool GetData();
   bool PublishData();
   bool ParseNMEAString(std::string &s);

   double Knots2MPS(double s);

 private: // Configuration variables

 private: // State variables
   unsigned int m_iterations;
   double       m_timewarp;
   std::string  m_prefix;

   CMOOSGeodesy m_Geodesy;
};

#endif
