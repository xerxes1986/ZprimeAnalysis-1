#include <iostream>

using namespace std;

// Local include(s):
#include "include/MistagSelectionCycle.h"
#include "include/TopFitCalc.h"
#include "include/NeutrinoHists.h"

ClassImp( MistagSelectionCycle );

MistagSelectionCycle::MistagSelectionCycle()
    : AnalysisCycle()
{

    // constructor, declare additional variables that should be
    // obtained from the steering-xml file

    // set the integrated luminosity per bin for the lumi-yield control plots
    SetIntLumiPerBin(500.);

    m_sys_var = e_Default;
    m_sys_unc = e_None;
    m_mttgencut = false;
    DeclareProperty( "ApplyMttbarGenCut", m_mttgencut );
    DeclareProperty( "Electron_Or_Muon_Selection", m_Electron_Or_Muon_Selection );

    //default: no btagging cuts applied, other cuts can be defined in config file
    m_Nbtags_min=0;
    m_Nbtags_max=int_infinity();
    DeclareProperty( "Nbtags_min", m_Nbtags_min);
    DeclareProperty( "Nbtags_max", m_Nbtags_max);

}

MistagSelectionCycle::~MistagSelectionCycle()
{
    // destructor
}

void MistagSelectionCycle::BeginCycle() throw( SError )
{
    // Start of the job, general set-up and definition of
    // objects are done here

    // Important: first call BeginCycle of base class
    AnalysisCycle::BeginCycle();

    return;
}

void MistagSelectionCycle::EndCycle() throw( SError )
{
    // clean-up, info messages and final calculations after the analysis


    // call the base cycle class for all standard methods
    // and a summary of the made selections
    AnalysisCycle::EndCycle();

    return;
}

void MistagSelectionCycle::BeginInputData( const SInputData& id ) throw( SError )
{
    // declaration of histograms and selections

    // Important: first call BeginInputData of base class
    AnalysisCycle::BeginInputData( id );

    // -------------------- set up the selections ---------------------------
    static Selection* mttbar_gen_selection = new Selection("Mttbar_Gen_Selection");
    if ( m_mttgencut && ((id.GetVersion() == "TTbar_0to700") || (id.GetVersion() == "TTbar") )  ) {
      m_logger << INFO << "Applying mttbar generator cut from 0 to 700 GeV." << SLogger::endmsg;
      mttbar_gen_selection->addSelectionModule(new MttbarGenCut(0,700));
      mttbar_gen_selection->EnableSelection();
    } else {
      m_logger << INFO << "Disabling mttbar generator cut." << SLogger::endmsg;
      mttbar_gen_selection->DisableSelection();
    }
    //Set-Up Selection

    bool doEle=false;
    bool doMu=false;

    Selection* trig_selection= new Selection("trig_selection");
    trig_selection->addSelectionModule(new TriggerSelection(m_lumi_trigger));

    Selection* first_selection= new Selection("first_selection");
    first_selection->addSelectionModule(new NPrimaryVertexSelection(1)); //at least one good PV
    first_selection->addSelectionModule(new NJetSelection(2,int_infinity(),50,2.4));//at least two jets

    first_selection->addSelectionModule(new NMuonSelection(1,int_infinity()));//at least one muon
    first_selection->addSelectionModule(new NMuonSelection(1,1));//exactly one muon
    first_selection->addSelectionModule(new NElectronSelection(0,0));//no ided electrons

    first_selection->addSelectionModule(new TwoDCut());

    Selection* second_selection= new Selection("second_selection");

    second_selection->addSelectionModule(new NTopJetSelection(1,1,400,2.4));// top jet candidate with pT > 400
    //second_selection->addSelectionModule(new NBTagSelection(0,0,e_CSVL)); //no b tags
    second_selection->addSelectionModule(new HTlepCut(150));
    second_selection->addSelectionModule(new METCut(20));

    Selection* NoBTagSel = new Selection("NoBTagSelection");
    NoBTagSel->addSelectionModule(new NBTagSelection(0,0,e_CSVL));

    Selection* TopTagSel = new Selection("TopTagSelection");
    TopTagSel->addSelectionModule(new NTopTagSelection(1,1));

    RegisterSelection(mttbar_gen_selection);
    RegisterSelection(trig_selection);
    RegisterSelection(first_selection);
    RegisterSelection(second_selection);
    RegisterSelection(NoBTagSel);
    RegisterSelection(TopTagSel);



    // ---------------- set up the histogram collections --------------------


    // control histogras
    RegisterHistCollection( new EventHists("Event_Presel") );
    RegisterHistCollection( new JetHists("Jets_Presel") );
    RegisterHistCollection( new ElectronHists("Electron_Presel") );
    RegisterHistCollection( new MuonHists("Muon_Presel") );
    RegisterHistCollection( new TauHists("Tau_Presel") );
    RegisterHistCollection( new TopJetHists("TopJets_Presel") );

    RegisterHistCollection( new EventHists("Event_Cleaned") );
    RegisterHistCollection( new JetHists("Jets_Cleaned") );
    RegisterHistCollection( new ElectronHists("Electron_Cleaned") );
    RegisterHistCollection( new MuonHists("Muon_Cleaned") );
    RegisterHistCollection( new TauHists("Tau_Cleaned") );
    RegisterHistCollection( new TopJetHists("TopJets_Cleaned") );

    RegisterHistCollection( new EventHists("Event_TopTagNoBTag") );
    RegisterHistCollection( new JetHists("Jets_TopTagNoBTag") );
    RegisterHistCollection( new ElectronHists("Electron_TopTagNoBTag") );
    RegisterHistCollection( new MuonHists("Muon_TopTagNoBTag") );
    RegisterHistCollection( new TauHists("Tau_TopTagNoBTag") );
    RegisterHistCollection( new TopJetHists("TopJets_TopTagNoBTag") );

    RegisterHistCollection( new EventHists("Event_TopTagBTag") );
    RegisterHistCollection( new JetHists("Jets_TopTagBTag") );
    RegisterHistCollection( new ElectronHists("Electron_TopTagBTag") );
    RegisterHistCollection( new MuonHists("Muon_TopTagBTag") );
    RegisterHistCollection( new TauHists("Tau_TopTagBTag") );
    RegisterHistCollection( new TopJetHists("TopJets_TopTagBTag") );

    RegisterHistCollection( new EventHists("Event_TopTag") );
    RegisterHistCollection( new JetHists("Jets_TopTag") );
    RegisterHistCollection( new ElectronHists("Electron_TopTag") );
    RegisterHistCollection( new MuonHists("Muon_TopTag") );
    RegisterHistCollection( new TauHists("Tau_TopTag") );
    RegisterHistCollection( new TopJetHists("TopJets_TopTag") );

    RegisterHistCollection( new EventHists("Event_NoTopTagNoBTag") );
    RegisterHistCollection( new JetHists("Jets_NoTopTagNoBTag") );
    RegisterHistCollection( new ElectronHists("Electron_NoTopTagNoBTag") );
    RegisterHistCollection( new MuonHists("Muon_NoTopTagNoBTag") );
    RegisterHistCollection( new TauHists("Tau_NoTopTagNoBTag") );
    RegisterHistCollection( new TopJetHists("TopJets_NoTopTagNoBTag") );

    RegisterHistCollection( new EventHists("Event_NoTopTagBTag") );
    RegisterHistCollection( new JetHists("Jets_NoTopTagBTag") );
    RegisterHistCollection( new ElectronHists("Electron_NoTopTagBTag") );
    RegisterHistCollection( new MuonHists("Muon_NoTopTagBTag") );
    RegisterHistCollection( new TauHists("Tau_NoTopTagBTag") );
    RegisterHistCollection( new TopJetHists("TopJets_NoTopTagBTag") );

    RegisterHistCollection( new EventHists("Event_NoTopTag") );
    RegisterHistCollection( new JetHists("Jets_NoTopTag") );
    RegisterHistCollection( new ElectronHists("Electron_NoTopTag") );
    RegisterHistCollection( new MuonHists("Muon_NoTopTag") );
    RegisterHistCollection( new TauHists("Tau_NoTopTag") );
    RegisterHistCollection( new TopJetHists("TopJets_NoTopTag") );

    RegisterHistCollection( new EventHists("Event_BTag") );
    RegisterHistCollection( new JetHists("Jets_BTag") );
    RegisterHistCollection( new ElectronHists("Electron_BTag") );
    RegisterHistCollection( new MuonHists("Muon_BTag") );
    RegisterHistCollection( new TauHists("Tau_BTag") );
    RegisterHistCollection( new TopJetHists("TopJets_BTag") );

    RegisterHistCollection( new EventHists("Event_NoBTag") );
    RegisterHistCollection( new JetHists("Jets_NoBTag") );
    RegisterHistCollection( new ElectronHists("Electron_NoBTag") );
    RegisterHistCollection( new MuonHists("Muon_NoBTag") );
    RegisterHistCollection( new TauHists("Tau_NoBTag") );
    RegisterHistCollection( new TopJetHists("TopJets_NoBTag") );

    RegisterHistCollection( new EventHists("Event_Kinesel") );
    RegisterHistCollection( new JetHists("Jets_Kinesel") );
    RegisterHistCollection( new ElectronHists("Electron_Kinesel") );
    RegisterHistCollection( new MuonHists("Muon_Kinesel") );
    RegisterHistCollection( new TauHists("Tau_Kinesel") );
    RegisterHistCollection( new TopJetHists("TopJets_Kinesel") );

    RegisterHistCollection( new EventHists("Event_Postsel") );
    RegisterHistCollection( new JetHists("Jets_Postsel") );
    RegisterHistCollection( new ElectronHists("Electron_Postsel") );
    RegisterHistCollection( new MuonHists("Muon_Postsel") );
    RegisterHistCollection( new TauHists("Tau_Postsel") );
    RegisterHistCollection( new TopJetHists("TopJets_Postsel") );

    // important: initialise histogram collections after their definition
    InitHistos();

    return;

}

void MistagSelectionCycle::EndInputData( const SInputData& id ) throw( SError )
{

    AnalysisCycle::EndInputData( id );

    return;
}

void MistagSelectionCycle::BeginInputFile( const SInputData& id ) throw( SError )
{
    // Connect all variables from the Ntuple file with the ones needed for the analysis
    // The variables are commonly stored in the BaseCycleContaincer

    // important: call to base function to connect all variables to Ntuples from the input tree
    AnalysisCycle::BeginInputFile( id );

    return;
}

void MistagSelectionCycle::ExecuteEvent( const SInputData& id, Double_t weight) throw( SError )
{
    // this is the most important part: here the full analysis happens
    // user should implement selections, filling of histograms and results

    // first step: call Execute event of base class to perform basic consistency checks
    // also, the good-run selection is performed there and the calculator is reset

    AnalysisCycle::ExecuteEvent( id, weight);

    EventCalc* calc = EventCalc::Instance();
    BaseCycleContainer* bcc = calc->GetBaseCycleContainer();

    // control histograms
    FillControlHists("_Presel");

    static Selection* trig_selection = GetSelection("trig_selection");
    static Selection* first_selection = GetSelection("first_selection");
    static Selection* second_selection = GetSelection("second_selection");
    static Selection* NoBTagSel = GetSelection("NoBTagSelection");
    static Selection* TopTagSel = GetSelection("TopTagSelection");

    static Selection* mttbar_gen_selection = GetSelection("Mttbar_Gen_Selection");
    if(!mttbar_gen_selection->passSelection())  throw SError( SError::SkipEvent );

    m_cleaner = new Cleaner();
    m_cleaner->SetJECUncertainty(m_jes_unc);

    // settings for jet correction uncertainties
    if (m_sys_unc==e_JEC){
      if (m_sys_var==e_Up) m_cleaner->ApplyJECVariationUp();
      if (m_sys_var==e_Down) m_cleaner->ApplyJECVariationDown();
    }
    if (m_sys_unc==e_JER){
      if (m_sys_var==e_Up) m_cleaner->ApplyJERVariationUp();
      if (m_sys_var==e_Down) m_cleaner->ApplyJERVariationDown();
    }

    if(bcc->pvs)  m_cleaner->PrimaryVertexCleaner(4, 24., 2.);
    if(bcc->electrons) m_cleaner->ElectronCleaner_noIso(35,2.5, m_reversed_electron_selection,true);
    if(bcc->muons) m_cleaner->MuonCleaner_noIso(45,2.1);
    if(bcc->jets) m_cleaner->JetLeptonSubtractor(m_corrector,false);
    if(!bcc->isRealData && bcc->jets) m_cleaner->JetEnergyResolutionShifter();

    //apply loose jet cleaning for 2D cut
    if(bcc->jets) m_cleaner->JetCleaner(25,double_infinity(),true);

    // control histograms
    FillControlHists("_Cleaned");

    if(!trig_selection->passSelection())  throw SError( SError::SkipEvent );

    if(!first_selection->passSelection())  throw SError( SError::SkipEvent );

    //apply tighter jet cleaning for further cuts and analysis steps
    if(bcc->jets) m_cleaner->JetCleaner(50,2.5,true);

    //remove all taus from collection for HTlep calculation
    if(bcc->taus) m_cleaner->TauCleaner(double_infinity(),0.0);

    if(!second_selection->passSelection())  throw SError( SError::SkipEvent );

    FillControlHists("_Kinesel");
    bool nobtag = NoBTagSel->passSelection();
    if(nobtag){
        FillControlHists("_NoBTag");
    } else {
        FillControlHists("_BTag");
    }

    bool toptag = TopTagSel->passSelection();

    if(toptag) {
        FillControlHists("_TopTag");
        if(nobtag) {
            FillControlHists("_TopTagNoBTag");
        } else {
            FillControlHists("_TopTagBTag");
        }
    } else {
        FillControlHists("_NoTopTag");
        if(nobtag) {
            FillControlHists("_NoTopTagNoBTag");
        } else {
            FillControlHists("_NoTopTagBTag");
        }
    }

    // control histograms
    FillControlHists("_Postsel");

    WriteOutputTree();

    return;
}

void MistagSelectionCycle::FillControlHists(TString postfix)
{
    // fill some control histograms, need to be defined in BeginInputData

    BaseHists* eventhists = GetHistCollection((std::string)("Event"+postfix));
    BaseHists* jethists = GetHistCollection((std::string)("Jets"+postfix));
    BaseHists* elehists = GetHistCollection((std::string)("Electron"+postfix));
    BaseHists* muonhists = GetHistCollection((std::string)("Muon"+postfix));
    BaseHists* tauhists = GetHistCollection((std::string)("Tau"+postfix));
    BaseHists* topjethists = GetHistCollection((std::string)("TopJets"+postfix));

    eventhists->Fill();
    jethists->Fill();
    elehists->Fill();
    muonhists->Fill();
    tauhists->Fill();
    topjethists->Fill();

}
