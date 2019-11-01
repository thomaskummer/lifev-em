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
 *  @file
 *  @brief This file contains an abstract class to implement different kinds of materials for structural dynamic problems (St. Venant-Kirchhoff, Neo-Hookean and Exponential materials right now )
 *
 *  @version 1.0
 *  @date 04-2014
 *  @author Simone Rossi <simone.rossi@epfl.ch>
 */

#ifndef _EMSTRUCTURALCONSTITUTIVELAW_H_
#define _EMSTRUCTURALCONSTITUTIVELAW_H_ 1

#include <boost/typeof/typeof.hpp>
#include <lifev/core/array/MatrixSmall.hpp>

#include <lifev/core/filter/ExporterHDF5.hpp>
#include <lifev/structure/solver/StructuralConstitutiveLaw.hpp>
//#include <lifev/em/solver/mechanics/materials/EMMaterial.hpp>
//#include <lifev/em/solver/mechanics/EMStructuralConstitutiveLaw.hpp>
#include <lifev/em/solver/mechanics/materials/EMMaterialType.hpp>
//#include <lifev/em/solver/mechanics/materials/EMActiveStressMaterial.hpp>
//#include <lifev/em/solver/mechanics/materials/EMActiveStrainMaterial.hpp>

//#include <lifev/em/util/EMUtility.hpp>
#include <lifev/electrophysiology/util/HeartUtility.hpp>
#include <lifev/em/solver/mechanics/materials/MaterialsList.hpp>
//#include <lifev/em/solver/mechanics/materials/functions/FunctionsList.hpp>

//#include <lifev/em/solver/mechanics/EMMechanicalExpressions.hpp>
#include <lifev/em/solver/EMETAFunctors.hpp>


namespace LifeV
{
    
/*!
  \class EMStructuralConstitutiveLaw
  \brief
  This class is an abstract class to define different type of models for the arterial wall.
  This class has just pure virtual methods. They are implemented in the specific class for one material
*/

//forward declaration
class EMData;

template <typename MeshType>
class EMStructuralConstitutiveLaw  : public StructuralConstitutiveLaw<MeshType>
{
public:

    //!@name Type definitions
    //@{
    typedef typename boost::shared_ptr<const Displayer>   displayerPtr_Type;
    typedef StructuralConstitutiveLaw<MeshType> super;
    typedef MatrixEpetra<Real>            matrix_Type;
    typedef boost::shared_ptr<matrix_Type>         matrixPtr_Type;
    typedef VectorEpetra           vector_Type;
    typedef boost::shared_ptr<vector_Type>         vectorPtr_Type;

    typedef ETFESpace<MeshType, MapEpetra, 3, 3 >         ETFESpace_Type;
    typedef boost::shared_ptr<ETFESpace_Type>             ETFESpacePtr_Type;

    typedef FESpace< MeshType, MapEpetra >                FESpace_Type;
    typedef boost::shared_ptr<FESpace_Type>               FESpacePtr_Type;

    typedef MeshType                                        mesh_Type;
    typedef ETFESpace< mesh_Type, MapEpetra, 3, 1 >                        scalarETFESpace_Type;
    typedef boost::shared_ptr<ETFESpace< mesh_Type, MapEpetra, 3, 1 > >    scalarETFESpacePtr_Type;

    typedef MapEpetra map_Type;
    typedef boost::shared_ptr<map_Type>             mapPtr_Type;

    //    typedef EMMaterial<MeshType>                              material_Type;

    typedef StructuralConstitutiveLawData          data_Type;
    typedef typename boost::shared_ptr<data_Type>  dataPtr_Type;

    //    typedef FactorySingleton<Factory<StructuralConstitutiveLaw<MeshType>, std::string> >  StructureMaterialFactory;

    typedef std::vector< typename MeshType::element_Type* > vectorVolumes_Type;

    typedef std::map< UInt, vectorVolumes_Type>           mapMarkerVolumes_Type;
    typedef boost::shared_ptr<mapMarkerVolumes_Type>      mapMarkerVolumesPtr_Type;

    typedef std::vector<UInt>                             vectorIndexes_Type;
    typedef std::map< UInt, vectorIndexes_Type>           mapMarkerIndexes_Type;
    typedef boost::shared_ptr<mapMarkerIndexes_Type>      mapMarkerIndexesPtr_Type;

    typedef EMMaterialType<MeshType>                      material_Type;
    typedef boost::shared_ptr<material_Type>              materialPtr_Type;

    typedef EMPassiveMaterialType<MeshType>               passiveMaterial_Type;
    typedef boost::shared_ptr<passiveMaterial_Type>       passiveMaterialPtr_Type;

    typedef EMActiveMaterialType<MeshType>                activeMaterial_Type;
    typedef boost::shared_ptr<activeMaterial_Type>        activeMaterialPtr_Type;

    
    
//    typedef StructuralConstitutiveLaw<Mesh>                 super;
//    
//    typedef typename super::data_Type                data_Type;
//    
//    typedef typename super::vector_Type              vector_Type;
//    typedef typename super::matrix_Type              matrix_Type;
//    
//    typedef typename super::matrixPtr_Type           matrixPtr_Type;
//    typedef typename super::vectorPtr_Type           vectorPtr_Type;
//    typedef typename super::dataPtr_Type             dataPtr_Type;
//    typedef typename super::displayerPtr_Type        displayerPtr_Type;
    
//    typedef typename super::mapMarkerVolumesPtr_Type mapMarkerVolumesPtr_Type;
//    typedef typename super::mapMarkerVolumes_Type mapMarkerVolumes_Type;
    typedef typename mapMarkerVolumes_Type::const_iterator mapIterator_Type;
    
    
    
    //@}



    //! @name Constructor &  Deconstructor
    //@{

    //! Empty constructor
    EMStructuralConstitutiveLaw();
    //! Destructor
    virtual ~EMStructuralConstitutiveLaw() {}

    //@}

    //getters


    //SOME METHODS FOR ACTIVATED MATERIALS
    inline void setFiberVector ( const vector_Type& fiberVector)
    {
        M_fiberVectorPtr.reset ( new vector_Type ( fiberVector ) );
        ElectrophysiologyUtility::normalize (*M_fiberVectorPtr);
    }

    inline void setSheetVector ( const vector_Type& sheetVector)
    {
        M_sheetVectorPtr.reset ( new vector_Type ( sheetVector ) );
        ElectrophysiologyUtility::normalize (*M_sheetVectorPtr);
    }

    inline void setFiberVectorPtr ( const vectorPtr_Type fiberVectorPtr)
    {
        M_fiberVectorPtr = fiberVectorPtr;
    }

    inline void setSheetVectorPtr ( const vectorPtr_Type sheetVectorPtr)
    {
        M_sheetVectorPtr = sheetVectorPtr;
    }



    inline  vectorPtr_Type const fiberVectorPtr() const
    {
        return M_fiberVectorPtr;
    }

    inline  vectorPtr_Type fiberVectorPtr()
    {
        return M_fiberVectorPtr;
    }

    inline  vectorPtr_Type const sheetVectorPtr() const
    {
        return M_sheetVectorPtr;
    }

    inline  vectorPtr_Type sheetVectorPtr()
    {
        return M_sheetVectorPtr;
    }

    inline  scalarETFESpacePtr_Type scalarETFESpacePtr()
    {
        return M_scalarETFESpacePtr;
    }


    inline void setupFiberVector ( std::string& name, boost::shared_ptr<mesh_Type> mesh )
    {
        ElectrophysiologyUtility::importFibers ( M_fiberVectorPtr, name, mesh  );
        ElectrophysiologyUtility::normalize (*M_fiberVectorPtr);
    }

    inline void setupFiberVector ( std::string& name, std::string& path )
    {
        /* These two lines are giving compilation errors that I have not been able to solve.
           The function importFibers is not defined in the ElectroPhysiologyUtility or the
           Heartutility. Paolo Tricerri, June, 10th, 2014
        */
       //ElectrophysiologyUtility::importFibers ( M_fiberVectorPtr, name, path  );
        //ElectrophysiologyUtility::normalize (*M_fiberVectorPtr);
    }

    void setupFiberVector ( Real fx, Real fy, Real fz )
    {
        ElectrophysiologyUtility::setupFibers ( *M_fiberVectorPtr, fx, fy, fz  );
        ElectrophysiologyUtility::normalize (*M_fiberVectorPtr);
    }

    inline void setupSheetVector ( std::string& name, boost::shared_ptr<mesh_Type> mesh )
    {
        ElectrophysiologyUtility::importFibers ( M_sheetVectorPtr, name, mesh  );
        ElectrophysiologyUtility::normalize (*M_sheetVectorPtr);
    }

    inline void setupSheetVector ( std::string& name, std::string& path )
    {
        /* These two lines are giving compilation errors that I have not been able to solve.
           The function importFibers is not defined in the ElectroPhysiologyUtility or the
           Heartutility. Paolo Tricerri, June, 10th, 2014
        */
        //ElectrophysiologyUtility::importFibers ( M_sheetVectorPtr, name, path  );
        //ElectrophysiologyUtility::normalize (*M_sheetVectorPtr);
    }

    void setupSheetVector ( Real sx, Real sy, Real sz )
    {
        ElectrophysiologyUtility::setupFibers ( *M_sheetVectorPtr, sx, sy, sz);
        ElectrophysiologyUtility::normalize (*M_sheetVectorPtr);
    }

    inline virtual vectorPtr_Type& fiberActivationPtr()
    {
    	return M_fiberActivationPtr;
    }
    inline virtual vectorPtr_Type& sheetActivationPtr()
    {
    	return M_sheetActivationPtr;
    }
    inline virtual vectorPtr_Type& normalActivationPtr()
    {
    	return M_normalActivationPtr;
    }


    //! Setup the created object of the class StructuralConstitutiveLaw
    /*!
      \param dFespace: the FiniteElement Space
      \param monolithicMap: the MapEpetra
      \param offset: the offset parameter used assembling the matrices
    */
    virtual void setup ( const FESpacePtr_Type& dFESpace,
                         const ETFESpacePtr_Type& ETFESpace,
                         const boost::shared_ptr<const MapEpetra>&   monolithicMap,
                         const UInt offset, const dataPtr_Type& dataMaterial,
                         const displayerPtr_Type& displayer  );


    //! Computes the linear part of the stiffness matrix StructuralSolver::buildSystem
    /*!
      \param dataMaterial the class with Material properties data
    */
    virtual  void computeLinearStiff ( dataPtr_Type& dataMaterial,
                                       const mapMarkerVolumesPtr_Type /*mapsMarkerVolumes*/,
                                       const mapMarkerIndexesPtr_Type /*mapsMarkerIndexes*/ ) {}

    //! Updates the Jacobian matrix in StructuralSolver::updateJacobian
    /*!
      \param disp: solution at the k-th iteration of NonLinearRichardson Method
      \param dataMaterial: a pointer to the dataType member in StructuralSolver class to get the
                           material coefficients (e.g. Young modulus, Poisson ratio..)
      \param displayer: a pointer to the Dysplaier member in the StructuralSolver class
    */
    virtual  void updateJacobianMatrix ( const vector_Type& disp, const dataPtr_Type& dataMaterial,
                                                const mapMarkerVolumesPtr_Type mapsMarkerVolumes,
                                                const mapMarkerIndexesPtr_Type mapsMarkerIndexes,
                                                const displayerPtr_Type& displayer );

    //! Computes the new Stiffness matrix in StructuralSolver given a certain displacement field.
    //! This function is used both in StructuralSolver::evalResidual and in
    //! StructuralSolver::updateSystem since the matrix is the expression of the matrix is the same.
    //!This is virtual and not pure virtual since in the linear St. Venant-Kirchhoff law it is not needed.
    /*!
      \param sol:  the solution vector
      \param factor: scaling factor used in FSI
      \param dataMaterial: a pointer to the dataType member in StructuralSolver class to get the
                           material coefficients (e.g. Young modulus, Poisson ratio..)
      \param displayer: a pointer to the Dysplaier member in the StructuralSolver class
    */
    inline virtual  void computeStiffness ( const vector_Type& disp, Real factor, const dataPtr_Type& dataMaterial,
                                            const mapMarkerVolumesPtr_Type mapsMarkerVolumes,
                                            const mapMarkerIndexesPtr_Type mapsMarkerIndexes,
                                            const displayerPtr_Type& displayer );
    


    //! Computes the deformation Gradient F, the cofactor of F Cof(F),
    //! the determinant of F J = det(F), the trace of C Tr(C).
    /*!
      \param dk_loc: local displacement vector
    */
    inline virtual  void computeKinematicsVariables ( const VectorElemental& dk_loc ) {}
    
    
    
    //! Output of the class
    /*!
       \param fileNamelinearStiff the filename where to apply the spy method for the linear part of the Stiffness matrix
       \param fileNameStiff the filename where to apply the spy method for the Stiffness matrix
    */
    virtual void showMe ( std::string const& fileNameStiff, std::string const& fileNameJacobian ) {}


    //! Compute the First Piola Kirchhoff Tensor
    /*!
       \param firstPiola Epetra_SerialDenseMatrix that has to be filled
       \param tensorF Epetra_SerialDenseMatrix the deformation gradient
       \param cofactorF Epetra_SerialDenseMatrix cofactor of F
       \param invariants std::vector with the invariants of C and the detF
       \param material UInt number to get the material parameteres form the VenantElasticData class
    */
    inline virtual void computeLocalFirstPiolaKirchhoffTensor ( Epetra_SerialDenseMatrix& firstPiola,
                                                                const Epetra_SerialDenseMatrix& tensorF,
                                                                const Epetra_SerialDenseMatrix& cofactorF,
                                                                const std::vector<Real>& invariants,
                                                                const UInt material)
    {
        
    }
    
    
    
    Epetra_SerialDenseVector matrixTimesVector( const Epetra_SerialDenseMatrix& A, const Epetra_SerialDenseVector& x ) const
    {
        Epetra_SerialDenseVector y (3);
        for (UInt i (0); i < 3; ++i)
        {
            for (UInt j (0); j < 3; ++j)
            {
                y(i) += A(i,j) * x(j);
            }
        }
        return y;
    }
    
    
    Epetra_SerialDenseMatrix scalarTimesMatrix( const double& s, const Epetra_SerialDenseMatrix& A ) const
    {
        Epetra_SerialDenseMatrix Y (3,3);
        for (UInt i (0); i < 3; ++i)
        {
            for (UInt j (0); j < 3; ++j)
            {
                Y(i,j) = s * A(i,j);
            }
        }
        return Y;
    }
    
    
    Epetra_SerialDenseMatrix matrixTimeMatrix( const Epetra_SerialDenseMatrix& A, const Epetra_SerialDenseMatrix& X ) const
    {
        Epetra_SerialDenseMatrix Y (3,3);
        for (UInt i (0); i < 3; ++i)
        {
            for (UInt j (0); j < 3; ++j)
            {
                for (UInt k (0); k < 3; ++k)
                {
                    Y(i,j) = A(i,k) * X(k,j);
                }
            }
        }
        return Y;
    }
    
    
    Epetra_SerialDenseMatrix tensorProduct( const Epetra_SerialDenseVector& v, const Epetra_SerialDenseVector& w ) const
    {
        Epetra_SerialDenseMatrix m (3,3);
        for (UInt i (0); i < 3; ++i)
        {
            for (UInt j (0); j < 3; ++j)
            {
                m(i,j) = v(i) * w(j);
            }
        }
        return m;
    }
    
    
    Epetra_SerialDenseVector crossProduct( const Epetra_SerialDenseVector& v, const Epetra_SerialDenseVector& w ) const
    {
        Epetra_SerialDenseVector y (3);
        y(0) = v(1)*w(2) - v(2)*w(1);
        y(1) = v(2)*w(0) - v(0)*w(2);
        y(1) = v(0)*w(1) - v(2)*w(0);
        return y;
    }
    
    
    inline virtual void computeLocalFirstPiolaKirchhoffTensor_ ( Epetra_SerialDenseMatrix& firstPiola,
                                                               const Epetra_SerialDenseMatrix& tensorF,
                                                               const Epetra_SerialDenseMatrix& cofactorF,
                                                               const Epetra_SerialDenseVector& fiber,
                                                               const Epetra_SerialDenseVector& sheet,
                                                               const std::vector<Real>& invariants,
                                                               const UInt material)
    {
        Epetra_SerialDenseMatrix I (3,3);
        I(0,0) = 1.; I(0,1) = 0., I(0,2) = 0.;
        I(1,0) = 0.; I(1,1) = 1., I(1,2) = 0.;
        I(2,0) = 0.; I(2,1) = 0., I(2,2) = 1.;
        
        auto I1 = invariants[0];
        auto J = invariants[1];
        auto I4f = invariants[2];
        auto I4s = invariants[3];
        auto I8fs = invariants[4];
        
        auto gammaf = invariants[5];
        auto gamman = 4.0 * gammaf;
        auto gammas = 1.0 / ( (1.0+gammaf)*(1.0+gamman) ) - 1.0;
        
        auto g1 = 1 - ( gamman * ( gamman + 2 ) / std::pow( gamman+1 , 2.0 ) );
        auto g4f = ( gamman * ( gamman + 2 ) / std::pow( gamman+1 , 2.0 ) ) - ( gammaf * ( gammaf + 2 ) / std::pow( gammaf+1 , 2.0 ) );
        auto g4s = ( gamman * ( gamman + 2 ) / std::pow( gamman+1 , 2.0 ) ) - ( gammas * ( gammas + 2 ) / std::pow( gammas+1 , 2.0 ) );
        
        auto I1E = g1 * I1 + g4f * I4f + g4s * I4s;
        auto JE = J;
        auto I1barE = std::pow(JE, -2.0/3.0 ) * I1E;
        auto I4fE = I4f / std::pow( gammaf + 1 , 2.0 );
        auto I4sE = I4s / std::pow( gammas + 1 , 2.0 );
        auto I8fsE = I8fs / ( (gammaf + 1) * (gammas + 1) );
        
        auto W1E = 0.5 * 3300 * std::exp( 9.242 * (I1barE - 3) );
        auto W4fE = 185350 * (I4fE - 1) * std::exp( 15.972 * std::pow(I4fE - 1, 2.0) ) * (I4fE > 1.0);
        auto W4sE = 25640 * (I4sE - 1) * std::exp (10.446 * std::pow(I4sE - 1, 2.0) ) * (I4sE > 1.0);
        auto W8fsE = 4170 * I8fsE * std::exp ( 11.602 * I8fsE * I8fsE );

        auto normal = crossProduct(fiber, sheet);
        auto f = matrixTimesVector(tensorF, fiber);
        auto s = matrixTimesVector(tensorF, sheet);
        auto f_f0 = tensorProduct(f, fiber);
        auto s_s0 = tensorProduct(s, sheet);
        auto f_s0 = tensorProduct(f, sheet);
        auto s_f0 = tensorProduct(s, fiber);

        
//        Epetra_SerialDenseMatrix FAinv (3,3);
//        FAinv += I;
//        FAinv += scalarTimesMatrix(- gammaf/(gammaf+1), tensorProduct(fiber, fiber));
//        FAinv += scalarTimesMatrix(- gammas/(gammas+1), tensorProduct(sheet, sheet));
//        FAinv += scalarTimesMatrix(- gamman/(gamman+1), tensorProduct(normal, normal));
//        auto FE = matrixTimeMatrix(tensorF, FAinv);
        
        
        // Pvol
        Epetra_SerialDenseMatrix Pvol (3,3);
        Pvol.Scale(0.0);
        Pvol += cofactorF;
        Pvol.Scale( J * (3500000 / 2.0) * (J - 1.0 + (1.0 / J) * std::log(J) ) );

        // P1
        Epetra_SerialDenseMatrix P1 (3,3);
        P1.Scale(0.0);
        P1 += cofactorF;
        P1.Scale(-I1/3);
        P1 += tensorF;
        P1.Scale( 2.0 * g1 * W1E * std::pow(J, -2.0/3.0 ) );

        // P4f
        Epetra_SerialDenseMatrix P4f (3,3);
        P4f.Scale(0.0);
        P4f += f_f0;
        P4f.Scale ( 2.0 * ( g4f * W1E + W4fE / std::pow( gammaf + 1.0 , 2.0 ) ) );

        // P4s
        Epetra_SerialDenseMatrix P4s (3,3);
        P4s.Scale(0.0);
        P4s += s_s0;
        P4s.Scale ( 2 * ( g4s * W1E + W4sE / std::pow( gammas + 1.0 , 2.0 ) ) );

        // P8fs
        Epetra_SerialDenseMatrix P8fs (3,3);
        P8fs.Scale(0.0);
        P8fs += f_s0;
        P8fs += s_f0;
        P8fs.Scale( W8fsE / ( (gammaf + 1.0) * (gammas + 1.0) ) );

        // Assemble first piola kirchhoff tensor
        firstPiola.Scale(0.0);
        firstPiola += Pvol;
        firstPiola += P1;
        firstPiola += P4f;
        firstPiola += P4s;
        firstPiola += P8fs;
    }

    
    //! Get the Stiffness matrix
    matrixPtr_Type const stiffMatrix() const
    {
        return super::M_jacobian;
    }

    //! Get the stiffness vector
    vectorPtr_Type const stiffVector() const
    {
        return M_residualVectorPtr;
    }

    virtual void apply ( const vector_Type& sol, vector_Type& res,
                         const mapMarkerVolumesPtr_Type mapsMarkerVolumes,
                         const mapMarkerIndexesPtr_Type mapsMarkerIndexes) {};



//    inline vectorPtr_Type activationPtr()
//    {
//        return M_fiberActivationPtr;
//    }

    inline void setFiberActivationPtr (vectorPtr_Type activationPtr)
    {
        M_fiberActivationPtr = activationPtr;
    }
    inline void setFiberActivation (vector_Type& activation)
    {
        *M_fiberActivationPtr = activation;
    }
    inline void setSheetActivationPtr (vectorPtr_Type activationPtr)
    {
        M_sheetActivationPtr = activationPtr;
    }
    inline void setSheetActivation (vector_Type& activation)
    {
        *M_sheetActivationPtr = activation;
    }
    inline void setNormalActivationPtr (vectorPtr_Type activationPtr)
    {
        M_normalActivationPtr = activationPtr;
    }
    inline void setNormalActivation (vector_Type& activation)
    {
        *M_normalActivationPtr = activation;
    }



    void setParameters(EMData& data, GetPot& dataFile);


    void showMaterialParameters()
    {
    	if(M_passiveMaterialPtr) M_passiveMaterialPtr-> showMe();
    	if(M_activeStressMaterialPtr) M_activeStressMaterialPtr-> showMe();
    }

    //@}

    
    

protected:
    virtual void setupVectorsParameters ( void ) {}

    scalarETFESpacePtr_Type                        M_scalarETFESpacePtr;

    vectorPtr_Type                                 M_fiberVectorPtr;

    vectorPtr_Type                                 M_sheetVectorPtr;

    vectorPtr_Type                                 M_residualVectorPtr;

    passiveMaterialPtr_Type                        M_passiveMaterialPtr;
    activeMaterialPtr_Type                         M_activeStressMaterialPtr;

    vectorPtr_Type                                 M_fiberActivationPtr;
    vectorPtr_Type                                 M_sheetActivationPtr;
    vectorPtr_Type                                 M_normalActivationPtr;

    
    
    
    
    //! Local stress vector
    boost::scoped_ptr<VectorElemental>                  M_elvecK;
    
    //! Elementary matrices
    boost::scoped_ptr<MatrixElemental>                  M_elmatK;
    
    //! Vector: stiffness non-linear
    boost::shared_ptr<boost::multi_array<Real, 3> >     M_CofFk;

    //vectorPtr_Type                                    M_stiff;
    
    //! First Piola-Kirchhoff stress tensor
    vectorPtr_Type                                      M_FirstPiolaKStress;
    

    
    
    class OrthonormalizeVector
    {
    public:
        typedef LifeV::VectorSmall<3> return_Type;
        
        return_Type operator() (const VectorSmall<3>& v)
        {
            return normalize(v, 0);
        }
        
        return_Type operator() (const VectorSmall<3>& v, const VectorSmall<3>& w)
        {
            auto f (v);
            auto s (w);
            
            s = normalize(s, 1);
            s = s - s.dot (f) * f;
            s = normalize(s, 2);
            
            return s;
        }
        
        return_Type normalize(const VectorSmall<3>& v, const UInt& comp)
        {
            auto V (v);
            Real norm = std::sqrt (v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
//            if ( norm >= 1e-13 )
            {
                V[0] = v[0] / norm;
                V[1] = v[1] / norm;
                V[2] = v[2] / norm;
            }
//            else
//            {
//                V *= 0.0;
//                V[comp] = 1.0;
//            }
            return V;
        }
        
        OrthonormalizeVector () {}
        ~OrthonormalizeVector () {}
    };
    
    
    class CrossProduct
    {
    public:
        typedef LifeV::VectorSmall<3> return_Type;
        
        return_Type operator() (const LifeV::VectorSmall<3>& v1, const LifeV::VectorSmall<3>& v2)
        {
            VectorSmall<3> v;
            v[0] = v1[1] * v2[2] - v1[2] * v2[1];
            v[1] = v1[2] * v2[0] - v1[0] * v2[2];
            v[2] = v1[0] * v2[1] - v1[1] * v2[0];
            return v;
        }
        
        CrossProduct() {}
        ~CrossProduct() {}
    };
    
    
    
    class DefGrad
    {
    public:
        typedef LifeV::MatrixSmall<3,3> return_Type;
        
        return_Type operator() (const LifeV::MatrixSmall<3,3>& du)
        {
            MatrixSmall<3,3> I;
            I(0,0) = 1.; I(0,1) = 0., I(0,2) = 0.;
            I(1,0) = 0.; I(1,1) = 1., I(1,2) = 0.;
            I(2,0) = 0.; I(2,1) = 0., I(2,2) = 1.;
            
            MatrixSmall<3,3> F;
            F = I + du;
            
            return F;
        }
        
        DefGrad() {}
        ~DefGrad() {}
    };

    
    class dPvol
    {
    public:
        typedef LifeV::MatrixSmall<3,3> return_Type;
        
        return_Type operator() (const LifeV::MatrixSmall<3,3>& F, const LifeV::MatrixSmall<3,3>& dphij)
        {
            auto J = F.determinant();
            auto FmT = F.minusTransposed();
            auto dJ = J * FmT;
            
            auto dJdF = dJ.dot(dphij);
            auto dFT = dphij.transpose();
            auto dFmTdF = - 1.0 * FmT * dFT * FmT;
            auto d2JdF = dJdF * FmT + J * dFmTdF;
            auto dWvol = ( 3500000 * ( J + J * std::log(J) - 1. ) ) / ( 2 * J );
            auto dPvol = dWvol * d2JdF;
            
            auto ddWvol = ( 3500000 * ( J + 1. ) ) / ( 2. * J * J );
            auto ddPvol = ddWvol * dJdF * dJ;
            
            return (dPvol + ddPvol);
        }
        
        dPvol() {}
        ~dPvol() {}
    };
    
    
    class ddP1E
    {
    public:
        typedef LifeV::MatrixSmall<3,3> return_Type;
        
        return_Type operator() (const LifeV::MatrixSmall<3,3>& F, const LifeV::MatrixSmall<3,3>& FAinv, const LifeV::MatrixSmall<3,3>& dphij)
        {
            auto FE = F * FAinv;
            
            auto JE = FE.determinant();
            auto JEm23 = pow ( JE , 2 / -3.0 );
            
            auto I1E = FE.dot(FE);
            auto I1barE = JEm23 * I1E;
            auto dI1barE = 2 * JEm23 * ( FE - (1/3.) * I1E * FE.minusTransposed() );

            
            MatrixSmall<3,3> ddP1E;
            ddP1E = d2W1(I1barE) * dI1barE.dot( dphij * FAinv ) * dI1barE * FAinv;
            
            return ddP1E;
        }
        
        Real d2W1 (const Real& I1barE)
        {
            return ( 3300 * 9.242 / 2.0 * std::exp( 9.242 * (I1barE - 3 ) ) );
        }
        
        ddP1E() {}
        ~ddP1E() {}
    };

    
    class dP1E
    {
    public:
        typedef LifeV::MatrixSmall<3,3> return_Type;
        
        return_Type operator() (const LifeV::MatrixSmall<3,3>& F, const LifeV::MatrixSmall<3,3>& FAinv, const LifeV::MatrixSmall<3,3>& dphij)
        {
            auto FE = F * FAinv;
            auto FEmT = FE.minusTransposed();

            auto JE = FE.determinant();
            auto JEm23 = pow ( JE , 2 / -3.0 );
            auto dJEm23 = (-2/3.) * JEm23 * FEmT;
            
            auto I1E = FE.dot(FE);
            auto I1barE = JEm23 * I1E;

            auto term1 = dJEm23.dot( dphij * FAinv ) * 2 * FE;
            auto term2 = JEm23 * 2 * ( dphij * FAinv );
            auto term3 = FE.dot(FE) * (-2.0/3.0) * ( JEm23 * ( (-1.0) * FEmT * ( dphij*FAinv ).transpose() * FEmT ) + dJEm23.dot( dphij*FAinv ) * FEmT );
            auto term4 = (2*FE).dot(dphij*FAinv) * dJEm23;
            

            MatrixSmall<3,3> dP1E;
            dP1E = dW1(I1barE) * ( term1 + term2 + term3 + term4 ) * FAinv;
            
            return dP1E;
        }
        
        Real dW1 (const Real& I1barE)
        {
            return ( 3300 / 2.0 * std::exp( 9.242 * (I1barE - 3 ) ) );
        }
        
        dP1E() {}
        ~dP1E() {}
    };
    
    
    
    class dP4fE
    {
    public:
        typedef LifeV::MatrixSmall<3,3> return_Type;
        
        return_Type operator() (const std::vector<MatrixSmall<3,3> >& M, const LifeV::VectorSmall<3>& f0, const Real& gf)
        {
            // dP
            auto F = M[0];
            auto FAinv = M[1];
            auto gradPhiJ = M[2];
            
            auto f = F * f0;
            auto I4fE = f.dot(f) / std::pow (gf + 1, 2.0);
            
            auto dFE = gradPhiJ * FAinv;
            auto d2I4fEdFE = 2.0 * outerProduct( dFE * f0, f0 );

            auto dP4fE = dW4f(I4fE) * d2I4fEdFE * FAinv;
            
            // ddP
            auto FE = F * FAinv;
            auto dI4fE = 2.0 * outerProduct( FE * f0, f0 );
            auto dI4fEdFE =  dI4fE.dot( dFE );
            
            auto ddP4fE = ddW4f(I4fE) * dI4fEdFE * dI4fE * FAinv;

            
            return (dP4fE + ddP4fE);
        }

        Real dW4f (const Real& I4fE)
        {
            auto I4m1fE = I4fE - 1.0;
            return ( 185350 * I4m1fE * std::exp (15.972 * I4m1fE * I4m1fE ) * (I4m1fE > 0. ? 1. : 0.) );
        }
        
        Real ddW4f (const Real& I4fE)
        {
            auto I4m1fE = I4fE - 1.0;
            return ( 185350 * std::exp ( 15.972 * I4m1fE * I4m1fE ) * ( 1.0 + 2.0 * 15.972 * I4m1fE * I4m1fE ) * (I4m1fE > 0. ? 1. : 0.) );
        }
        
        return_Type outerProduct(const LifeV::VectorSmall<3>& f, const LifeV::VectorSmall<3>& s)
        {
            MatrixSmall<3,3> M;
            for (UInt i (0); i < 3; ++i)
            {
                for (UInt j (0); j < 3; ++j)
                {
                    M(i,j) = f(i) * s(j);
                }
            }
            return M;
        }
        
        dP4fE() {}
        ~dP4fE() {}
    };

    
    class dP4sE
    {
    public:
        typedef LifeV::MatrixSmall<3,3> return_Type;
        
        return_Type operator() (const std::vector<MatrixSmall<3,3> >& M, const LifeV::VectorSmall<3>& f0, const Real& gf)
        {
            // dP
            auto F = M[0];
            auto FAinv = M[1];
            auto gradPhiJ = M[2];
            
            auto f = F * f0;
            auto I4fE = f.dot(f) / std::pow (gf + 1, 2.0);
            
            auto dFE = gradPhiJ * FAinv;
            auto d2I4fEdFE = 2.0 * outerProduct( dFE * f0, f0 );
            
            auto dP4fE = dW4s(I4fE) * d2I4fEdFE * FAinv;
            
            // ddP
            auto FE = F * FAinv;
            auto dI4fE = 2.0 * outerProduct( FE * f0, f0 );
            auto dI4fEdFE =  dI4fE.dot( dFE );
            
            auto ddP4fE = ddW4s(I4fE) * dI4fEdFE * dI4fE * FAinv;
            
            
            return (dP4fE + ddP4fE);
        }
        
        Real dW4s (const Real& I4fE)
        {
            auto I4m1fE = I4fE - 1.0;
            return ( 25640 * I4m1fE * std::exp (10.446 * I4m1fE * I4m1fE ) * (I4m1fE > 0. ? 1. : 0.) );
        }
        
        Real ddW4s (const Real& I4fE)
        {
            auto I4m1fE = I4fE - 1.0;
            return ( 25640 * std::exp ( 10.446 * I4m1fE * I4m1fE ) * ( 1.0 + 2.0 * 10.446 * I4m1fE * I4m1fE ) * (I4m1fE > 0. ? 1. : 0.) );
        }
        
        return_Type outerProduct(const LifeV::VectorSmall<3>& f, const LifeV::VectorSmall<3>& s)
        {
            MatrixSmall<3,3> M;
            for (UInt i (0); i < 3; ++i)
            {
                for (UInt j (0); j < 3; ++j)
                {
                    M(i,j) = f(i) * s(j);
                }
            }
            return M;
        }
        
        dP4sE() {}
        ~dP4sE() {}
    };
    
    
    class dP8fsE
    {
    public:
        typedef LifeV::MatrixSmall<3,3> return_Type;
        
        return_Type operator() (const std::vector<MatrixSmall<3,3> >& M, const LifeV::VectorSmall<3>& f0, const LifeV::VectorSmall<3>& s0, const Real& gf)
        {
            // dP
            auto F = M[0];
            auto FAinv = M[1];
            auto gradPhiJ = M[2];
            
            auto gn = 4 * gf;
            auto gs = 1 / ( (gf + 1) * (gn + 1) ) - 1;
            
            auto f = F * f0;
            auto s = F * s0;
            auto I8fsE = f.dot(s) / ( (gf + 1) * (gs + 1) );
            
            auto dFE = gradPhiJ * FAinv;
            auto d2I8EdFE = dFE * ( outerProduct( f0, s0 ) + outerProduct( s0, f0 ) );
            
            auto dP8fsE = dW8fs(I8fsE) * d2I8EdFE * FAinv;
            
            // ddP
            auto FE = F * FAinv;
            auto dI8E = FE * ( outerProduct( f0, s0 ) + outerProduct( s0, f0 ) );
            auto dI8EdFE = dI8E.dot ( dFE );
            auto ddP8fsE = ddW8fs(I8fsE) * dI8EdFE * dI8E * FAinv;
            
            return (dP8fsE + ddP8fsE);
        }
        
        Real dW8fs (const Real& I8fsE)
        {
            return ( 4170 * I8fsE * std::exp (11.602 * I8fsE * I8fsE ) );
        }
        
        Real ddW8fs (const Real& I8fsE)
        {
            return ( 4170 * std::exp ( 11.602 * I8fsE * I8fsE ) * ( 2.0 * 11.602 * I8fsE * I8fsE + 1.0 ) );
        }
        
        return_Type outerProduct(const LifeV::VectorSmall<3>& f, const LifeV::VectorSmall<3>& s)
        {
            MatrixSmall<3,3> M;
            for (UInt i (0); i < 3; ++i)
            {
                for (UInt j (0); j < 3; ++j)
                {
                    M(i,j) = f(i) * s(j);
                }
            }
            return M;
        }
        
        dP8fsE() {}
        ~dP8fsE() {}
    };
    
    
//    class NonlinearMaterial
//    {
//    public:
//
//        typedef LifeV::MatrixSmall<3,3> return_Type;
//
//        NonlinearMaterial() {}
//        ~NonlinearMaterial() {}
//
//        virtual return_Type operator() (const std::vector<MatrixSmall<3,3> >& matrices, const std::vector<VectorSmall<3> >& vectors, const Real& g) = 0;
//
//    protected:
//
//        MatrixSmall<3,3> deformationGradient (const LifeV::MatrixSmall<3,3>& du) const
//        {
//            MatrixSmall<3,3> I;
//            I(0,0) = 1.; I(0,1) = 0., I(0,2) = 0.;
//            I(1,0) = 0.; I(1,1) = 1., I(1,2) = 0.;
//            I(2,0) = 0.; I(2,1) = 0., I(2,2) = 1.;
//
//            MatrixSmall<3,3> F;
//            F = I + du;
//
//            return F;
//        }
//
//        VectorSmall<3> crossProduct (const LifeV::VectorSmall<3>& v1, const LifeV::VectorSmall<3>& v2) const
//        {
//            VectorSmall<3> v;
//            v[0] = v1[1] * v2[2] - v1[2] * v2[1];
//            v[1] = v1[2] * v2[0] - v1[0] * v2[2];
//            v[2] = v1[0] * v2[1] - v1[1] * v2[0];
//            return v;
//        }
//
//        MatrixSmall<3,3> outerProduct (const LifeV::VectorSmall<3>& f, const LifeV::VectorSmall<3>& s) const
//        {
//            MatrixSmall<3,3> M;
//            for (UInt i (0); i < 3; ++i)
//            {
//                for (UInt j (0); j < 3; ++j)
//                {
//                    M(i,j) = f(i) * s(j);
//                }
//            }
//            return M;
//        }
//
//        void orthoNormalize (VectorSmall<3>& s, const VectorSmall<3>& f) const
//        {
//            normalize(s);
//            s = s - s.dot(f) * f;
//            normalize(s);
//        }
//
//        void normalize (VectorSmall<3>& v) const
//        {
//            Real norm = std::sqrt (v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
//            //            if ( norm >= 1e-13 )
//            //            {
//            v[0] = v[0] / norm;
//            v[1] = v[1] / norm;
//            v[2] = v[2] / norm;
//            //            }
//            //            else
//            //            {
//            //                V *= 0.0;
//            //                V[comp] = 1.0;
//            //            }
//        }
//
//        MatrixSmall<3,3> identity () const
//        {
//            MatrixSmall<3,3> I;
//            I(0,0) = 1.; I(0,1) = 0., I(0,2) = 0.;
//            I(1,0) = 0.; I(1,1) = 1., I(1,2) = 0.;
//            I(2,0) = 0.; I(2,1) = 0., I(2,2) = 1.;
//            return I;
//        }
//
//    };
    
    
    class NeoHookeanMaterial
    {
    public:
        
        typedef LifeV::MatrixSmall<3,3> return_Type;

        return_Type operator() (const std::vector<MatrixSmall<3,3> >& matrices, const std::vector<VectorSmall<3> >& vectors, const Real& g)
        {
            auto grad_u = matrices[0];
            auto grad_phij = matrices[1];
            
//            auto f0 = vectors[0];
//            auto s0 = vectors[1];
//            normalize(f0);
//            orthoNormalize(s0, f0);
//            auto n0 = crossProduct(f0, s0);
//
//            auto gf = g;
//            auto gn = 4 * gf;
//            auto gs = 1 / ( (gf + 1) * (gn + 1) ) - 1;
            
            auto F = deformationGradient(grad_u);
            
//            MatrixSmall<3,3> FAinv;
//            FAinv = identity() - gf/(gf+1) * outerProduct(f0,f0) - gs/(gs+1) * outerProduct(s0,s0) - gn/(gn+1) * outerProduct(n0,n0);
            
            
            // ===============================//
            // Pvol
            // ===============================//

            auto J = F.determinant();
            auto FmT = F.minusTransposed();
            auto dJ = J * FmT;
            
            auto dJdF = dJ.dot(grad_phij);
            auto dFT = grad_phij.transpose();
            auto dFmTdF = - 1.0 * FmT * dFT * FmT;
            auto d2JdF = dJdF * FmT + J * dFmTdF;
            //auto dWvol = ( 3500000 * ( J + J * std::log(J) - 1. ) ) / ( 2 * J );
            auto dWvol = 3500000 * ( J*(J-1) + std::log(J) ) / ( 2 * J );
            auto dPvol = dWvol * d2JdF;
            
            //auto ddWvol = ( 3500000 * ( J + 1. ) ) / ( 2. * J * J );
            auto ddWvol = 3500000  / (2 * J * J) * ( 1 + J * J - std::log(J) );
            auto ddPvol = ddWvol * dJdF * dJ;
            
            
            // ===============================//
            // NK
            // ===============================//

            auto dF = grad_phij;
            auto Jm23 = pow(J, - 2. / 3.);
            auto dJm23 = (- 2. / 3. ) * Jm23 * FmT;
            auto d2Jm23dF = ( -2. / 3. ) * dJm23.dot(dF) * FmT + Jm23 * dFmTdF;
            auto I1 = F.dot(F);
            auto dI1 = (2.0) * F;
            auto d2I1 = (2.0) * dF;
            auto I1bar = Jm23 * I1;
            auto dI1bar = dJm23 * I1 + Jm23 * dI1;
            auto d2I1bardF = dJm23.dot(dF) * dI1 + Jm23 * d2I1 + dJm23 * dI1.dot(dF) + d2Jm23dF * I1;
            auto dNK = 0.5 * 4960 * d2I1bardF;
            // auto dNK = 0.385 * F;


            return ( dPvol + ddPvol + dNK);
            
        }
        
        MatrixSmall<3,3> deformationGradient (const LifeV::MatrixSmall<3,3>& du) const
        {
            MatrixSmall<3,3> I;
            I(0,0) = 1.; I(0,1) = 0., I(0,2) = 0.;
            I(1,0) = 0.; I(1,1) = 1., I(1,2) = 0.;
            I(2,0) = 0.; I(2,1) = 0., I(2,2) = 1.;
            
            MatrixSmall<3,3> F;
            F = I + du;
            
            return F;
        }

        VectorSmall<3> crossProduct (const LifeV::VectorSmall<3>& v1, const LifeV::VectorSmall<3>& v2) const
        {
            VectorSmall<3> v;
            v[0] = v1[1] * v2[2] - v1[2] * v2[1];
            v[1] = v1[2] * v2[0] - v1[0] * v2[2];
            v[2] = v1[0] * v2[1] - v1[1] * v2[0];
            return v;
        }
        
        MatrixSmall<3,3> outerProduct (const LifeV::VectorSmall<3>& f, const LifeV::VectorSmall<3>& s) const
        {
            MatrixSmall<3,3> M;
            for (UInt i (0); i < 3; ++i)
            {
                for (UInt j (0); j < 3; ++j)
                {
                    M(i,j) = f(i) * s(j);
                }
            }
            return M;
        }
        
        void orthoNormalize (VectorSmall<3>& s, const VectorSmall<3>& f) const
        {
            normalize(s);
            s = s - s.dot(f) * f;
            normalize(s);
        }
        
        void normalize (VectorSmall<3>& v) const
        {
            Real norm = std::sqrt (v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
            //            if ( norm >= 1e-13 )
            //            {
            v[0] = v[0] / norm;
            v[1] = v[1] / norm;
            v[2] = v[2] / norm;
            //            }
            //            else
            //            {
            //                V *= 0.0;
            //                V[comp] = 1.0;
            //            }
        }

        MatrixSmall<3,3> identity () const
        {
            MatrixSmall<3,3> I;
            I(0,0) = 1.; I(0,1) = 0., I(0,2) = 0.;
            I(1,0) = 0.; I(1,1) = 1., I(1,2) = 0.;
            I(2,0) = 0.; I(2,1) = 0., I(2,2) = 1.;
            return I;
        }
        
        NeoHookeanMaterial() {}
        ~NeoHookeanMaterial() {}
        
    };
    
    class HolzapfelOgdenMaterial
    {
    public:
        
        typedef LifeV::MatrixSmall<3,3> return_Type;
        
        return_Type operator() (const std::vector<MatrixSmall<3,3> >& matrices, const std::vector<VectorSmall<3> >& vectors, const Real& g)
        {
            auto grad_u = matrices[0];
            auto grad_phij = matrices[1];
            
            auto f0 = vectors[0];
            auto s0 = vectors[1];
            normalize(f0);
            orthoNormalize(s0, f0);
            auto n0 = crossProduct(f0, s0);

            auto gf = g;
            auto gn = 4 * gf;
            auto gs = 1 / ( (gf + 1) * (gn + 1) ) - 1;
            
            auto F = deformationGradient(grad_u);
            
            MatrixSmall<3,3> FAinv;
            FAinv = identity() - gf/(gf+1) * outerProduct(f0,f0) - gs/(gs+1) * outerProduct(s0,s0) - gn/(gn+1) * outerProduct(n0,n0);
            
            
            // ===============================//
            // Pvol
            // ===============================//

            auto J = F.determinant();
            auto FmT = F.minusTransposed();
            auto dJ = J * FmT;
            
            auto dJdF = dJ.dot(grad_phij);
            auto dFT = grad_phij.transpose();
            auto dFmTdF = - 1.0 * FmT * dFT * FmT;
            auto d2JdF = dJdF * FmT + J * dFmTdF;
            //auto dWvol = ( 3500000 * ( J + J * std::log(J) - 1. ) ) / ( 2 * J );
            auto dWvol = 3500000 * ( J*(J-1) + std::log(J) ) / ( 2 * J );
            auto dPvol = dWvol * d2JdF;
            
            //auto ddWvol = ( 3500000 * ( J + 1. ) ) / ( 2. * J * J );
            auto ddWvol = 3500000  / (2 * J * J) * ( 1 + J * J - std::log(J) );
            auto ddPvol = ddWvol * dJdF * dJ;
            
            
            // ===============================//
            // P1E
            // ===============================//
            auto FE = F * FAinv;
            auto FEmT = FE.minusTransposed();
            
            auto JE = FE.determinant();
            auto JEm23 = pow ( JE , 2 / -3.0 );
            auto dJEm23 = (-2/3.) * JEm23 * FEmT;
            
            auto I1E = FE.dot(FE);
            auto I1barE = JEm23 * I1E;
            
            auto term1 = dJEm23.dot( grad_phij * FAinv ) * 2 * FE;
            auto term2 = JEm23 * 2 * ( grad_phij * FAinv );
            auto term3 = FE.dot(FE) * (-2.0/3.0) * ( JEm23 * ( (-1.0) * FEmT * ( grad_phij*FAinv ).transpose() * FEmT ) + dJEm23.dot( grad_phij*FAinv ) * FEmT );
            auto term4 = (2*FE).dot(grad_phij*FAinv) * dJEm23;
            
            MatrixSmall<3,3> dP1E;
            dP1E = dW1(I1barE) * ( term1 + term2 + term3 + term4 ) * FAinv;
            
            
            auto dI1barE = 2 * JEm23 * ( FE - (1/3.) * I1E * FE.minusTransposed() );
        
            MatrixSmall<3,3> ddP1E;
            ddP1E = d2W1(I1barE) * dI1barE.dot( grad_phij * FAinv ) * dI1barE * FAinv;
            

            // ===============================//
            // P4fE
            // ===============================//
            auto f = F * f0;
            auto I4fE = f.dot(f) / std::pow (gf + 1, 2.0);
            
            auto dFE = grad_phij * FAinv;
            auto d2I4fEdFE = 2.0 * outerProduct( dFE * f0, f0 );
            
            auto dP4fE = dW4f(I4fE) * d2I4fEdFE * FAinv;
            
            auto dI4fE = 2.0 * outerProduct( FE * f0, f0 );
            auto dI4fEdFE =  dI4fE.dot( dFE );
            
            auto ddP4fE = ddW4f(I4fE) * dI4fEdFE * dI4fE * FAinv;
        
        
            // ===============================//
            // P4sE
            // ===============================//
            auto s = F * s0;
            auto I4sE = s.dot(s) / std::pow (gs + 1, 2.0);
            
            auto d2I4sEdFE = 2.0 * outerProduct( dFE * s0, s0 );
            
            auto dP4sE = dW4s(I4sE) * d2I4sEdFE * FAinv;
            
            auto dI4sE = 2.0 * outerProduct( FE * s0, s0 );
            auto dI4sEdFE =  dI4sE.dot( dFE );
            
            auto ddP4sE = ddW4s(I4sE) * dI4sEdFE * dI4sE * FAinv;
            
            
            
            // ===============================//
            // P8fsE
            // ===============================//
            auto I8fsE = f.dot(s) / ( (gf + 1) * (gs + 1) );
            
            auto d2I8EdFE = dFE * ( outerProduct( f0, s0 ) + outerProduct( s0, f0 ) );
            
            auto dP8fsE = dW8fs(I8fsE) * d2I8EdFE * FAinv;
            
            auto dI8E = FE * ( outerProduct( f0, s0 ) + outerProduct( s0, f0 ) );
            auto dI8EdFE = dI8E.dot ( dFE );
            auto ddP8fsE = ddW8fs(I8fsE) * dI8EdFE * dI8E * FAinv;
            

            return ( dPvol + ddPvol + dP1E + ddP1E + dP4fE + ddP4fE + dP4sE + ddP4sE + dP8fsE + ddP8fsE );
            
        }
        
        Real dW1 (const Real& I1barE)
        {
            return ( 3300 / 2.0 * std::exp( 9.242 * (I1barE - 3 ) ) );
        }

        Real d2W1 (const Real& I1barE)
        {
            return ( 3300 * 9.242 / 2.0 * std::exp( 9.242 * (I1barE - 3 ) ) );
        }
        
        Real dW4f (const Real& I4fE) const
        {
            auto I4m1fE = I4fE - 1.0;
            return ( 185350 * I4m1fE * std::exp (15.972 * I4m1fE * I4m1fE ) * (I4m1fE > 0. ? 1. : 0.) );
        }
        
        Real ddW4f (const Real& I4fE) const
        {
            auto I4m1fE = I4fE - 1.0;
            return ( 185350 * std::exp ( 15.972 * I4m1fE * I4m1fE ) * ( 1.0 + 2.0 * 15.972 * I4m1fE * I4m1fE ) * (I4m1fE > 0. ? 1. : 0.) );
        }
        
        Real dW4s (const Real& I4fE) const
        {
            auto I4m1fE = I4fE - 1.0;
            return ( 25640 * I4m1fE * std::exp (10.446 * I4m1fE * I4m1fE ) * (I4m1fE > 0. ? 1. : 0.) );
        }
        
        Real ddW4s (const Real& I4fE) const
        {
            auto I4m1fE = I4fE - 1.0;
            return ( 25640 * std::exp ( 10.446 * I4m1fE * I4m1fE ) * ( 1.0 + 2.0 * 10.446 * I4m1fE * I4m1fE ) * (I4m1fE > 0. ? 1. : 0.) );
        }

        Real dW8fs (const Real& I8fsE) const
        {
            return ( 4170 * I8fsE * std::exp (11.602 * I8fsE * I8fsE ) );
        }
        
        Real ddW8fs (const Real& I8fsE) const
        {
            return ( 4170 * std::exp ( 11.602 * I8fsE * I8fsE ) * ( 2.0 * 11.602 * I8fsE * I8fsE + 1.0 ) );
        }
        
        MatrixSmall<3,3> deformationGradient (const LifeV::MatrixSmall<3,3>& du) const
        {
            MatrixSmall<3,3> I;
            I(0,0) = 1.; I(0,1) = 0., I(0,2) = 0.;
            I(1,0) = 0.; I(1,1) = 1., I(1,2) = 0.;
            I(2,0) = 0.; I(2,1) = 0., I(2,2) = 1.;
            
            MatrixSmall<3,3> F;
            F = I + du;
            
            return F;
        }

        VectorSmall<3> crossProduct (const LifeV::VectorSmall<3>& v1, const LifeV::VectorSmall<3>& v2) const
        {
            VectorSmall<3> v;
            v[0] = v1[1] * v2[2] - v1[2] * v2[1];
            v[1] = v1[2] * v2[0] - v1[0] * v2[2];
            v[2] = v1[0] * v2[1] - v1[1] * v2[0];
            return v;
        }
        
        MatrixSmall<3,3> outerProduct (const LifeV::VectorSmall<3>& f, const LifeV::VectorSmall<3>& s) const
        {
            MatrixSmall<3,3> M;
            for (UInt i (0); i < 3; ++i)
            {
                for (UInt j (0); j < 3; ++j)
                {
                    M(i,j) = f(i) * s(j);
                }
            }
            return M;
        }
        
        void orthoNormalize (VectorSmall<3>& s, const VectorSmall<3>& f) const
        {
            normalize(s);
            s = s - s.dot(f) * f;
            normalize(s);
        }
        
        void normalize (VectorSmall<3>& v) const
        {
            Real norm = std::sqrt (v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
            //            if ( norm >= 1e-13 )
            //            {
            v[0] = v[0] / norm;
            v[1] = v[1] / norm;
            v[2] = v[2] / norm;
            //            }
            //            else
            //            {
            //                V *= 0.0;
            //                V[comp] = 1.0;
            //            }
        }

        MatrixSmall<3,3> identity () const
        {
            MatrixSmall<3,3> I;
            I(0,0) = 1.; I(0,1) = 0., I(0,2) = 0.;
            I(1,0) = 0.; I(1,1) = 1., I(1,2) = 0.;
            I(2,0) = 0.; I(2,1) = 0., I(2,2) = 1.;
            return I;
        }
        
        HolzapfelOgdenMaterial() {}
        ~HolzapfelOgdenMaterial() {}
    };
    
    
    class MatrixStdVector
    {
    public:
        typedef std::vector<MatrixSmall<3,3> > return_Type;
        
        return_Type operator() (const LifeV::MatrixSmall<3,3>& F1)
        {
            return_Type a;
            a.push_back(F1);
            return a;
        }

        return_Type operator() (const LifeV::MatrixSmall<3,3>& F1, const LifeV::MatrixSmall<3,3>& F2)
        {
            return_Type a;
            a.push_back(F1);
            a.push_back(F2);
            return a;
        }
        
        return_Type operator() (const LifeV::MatrixSmall<3,3>& F1, const LifeV::MatrixSmall<3,3>& F2, const LifeV::MatrixSmall<3,3>& F3)
        {
            return_Type a;
            a.push_back(F1);
            a.push_back(F2);
            a.push_back(F3);
            return a;
        }
                                
        return_Type operator() (const LifeV::MatrixSmall<3,3>& F1, const LifeV::MatrixSmall<3,3>& F2, const LifeV::MatrixSmall<3,3>& F3, const LifeV::MatrixSmall<3,3>& F4)
        {
            return_Type a;
            a.push_back(F1);
            a.push_back(F2);
            a.push_back(F3);
            a.push_back(F4);
            return a;
        }
        
        MatrixStdVector() {}
        ~MatrixStdVector() {}
    };

    
    class VectorStdVector
    {
    public:
        typedef std::vector<VectorSmall<3> > return_Type;
        
        return_Type operator() (const LifeV::VectorSmall<3>& F1)
        {
            return_Type a;
            a.push_back(F1);
            return a;
        }
        
        return_Type operator() (const LifeV::VectorSmall<3>& F1, const LifeV::VectorSmall<3>& F2)
        {
            return_Type a;
            a.push_back(F1);
            a.push_back(F2);
            return a;
        }
        
        return_Type operator() (const LifeV::VectorSmall<3>& F1, const LifeV::VectorSmall<3>& F2, const LifeV::VectorSmall<3>& F3)
        {
            return_Type a;
            a.push_back(F1);
            a.push_back(F2);
            a.push_back(F3);
            return a;
        }
        
        return_Type operator() (const LifeV::VectorSmall<3>& F1, const LifeV::VectorSmall<3>& F2, const LifeV::VectorSmall<3>& F3, const LifeV::VectorSmall<3>& F4)
        {
            return_Type a;
            a.push_back(F1);
            a.push_back(F2);
            a.push_back(F3);
            a.push_back(F4);
            return a;
        }
        
        VectorStdVector() {}
        ~VectorStdVector() {}
    };

    
//    class ScalarStdVector
//    {
//    public:
//        typedef std::vector<Real> return_Type;
//        
//        return_Type operator() (const Real& F1)
//        {
//            return_Type a;
//            a.push_back(F1);
//            return a;
//        }
//        
//        return_Type operator() (const Real& F1, const Real& F2)
//        {
//            return_Type a;
//            a.push_back(F1);
//            a.push_back(F2);
//            return a;
//        }
//        
//        return_Type operator() (const Real& F1, const Real& F2, const Real& F3)
//        {
//            return_Type a;
//            a.push_back(F1);
//            a.push_back(F2);
//            a.push_back(F3);
//            return a;
//        }
//        
//        return_Type operator() (const Real& F1, const Real& F2, const Real& F3, const Real& F4)
//        {
//            return_Type a;
//            a.push_back(F1);
//            a.push_back(F2);
//            a.push_back(F3);
//            a.push_back(F4);
//            return a;
//        }
//        
//        ScalarStdVector() {}
//        ~ScalarStdVector() {}
//    };
    
    
    class FAInverse
    {
    public:
        typedef LifeV::MatrixSmall<3,3> return_Type;
        
        return_Type operator() (const LifeV::VectorSmall<3>& f0, const LifeV::VectorSmall<3>& s0, const Real& gf)
        {
            MatrixSmall<3,3> I;
            I(0,0) = 1.; I(0,1) = 0., I(0,2) = 0.;
            I(1,0) = 0.; I(1,1) = 1., I(1,2) = 0.;
            I(2,0) = 0.; I(2,1) = 0., I(2,2) = 1.;
            
            VectorSmall<3> n0;
            n0 = crossProduct(f0, s0);
            
            auto gn = 4 * gf;
            auto gs = 1 / ( (gf + 1) * (gn + 1) ) - 1;
            
            MatrixSmall<3,3> FA;
            FA = I - gf/(gf+1) * outerProduct(f0) - gs/(gs+1) * outerProduct(s0) - gn/(gn+1) * outerProduct(n0);
            
            return FA;
        }
        
        LifeV::VectorSmall<3> crossProduct (const LifeV::VectorSmall<3>& v1, const LifeV::VectorSmall<3>& v2)
        {
            VectorSmall<3> v;
            v[0] = v1[1] * v2[2] - v1[2] * v2[1];
            v[1] = v1[2] * v2[0] - v1[0] * v2[2];
            v[2] = v1[0] * v2[1] - v1[1] * v2[0];
            return v;
        }
        
        return_Type outerProduct(const LifeV::VectorSmall<3>& i0)
        {
            MatrixSmall<3,3> M;
            for (UInt i (0); i < 3; ++i)
            {
                for (UInt j (0); j < 3; ++j)
                {
                    M(i,j) = i0(i) * i0(j);
                }
            }
            return M;
        }
        
        FAInverse() {}
        ~FAInverse() {}
    };
    
    
    class HeavisideFct
    {
    public:
        typedef Real return_Type;
        
        return_Type operator() (const Real& I4f)
        {
            return (I4f > 0. ? 1. : 0.);
        }
        
        HeavisideFct() {}
        ~HeavisideFct() {}
    };
    
    
//    class Cardiopathy
//    {
//    public:
//        typedef Real return_Type;
//        
//        return_Type operator() (const std::vector<VectorSmall<3> >& vectors, const std::vector<Real>& scalars)
//        {
//            
//            auto gf = scalars[0];
////            auto pathologyRadius = scalars[1];
////            auto pathologyStrength = scalars[2];
////            auto pathologyCenter = vectors[1];
////            auto X = vectors[0];
////            
////            auto X_PC = X - pathologyCenter;
////            auto X_PCnorm = std::sqrt( X_PC[0] * X_PC[0] + X_PC[1] * X_PC[1] + X_PC[2] * X_PC[2] );
////            if ( X_PCnorm <= pathologyRadius ) gf *= pathologyStrength;
//
//            return gf;
//        }
//        
//        Cardiopathy() {}
//        ~Cardiopathy() {}
//    };
    

};

template<typename MeshType>
EMStructuralConstitutiveLaw<MeshType>::EMStructuralConstitutiveLaw() :
    super                           ( ),
    M_scalarETFESpacePtr            ( ),
    M_fiberVectorPtr                ( ),
    M_sheetVectorPtr                ( ),
    M_passiveMaterialPtr            ( ),
    M_activeStressMaterialPtr       ( ),
    M_residualVectorPtr             ( ),
    M_fiberActivationPtr                 ( )
{}

template <typename MeshType>
void
EMStructuralConstitutiveLaw<MeshType>::setup ( const FESpacePtr_Type&                      dFESpace,
                                               const ETFESpacePtr_Type&                    dETFESpace,
                                               const boost::shared_ptr<const MapEpetra>&   monolithicMap,
                                               const UInt                                  offset,
                                               const dataPtr_Type&                         dataMaterial,
                                               const displayerPtr_Type&                    displayer)
{
    this->M_displayer = displayer;
    this->M_dataMaterial  = dataMaterial;

    this->M_dispFESpace                 = dFESpace;
    this->M_dispETFESpace               = dETFESpace;
    this->M_localMap                    = monolithicMap;
    this->M_offset                      = offset;
    this->M_dataMaterial                = dataMaterial;
    this->M_displayer                   = displayer;

    M_residualVectorPtr.reset ( new vector_Type (*this->M_localMap, Repeated) );

    M_fiberVectorPtr.reset             ( new vector_Type (*this->M_localMap, Repeated) );
    M_sheetVectorPtr.reset             ( new vector_Type (*this->M_localMap, Repeated) );

    M_scalarETFESpacePtr.reset         ( new scalarETFESpace_Type ( dETFESpace -> mesh(),
    																&( dETFESpace -> refFE() ),
                                                                    dFESpace->map().commPtr() ) );

    M_fiberActivationPtr.reset (new vector_Type (M_scalarETFESpacePtr -> map() ) );

    std::string passiveMaterialType ( dataMaterial -> passiveType() );
    std::string activeStressMaterialType (dataMaterial -> activeStressType() );
    
    if (displayer->isLeader())
    {
        std::cout << "EMStructuralConstitutiveLawAHO: setup - done\n";
    }

}

//const std::vector<Vector3D> currentPosition(const VectorEpetra& disp) const
//{
//    Int nLocalDof = disp.blockMap().NumGlobalElements(); //disp.epetraVector().MyLength();
//    Int nComponentLocalDof = nLocalDof / 3;
//    
//    std::vector<Vector3D> boundaryCoordinates(M_boundaryPoints.size());
//    
//    for ( unsigned int i (0) ; i < M_boundaryPoints.size() ; ++i )
//    {
//        int root; int LID;
//        disp.blockMap().RemoteIDList(1, &M_boundaryPoints[i], &root, &LID);
//        
//        if ( disp.blockMap().MyGID( M_boundaryPoints[i] ) )
//        {
//            Vector3D pointCoordinates;
//            
//            UInt iGID = M_boundaryPoints[i];
//            UInt jGID = M_boundaryPoints[i] + nComponentLocalDof;
//            UInt kGID = M_boundaryPoints[i] + 2 * nComponentLocalDof;
//            
//            pointCoordinates[0] = M_fullMesh.point (iGID).x() + disp[iGID];
//            pointCoordinates[1] = M_fullMesh.point (iGID).y() + disp[jGID];
//            pointCoordinates[2] = M_fullMesh.point (iGID).z() + disp[kGID];
//            
//            boundaryCoordinates[i] = pointCoordinates;
//        }
//        
//        MPI_Bcast(&boundaryCoordinates[i], 3, MPI_DOUBLE, root, MPI_COMM_WORLD);
//    }
//    
//    MPI_Barrier(MPI_COMM_WORLD);
//    return boundaryCoordinates;
//}
//    
//    Int p1nCompLocalDof = p1PositionVector.epetraVector().MyLength() / 3;
//    for (int j (0); j < p1nCompLocalDof; j++)
//    {
//        UInt iGID = p1PositionVector.blockMap().GID (j);
//        UInt jGID = p1PositionVector.blockMap().GID (j + p1nCompLocalDof);
//        UInt kGID = p1PositionVector.blockMap().GID (j + 2 * p1nCompLocalDof);
//        
//        p1PositionVector[iGID] = M_fullMesh.point (iGID).x();
//        p1PositionVector[jGID] = M_fullMesh.point (iGID).y();
//        p1PositionVector[kGID] = M_fullMesh.point (iGID).z();
//    }
//
//    
//VectorEpetra pathologic activation ( VectorEpetra& vec, boost::shared_ptr<  RegionMesh<LinearTetra> > fullMesh, Real value, std::vector<UInt> flags)
//{
//    VectorEpetra fiberActivation ( M_fiberActivationPtr );
//
//    for ( int j (0); j < fiberActivation.epetraVector().MyLength() ; ++j )
//    {
//        for ( UInt k (0); k < flags.size(); k++ )
//        {
//            if ( fullMesh -> point ( vec.blockMap().GID (j) ).markerID() == flags.at (k) )
//            {
//                if ( vec.blockMap().LID ( vec.blockMap().GID (j) ) != -1 )
//                {
//                    (vec) ( vec.blockMap().GID (j) ) = value;
//                }
//            }
//        }
//    }
//}

    
    

    


    
    
template <typename MeshType>
void EMStructuralConstitutiveLaw<MeshType>::updateJacobianMatrix ( const vector_Type&       disp,
                                                                   const dataPtr_Type&      dataMaterial,
                                                                   const mapMarkerVolumesPtr_Type mapsMarkerVolumes,
                                                                   const mapMarkerIndexesPtr_Type mapsMarkerIndexes,
                                                                   const displayerPtr_Type& displayer )
{
    this->M_jacobian.reset (new matrix_Type (*this->M_localMap) );

    * (this->M_jacobian) *= 0.0;
    

    boost::shared_ptr<MatrixStdVector> msv (new MatrixStdVector);
    boost::shared_ptr<VectorStdVector> vsv (new VectorStdVector);
    //boost::shared_ptr<ScalarStdVector> ssv (new ScalarStdVector);

//    boost::shared_ptr<HolzapfelOgdenMaterial> hom (new HolzapfelOgdenMaterial);
//    boost::shared_ptr<NeoHookeanMaterial> nkm (new NeoHookeanMaterial);

    {
        using namespace ExpressionAssembly;
        
        
//        auto grad_u =  grad(super::M_dispETFESpace, disp, 0);
//
//        auto f_0 = value (super::M_dispETFESpace, *M_fiberVectorPtr);
//        auto s_0 = value (super::M_dispETFESpace, *M_sheetVectorPtr);
//
//        auto gf = value (M_scalarETFESpacePtr, *M_fiberActivationPtr);
//
//        auto vectors = eval(vsv, f_0, s_0);
//        auto matrices = eval(msv, grad_u, grad(phi_j));
        
        
        // Holzapfel-Ogden
        //auto dP = eval(hom, matrices, vectors, gf);
        
        // NeoHookean
//        auto dP = eval(nkm, matrices, vectors, gf);
        
        // NeoHookean
        MatrixSmall<3,3> I;
        I(0,0) = 1.; I(0,1) = 0., I(0,2) = 0.;
        I(1,0) = 0.; I(1,1) = 1., I(1,2) = 0.;
        I(2,0) = 0.; I(2,1) = 0., I(2,2) = 1.;
        
        
        auto dF = grad(phi_j);
        auto GradU = grad(super::M_dispETFESpace, disp, 0);
        auto F = I + GradU;
        auto FmT = minusT(F);
        auto dFmTdF = value(-1.0) * FmT * transpose(dF) * FmT;
        auto J = det(F);
        auto Jm23 = pow(J, - 2. / 3.);
        auto dJm23 = value(- 2. / 3. ) * Jm23 * FmT;
        auto d2Jm23dF = value( -2. / 3. ) * ( dot( dJm23, dF ) * FmT + Jm23 * dFmTdF );
        auto I1 = dot(F, F);
        auto dI1 = value(2.0) * F;
        auto d2I1 = value(2.0) * dF;
        auto I1bar = Jm23 * I1;
        auto dI1bar = dJm23 * I1 + Jm23 * dI1;
        auto d2I1bardF = dot(dJm23, dF) * dI1 + Jm23 * d2I1 + dJm23 * dot(dI1, dF) + d2Jm23dF * I1;
        
        auto dJ = J * FmT;
        auto dJdF = dot(dJ,dF);
        auto dFT = transpose(dF);
        //auto dFmTdF = - 1.0 * FmT * dFT * FmT;
        auto d2JdF = dJdF * FmT + J * dFmTdF;
        auto dWvol = 3500000 * ( J*(J-1) + log(J) ) / ( 2 * J );
        auto dPvol = dWvol * d2JdF;
        auto ddWvol = 3500000  / (2 * J * J) * ( 1 + J * J - log(J) );
        auto ddPvol = ddWvol * dJdF * dJ;
        
        auto dP = dPvol + ddPvol + 25 * 0.5 * 4960 * d2I1bardF;


        
        integrate ( elements ( super::M_dispETFESpace->mesh() ) ,
                   quadRuleTetra4pt,
                   super::M_dispETFESpace,
                   super::M_dispETFESpace,
                   dot ( dP , grad (phi_i) )
                   ) >> this->M_jacobian;
        
    }
    
    this->M_jacobian->globalAssemble();
}

    
template <typename MeshType>
void EMStructuralConstitutiveLaw<MeshType>::computeStiffness ( const vector_Type&       disp,
                                                               Real                     factor,
                                                               const dataPtr_Type&      dataMaterial,
                                                               const mapMarkerVolumesPtr_Type mapsMarkerVolumes,
                                                               const mapMarkerIndexesPtr_Type mapsMarkerIndexes,
                                                               const displayerPtr_Type& displayer )
{
    
    * (M_residualVectorPtr) *= 0.0;
    
    MatrixSmall<3,3> I;
    I(0,0) = 1.; I(0,1) = 0., I(0,2) = 0.;
    I(1,0) = 0.; I(1,1) = 1., I(1,2) = 0.;
    I(2,0) = 0.; I(2,1) = 0., I(2,2) = 1.;

    
    boost::shared_ptr<FAInverse> fAInversefct (new FAInverse);

    boost::shared_ptr<HeavisideFct> heaviside (new HeavisideFct);
    boost::shared_ptr<CrossProduct> crossProduct (new CrossProduct);
    boost::shared_ptr<OrthonormalizeVector> orthonormalizeVector (new OrthonormalizeVector);
    
    {
        using namespace ExpressionAssembly;
        
        
        auto F = I + grad(super::M_dispETFESpace, disp, 0);
        auto J = det(F);
        auto Jm23 = pow(J, 2 / (-3.) );
        auto FmT = minusT(F);
        auto I1 = dot(F, F);
        auto dI1bar = value(2.0) * Jm23 * ( F + value(1/(-3.)) * I1 * FmT );

        
//        // Orthotropic activation
//        auto k = 4.0;
//
//        auto gf = value (M_scalarETFESpacePtr, *M_fiberActivationPtr);
//        auto gn = k * gf;
//        auto gs = 1 / ( (gf + 1) * (gn + 1) ) - 1;
//        auto gm = value(-1.0) * ( gf ) / ( ( gf ) + 1.0 );
//        auto go = gf * ( k + gf * k + value(1.0) );
//        auto gmn = value(-1.0) * ( k*gf ) / ( ( k*gf ) + 1.0 ) ;
//
//
//        // Active strain
//        auto f0 = eval (orthonormalizeVector, value (super::M_dispETFESpace, *M_fiberVectorPtr));
//        auto s0 = eval (orthonormalizeVector, f0,  value (super::M_dispETFESpace, *M_sheetVectorPtr));
//        auto n0 = eval (crossProduct, f0, s0);
//        auto f = F * f0;
//        auto s = F * s0;
//        auto FAinv = eval(fAInversefct, f0, s0, gf);
//        auto FE =  F * FAinv;
        
        
        // Pvol
        //auto dWvol = ( 3500000 * ( J + J * log(J) - 1. ) ) / ( 2 * J );
        auto dWvol = 3500000 * ( J*(J-1) + log(J) ) / ( 2 * J );
        auto dJ = det(F) * minusT(F);
        auto Pvol = dWvol * dJ;
        

//        // P1E
//        auto I1barE = pow ( det(FE), 2 / -3.0 ) *  dot( FE, FE );
//        auto dI1barE = pow ( det(FE), 2 / -3.0 ) * ( value(2.0) * FE + dot( FE, FE ) * value(-2.0/3.0) * minusT(FE) );
//        auto dWI1E = 3300 / 2.0 * exp ( 9.242 * ( I1barE - 3 ) );
//        auto P1E = dWI1E * dI1barE * FAinv;
//
//
//        // P4fE
//        auto I4fE = dot (f,f) / pow (gf + 1, 2.0);
//        auto I4m1fE = I4fE - 1.0;
//        auto dW4fE = 185350 * I4m1fE * exp (15.972 * I4m1fE * I4m1fE ) * eval(heaviside, I4m1fE);
//        auto dI4fE = pow(gf + 1, -2.0);
//        auto dI4f = value(2.0) * outerProduct( f, f0 );
//        auto P4fE = dW4fE * dI4fE * dI4f;
//
//
//        // P4sE
//        auto I4sE = dot (s,s) / pow (gs + 1, 2.0);
//        auto I4m1sE = I4sE - 1.0;
//        auto dW4sE = 25640 * I4m1sE * exp (10.446 * I4m1sE * I4m1sE ) * eval(heaviside, I4m1sE);
//        auto dI4sE = pow(gs + 1, -2.0);
//        auto dI4s = value(2.0) * outerProduct( s, s0 );
//        auto P4sE = dW4sE * dI4sE * dI4s;
//
//
//        // P8fsE
//        auto I8fsE = dot (f,s) / ( (gf + 1) * (gs + 1) );
//        auto dW8fsE = 4170 * I8fsE * exp ( 11.602 * I8fsE * I8fsE );
//        auto dI8fsE = 1 / ( (gf + 1) * (gs + 1) );
//        auto dI8fs = F * ( outerProduct( f0, s0 ) + outerProduct( s0, f0 ) );
//        auto P8fsE = dW8fsE * dI8fsE * dI8fs;
//
//
//        // Sum up contributions and integrate /
//        auto P = Pvol + P1E + P4fE + P4sE + P8fsE;



            auto P = Pvol + 25 * 0.5 * 4960 * dI1bar ;


        // auto P = 4960 * F; // 0.385 * F;
        
        integrate ( elements ( super::M_dispETFESpace->mesh() ) ,
                   quadRuleTetra4pt,
                   super::M_dispETFESpace,
                   dot ( P, grad (phi_i) )
                   ) >> M_residualVectorPtr;
    
    }

    this->M_residualVectorPtr->globalAssemble();
}

template <typename MeshType>
void EMStructuralConstitutiveLaw<MeshType>::setParameters(EMData& data, GetPot& dataFile)
{
    if (M_activeStressMaterialPtr)
    {
        M_activeStressMaterialPtr-> setParameters(data);
    }

    
    if (M_passiveMaterialPtr)
    {
        M_passiveMaterialPtr-> setParameters(data);
    }
}




template <typename MeshType>
inline StructuralConstitutiveLaw<MeshType>* createEMMaterial()
{
    return new EMStructuralConstitutiveLaw<MeshType>();
}
namespace
{
static bool registerEM = StructuralConstitutiveLaw<LifeV::RegionMesh<LinearTetra> >::StructureMaterialFactory::instance().registerProduct ( "EMMaterial", &createEMMaterial<LifeV::RegionMesh<LinearTetra> > );
}


}
#endif /*_STRUCTURALMATERIAL_H*/
