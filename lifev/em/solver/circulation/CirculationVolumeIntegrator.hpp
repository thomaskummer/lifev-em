//
//  CirculationTimeIntegrator.hpp
//  Circulation
//
//  Created by Thomas Kummer on 16.05.15.
//  Copyright (c) 2015 Thomas Kummer. All rights reserved.
//

#include <stdio.h>
#include <vector>
#include <string>


namespace LifeV
{

class VolumeIntegrator {
public:
    
    VolumeIntegrator(const std::vector<int>& bdFlags,
                     const std::string& domain,
                     const boost::shared_ptr <RegionMesh<LinearTetra> > fullMeshPtr,
                     const boost::shared_ptr <RegionMesh<LinearTetra> > localMeshPtr,
                     const boost::shared_ptr <ETFESpace<RegionMesh<LinearTetra>, MapEpetra, 3, 1> > ETFESpace,
                     const boost::shared_ptr <FESpace<RegionMesh<LinearTetra>, MapEpetra> > FESpace ) :
                M_localMeshPtr  ( localMeshPtr ),
                M_fullMesh      ( *fullMeshPtr ),
                M_bdFlags       ( bdFlags ),
                M_domain        ( domain ),
                M_ETFESpace     ( ETFESpace ),
                M_FESpace       ( FESpace )
    {
        if ( M_fullMesh.comm()->MyPID() == 0 )
        {
            std::cout << "\nVolume integrator " << M_domain << " created";
        }
       
        //initialize();
        
        if ( M_boundaryPoints.size() > 0 && M_fullMesh.comm()->MyPID() == 0 )
        {
            std::cout << "Volume integrator " << M_domain << " closed by " << M_boundaryPoints.size() << " boundary points" << std::endl;
        }
    }
    
    virtual ~VolumeIntegrator() {}
    
    void initialize()
    {
        findBoundaryPoints();
        sortBoundaryPoints();
    }
    
    void findBoundaryPoints()
    {	
        std::set<unsigned int> vertexIds;
        for (UInt iBFaceIn = 0; iBFaceIn < M_fullMesh.numBFaces(); ++iBFaceIn)
        {
            UInt markerIdIn = M_fullMesh.boundaryFace(iBFaceIn).markerID();
            if ( std::find(M_bdFlags.begin(), M_bdFlags.end(), markerIdIn) != M_bdFlags.end() )
            {
                for (UInt iBFaceOut = 0; iBFaceOut < M_fullMesh.numBFaces(); ++iBFaceOut)
                {
                    UInt markerIdOut = M_fullMesh.boundaryFace(iBFaceOut).markerID();
                    if ( std::find(M_bdFlags.begin(), M_bdFlags.end(), markerIdOut) == M_bdFlags.end() )
                    {
                        for (UInt iBPointIn = 0; iBPointIn < M_fullMesh.boundaryFace(iBFaceIn).S_numPoints; ++iBPointIn)
                        {
                            for (UInt iBPointOut = 0; iBPointOut < M_fullMesh.boundaryFace(iBFaceOut).S_numPoints; ++iBPointOut)
                            {
                                UInt pointIdIn = M_fullMesh.boundaryFace(iBFaceIn).point(iBPointIn).id();
                                UInt pointIdOut = M_fullMesh.boundaryFace(iBFaceOut).point(iBPointOut).id();
                                
                                if ( pointIdIn == pointIdOut )
                                {
                                    vertexIds.insert(pointIdIn);
                                }
                            }
                        }
                    }
                }
            }
        }

        M_boundaryPoints.clear();
        for (auto it = vertexIds.begin(); it != vertexIds.end(); ++it) M_boundaryPoints.push_back(*it);
    }
    
    
    void sortBoundaryPoints()
    {
        std::vector<int> pointsOrdered;
        
        unsigned int idx1 ( M_boundaryPoints[0] );
        int idx ( - 1 );
        Vector3D v1;
        Vector3D v;

        while ( idx != M_boundaryPoints[0] )
        {
            double distMin ( - 1.0 );
            for ( unsigned int i (0) ; i < M_boundaryPoints.size() ; ++i )
            {
                const unsigned int idx2 ( M_boundaryPoints[i] );
                if ( idx1 != idx2 && std::find(pointsOrdered.begin(), pointsOrdered.end(), idx2) == pointsOrdered.end() )
                {
                    Vector3D v2 = M_fullMesh.point(idx2).coordinates() - M_fullMesh.point(idx1).coordinates();
                    const Vector3D v1N = ( v1.norm() > 0 ? v1.normalized() : v1 );
                    const Vector3D v2N = ( v2.norm() > 0 ? v2.normalized() : v2 );
                    
                    const double sc = ( v1.norm() > 0.0 ? v1N.dot(v2N) : 1.0 );
                    const double dist = v2.norm() / std::max( std::pow(sc + 1, 2) , 0.25 );

                    if ( dist < distMin || distMin < 0.0 )
                    {
                        distMin = dist;
                        idx = idx2;
                        v = v2;
                    }
                }
            }

            pointsOrdered.push_back(idx);
            idx1 = idx;
            v1 = v;
        }
        
        if ( pointsOrdered.size() != M_boundaryPoints.size() &&  M_fullMesh.comm()->MyPID() == 0 )
        {
            throw std::runtime_error( "Sorting boundary points in " + M_domain + " failed!" );
        }

        M_boundaryPoints = pointsOrdered;
     }
    
    
    const Real computeOpenEndVolume (const VectorEpetra& disp,
                                     const int direction = - 1,
                                     const unsigned int component = 0) const
    {
        Vector3D componentVector; componentVector (component) = 1;

        auto boundaryCoordinates ( currentPosition(disp) );
        Vector3D centerPoint ( center(boundaryCoordinates) );

        // Compute volume
        auto itNext = boundaryCoordinates.begin();
        unsigned int i (0);
        Real volume (0.0);
        
        if ( M_fullMesh.comm()->MyPID() == 0 )
        {
            for (auto it = boundaryCoordinates.begin(); it != boundaryCoordinates.end(); ++it)
            {
                if ( i++ < boundaryCoordinates.size() - 1 ) std::advance(itNext, 1);
                else std::advance(itNext, - (boundaryCoordinates.size() - 1));
                
                Vector3D P1 = *it;
                Vector3D P2 = *itNext;
                
                Vector3D v1 = P1 - centerPoint;
                Vector3D v2 = P2 - centerPoint;
                
                Vector3D centerTriangle = ( P1 + P2 + centerPoint ) / 3;
                Real area = ( v1.cross(v2) ).norm() / 2;
                Vector3D normal = ( area > 0 ? ( v1.cross(v2) ).normalized() : Vector3D (0,0,0) );

                Real areaProjected = area * normal.dot(componentVector);
                
                volume += areaProjected * centerTriangle.dot(componentVector);
            }
        }
       
        MPI_Barrier(MPI_COMM_WORLD);
        MPI_Bcast(&volume, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
        return direction * volume;
    }
    
    
    template<class space>
    Real computeBoundaryVolume (const VectorEpetra& disp,
                                const boost::shared_ptr <space> dETFESpace,
                                int bdFlag) const
    {
        MatrixSmall<3, 3> Id;
        Id (0, 0) = 1.; Id (0, 1) = 0.; Id (0, 2) = 0.;
        Id (1, 0) = 0.; Id (1, 1) = 1.; Id (1, 2) = 0.;
        Id (2, 0) = 0.; Id (2, 1) = 0.; Id (2, 2) = 1.;
        VectorSmall<3> E1;
        E1 (0) = 1.; E1 (1) = 0.; E1 (2) = 0.;
        
        const VectorEpetra positionVector ( currentPositionVector(disp) );
        boost::shared_ptr<VectorEpetra> intergral ( new VectorEpetra ( positionVector.map() ) );
        
        {
            using namespace ExpressionAssembly;

            BOOST_AUTO_TPL (I, value (Id) );
            BOOST_AUTO_TPL (vE1, value (E1) );
            BOOST_AUTO_TPL (Grad_u, grad (dETFESpace, disp, 0) );
            BOOST_AUTO_TPL (F, (Grad_u + I) );
            BOOST_AUTO_TPL (FmT, minusT (F) );
            BOOST_AUTO_TPL (J, det (F) );
            
            QuadratureBoundary myBDQR (buildTetraBDQR (quadRuleTria7pt) );

            *intergral *= 0.0;
            integrate (boundary (M_localMeshPtr, bdFlag),
                       myBDQR,
                       M_ETFESpace,
                       value(-1.0) * J * dot (vE1, FmT * Nface) * phi_i) >> intergral;

            intergral->globalAssemble();

        }

//        std::cout << "bdFlag: " << bdFlag << std::endl;
//        std::cout << "\n Length: " << positionVector.epetraVector().MyLength() << "  " << (*intergral).epetraVector().MyLength() << std::endl;
//        std::cout << "\n Norm: " << positionVector.norm2() << "  " << (*intergral).norm2() << std::endl;

        return positionVector.dot (*intergral);
    }
    
    
    template<class space>
    const Real volume(const VectorEpetra& disp,
                      const boost::shared_ptr <space> dETFESpace,
                      const int direction = 1,
                      const unsigned int component = 0)
    {
        const boost::shared_ptr<Epetra_Comm> comm = M_fullMesh.comm();
        
        // Compute volume over boundary
        Real volumeBoundary (0);
        for ( auto& bdFlag : M_bdFlags )
        {
            volumeBoundary += computeBoundaryVolume(disp, dETFESpace, bdFlag);
        }
        
        // Compute volume over open-end-boundary
        //Real volumeOpenEnd = computeOpenEndVolume(disp, direction, component);
        
        // Compute total volume
        Real totalVolume = volumeBoundary;// + volumeOpenEnd;
        
        if (comm->MyPID() == 0)
        {
            std::cout << "\n=============================================================\n";
            std::cout << "Volume (boundary) in " << M_domain << ": \t" << volumeBoundary << std::endl;
            //std::cout << "Volume (open-end) in " << M_domain << ": \t" << volumeOpenEnd << std::endl;
            std::cout << "Volume (total) in " << M_domain << ": \t" << totalVolume << std::endl;
            std::cout << "=============================================================\n";
        }
        
        return totalVolume;
    }
    
    
protected:
    
    const std::vector<Vector3D> currentPosition(const VectorEpetra& disp) const
    {
        Int nLocalDof = disp.blockMap().NumGlobalElements(); //disp.epetraVector().MyLength();
        Int nComponentLocalDof = nLocalDof / 3;
        
        std::vector<Vector3D> boundaryCoordinates(M_boundaryPoints.size());

        for ( unsigned int i (0) ; i < M_boundaryPoints.size() ; ++i )
        {
            int root; int LID;
            disp.blockMap().RemoteIDList(1, &M_boundaryPoints[i], &root, &LID);

            if ( disp.blockMap().MyGID( M_boundaryPoints[i] ) )
            {
                Vector3D pointCoordinates;

            	UInt iGID = M_boundaryPoints[i];
            	UInt jGID = M_boundaryPoints[i] + nComponentLocalDof;
            	UInt kGID = M_boundaryPoints[i] + 2 * nComponentLocalDof;
   	
            	pointCoordinates[0] = M_fullMesh.point (iGID).x() + disp[iGID];
            	pointCoordinates[1] = M_fullMesh.point (iGID).y() + disp[jGID];
            	pointCoordinates[2] = M_fullMesh.point (iGID).z() + disp[kGID];

                boundaryCoordinates[i] = pointCoordinates;
            }

            MPI_Bcast(&boundaryCoordinates[i], 3, MPI_DOUBLE, root, MPI_COMM_WORLD);
        }
	
        MPI_Barrier(MPI_COMM_WORLD);
        return boundaryCoordinates;
    }
    
    
    const VectorEpetra currentPositionVector (const VectorEpetra& disp) const
    {
        // New P1 Space
        FESpace<RegionMesh<LinearTetra> , MapEpetra > p1FESpace ( M_localMeshPtr, "P1", 3, M_fullMesh.comm() );

        // Create P1 VectorEpetra
        VectorEpetra p1PositionVector (p1FESpace.map());

        // Fill P1 vector with mesh values
        Int p1nCompLocalDof = p1PositionVector.epetraVector().MyLength() / 3;
        for (int j (0); j < p1nCompLocalDof; j++)
        {
            UInt iGID = p1PositionVector.blockMap().GID (j);
            UInt jGID = p1PositionVector.blockMap().GID (j + p1nCompLocalDof);
            UInt kGID = p1PositionVector.blockMap().GID (j + 2 * p1nCompLocalDof);
            
            p1PositionVector[iGID] = M_fullMesh.point (iGID).x();
            p1PositionVector[jGID] = M_fullMesh.point (iGID).y();
            p1PositionVector[kGID] = M_fullMesh.point (iGID).z();
        }
        
        // Interpolate position vector from P1-space to current space
        VectorEpetra positionVector ( disp.map() );
        positionVector = M_FESpace -> feToFEInterpolate(p1FESpace, p1PositionVector);
        
        // Add displacement to position vector
        positionVector += disp;
        
        return positionVector;
    }
    
    
    const Vector3D center(const std::vector<Vector3D>& boundaryCoordinates) const
    {
        Vector3D center;
        for (auto it = boundaryCoordinates.begin(); it != boundaryCoordinates.end(); ++it)
        {
            center += *it / boundaryCoordinates.size();
        }
        return center;
    }
    
    
    const Vector3D center() const
    {
        Vector3D center0;
        for (auto it = M_boundaryPoints.begin(); it != M_boundaryPoints.end(); ++it)
        {
            center0 += M_fullMesh.point(*it).coordinates() / M_boundaryPoints.size();
        }
        return center0;
    }
    
    
    const Vector3D normal(const Vector3D& center0) const
    {
        auto itNext0 = M_boundaryPoints.begin();
        unsigned int j (0);
        Vector3D normal0;
        for (auto it = M_boundaryPoints.begin(); it != M_boundaryPoints.end(); ++it)
        {
            if ( j++ < M_boundaryPoints.size() - 1 ) std::advance(itNext0, 1);
            else std::advance(itNext0, - (M_boundaryPoints.size() - 1));
            
            Vector3D P1 ( M_fullMesh.point(*it).coordinates() );
            Vector3D P2 ( M_fullMesh.point(*itNext0).coordinates() );
            
            Vector3D v1 = P1 - center0;
            Vector3D v2 = P2 - center0;
            
            normal0 += ( v1.cross(v2) ).normalized();
        }
        return normal0.normalized();
    }
    
    
    const boost::shared_ptr<RegionMesh<LinearTetra> > M_localMeshPtr;
    const RegionMesh<LinearTetra>& M_fullMesh;
    const boost::shared_ptr <ETFESpace<RegionMesh<LinearTetra>, MapEpetra, 3, 1> > M_ETFESpace;
    const boost::shared_ptr <FESpace<RegionMesh<LinearTetra>, MapEpetra> > M_FESpace;

    const std::vector<int> M_bdFlags;
    const std::string M_domain;
    std::vector<int> M_boundaryPoints;
    
};

}
