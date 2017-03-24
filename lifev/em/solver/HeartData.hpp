//
//  HeartData.hpp
//  LifeV
//
//  Created by Thomas Kummer on 22.03.17.
//  Copyright © 2017 Thomas Kummer. All rights reserved.
//

#ifndef HeartData_hpp
#define HeartData_hpp

#include <stdio.h>


namespace LifeV
{
    
class HeartData
{
public:
    
    HeartData() {}
    
    virtual ~HeartData() {};
    
    void setup(const GetPot& datafile)
    {
        setGetPotFile(datafile);
        readFile();
    }
    
    void showMe() const
    {
        std::cout << "\nHeartData - showMe() --------------------------";
        std::cout << "\ndt_activation: " << M_dt_activation;
        std::cout << "\ndt_loadstep: " << M_dt_loadstep;
        std::cout << "\nactivationLimit_loadstep: " << M_activationLimit_loadstep;
        std::cout << "\ndt_mechanics: " << M_dt_mechanics;
        std::cout << "\ndt_safe: " << M_dt_save;
        std::cout << "\nendtime: " << M_endtime;
        std::cout << "\nmechanicsLoadstepIter: " << M_mechanicsLoadstepIter;
        std::cout << "\nmechanicsCouplingIter: " << M_mechanicsCouplingIter;
        std::cout << "\nmaxiter: " << M_maxiter;
        std::cout << "\npreloadSteps: " << M_preloadSteps;
        std::cout << "\nsafePreload: " << M_safePreload;
        
        std::cout << "\npPerturbationFe: " << M_pPerturbationFe;
        std::cout << "\npPerturbationCirc: " << M_pPerturbationCirc;
        std::cout << "\ncouplingError: " << M_couplingError;
        std::cout << "\ncouplingJFeSubIter: " << M_couplingJFeSubIter;
        std::cout << "\ncouplingJFeSubStart: " << M_couplingJFeSubStart;
        std::cout << "\ncouplingJFeIter: " << M_couplingJFeIter;
        
        std::cout << "\nelementOrder: " << M_elementOrder;
        
        std::cout << "\nHeartData - showMe() --------------------------";
    }
    
    const Real& dt_activation () const { return M_dt_activation; }
    const Real& dt_loadstep () const { return M_dt_loadstep; }
    const Real& activationLimit_loadstep () const { return M_activationLimit_loadstep; }
    const Real& dt_mechanics () const { return M_dt_mechanics; }
    const Real& dt_save () const { return M_dt_save; }
    const Real& dt_chrono () const { return M_dt_chrono; }
    const Real& endtime () const { return M_endtime; }
    const UInt& chronoIter () const { return M_chronoIter; }
    const UInt& mechanicsLoadstepIter () const { return M_mechanicsLoadstepIter; }
    const UInt& mechanicsCouplingIter () const { return M_mechanicsCouplingIter; }
    const UInt& maxiter () const { return M_maxiter; }
    const UInt& preloadSteps () const { return M_preloadSteps; }
    const bool& safePreload () const { return M_safePreload; }
    
    const Real& pPerturbationFe () const { return M_pPerturbationFe; }
    const Real& pPerturbationCirc () const { return M_pPerturbationCirc; }
    const Real& couplingError () const { return M_couplingError; }
    const UInt& couplingJFeSubIter () const { return M_couplingJFeSubIter; }
    const UInt& couplingJFeSubStart () const { return M_couplingJFeSubStart; }
    const UInt& couplingJFeIter () const { return M_couplingJFeIter; }
    
    const std::string& elementOrder () const { return M_elementOrder; }
    
    const std::string& meshName () const { return M_meshName; }
    const std::string& meshPath () const { return M_meshPath; }

    const std::string& fiberFileName () const { return M_fiberFileName; }
    const std::string& fiberFieldName () const { return M_fiberFieldName; }
    const std::string& sheetFileName () const { return M_sheetFileName; }
    const std::string& sheetFieldName () const { return M_sheetFieldName; }

    const std::vector<Real>& scale () const { return M_scale; }
    const std::vector<Real>& rotate () const { return M_rotate; }
    const std::vector<Real>& translate () const { return M_translate; }
    
    const GetPot& datafile () const { return M_datafile; }
    
protected:
    
    
    void setGetPotFile(const GetPot& datafile)
    {
        M_datafile = datafile;
    }
    
    void readFile()
    {
        M_dt_activation = M_datafile ("activation/time_discretization/timestep", 0.05 );
        M_dt_loadstep =  M_datafile ( "solid/time_discretization/dt_loadstep", 1.0 );
        M_activationLimit_loadstep =  M_datafile ( "solid/time_discretization/activation_limit_loadstep", 0.0 );
        M_dt_mechanics = M_datafile ("solid/time_discretization/timestep", 1.0 );
        M_dt_save = M_datafile ( "exporter/save", 10. );
        M_dt_chrono = M_datafile ( "exporter/chrono", 10. );
        M_endtime = M_datafile ("solid/time_discretization/endtime", 100000);
        M_chronoIter = static_cast<UInt>( M_dt_chrono / M_dt_activation );
        M_mechanicsLoadstepIter = static_cast<UInt>( M_dt_loadstep / M_dt_activation );
        M_mechanicsCouplingIter = static_cast<UInt>( M_dt_mechanics / M_dt_activation );
        M_maxiter = static_cast<UInt>( M_endtime / M_dt_activation ) ;
        M_preloadSteps = M_datafile ( "solid/boundary_conditions/numPreloadSteps", 0);
        M_safePreload = M_datafile ( "exporter/savePreload", false );
        
        M_pPerturbationFe = M_datafile ( "solid/coupling/pPerturbationFe", 1e-2 );
        M_pPerturbationCirc = M_datafile ( "solid/coupling/pPerturbationCirc", 1e-3 );
        M_couplingError = M_datafile ( "solid/coupling/couplingError", 1e-6 );
        M_couplingJFeSubIter = M_datafile ( "solid/coupling/couplingJFeSubIter", 1 );
        M_couplingJFeSubStart = M_datafile ( "solid/coupling/couplingJFeSubStart", 1 );
        M_couplingJFeIter = M_datafile ( "solid/coupling/couplingJFeIter", 1 );
        
        M_elementOrder = M_datafile ( "solid/space_discretization/order", "P2");
        
        M_meshName = M_datafile("solid/space_discretization/mesh_file", "cube4.mesh");
        M_meshPath = M_datafile("solid/space_discretization/mesh_dir", "./");
        
        M_fiberFileName  =  M_datafile ( "solid/space_discretization/fiber_name", "FiberDirection");
        M_sheetFileName  =  M_datafile ( "solid/space_discretization/sheet_name", "SheetsDirection");
        M_fiberFieldName =  M_datafile ( "solid/space_discretization/fiber_fieldname", "fibers");
        M_sheetFieldName =  M_datafile ( "solid/space_discretization/sheet_fieldname", "sheets");

        M_scale = std::vector<Real> (3, M_datafile("solid/space_discretization/mesh_scaling", 1.0));
        M_rotate = std::vector<Real> { M_datafile("solid/space_discretization/mesh_rotation_0", 0.0) , M_datafile("solid/space_discretization/mesh_rotation_1", 0.0) , M_datafile("solid/space_discretization/mesh_rotation_2", 0.0) };
        M_translate = std::vector<Real> { M_datafile("solid/space_discretization/mesh_translation_0", 0.0) , M_datafile("solid/space_discretization/mesh_translation_1", 0.0) , M_datafile("solid/space_discretization/mesh_translation_2", 0.0) };
        
        //        const Real dpMax = dataFile ( "solid/coupling/dpMax", 0.1 );
        //
        //        std::vector<std::vector<std::string> > bcNames { { "lv" , "p" } , { "rv" , "p" } };
        //        std::vector<double> bcValues { p ( "lv" ) , p ( "rv") };
        //        std::vector<double> bcValuesPre ( bcValues );
        //
        //        VectorSmall<4> ABdplv, ABdprv, ABcoef;
        //        ABcoef (0) = 55/24; ABcoef (1) = -59/24; ABcoef (2) = 37/24; ABcoef (3) = -3/8;
        //
        //        VectorSmall<2> VCirc, VCircNew, VCircPert, VFe, VFeNew, VFePert, R, dp;
        //        MatrixSmall<2,2> JFe, JCirc, JR;
        //
        //        UInt iter (0);
        //        Real t (0);
        
        
    }
    
    
    Real M_dt_activation;
    Real M_dt_loadstep;
    Real M_activationLimit_loadstep;
    Real M_dt_mechanics;
    Real M_dt_save;
    Real M_dt_chrono;
    Real M_endtime;
    UInt M_chronoIter;
    UInt M_mechanicsLoadstepIter;
    UInt M_mechanicsCouplingIter;
    UInt M_maxiter;
    UInt M_preloadSteps;
    bool M_safePreload;
    
    Real M_pPerturbationFe;
    Real M_pPerturbationCirc;
    Real M_couplingError;
    UInt M_couplingJFeSubIter;
    UInt M_couplingJFeSubStart;
    UInt M_couplingJFeIter;
    
    std::string M_elementOrder;
    
    std::string M_meshName;
    std::string M_meshPath;
    
    std::string M_fiberFileName;
    std::string M_sheetFileName;
    std::string M_fiberFieldName;
    std::string M_sheetFieldName;

    std::vector<Real> M_scale;
    std::vector<Real> M_rotate;
    std::vector<Real> M_translate;
    
    
    GetPot M_datafile;
    
};
    
}

#endif /* HeartData_hpp */

