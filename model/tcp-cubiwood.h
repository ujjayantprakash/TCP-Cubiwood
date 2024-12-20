/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2014 Natale Patriciello <natale.patriciello@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#ifndef TCPCUBIWOOD_H
#define TCPCUBIWOOD_H

#include "ns3/tcp-congestion-ops.h"
#include "ns3/tcp-socket-base.h"


namespace ns3 {

/**
 * \brief The Cubiwood Congestion Control Algorithm
 *
 * TCP Cubiwood is a protocol that enhances the fairness property
 * of Bic while retaining its scalability and stability. The main feature is
 * that the window growth function is defined in real time in order to be independent
 * from the RTT. More specifically, the congestion window of Cubiwood is determined
 * by a function of the elapsed time from the last window reduction.
 *
 * The Cubiwood code is quite similar to that of Bic.
 * The main difference is located in the method Update, an edit
 * necessary for satisfying the Cubiwood window growth, that can be tuned with
 * the attribute C (the Cubiwood scaling factor).
 *
 * Following the Linux implementation, we included the Hybrid Slow Start,
 * that effectively prevents the overshooting of slow start
 * while maintaining a full utilization of the network. This new type of slow
 * start can be disabled through the \Attribute{HyStart} attribute.
 *
 * CUBIC TCP is implemented and used by default in Linux kernels 2.6.19
 * and above; this version follows the implementation in Linux 3.14, which
 * slightly differ from the CUBIC paper. It also features HyStart.
 *
 * Home page:
 *      http://netsrv.csc.ncsu.edu/twiki/bin/view/Main/BIC
 * The work starts from the implementation of CUBIC TCP in
 * Sangtae Ha, Injong Rhee and Lisong Xu,
 * "CUBIC: A New TCP-Friendly High-Speed TCP Variant"
 * in ACM SIGOPS Operating System Review, July 2008.
 * Available from:
 *  http://netsrv.csc.ncsu.edu/export/cubic_a_new_tcp_2008.pdf
 *
 * CUBIC integrates a new slow start algorithm, called HyStart.
 * The details of HyStart are presented in
 *  Sangtae Ha and Injong Rhee,
 *  "Taming the Elephants: New TCP Slow Start", NCSU TechReport 2008.
 * Available from:
 *  http://netsrv.csc.ncsu.edu/export/hystart_techreport_2008.pdf
 *
 * More information on this implementation: http://dl.acm.org/citation.cfm?id=2756518
 */
class TcpCubiwood : public TcpCongestionOps
{
public:
  /**
   * \brief Get the type ID.
   * \return the object TypeId
   */
  static TypeId GetTypeId (void);

  TcpCubiwood ();
  TcpCubiwood (const TcpCubiwood& sock);

  virtual std::string GetName () const;
  virtual void PktsAcked (Ptr<TcpSocketState> tcb, uint32_t segmentsAcked, const Time& rtt);
  virtual void IncreaseWindow (Ptr<TcpSocketState> tcb, uint32_t segmentsAcked);
  virtual uint32_t GetSsThresh (Ptr<const TcpSocketState> tcb, uint32_t bytesInFlight);
  virtual void CongestionStateSet (Ptr<TcpSocketState> tcb,
                                   const TcpSocketState::TcpCongState_t newState);

  virtual Ptr<TcpCongestionOps> Fork ();

private:
  /**
   * \brief Values to detect the Slow Start mode of HyStart
   */
  enum HybridSSDetectionMode
  {
    PACKET_TRAIN = 0x1, //!< Detection by trains of packet
    DELAY        = 0x2  //!< Detection by delay value
  };

   /**
   * \brief Protocol variant (Westwood or Westwood+)
   */
  enum ProtocolType 
  {
    WESTWOOD,
    WESTWOODPLUS
  };

  /**
   * \brief Filter type (None or Tustin)
   */
  enum FilterType 
  {
    NONE,
    TUSTIN
  };

  bool     m_fastConvergence;  //!< Enable or disable fast convergence algorithm
  double   m_beta;             //!< Beta for cubic multiplicative increase
  double   alpha;              //!< Alpha value for TUSTIN approximation

  bool     m_hystart;          //!< Enable or disable HyStart algorithm
  int      m_hystartDetect;    //!< Detect way for HyStart algorithm \see HybridSSDetectionMode
  uint32_t m_hystartLowWindow; //!< Lower bound cWnd for hybrid slow start (segments)
  Time     m_hystartAckDelta;  //!< Spacing between ack's indicating train
  Time     m_hystartDelayMin;  //!< Minimum time for hystart algorithm
  Time     m_hystartDelayMax;  //!< Maximum time for hystart algorithm
  uint8_t  m_hystartMinSamples; //!< Number of delay samples for detecting the increase of delay

  uint32_t m_initialCwnd;      //!< Initial cWnd
  uint8_t  m_cntClamp;         //!< Modulo of the (avoided) float division for cWnd

  double   m_c;                //!< Cubiwood Scaling factor

  // Cubiwood parameters
  uint32_t     m_cWndCnt;         //!<  cWnd integer-to-float counter
  uint32_t     m_lastMaxCwnd;     //!<  Last maximum cWnd
  uint32_t     m_bicOriginPoint;  //!<  Origin point of bic function
  double       m_bicK;            //!<  Time to origin point from the beginning
                                  //    of the current epoch (in s)
  Time         m_delayMin;        //!<  Min delay
  Time         m_epochStart;      //!<  Beginning of an epoch
  bool         m_found;           //!<  The exit point is found?
  Time         m_roundStart;      //!<  Beginning of each round
  SequenceNumber32   m_endSeq;    //!<  End sequence of the round
  Time         m_lastAck;         //!<  Last time when the ACK spacing is close
  Time         m_cubicDelta;      //!<  Time to wait after recovery before update
  Time         m_currRtt;         //!<  Current Rtt
  uint32_t     m_sampleCnt;       //!<  Count of samples for HyStart
  TracedValue<double>    m_currentBW;              //!< Current value of the estimated BW
  double                 m_lastSampleBW;           //!< Last bandwidth sample
  double                 m_lastBW;                 //!< Last bandwidth sample after being filtered
  enum ProtocolType      m_pType;                  //!< 0 for Westwood, 1 for Westwood+
  enum FilterType        m_fType;                  //!< 0 for none, 1 for Tustin

  uint32_t               m_ackedSegments;          //!< The number of segments ACKed between RTTs
  bool                   m_IsCount;                //!< Start keeping track of m_ackedSegments for Westwood+ if TRUE
  EventId                m_bwEstimateEvent;        //!< The BW estimation event for Westwood+

private:
  /**
   * \brief Reset HyStart parameters
   */
  void HystartReset (Ptr<const TcpSocketState> tcb);

  void CubiwoodReset (Ptr<const TcpSocketState> tcb);

  /**
   * \brief Cubiwood window update after a new ack received
   */
  uint32_t Update (Ptr<TcpSocketState> tcb);

  /**
   * \brief Update HyStart parameters
   *
   * \param tcb Transmission Control Block of the connection
   * \param delay Delay for HyStart algorithm
   */
  void HystartUpdate (Ptr<TcpSocketState> tcb, const Time &delay);
  
  void EstimateBW (const Time& rtt, Ptr<TcpSocketState> tcb);

  /**
   * \brief Clamp time value in a range
   *
   * The returned value is t, clamped in a range specified
   * by attributes (HystartDelayMin < t < HystartDelayMax)
   *
   * \param t Time value to clamp
   * \return t itself if it is in range, otherwise the min or max
   * value
   */
  Time HystartDelayThresh (const Time &t) const;
};

} // namespace ns3

#endif // TCPCUBIWOOD_H
