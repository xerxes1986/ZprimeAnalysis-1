// Dear emacs, this is -*- c++ -*-
#ifndef MistagSelectionCycle_H
#define MistagSelectionCycle_H

// SFrame include(s):
#include "include/AnalysisCycle.h"
#include "Cleaner.h"
#include "HypothesisDiscriminator.h"
#include "HypothesisHists.h"
#include "include/SelectionModules.h"
#include "HypothesisStatistics.h"

#include "EventHists.h"
#include "JetHists.h"
#include "ElectronHists.h"
#include "MuonHists.h"
#include "TauHists.h"
#include "TopJetHists.h"

/**
 *  @short Selection cycle to perform
 *         full selection for Z'->ttbar analysis
 *  @author Thomas Peiffer
 */

class MistagSelectionCycle : public AnalysisCycle {

public:
  /// Default constructor
  MistagSelectionCycle();
  /// Default destructor
  ~MistagSelectionCycle();

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
  void FillControlHists(TString postfix="");

private:
  //
  // Put all your private variables here
  //

  std::string m_Electron_Or_Muon_Selection;

  // Flg use to reverse electron selection
  bool m_reversed_electron_selection;
  bool m_mttgencut;
  int m_Nbtags_max;
  int m_Nbtags_min;

  std::string m_flavor_selection;

  Cleaner* m_cleaner;

  // Macro adding the functions for dictionary generation
  ClassDef( MistagSelectionCycle, 0 );

}; // class MistagSelectionCycle

#endif // MistagSelectionCycle_H

