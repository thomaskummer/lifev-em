//@HEADER
/*
 *******************************************************************************

 Copyright (C) 2004, 2005, 2007 EPFL, Politecnico di Milano, INRIA
 Copyright (C) 2010 EPFL, Politecnico di Milano, Emory University

 This file is part of LifeV.

 LifeV is free software; you can redistribute it and/or modify
 it under the terms of the GNU Lesser General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 LifeV is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public License
 along with LifeV.  If not, see <http://www.gnu.org/licenses/>.

 *******************************************************************************
 */
//@HEADER
/*!
 @file
 @brief Class for solving the Monodomain equations in electrophysiology.

 @date 02-2013
 @author Simone Rossi <simone.rossi@epfl.ch>

 @last update 02-2013

 This class provides interfaces to solve the monodomain equation
 ( reaction diffusion equation ) using the ETA framework.
 The solution can be performed using three different methods:
 -operator splitting method (at this point available only with forward Euler
 for the reaction step and backward Euler for the diffusion step. );
 -Ionic Currents Interpolation (at this point only forward Euler);
 -State Variable interpolation (at this point only forward Euler).
 */

#ifndef _EMSOLVER_H_
#define _EMSOLVER_H_

#include <lifev/core/mesh/MeshLoadingUtility.hpp>

#include <lifev/em/solver/electrophysiology/EMMonodomainSolver.hpp>
#include <lifev/em/solver/electrophysiology/IonicModelsList.hpp>
#include <lifev/em/solver/mechanics/EMStructuralOperator.hpp>


#include <lifev/structure/solver/WallTensionEstimator.hpp>
#include <lifev/em/solver/activation/ActivationModelsList.hpp>

#include <lifev/bc_interface/3D/bc/BCInterface3D.hpp>


namespace LifeV
{

//! EMSolver - Class featuring the solution of the electromechanical problem with monodomain equation

template<typename Mesh , typename ElectroSolver>
class EMSolver
{
public:
    typedef Mesh                                              mesh_Type;

    typedef boost::shared_ptr<mesh_Type>                      meshPtr_Type;

    typedef Epetra_Comm                                       comm_Type;

    typedef boost::shared_ptr<Epetra_Comm>                    commPtr_Type;

    typedef VectorEpetra                                      vector_Type;
    
    typedef StructuralConstitutiveLawData                     structureData_Type;

    typedef boost::shared_ptr<structureData_Type>             structureDataPtr_Type;

    typedef EMStructuralOperator< mesh_Type >                 structuralOperator_Type;

    typedef boost::shared_ptr< structuralOperator_Type >      structuralOperatorPtr_Type;

    typedef BCHandler                                          bc_Type;

    typedef boost::shared_ptr< bc_Type >                       bcPtr_Type;

    typedef StructuralOperator< mesh_Type >                   physicalSolver_Type;

    typedef BCInterface3D< bc_Type, physicalSolver_Type >  bcInterface_Type;

    typedef boost::shared_ptr< bcInterface_Type >              bcInterfacePtr_Type;

    typedef boost::shared_ptr<Activation>                     activationModelPtr_Type;

    typedef ElectroSolver                                      electroSolver_Type;

    typedef boost::shared_ptr<electroSolver_Type>              electroSolverPtr_Type;

    typedef ElectroIonicModel                                  ionicModel_Type;

    typedef boost::shared_ptr<ionicModel_Type>                 ionicModelPtr_Type;

    typedef ExporterHDF5< Mesh >                               exporter_Type;

    typedef boost::shared_ptr<ExporterHDF5< Mesh > >           exporterPtr_Type;

    typedef FESpace< RegionMesh<LinearTetra>, MapEpetra >      solidFESpace_Type;

    typedef boost::shared_ptr<solidFESpace_Type>                solidFESpacePtr_Type;

    typedef ETFESpace< RegionMesh<LinearTetra>, MapEpetra, 3, 3 > solidETFESpace_Type;

    typedef boost::shared_ptr<solidETFESpace_Type>              solidETFESpacePtr_Type;

    typedef boost::function < Real (const Real& t,
                                    const Real&    x,
                                    const Real&    y,
                                    const Real& z,
                                    const ID&   /*i*/ ) >       function_Type;



    EMSolver(commPtr_Type comm);

    EMSolver (const EMSolver& solver);



    void loadMesh (std::string meshName, std::string meshPath)
    {
//        if (M_commPtr -> MyPID() == 0)
//        {
//            std::cout << "\n\nEMSolver: loadMesh ... " << std::endl;
//        }

        M_fullMeshPtr.reset( new Mesh() );
        MeshUtility::loadMesh (M_localMeshPtr, M_fullMeshPtr, meshName, meshPath);
        if(M_commPtr)
        {
			M_localMeshPtr->setComm(M_commPtr);
			M_fullMeshPtr->setComm(M_commPtr);
        }
        else
        {
        	M_commPtr = M_localMeshPtr -> comm();
        }
        
        if (M_commPtr -> MyPID() == 0)
        {
            std::cout << "\nEMSolver: loadMesh - done";
        }

    }

    void setupElectroExporter ( std::string problemFolder = "./", std::string outputFileName = "ElectroSolution" )
    {
        M_electroSolverPtr -> setupExporter (*M_electroExporterPtr, outputFileName, problemFolder);
    }

    void setupActivationExporter ( std::string problemFolder = "./", std::string outputFileName = "ActivationSolution" )
    {
        EMUtility::setupExporter<Mesh> (*M_activationExporterPtr, M_localMeshPtr, M_commPtr, outputFileName, problemFolder);
    }
    
//    void setupActivationTimeExporter ( std::string problemFolder = "./", std::string outputFileName = "ActivationTimeSolution" )
//    {
//        EMUtility::setupExporter<Mesh> (*M_activationTimeExporterPtr, M_localMeshPtr, M_commPtr, outputFileName, problemFolder);
//    }

    void setupVonMisesStressExporter ( std::string problemFolder = "./", std::string outputFileName = "VonMisesStress" )
    {
        EMUtility::setupExporter<Mesh> (*M_vonMisesStressExporterPtr, M_localMeshPtr, M_commPtr, outputFileName, problemFolder);
    }
    
//    void setupVonMisesStressExporterP ( std::string problemFolder = "./", std::string outputFileName = "VonMisesStressP" )
//    {
//        EMUtility::setupExporter<Mesh> (*M_vonMisesStressExporterPtrP, M_localMeshPtr, M_commPtr, outputFileName, problemFolder);
//    }
//    
//    void setupVonMisesStressExporterA ( std::string problemFolder = "./", std::string outputFileName = "VonMisesStressA" )
//    {
//        EMUtility::setupExporter<Mesh> (*M_vonMisesStressExporterPtrA, M_localMeshPtr, M_commPtr, outputFileName, problemFolder);
//    }

    void setupMechanicsExporter ( std::string problemFolder = "./", std::string outputFileName = "MechanicalSolution" )
    {
        if (M_mechanicsExporterPtr)
        {
            EMUtility::setupExporter<Mesh> (*M_mechanicsExporterPtr, M_localMeshPtr, M_commPtr, outputFileName, problemFolder);
        }
    }

    void importHdf5 ();

    void setupExporters (std::string problemFolder   = "./",
                         std::string electroFileName = "ElectroSolution",
                         std::string activationFileName  = "ActivationSolution",
                         //std::string activationTimeFileName  = "ActivationTimeSolution",
                         std::string mechanicsFileName  = "MechanicalSolution",
                         std::string vonMisesStressFileName  = "VonMisesStress");

    void setupElectroSolver ( GetPot& dataFile, Teuchos::ParameterList& list)
    {
        setupElectroSolver (dataFile);
    }

    void setupElectroSolver ( GetPot& dataFile );
    
    void setupMechanicalSolver ( GetPot& dataFile);

    void setupMechanicalBC (std::string data_file_name,
                            std::string section,
                            solidFESpacePtr_Type dFESpace);

    void setupActivation (const MapEpetra& map)
    {
//        if (M_commPtr -> MyPID() == 0)
//        {
//            std::cout << "\nEMSolver: setupActivation ... " << std::endl;
//        }
        
        M_activationModelPtr.reset ( Activation::EMActivationFactory::instance().createObject ( M_data.activationParameter<std::string>( "ActivationModel" ) ) );
        M_activationModelPtr->setup(M_data, map);
        M_activationModelPtr->setVariablesPtr(*M_electroSolverPtr);
        M_activationModelPtr->setI4fPtr( M_EMStructuralOperatorPtr -> I4fPtr() );

        if (M_commPtr -> MyPID() == 0)
        {
            std::cout << "\nEMSolver: setupActivation - done";
        }

    }

    void setup (GetPot& dataFile, Teuchos::ParameterList& list)
    {
        setup (dataFile);
    }

    void setup (GetPot& dataFile );
    
    void buildMechanicalSystem()
    {
//        if (M_commPtr -> MyPID() == 0) std::cout << "\nEMSolver: buildMechanicalSystem ... " << std::endl;
        
    	//Here we call the buildSystem Of the Structural operator
    	// the coefficient is the density in front of the mass matrix
        M_EMStructuralOperatorPtr -> buildSystem (1.0);
        
        if (M_commPtr -> MyPID() == 0) std::cout << "\nEMSolver: buildMechanicalSystem - done";
    }

    void buildElectroSystem()
    {
//        if (M_commPtr -> MyPID() == 0) std::cout << "\nEMSolver: buildElectroSystem ... " << std::endl;

        M_electroSolverPtr -> setupMatrices();
        
        if (M_commPtr -> MyPID() == 0) std::cout << "\nEMSolver: buildElectroSystem - done";
    }

     void buildSystem()
    {
        buildMechanicalSystem();
        buildElectroSystem();
    }

    void initializeElectroVariables()
    {
//        if (M_commPtr -> MyPID() == 0) std::cout << "\nEMSolver: initializeElectroVariables ... " << std::endl;

        M_electroSolverPtr -> setInitialConditions();
        
        if (M_commPtr -> MyPID() == 0) std::cout << "\nEMSolver: initializeElectroVariables - done";

    }

    void initialize()
    {
        initializeElectroVariables();
    }

    bcInterfacePtr_Type bcInterfacePtr()
    {
        return M_bcInterfacePtr;
    }


    void setupFiberVector( const std::string& fileName,
						   const std::string& fieldName,
						   const std::string& postDir = "./",
						   const std::string& polynomialDegree = "P1"  )
    {
//        if (M_commPtr -> MyPID() == 0) std::cout << "\nEMSolver: setupFiberVector ... " << std::endl;

    	setupMechanicalFiberVector(fileName, fieldName, postDir, polynomialDegree);
    	M_electroSolverPtr->setFiberPtr(getMechanicsFibers());
        
        if (M_commPtr -> MyPID() == 0) std::cout << "\nEMSolver: setupFiberVector - done";

    }

    void setupMechanicalFiberVector( const std::string& fileName,
    		                         const std::string& fieldName,
									 const std::string& postDir = "./",
                                     const std::string& polynomialDegree = "P1"  )
    {
//        if (M_commPtr -> MyPID() == 0) std::cout << "\nEMSolver: setupMechanicalFiberVector ... " << std::endl;
        
        ElectrophysiologyUtility::importVectorField (getMechanicsFibers(),  fileName,  fieldName, M_localMeshPtr, postDir, polynomialDegree );
        
//        if ( polynomialDegree == "P1" )
//        {
//            FESpace<RegionMesh<LinearTetra> , MapEpetra > p2FESpace ( fullMeshPtr(), "P2", 1, fullMeshPtr() -> comm() );
//            vector_Type p2FibersRep (*getMechanicsFibers(), Repeated);
//            vector_Type p1FibersRep = M_EMStructuralOperatorPtr -> dispFESpacePtr() -> feToFEInterpolate(p2FESpace, p2FibersRep);
//            getMechanicsFibers().reset ( new vector_Type (p1FibersRep, Unique) );
//        }
        if (M_commPtr -> MyPID() == 0) std::cout << "\nEMSolver: setupMechanicalFiberVector - done";

    }

    void setupMechanicalSheetVector( const std::string& fileName,
    		                         const std::string& fieldName,
									 const std::string& postDir = "./",
                                     const std::string& polynomialDegree = "P1"  )
    {
//        if (M_commPtr -> MyPID() == 0) std::cout << "\nEMSolver: setupMechanicalSheetVector ... " << std::endl;

        ElectrophysiologyUtility::importVectorField (getMechanicsSheets(),  fileName,  fieldName, M_localMeshPtr, postDir, polynomialDegree );
        if (M_commPtr -> MyPID() == 0) std::cout << "\nEMSolver: setupMechanicalSheetVector - done";

    }

    void setupElectroFiberVector( const std::string& fileName,
								  const std::string& fieldName,
								  const std::string& postDir = "./",
								  const std::string& polynomialDegree = "P1"  )
    {
        ElectrophysiologyUtility::importVectorField (getElectroFibers(), fileName,  fieldName, M_localMeshPtr, postDir, polynomialDegree );
    }



    void setupMechanicalFiberVector ( Real fx, Real fy, Real fz )
    {
        M_EMStructuralOperatorPtr -> EMMaterial() -> setupFiberVector ( fx, fy, fz);
    }

    void setupElectroFiberVector ( VectorSmall<3>& fibers)
    {
        M_electroSolverPtr -> setupFibers (fibers);
    }


    void setupFiberVector ( Real fx, Real fy, Real fz )
    {
        VectorSmall<3> f;
        f[0] = fx;
        f[1] = fy;
        f[2] = fz;
        setupElectroFiberVector (f);
        M_EMStructuralOperatorPtr -> EMMaterial() -> setupFiberVector(fx, fy, fz);
    }

    void setupSheetVector ( Real sx, Real sy, Real sz )
    {
        M_EMStructuralOperatorPtr -> EMMaterial() -> setupSheetVector(sx, sy, sz);
    }

    electroSolverPtr_Type electroSolverPtr()
    {
        return M_electroSolverPtr;
    }

    structuralOperatorPtr_Type structuralOperatorPtr()
    {
        return M_EMStructuralOperatorPtr;
    }

    activationModelPtr_Type  activationModelPtr()
    {
        return M_activationModelPtr;
    }
    
    vectorPtr_Type activationTimePtr()
    {
        return M_activationTimePtr;
    }

    void saveSolution (Real time, const bool& restart = 0);
    
    void setTimeIndex (const UInt& time);


    void closeExporters();

    void oneWayCoupling();

    void twoWayCoupling();

    void setAppliedCurrent (function_Type& stimulus, Real time = 0.0)
    {
        M_electroSolverPtr -> setAppliedCurrentFromFunction (stimulus, time);
    }


    void solveMechanics()
    {
        M_EMStructuralOperatorPtr -> iterate ( M_bcInterfacePtr -> handler() );
    }

    void solveMechanicsLin()
    {
        M_EMStructuralOperatorPtr -> solveLin ();
    }

    void solveElectrophysiology (function_Type& stimulus, Real time = 0.0);


    void solveActivation (Real dt);
    
    void computeI4f (VectorEpetra& i4f, VectorEpetra& f0_, VectorEpetra& disp, solidFESpacePtr_Type feSpacePtr);

    void computeDeformedFiberDirection (VectorEpetra& f, VectorEpetra& f0_, VectorEpetra& disp, solidFESpacePtr_Type feSpacePtr);
    
    vectorPtr_Type getElectroFibers()
    {
    	return M_electroSolverPtr -> fiberPtr();
    }
    vectorPtr_Type getMechanicsFibers()
    {
    	return M_EMStructuralOperatorPtr -> EMMaterial() -> fiberVectorPtr();
    }
    vectorPtr_Type getMechanicsSheets()
    {
    	return M_EMStructuralOperatorPtr -> EMMaterial() -> sheetVectorPtr();
    }
    //  bcInterface_Type bcInterface()
    //  {
    //      return *M_bcInterfacePtr;
    //  }
    EMData& data()
    {
    	return M_data;
    }


    commPtr_Type comm() const
    {
    	return M_commPtr;
    }

    void setComm(commPtr_Type comm)
    {
    	M_commPtr = comm;
    }

    void showMe() const {}
    
    WallTensionEstimator<RegionMesh<LinearTetra> >& tensionEstimator()
    {
        return M_wteTotal;
    }
    
    meshPtr_Type fullMeshPtr()
    {
        return M_fullMeshPtr;
    }
    
    meshPtr_Type localMeshPtr()
    {
        return M_localMeshPtr;
    }

    std::vector<meshPtr_Type> mesh()
    {
        std::vector<meshPtr_Type> meshVector;
        meshVector.push_back(M_fullMeshPtr);
        meshVector.push_back(M_localMeshPtr);
        return meshVector;
    }
    
protected:
public:
    electroSolverPtr_Type                M_electroSolverPtr;
    activationModelPtr_Type              M_activationModelPtr;
    bcInterfacePtr_Type                  M_bcInterfacePtr;
    structuralOperatorPtr_Type           M_EMStructuralOperatorPtr;

    exporterPtr_Type                     M_electroExporterPtr;
    exporterPtr_Type                     M_activationExporterPtr;
    exporterPtr_Type                     M_mechanicsExporterPtr;
//    exporterPtr_Type                     M_activationTimeExporterPtr;
    exporterPtr_Type                     M_vonMisesStressExporterPtr;
//    exporterPtr_Type                     M_vonMisesStressExporterPtrP;
//    exporterPtr_Type                     M_vonMisesStressExporterPtrA;
    
    meshPtr_Type                         M_localMeshPtr;
    meshPtr_Type                         M_fullMeshPtr;
    
    vectorPtr_Type                       M_activationTimePtr;

    bool                                 M_oneWayCoupling;
    
    WallTensionEstimator<RegionMesh<LinearTetra> > M_wteTotal;
//    WallTensionEstimator<RegionMesh<LinearTetra> > M_wtePassive;
//    WallTensionEstimator<RegionMesh<LinearTetra> > M_wteActive;


    commPtr_Type                         M_commPtr;

    EMData                               M_data;


};

/////////////////////
// CONSTRUCTORS
template<typename Mesh , typename ElectroSolver>
EMSolver<Mesh, ElectroSolver>::EMSolver(commPtr_Type comm) :
    M_electroSolverPtr       ( ),
    M_activationModelPtr    ( ),
    M_bcInterfacePtr        ( ),
    M_EMStructuralOperatorPtr(),
    M_electroExporterPtr ( ),
    M_activationExporterPtr ( ),
    M_mechanicsExporterPtr  ( ),
    M_localMeshPtr      ( ),
    M_fullMeshPtr      ( ),
    M_activationTimePtr     ( ),
    M_oneWayCoupling     (true),
    M_wteTotal ( ),
//    M_wtePassive ( ),
//    M_wteActive ( ),
    M_commPtr               (comm),
    M_data                    ()
{
}


/////////////////////
// COPY CONSTRUCTORS
template<typename Mesh , typename ElectroSolver>
EMSolver<Mesh, ElectroSolver>::EMSolver (const EMSolver& solver) :
    M_electroSolverPtr (solver.M_electroSolverPtr),
    M_activationModelPtr (solver.M_activationModelPtr),
    M_bcInterfacePtr        ( solver.M_bcInterfacePtr),
    M_EMStructuralOperatorPtr (solver.M_EMStructuralOperatorPtr),
    M_electroExporterPtr ( solver.M_electroExporterPtr),
    M_activationExporterPtr ( solver.M_activationExporterPtr),
    M_mechanicsExporterPtr  ( solver.M_mechanicsExporterPtr),
    M_localMeshPtr      ( solver.M_localMeshPtr),
    M_fullMeshPtr      ( solver.M_fullMeshPtr),
    M_activationTimePtr     ( solver.M_activationTimePtr),
    M_oneWayCoupling     ( solver.M_oneWayCoupling),
    M_wteTotal                   (solver.M_wteTotal),
//    M_wtePassive                   (solver.M_wtePassive),
//    M_wteActive                   (solver.M_wteActive),
    M_commPtr               ( solver.M_commPtr),
    M_data                   (solver.M_data)

{
}


/////////////////////
// Setting up the electrophysiology solver



/////////////////////
// Setting up the electrophysiology solver
template<typename Mesh , typename ElectroSolver>
void
EMSolver<Mesh, ElectroSolver>::setup ( GetPot& dataFile )
{
    M_data.setup (dataFile);
    
//    if (M_commPtr -> MyPID() == 0)
//    {
//        std::cout << "\n\nEMSolver: setup (simulation endtime = " << M_data.activationParameter<Real>("endtime") << ")" << " ...";
//    }
    
    setupElectroSolver ( dataFile );
    setupMechanicalSolver ( dataFile );
    setupActivation ( M_electroSolverPtr -> potentialPtr() ->map() );
    
    if (M_commPtr -> MyPID() == 0)
    {
        std::cout << "\nEMSolver: setup (simulation endtime = " << M_data.activationParameter<Real>("endtime") << ")" << " - done";
    }

}



template<typename Mesh , typename ElectroSolver>
void
EMSolver<Mesh, ElectroSolver>::setupElectroSolver ( GetPot& dataFile )
{
//    if (M_commPtr -> MyPID() == 0)
//    {
//        std::cout << "\nEMSolver: setupElectroSolver ... " << std::endl;
//    }
	ionicModelPtr_Type ionicModelPtr;
	std::string ionicModelName = M_data.electroParameter<std::string>("IonicModel");
	ionicModelPtr.reset (ionicModel_Type::IonicModelFactory::instance().createObject ( ionicModelName ) );

//    M_electroSolverPtr.reset( new ElectroSolver ( meshName, meshPath, dataFile , ionicModelPtr ) );

    M_electroSolverPtr.reset ( new ElectroSolver() );
    M_electroSolverPtr -> setIonicModelPtr (ionicModelPtr);
    M_electroSolverPtr->setParameters();
    //if (M_commPtr -> MyPID() == 0) M_electroSolverPtr ->showParameters();
	M_electroSolverPtr -> setParametersFromEMData ( M_data );
    //if (M_commPtr -> MyPID() == 0) M_electroSolverPtr ->showParameters();
    M_electroSolverPtr->init (M_localMeshPtr); //(M_commPtr);

//    if (M_localMeshPtr)
//    {
//        M_electroSolverPtr -> setLocalMeshPtr (M_localMeshPtr);
        if (M_fullMeshPtr)
        {
            M_electroSolverPtr -> setFullMeshPtr (M_fullMeshPtr);
        }
        M_electroSolverPtr ->  setup (dataFile, ionicModelPtr->Size() );
//    }
    
    M_activationTimePtr.reset (new vector_Type ( M_electroSolverPtr->potentialPtr() -> map() ));
    *M_activationTimePtr = -1.0;
    
    if (M_commPtr -> MyPID() == 0)
    {
        std::cout << "\nEMSolver: setupElectroSolver - done";
    }

}


/////////////////////
// Setting up the electrophysiology solver
template<typename Mesh , typename ElectroSolver>
void
EMSolver<Mesh, ElectroSolver>::setupMechanicalSolver ( GetPot& dataFile)
{
//    if (M_commPtr -> MyPID() == 0)
//    {
//        std::cout << "\nEMSolver: setupMechanicalSolver ... " << std::endl;
//    }
    
    boost::shared_ptr<StructuralConstitutiveLawData> dataStructure (new StructuralConstitutiveLawData( ) );
    dataStructure->setup (dataFile);

    std::string dOrder =  dataFile ( "solid/space_discretization/order", "P1");
    solidFESpacePtr_Type dFESpace ( new solidFESpace_Type (M_localMeshPtr, dOrder, 3, M_commPtr) );
    solidETFESpacePtr_Type dETFESpace ( new solidETFESpace_Type ( M_localMeshPtr,
                                                                  & (dFESpace->refFE() ),
                                                                  & (dFESpace->fe().geoMap() ),
                                                                  M_commPtr) );
    std::string data_file_name = dataFile.get (0, "NO_DATA_FILENAME_FOUND");

    dFESpace->setQuadRule(quadRuleTetra4pt);
    //if (M_commPtr -> MyPID() == 0) std::cout << "\nPolynomial degree: " << dFESpace->polynomialDegree();
    //if (M_commPtr -> MyPID() == 0) std::cout << "\nDof: " << dFESpace->dof() << std::endl;
    
    setupMechanicalBC (data_file_name, "solid",  dFESpace);
    M_EMStructuralOperatorPtr.reset (new structuralOperator_Type() );
    M_EMStructuralOperatorPtr->setup ( dataStructure,
                                       dFESpace,
                                       dETFESpace,
                                       M_bcInterfacePtr->handler(),
                                       M_commPtr);
    M_EMStructuralOperatorPtr->setDataFromGetPot (dataFile);
    M_EMStructuralOperatorPtr->EMMaterial()->setParameters(M_data, dataFile);

    M_wteTotal.setup(dataStructure, dFESpace, dETFESpace, M_commPtr, 0, M_EMStructuralOperatorPtr->EMMaterial());
//    M_wtePassive.setup(dataStructure, dFESpace, dETFESpace, M_commPtr, 0, "passive");
//    M_wteActive.setup(dataStructure, dFESpace, dETFESpace, M_commPtr, 0, "active");
    
    if (M_commPtr -> MyPID() == 0)
    {
        std::cout << "\nEMSolver: setupMechanicalSolver - done";
    }

}

/////////////////////
// Setting up the electrophysiology solver
template<typename Mesh , typename ElectroSolver>
void
EMSolver<Mesh, ElectroSolver>::setupMechanicalBC (std::string data_file_name,
                                                                   std::string section,
                                                                   solidFESpacePtr_Type dFESpace)
{
//    if (M_commPtr -> MyPID() == 0)
//    {
//        std::cout << "\nEMSolver: setupMechanicalBC ... " << std::endl;
//    }

    M_bcInterfacePtr.reset (new bcInterface_Type() );
    M_bcInterfacePtr->createHandler();
    M_bcInterfacePtr->fillHandler ( data_file_name, "solid" );
    // M_bcInterfacePtr->handler()->bcUpdate ( *dFESpace->mesh(), dFESpace->feBd(), dFESpace->dof() );
    
    if (M_commPtr -> MyPID() == 0)
    {
        std::cout << "\nEMSolver: setupMechanicalBC - done";
    }
}

    
/////////////////////
//restart hdf5
template<typename Mesh , typename ElectroSolver>
void
EMSolver<Mesh, ElectroSolver>::importHdf5 ()
{
    M_electroExporterPtr -> import (30);
    M_activationExporterPtr -> importHdf5 (30);
    //M_activationTimeExporterPtr -> importHdf5 (30);
    M_vonMisesStressExporterPtr -> importHdf5 (30);
    //M_vonMisesStressExporterPtrP -> importHdf5 (30);
    //M_vonMisesStressExporterPtrA -> importHdf5 (30);
    M_mechanicsExporterPtr -> importHdf5 (30);
}
                                                   

/////////////////////
//Setup exporters
template<typename Mesh , typename ElectroSolver>
void
EMSolver<Mesh, ElectroSolver>::setupExporters ( std::string problemFolder,
                                                std::string electroFileName,
                                                std::string activationFileName,
                                                //std::string activationTimeFileName,
                                                std::string mechanicsFileName,
                                                std::string vonMisesStressFileName)
{
//    if (M_commPtr -> MyPID() == 0)
//    {
//        std::cout << "\nEMSolver: setupExporters ... " << std::endl;
//    }
    
    // Electrophysiology
    M_electroExporterPtr.reset (new exporter_Type() );
    setupElectroExporter (problemFolder, electroFileName);
    M_electroExporterPtr -> addVariable ( ExporterData<RegionMesh<LinearTetra> >::VectorField,
                                            "fibers",
                                            M_EMStructuralOperatorPtr -> dispFESpacePtr(),
                                            M_electroSolverPtr -> fiberPtr(),
                                            UInt (0) );

    // Activation
    M_activationExporterPtr.reset (new exporter_Type() );
    setupActivationExporter (problemFolder, activationFileName );
    M_activationExporterPtr -> addVariable ( ExporterData<RegionMesh<LinearTetra> >::ScalarField,
                                             "Activation",
                                             M_electroSolverPtr -> feSpacePtr(),
                                             M_activationModelPtr -> fiberActivationPtr(),
                                             UInt (0) );

//    // Activation time
//    M_activationTimeExporterPtr.reset (new exporter_Type() );
//    setupActivationTimeExporter (problemFolder, activationTimeFileName );
//    
//    M_activationTimePtr.reset (new vector_Type ( M_electroSolverPtr->potentialPtr() -> map() ) );
//    *M_activationTimePtr = -1.0;
//
//    M_activationTimeExporterPtr -> addVariable ( ExporterData<RegionMesh<LinearTetra> >::ScalarField,
//                                            "Activation Time",
//                                            M_electroSolverPtr -> feSpacePtr(),
//                                            M_activationTimePtr,
//                                            UInt (0) );
    
    // Von Mises stress
    M_vonMisesStressExporterPtr.reset (new exporter_Type() );
    setupVonMisesStressExporter (problemFolder, vonMisesStressFileName );
    
    M_vonMisesStressExporterPtr -> addVariable ( ExporterData<RegionMesh<LinearTetra> >::ScalarField,
                                                "Von Mises Stress",
                                                M_electroSolverPtr -> feSpacePtr(),
                                                M_wteTotal.vonMisesStressPtr(),
                                                UInt (0) );
    
//    M_vonMisesStressExporterPtrP.reset (new exporter_Type() );
//    setupVonMisesStressExporterP (problemFolder, vonMisesStressFileNameP );
//    
//    M_vonMisesStressExporterPtrP -> addVariable ( ExporterData<RegionMesh<LinearTetra> >::ScalarField,
//                                                "Von Mises Stress Total P",
//                                                M_electroSolverPtr -> feSpacePtr(),
//                                                M_wteTotal.vonMisesStressPtr(),
//                                                UInt (0) );
//
//    M_vonMisesStressExporterPtrA.reset (new exporter_Type() );
//    setupVonMisesStressExporterA (problemFolder, vonMisesStressFileNameA );
//    
//    M_vonMisesStressExporterPtrA -> addVariable ( ExporterData<RegionMesh<LinearTetra> >::ScalarField,
//                                                "Von Mises Stress Total A",
//                                                M_electroSolverPtr -> feSpacePtr(),
//                                                M_wteTotal.vonMisesStressPtr(),
//                                                UInt (0) );

//    M_vonMisesStressExporterPtr -> addVariable ( ExporterData<RegionMesh<LinearTetra> >::VectorField,
//                                                "X Stress Total",
//                                                M_EMStructuralOperatorPtr -> dispFESpacePtr(),
//                                                M_wteTotal.sigmaXPtr(),
//                                                UInt (0) );
//
//    M_vonMisesStressExporterPtr -> addVariable ( ExporterData<RegionMesh<LinearTetra> >::VectorField,
//                                                "Y Stress Total",
//                                                M_EMStructuralOperatorPtr -> dispFESpacePtr(),
//                                                M_wteTotal.sigmaYPtr(),
//                                                UInt (0) );
//
//    M_vonMisesStressExporterPtr -> addVariable ( ExporterData<RegionMesh<LinearTetra> >::VectorField,
//                                                "Z Stress Total",
//                                                M_EMStructuralOperatorPtr -> dispFESpacePtr(),
//                                                M_wteTotal.sigmaZPtr(),
//                                                UInt (0) );
//
//    M_vonMisesStressExporterPtr -> addVariable ( ExporterData<RegionMesh<LinearTetra> >::ScalarField,
//                                                "Von Mises Stress Passive",
//                                                M_electroSolverPtr -> feSpacePtr(),
//                                                M_wtePassive.vonMisesStressPtr(),
//                                                UInt (0) );
//
//    M_vonMisesStressExporterPtr -> addVariable ( ExporterData<RegionMesh<LinearTetra> >::VectorField,
//                                                "X Stress Passive",
//                                                M_EMStructuralOperatorPtr -> dispFESpacePtr(),
//                                                M_wtePassive.sigmaXPtr(),
//                                                UInt (0) );
//
//    M_vonMisesStressExporterPtr -> addVariable ( ExporterData<RegionMesh<LinearTetra> >::VectorField,
//                                                "Y Stress Passive",
//                                                M_EMStructuralOperatorPtr -> dispFESpacePtr(),
//                                                M_wtePassive.sigmaYPtr(),
//                                                UInt (0) );
//
//    M_vonMisesStressExporterPtr -> addVariable ( ExporterData<RegionMesh<LinearTetra> >::VectorField,
//                                                "Z Stress Passive",
//                                                M_EMStructuralOperatorPtr -> dispFESpacePtr(),
//                                                M_wtePassive.sigmaZPtr(),
//                                                UInt (0) );
//
//    M_vonMisesStressExporterPtr -> addVariable ( ExporterData<RegionMesh<LinearTetra> >::ScalarField,
//                                                "Von Mises Stress Active",
//                                                M_electroSolverPtr -> feSpacePtr(),
//                                                M_wteActive.vonMisesStressPtr(),
//                                                UInt (0) );
//    
//    M_vonMisesStressExporterPtr -> addVariable ( ExporterData<RegionMesh<LinearTetra> >::VectorField,
//                                                "X Stress Active",
//                                                M_EMStructuralOperatorPtr -> dispFESpacePtr(),
//                                                M_wteActive.sigmaXPtr(),
//                                                UInt (0) );
//
//    M_vonMisesStressExporterPtr -> addVariable ( ExporterData<RegionMesh<LinearTetra> >::VectorField,
//                                                "Y Stress Active",
//                                                M_EMStructuralOperatorPtr -> dispFESpacePtr(),
//                                                M_wteActive.sigmaYPtr(),
//                                                UInt (0) );
//    
//    M_vonMisesStressExporterPtr -> addVariable ( ExporterData<RegionMesh<LinearTetra> >::VectorField,
//                                                "Z Stress Active",
//                                                M_EMStructuralOperatorPtr -> dispFESpacePtr(),
//                                                M_wteActive.sigmaZPtr(),
//                                                UInt (0) );
    
    // Mechanics
    M_mechanicsExporterPtr.reset (new exporter_Type() );
    setupMechanicsExporter (problemFolder, mechanicsFileName);
    M_mechanicsExporterPtr -> addVariable ( ExporterData<RegionMesh<LinearTetra> >::VectorField,
                                            "displacement",
                                            M_EMStructuralOperatorPtr -> dispFESpacePtr(),
                                            M_EMStructuralOperatorPtr -> displacementPtr(),
                                            UInt (0) );
    M_mechanicsExporterPtr -> addVariable ( ExporterData<RegionMesh<LinearTetra> >::VectorField,
                                            "fibers",
                                            M_EMStructuralOperatorPtr -> dispFESpacePtr(),
                                            M_EMStructuralOperatorPtr -> EMMaterial() -> fiberVectorPtr(),
                                            UInt (0) );
    M_mechanicsExporterPtr -> addVariable ( ExporterData<RegionMesh<LinearTetra> >::VectorField,
                                            "sheets",
                                            M_EMStructuralOperatorPtr -> dispFESpacePtr(),
                                            M_EMStructuralOperatorPtr -> EMMaterial() -> sheetVectorPtr(),
                                            UInt (0) );
    
    if (M_commPtr -> MyPID() == 0)
    {
        std::cout << "\nEMSolver: setupExporters - done";
        std::cout << "\n\n";
    }
}

template<typename Mesh , typename ElectroSolver>
void
EMSolver<Mesh, ElectroSolver>::setTimeIndex (const UInt& time)
{
    M_electroExporterPtr -> setTimeIndex (time);
    M_activationExporterPtr -> setTimeIndex (time);
    //M_activationTimeExporterPtr -> setTimeIndex (time);
    M_vonMisesStressExporterPtr -> setTimeIndex (time);
    //M_vonMisesStressExporterPtrP -> setTimeIndex (time);
    //M_vonMisesStressExporterPtrA -> setTimeIndex (time);
    M_mechanicsExporterPtr -> setTimeIndex (time);
}
    
template<typename Mesh , typename ElectroSolver>
void
EMSolver<Mesh, ElectroSolver>::saveSolution (Real time, const bool& restart)
{
    M_wteTotal.setDisplacement ( M_EMStructuralOperatorPtr -> displacement() );
    M_wteTotal.analyzeTensionsRecoveryVonMisesStress();
    
    M_vonMisesStressExporterPtr -> postProcess (time);
    M_electroExporterPtr -> postProcess (time);//, restart);
    M_activationExporterPtr -> postProcess (time);//, restart );
    //M_activationTimeExporterPtr -> postProcess (time);
    M_mechanicsExporterPtr -> postProcess (time);//, restart);
}

template<typename Mesh , typename ElectroSolver>
void
EMSolver<Mesh, ElectroSolver>::closeExporters()
{
    M_electroExporterPtr -> closeFile();
    M_activationExporterPtr -> closeFile();
    //M_activationTimeExporterPtr -> closeFile();
    M_mechanicsExporterPtr -> closeFile();
    M_vonMisesStressExporterPtr -> closeFile();
}


///////////////////////////////
template<typename Mesh , typename ElectroSolver>
void
EMSolver<Mesh, ElectroSolver>::oneWayCoupling()
{
    M_electroSolverPtr -> setMechanicsModifiesConductivity (false);
    M_electroSolverPtr -> displacementPtr().reset();
    M_activationModelPtr -> setupActivationPtrs( M_EMStructuralOperatorPtr -> EMMaterial() -> fiberActivationPtr(),
												 M_EMStructuralOperatorPtr -> EMMaterial() -> sheetActivationPtr(),
												 M_EMStructuralOperatorPtr -> EMMaterial() -> normalActivationPtr()
												 );
}

template<typename Mesh , typename ElectroSolver>
void
EMSolver<Mesh, ElectroSolver>::twoWayCoupling()
{
    M_electroSolverPtr -> setMechanicsModifiesConductivity (true);
    M_electroSolverPtr -> setDisplacementPtr ( M_EMStructuralOperatorPtr -> displacementPtr() );
    M_activationModelPtr -> setupActivationPtrs( M_EMStructuralOperatorPtr -> EMMaterial() -> fiberActivationPtr(),
												 M_EMStructuralOperatorPtr -> EMMaterial() -> sheetActivationPtr(),
												 M_EMStructuralOperatorPtr -> EMMaterial() -> normalActivationPtr()
												 );
}

////////////////////////////
template<typename Mesh , typename ElectroSolver>
void
EMSolver<Mesh, ElectroSolver>::solveElectrophysiology (function_Type& stimulus, Real time )
{
//    if (M_commPtr -> MyPID() == 0)
//    {
//        std::cout << "\nEMSolver: solveElectrophysiology ... " << std::endl;
//    }
    
    setAppliedCurrent ( stimulus, time );

    M_electroSolverPtr -> solveOneStepGatingVariablesFE();
    //M_electroSolverPtr -> solveOneStepGatingVariablesRL();
    
    M_electroSolverPtr -> solveOneICIStep();
    
    M_electroSolverPtr -> registerActivationTime (*M_activationTimePtr, time, 0.9);
    
    if (M_commPtr -> MyPID() == 0)
    {
        std::cout << "\nEMSolver: solveElectrophysiology - done";
    }
}

template<typename Mesh , typename ElectroSolver>
void
EMSolver<Mesh, ElectroSolver>::solveActivation (Real dt)
    
{
//    if (M_commPtr -> MyPID() == 0)
//    {
//        std::cout << "\nEMSolver: solveActivation ... " << std::endl;
//    }
    
    computeI4f (M_activationModelPtr->I4f(), *M_EMStructuralOperatorPtr->EMMaterial()->fiberVectorPtr(), *M_EMStructuralOperatorPtr->displacementPtr(), M_EMStructuralOperatorPtr->dispFESpacePtr());

    M_activationModelPtr -> solveModelPathology ( dt, M_fullMeshPtr, M_EMStructuralOperatorPtr -> dispFESpacePtr() );
    
    if (M_commPtr -> MyPID() == 0)
    {
        std::cout << "\nEMSolver: solveActivation - done\n";
    }
}

template<typename Mesh , typename ElectroSolver>
void
EMSolver<Mesh, ElectroSolver>::computeI4f (VectorEpetra& i4f, VectorEpetra& f0_, VectorEpetra& disp, solidFESpacePtr_Type feSpacePtr)
{
    VectorEpetra dUdx (disp);
    VectorEpetra dUdy (disp);
    VectorEpetra dUdz (disp);
    
    dUdx = GradientRecovery::ZZGradient (feSpacePtr, disp, 0);
    dUdy = GradientRecovery::ZZGradient (feSpacePtr, disp, 1);
    dUdz = GradientRecovery::ZZGradient (feSpacePtr, disp, 2);
    
    int n = i4f.epetraVector().MyLength();
    int i (0); int j (0); int k (0);
    MatrixSmall<3,3> F; VectorSmall<3> f0;
    
    for (int p (0); p < n; p++)
    {
        i = dUdx.blockMap().GID (p);
        j = dUdx.blockMap().GID (p + n);
        k = dUdx.blockMap().GID (p + 2 * n);
        
//        F(0,0) = 1.0 + dUdx[i];
//        F(0,1) =       dUdy[i];
//        F(0,2) =       dUdz[i];
//        F(1,0) =       dUdx[j];
//        F(1,1) = 1.0 + dUdy[j];
//        F(1,2) =       dUdz[j];
//        F(2,0) =       dUdx[k];
//        F(2,1) =       dUdy[k];
//        F(2,2) = 1.0 + dUdz[k];
        
        F(0,0) = 1.0 + dUdx[i];
        F(0,1) =       dUdx[j];
        F(0,2) =       dUdx[k];
        F(1,0) =       dUdy[i];
        F(1,1) = 1.0 + dUdy[j];
        F(1,2) =       dUdy[k];
        F(2,0) =       dUdz[i];
        F(2,1) =       dUdz[j];
        F(2,2) = 1.0 + dUdz[k];
        
        f0(0) = f0_[i];
        f0(1) = f0_[j];
        f0(2) = f0_[k];
        
        f0.normalize();
        
        auto f = F * f0;
        i4f[i] = f.dot(f);
    }
}

    
template<typename Mesh , typename ElectroSolver>
void
EMSolver<Mesh, ElectroSolver>::computeDeformedFiberDirection (VectorEpetra& f_, VectorEpetra& f0_, VectorEpetra& disp, solidFESpacePtr_Type feSpacePtr)
{
//    f_ = VectorEpetra(disp, Unique);
    
    VectorEpetra dUdx (Unique);
    VectorEpetra dUdy (disp.map());
    VectorEpetra dUdz (disp.map());
    
    dUdx = GradientRecovery::ZZGradient (feSpacePtr, disp, 0);
    dUdy = GradientRecovery::ZZGradient (feSpacePtr, disp, 1);
    dUdz = GradientRecovery::ZZGradient (feSpacePtr, disp, 2);
    
    f_ = dUdx;
    return;
    
    int n = f_.epetraVector().MyLength() / 3;
    int i (0); int j (0); int k (0);
    MatrixSmall<3,3> F; VectorSmall<3> f0;
    
    for (int p (0); p < n; p++)
    {
        i = dUdx.blockMap().GID (p);
        j = dUdx.blockMap().GID (p + n);
        k = dUdx.blockMap().GID (p + 2 * n);
        
//        F(0,0) = 1.0 + dUdx[i];
//        F(0,1) =       dUdy[i];
//        F(0,2) =       dUdz[i];
//        F(1,0) =       dUdx[j];
//        F(1,1) = 1.0 + dUdy[j];
//        F(1,2) =       dUdz[j];
//        F(2,0) =       dUdx[k];
//        F(2,1) =       dUdy[k];
//        F(2,2) = 1.0 + dUdz[k];
  
        F(0,0) = 1.0 + dUdx[i];
        F(0,1) =       dUdx[j];
        F(0,2) =       dUdx[k];
        F(1,0) =       dUdy[i];
        F(1,1) = 1.0 + dUdy[j];
        F(1,2) =       dUdy[k];
        F(2,0) =       dUdz[i];
        F(2,1) =       dUdz[j];
        F(2,2) = 1.0 + dUdz[k];
        
        f0(0) = f0_[i];
        f0(1) = f0_[j];
        f0(2) = f0_[k];
        
        f0.normalize();
        
        auto f = F * f0;
        f_[i] = f(0);
        f_[j] = f(1);
        f_[k] = f(2);
    }
}


} // namespace LifeV


#endif //_MONODOMAINSOLVER_H_
