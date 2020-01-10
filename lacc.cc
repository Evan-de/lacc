// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
//     Author: Evan Kim (evandde@gmail.com)
//     This code was tested with Geant4 10.6.
//

// Mandatory classes
#include "DetectorConstruction.hh"
#include "PhysicsList.hh"
#include "G4PhysListFactory.hh"
#include "ActionInitialization.hh"

// G4Runmanager
#ifdef G4MULTITHREADED
#include "G4MTRunManager.hh"
#include "G4Threading.hh"
#else
#include "G4RunManager.hh"
#endif

// Randomize class to set seed number
#include "Randomize.hh"

// UI and visualization classes
#include "G4UImanager.hh"
#include "G4UIExecutive.hh"
#include "G4VisExecutive.hh"

namespace
{
void PrintUsage()
{
    G4cerr << " Usage: " << G4endl
           << " ProjectName [-option1 value1] [-option2 value2] ..." << G4endl;
    G4cerr << "\t--- Option lists ---"
           << "\n\t[-m] <Set macrofilepath> default: vis.mac, inputtype: string"
#ifdef G4MULTITHREADED
           << "\n\t[-t] <Set nThreads> default: 1, inputtype: int, Max: "
           << G4Threading::G4GetNumberOfCores()
#endif
           << "\n\t[-p] <Set physics> default: 'code', inputtype: string"
           << G4endl;
}
}

int main(int argc, char** argv)
{
    // --- Default setting for main() arguments --- //
    G4String macroFilePath;
#ifdef G4MULTITHREADED
    G4int nThreads = 1;
#endif
    G4String physName;

    // --- Parsing main() Arguments --- //
    for(G4int i = 1; i<argc; i = i + 2)
    {
        if(G4String(argv[i])=="-m") macroFilePath = argv[i+1];
#ifdef G4MULTITHREADED
        else if(G4String(argv[i])=="-t") nThreads = G4UIcommand::ConvertToInt(argv[i+1]);
#endif
        else if(G4String(argv[i])=="-p") physName = argv[i+1];
        else
        {
            PrintUsage();
            return 1;
        }
    }
    if (argc>7)
    {
        PrintUsage();
        return 1;
    }

    // --- Choose the Random engine --- //
    G4Random::setTheEngine(new CLHEP::RanecuEngine);
    G4Random::setTheSeed(time(nullptr));

    // Construct runmanager
#ifdef G4MULTITHREADED
    auto runManager = std::make_unique<G4MTRunManager>();
    runManager->SetNumberOfThreads(nThreads);
#else
    auto runManager = std::make_unique<G4RunManager>();
#endif

    runManager->SetUserInitialization(new DetectorConstruction());
    G4VModularPhysicsList* phys;
    if(physName.empty()) phys = new PhysicsList();
    else
    {
        G4PhysListFactory factory;
        phys = factory.GetReferencePhysList(physName);
    }
    runManager->SetUserInitialization(phys);
    runManager->SetUserInitialization(new ActionInitialization());

    runManager->Initialize();

    // Initialize visualization
    auto visManager = std::make_unique<G4VisExecutive>();
    visManager->Initialize();

    // Get the pointer to the User Interface manager
    auto UImanager = G4UImanager::GetUIpointer();

    if(!macroFilePath.empty())
    {
        // Batch mode
        G4String command = "/control/execute ";
        UImanager->ApplyCommand(command + macroFilePath);
    }
    else
    {
        // interactive mode: define UI session
        auto ui = std::make_unique<G4UIExecutive>(argc, argv, "qt");
        UImanager->ApplyCommand("/control/execute vis.mac");
        ui->SessionStart();
    }

    return 0;
}
