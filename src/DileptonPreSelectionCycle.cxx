#include <iostream>

using namespace std;

// Local include(s):
#include "include/DileptonPreSelectionCycle.h"
#include "include/SelectionModules.h"
#include "JetCorrectorParameters.h"

ClassImp( DileptonPreSelectionCycle );

DileptonPreSelectionCycle::DileptonPreSelectionCycle()
    : AnalysisCycle()
{

    // constructor, declare additional variables that should be
    // obtained from the steering-xml file

    // set the integrated luminosity per bin for the lumi-yield control plots
    SetIntLumiPerBin(25.);

    DeclareProperty( "Electron_Or_Muon_Selection", m_Electron_Or_Muon_Selection );

    // steering property for data-driven qcd in electron channel
    m_reversed_electron_selection = false;
    DeclareProperty( "ReversedElectronSelection", m_reversed_electron_selection);
}

DileptonPreSelectionCycle::~DileptonPreSelectionCycle()
{
    // destructor

}

void DileptonPreSelectionCycle::BeginCycle() throw( SError )
{
    // Start of the job, general set-up and definition of
    // objects are done here

    // Important: first call BeginCycle of base class
    AnalysisCycle::BeginCycle();

    return;

}

void DileptonPreSelectionCycle::EndCycle() throw( SError )
{
    // clean-up, info messages and final calculations after the analysis


    // call the base cycle class for all standard methods
    // and a summary of the made selections
    AnalysisCycle::EndCycle();

    return;

}

void DileptonPreSelectionCycle::BeginInputData( const SInputData& id ) throw( SError )
{
    // declaration of histograms and selections

    // Important: first call BeginInputData of base class
    AnalysisCycle::BeginInputData( id );

    // -------------------- set up the selections ---------------------------

    Selection* preselection = new Selection("preselection");

    if(m_Electron_Or_Muon_Selection=="Electrons" || m_Electron_Or_Muon_Selection=="Electron" || m_Electron_Or_Muon_Selection=="Ele" || m_Electron_Or_Muon_Selection=="ELE") {
        preselection->addSelectionModule(new NElectronSelection(2,int_infinity()));//at least two electrons
        preselection->addSelectionModule(new NMuonSelection(0,0));//no muons
    } else if(m_Electron_Or_Muon_Selection=="Muon" || m_Electron_Or_Muon_Selection=="Muons" || m_Electron_Or_Muon_Selection=="Mu" || m_Electron_Or_Muon_Selection=="MU") {
        preselection->addSelectionModule(new NElectronSelection(0,0));//no electron
        preselection->addSelectionModule(new NMuonSelection(2,int_infinity()));//at least two muons
    } else {
        m_logger << ERROR << "Electron_Or_Muon_Selection is not defined in your xml config file --- should be either `ELE` or `MU`" << SLogger::endmsg;
    }

    //preselection->addSelectionModule(new NJetSelection(2));//at least two jets

    RegisterSelection(preselection);

    return;
}

void DileptonPreSelectionCycle::EndInputData( const SInputData& id ) throw( SError )
{
    AnalysisCycle::EndInputData( id );

    return;
}

void DileptonPreSelectionCycle::BeginInputFile( const SInputData& id ) throw( SError )
{
    // Connect all variables from the Ntuple file with the ones needed for the analysis
    // The variables are commonly stored in the BaseCycleContaincer

    // important: call to base function to connect all variables to Ntuples from the input tree
    AnalysisCycle::BeginInputFile( id );

    return;
}

void DileptonPreSelectionCycle::ExecuteEvent( const SInputData& id, Double_t weight) throw( SError )
{
    // this is the most important part: here the full analysis happens
    // user should implement selections, filling of histograms and results

    // first step: call Execute event of base class to perform basic consistency checks
    // also, the good-run selection is performed there and the calculator is reset
    AnalysisCycle::ExecuteEvent( id, weight);

    Cleaner cleaner;
    static Selection* preselection = GetSelection("preselection");

    EventCalc* calc = EventCalc::Instance();
    BaseCycleContainer* bcc = calc->GetBaseCycleContainer();

    //save uncleaned jet collection and MET to be stored in output
    std::vector<Jet> uncleaned_jets;
    for(unsigned int i=0; i<bcc->jets->size(); ++i) {
        uncleaned_jets.push_back(bcc->jets->at(i));
    }
    MET uncleaned_met = *bcc->met;

    //clean collections here

    if(bcc->muons) cleaner.MuonCleaner_noIso(45,2.1);
    if(bcc->electrons) cleaner.ElectronCleaner_noIso(35,2.5,m_reversed_electron_selection);
    if(bcc->jets) cleaner.JetLeptonSubtractor(m_corrector,false);
    if(!bcc->isRealData && bcc->jets) cleaner.JetEnergyResolutionShifter();
    if(bcc->jets) cleaner.JetCleaner(30,2.5,true);

    //get the jet selections

    if(!preselection->passSelection())  throw SError( SError::SkipEvent );

    //fill the uncleaned collections back to bcc to store them in output tree
    bcc->met->set_pt (uncleaned_met.pt());
    bcc->met->set_phi (uncleaned_met.phi());
    bcc->jets->clear();
    for(unsigned int i=0; i<uncleaned_jets.size(); ++i) {
        bcc->jets->push_back(uncleaned_jets.at(i));
    }

    WriteOutputTree();

    return;
}

