// Dear emacs, this is -*- c++ -*-
// $Id: ZprimePostSelectionCycle.h,v 1.9 2013/04/30 07:52:00 peiffer Exp $
#ifndef ZprimePostSelectionCycle_H
#define ZprimePostSelectionCycle_H

#include <iostream>

// SFrame include(s):
#include "include/AnalysisCycle.h"
#include "include/SelectionModules.h"
#include "include/HypothesisHists.h"

#include "EventHists.h"
#include "JetHists.h"
#include "ElectronHists.h"
#include "MuonHists.h"
#include "TauHists.h"
#include "TopJetHists.h"
#include "BTagEffHists.h"

/**
 *   @short Example of an analysis cycle
 *
 *          This is an example of an analysis cycle. It can be used
 *          as a template for writing your own analysis. Also should
 *          be used for quick cross checks of the system setup.
 *
 *  @author Roman Kogler
 *  @version $Revision: 1.9 $
 */

class ZprimePostSelectionCycle : public AnalysisCycle {

public:
  /// Default constructor
  ZprimePostSelectionCycle();
  /// Default destructor
  ~ZprimePostSelectionCycle();

  /// Function called at the beginning of the cycle
  void BeginCycle() throw( SError );
  /// Function called at the end of the cycle
  void EndCycle() throw( SError );

  /// Function called at the beginning of a new input data
  void BeginInputData( const SInputData& ) throw( SError );
  /// Function called after finishing to process an input data
  void EndInputData  ( const SInputData& ) throw( SError );

  /// Function called after opening each new input file
  void BeginInputFile( const SInputData& ) throw( SError );

  /// Function called for every event
  void ExecuteEvent( const SInputData&, Double_t ) throw( SError );

  /// Fill control histograms
  void FillControlHistos(TString postfix="");

  /// Scale control histograms
  void ScaleHistos(TString postfix="", double scale=1.0);

private:
  //
  // Put all your private variables here
  //

  std::string m_Electron_Or_Muon_Selection;

  std::string m_dobsf;
  std::string m_dotsf;
  E_BtagType m_btagtype;
  E_BtagType x_btagtype;
  BTaggingScaleFactors* m_bsf;
  TopTaggingScaleFactors* m_tsf;

  bool m_mttgencut;

  bool m_writeeventlist;
  ofstream m_eventlist;

  std::string m_flavor_selection;
  std::string m_filter_file;

  bool m_applyEleORJetTriggerSF;
  bool m_correctTopPtWeights;

  // Macro adding the functions for dictionary generation
  ClassDef( ZprimePostSelectionCycle, 0 );
}; // class ZprimePostSelectionCycle

#endif // ZprimePostSelectionCycle_H

