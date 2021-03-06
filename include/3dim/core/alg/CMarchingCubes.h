///////////////////////////////////////////////////////////////////////////////
// $Id$
//
// 3DimViewer
// Lightweight 3D DICOM viewer.
//
// Copyright 2008-2016 3Dim Laboratory s.r.o.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//   http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef CMarchingCubes_H
#define CMarchingCubes_H

////////////////////////////////////////////////////////////
// Includes

#include <geometry/base/CMesh.h>

// VPL
#include <VPL/Image/DensityVolume.h>
#include <VPL/Module/Serializable.h>
#include <VPL/Module/Progress.h>
#include <VPL/Image/Vector3.h>

// STL
#include <vector>
#include <set>
#include <map>

////////////////////////////////////////////////////////////
//! Enum for marking each vertex of mesh.
enum VertexType
{
    VT_NONE = 0,
    VT_FLAT,
    VT_NEAR,
    VT_EDGE,
    VT_CORNER
};

////////////////////////////////////////////////////////////
//! Marching Cubes Functor
class IMarchingCubesFunctor
{
public:
    virtual unsigned char operator()(int x, int y, int z) const = 0;
    virtual vpl::img::CSize3i getVolumeDimensions() const = 0;
    virtual vpl::img::CSize3d getVoxelSize() const = 0;
};

////////////////////////////////////////////////////////////
//! Marching Cubes Worker - MC algorithm implementation class.
class CMarchingCubesWorker
{
private:
    //! Mesh
    geometry::CMesh m_mesh;
    std::set<std::pair<geometry::CMesh::VertexHandle, int> > m_openEdgeVertices;

    vpl::img::CSize3d m_voxelSize;

    //! Pointer on up codes of cubes.
    unsigned char *m_cube_code_matrix;

    //! Pointer on down codes of cubes.
    unsigned char *m_down_cube_code_matrix;

    //! Pointer on voxels state matrix, up and down.
    unsigned char *m_state_matrix_up;
    unsigned char *m_state_matrix_down;

    //! Actual cube vertices coordinates.
    geometry::CMesh::Point m_cube_vertices[8];

    //! Pointer on up work matrices of edges vertices.
    geometry::CMesh::VertexHandle *m_node_matrix_up_h;
    geometry::CMesh::VertexHandle *m_node_matrix_up_v;

    //! Pointer on down work matrices of edges vertices.
    geometry::CMesh::VertexHandle *m_node_matrix_down_h;
    geometry::CMesh::VertexHandle *m_node_matrix_down_v;

    //! Pointer on middle work matrices of edges vertices.
    geometry::CMesh::VertexHandle *m_node_matrix_middle;

    //! Size of help work matrices.
    int m_work_matrices_size_x;
    int m_work_matrices_size_y;

    //! actual volume grid size
    vpl::tSize m_volume_start_x;
    vpl::tSize m_volume_start_y;
    vpl::tSize m_volume_start_z;
    vpl::tSize m_volume_end_x;
    vpl::tSize m_volume_end_y;
    vpl::tSize m_volume_end_z;

public:
    CMarchingCubesWorker();
    ~CMarchingCubesWorker();

    geometry::CMesh &getMesh();
    std::set<std::pair<geometry::CMesh::VertexHandle, int> > &getOpenEdgeVertices();

    //! Set volume sizes - in voxels.
    void setVolumeOfInterest(vpl::tSize startX, vpl::tSize startY, vpl::tSize startZ, vpl::tSize endX, vpl::tSize endY, vpl::tSize endZ);

    //! Marching cubes triangle mesh generation.
    bool generateMesh(const IMarchingCubesFunctor *volume_functor, bool markFaces);

    //! Reduce flat areas of tri mesh generated by Marching Cubes algorithm
    void reduceFlatAreas(int numOfIterations = 1, bool eliminateNearVertices = true, float maxEdgeLength = 0.0f);
    static void reduceFlatAreas(geometry::CMesh &mesh, std::vector<geometry::CMesh::VertexHandle> &vertices, int numOfIterations = 1, bool eliminateNearVertices = true, float maxEdgeLength = 0.0f);

protected:
    void updateOpenEdgeVertices();
    void lockOpenEdges();
    void unlockOpenEdges();

    //! (De)Allocation of work matrices.
    void allocWorktMem(int size_x, int size_y);
    void deallocWorktMem();

    //! Make tris for actual cube, given coordinates of processed planes and cube code.
    void makeTri(int x, int y, unsigned char cube_code);

    //! Patching holes of tri mesh in case of complement cube code
    void holeFilling(int x, int y, unsigned char code, bool fill_down, bool fill_front, bool fill_left);

    //! Get/Set pointer on vertex of cube node by given coordinates and cube edge index.
    OpenMesh::VertexHandle getCubeEdgeNode(int edge_index, int x, int y);
    void setCubeEdgeNode(int edge_index, int x, int y, geometry::CMesh::VertexHandle new_vertex);

    //! Set cube node Z/Y/X coordinates.
    void setCodeCoordinateZ(double z, double dz);
    void setCodeCoordinateY(double y, double dy);
    void setCodeCoordinateX(double x, double dx);

    //! Get cube code of down/front/left cube.
    unsigned char getCubeCodeDown(int x, int y);
    unsigned char getCubeCodeFront(int x, int y);
    unsigned char getCubeCodeLeft(int x, int y);

    //! Create actual cube code and save it into code matrix
    unsigned char makeCubeCode(int x, int y);

    //! Calculate number of nodes for cube code.
    int cubeCodeNodeNumber(unsigned char cube_code);

    //! Calculate tri mesh all tris quality and save it as a value.
    static void makeAllTrisQuality(geometry::CMesh &mesh);

    //! Markup all vertices of input tri mesh (divide them on three types: flat, edge and corner)
    static void markupVertices(geometry::CMesh &mesh, std::vector<geometry::CMesh::VertexHandle> &vertices);
    static void resolveVertexFlag(geometry::CMesh &mesh, geometry::CMesh::VertexHandle vertex, OpenMesh::VPropHandleT<int> &vProp_flag, OpenMesh::FPropHandleT<short> &fProp_flag);
    static void resolveNearVertexFlag(geometry::CMesh &mesh, geometry::CMesh::VertexHandle vertex, OpenMesh::VPropHandleT<int> &vProp_flag);

    //! Eliminate vertex
    static bool eliminateFlatVertex(geometry::CMesh &mesh, geometry::CMesh::VertexHandle eliminate_vertex, float maxEdgeLength);
    static bool eliminateNearVertex(geometry::CMesh &mesh, geometry::CMesh::VertexHandle eliminate_vertex, float maxEdgeLength);
    static bool eliminateEdgeVertex(geometry::CMesh &mesh, geometry::CMesh::VertexHandle eliminate_vertex, float maxEdgeLength);

    //! Swap all edges of input tri mesh to improve their quality (shape). 
    static void swapTriEdges(geometry::CMesh &mesh);

    //! Swap maximal edge of input tri to improve his quality.
    static bool swapTriImproveQuality(
        geometry::CMesh &mesh,
        geometry::CMesh::FaceHandle swap_tri,
        int loop,
        OpenMesh::FPropHandleT<double> fProp_quality,
        OpenMesh::FPropHandleT<geometry::CMesh::EdgeHandle> fProp_maxEdge,
        OpenMesh::FPropHandleT<geometry::CMesh::FaceHandle> fProp_neighbour,
        OpenMesh::FPropHandleT<int> fProp_loop);
};

////////////////////////////////////////////////////////////
//! Marching Cubes class.
class CMarchingCubes : public vpl::mod::CProgress
{
private:
    //! Pointer on created tri mesh.
    geometry::CMesh *m_mesh;

    //! actual volume voxel size
    vpl::img::CSize3d m_voxelSize;

public:
    //! Marching cubes class constructor.
    CMarchingCubes();

    //! Marching cubes class destructor.
    ~CMarchingCubes();

    //! Marching cubes triangle mesh generation.
    bool generateMesh(geometry::CMesh &mesh, const IMarchingCubesFunctor *volumeFunctor, bool reduceFlatAreas, int numOfIterations = 1, bool eliminateNearVertices = true, float maxEdgeLength = 0.0f);

    //! merge sourceMesh into targetMesh
    void mergeMeshes(geometry::CMesh &sourceMesh, std::set<std::pair<geometry::CMesh::VertexHandle, int> > &openEdgeVertices, geometry::CMesh &targetMesh, std::map<int, std::set<std::pair<geometry::CMesh::VertexHandle, int> > > &vertexMap, std::vector<geometry::CMesh::VertexHandle> &seamVertices);
};

////////////////////////////////////////////////////////////
//! Functor for thresholding volume on a specified range.
template <typename V, typename T>
class CThresholdFunctor : public IMarchingCubesFunctor
{
protected:
    //! Low and high threshold values.
    T m_low_threshold, m_high_threshold;

    //! Used volume
    V *m_volume;

    vpl::img::CSize3d m_voxelSize;

public:
    //! Default constructor.
    CThresholdFunctor(const T& low_threshold, const T& high_threshold, V* volume, vpl::img::CSize3d voxelSize)
        : m_low_threshold(low_threshold)
        , m_high_threshold(high_threshold)
        , m_volume(volume)
        , m_voxelSize(voxelSize)
    { }

    //! Limits value of a given parameter.
    virtual unsigned char operator()(int x, int y, int z) const
    {
        if (!m_volume->checkPosition(x, y, z))
        {
            return 0;
        }

        // take voxel value for given coordinates
        T voxel_value = m_volume->at(x, y, z);

        // test volume value to threshold
        if ((voxel_value >= m_low_threshold) && (voxel_value <= m_high_threshold))
        {
            return 1;
        }
        else
        {
            return 0;
        }
    }

    //! Set volume
    void SetVolume(V *volume)
    {
        m_volume = volume;
    }

    virtual vpl::img::CSize3i getVolumeDimensions() const
    {
        return m_volume->getSize();
    }

    virtual vpl::img::CSize3d getVoxelSize() const
    {
        return m_voxelSize;
    }
};

////////////////////////////////////////////////////////////
//! Functor for thresholding volume on a specified range.
template <typename V, typename T, typename MV, typename MT>
class CMaskedThresholdFunctor : public IMarchingCubesFunctor
{
protected:
    //! Low and high threshold values.
    T m_low_threshold, m_high_threshold;

    //! Used volume
    V *m_volume;

    //! Mask value to get actual voxel's bit value
    MT m_mask_value;

    //! Mask bit value to compare (for inversion support)
    MT m_mask_bit;

    //! Mask volume
    MV *m_mask_volume;

    //! Neighbourhood samples
    int m_samples;

    //! Limit for neighbourhood sample count considered as "good"
    double m_limit;

    vpl::img::CSize3d m_voxelSize;

public:
    //! Default constructor.
    CMaskedThresholdFunctor(const T& low_threshold, const T& high_threshold, V *volume, const MT &mask_value, const MT& mask_bit, MV *mask_volume, vpl::img::CSize3d voxelSize, double limit /* = 0.5 */, int samples /* = 1 */)
        : m_low_threshold(low_threshold)
        , m_high_threshold(high_threshold)
        , m_volume(volume)
        , m_mask_value(mask_value)
        , m_mask_volume(mask_volume)
        , m_mask_bit(mask_bit)
        , m_samples(std::abs(samples))
        , m_limit(limit)
        , m_voxelSize(voxelSize)
    { }

    //! Limits value of a given parameter.
    virtual unsigned char operator()(int x, int y, int z) const
    {
        if (!m_volume->checkPosition(x, y, z) || (NULL!=m_mask_volume && !m_mask_volume->checkPosition(x, y, z)))
        {
            return 0;
        }

        // take mask voxel value for given coordinates
		if (NULL!=m_mask_volume)
		{
			MT mask_voxel_value = m_mask_volume->at(x, y, z);
			mask_voxel_value &= m_mask_value;

			// voxel is disabled by mask
			if (mask_voxel_value != m_mask_bit)
			{
				return 0;
			}
		}

        // analyze voxel and its neighbourhood
        double step = m_samples == 0 ? 0.0 : 0.5 / m_samples;
        int start = m_samples > 0 ? 1 - m_samples : 0;
        int sampleCount = m_samples * 2 + (m_samples > 0 ? 0 : 1);
        sampleCount = sampleCount * sampleCount * sampleCount;
        const double maxZ = m_volume->getZSize() - 1;
        const double maxY = m_volume->getYSize() - 1;
        const double maxX = m_volume->getXSize() - 1;
        int sum = 0;
#if(0)        
        for (int zOffset = start; zOffset <= m_samples; ++zOffset)
        {
			const double tz = z + zOffset * step;
			if (tz < 0.0 || tz > maxZ)
				continue;
            for (int yOffset = start; yOffset <= m_samples; ++yOffset)
            {
				const double ty = y + yOffset * step;
				if (ty < 0.0 || ty > maxY)
					continue;
                for (int xOffset = start; xOffset <= m_samples; ++xOffset)
                {
					const double tx = x + xOffset * step;
					if (tx < 0.0 || tx > maxX)
						continue;

                    // take voxel value for given coordinates
					//T voxel_value = m_volume->at(tx,ty,tz);
					T voxel_value = m_volume->interpolate(vpl::img::CPoint3d(tx, ty, tz));

                    // test volume value to threshold
                    if ((voxel_value >= m_low_threshold) && (voxel_value <= m_high_threshold))
                    {
                        sum++;
                    }

                    if (sum > sampleCount * m_limit)
                    {
                        return 1;
                    }
                }
            }
        }
#else   // slightly optimized version
        const vpl::tSize volYOffset = m_volume->getYOffset();
        const vpl::tSize volZOffset = m_volume->getZOffset();
        for (int zOffset = start; zOffset <= m_samples; ++zOffset)
        {
            const double tz = z + zOffset * step;
            if (tz < 0.0 || tz > maxZ)
                continue;
            const vpl::tSize iZ = vpl::tSize(tz);
            const double dZ = tz - double(iZ);
            const double dInvZ = 1.0 - dZ;
            for (int yOffset = start; yOffset <= m_samples; ++yOffset)
            {
                const double ty = y + yOffset * step;
                if (ty < 0.0 || ty > maxY)
                    continue;
                const vpl::tSize iY = vpl::tSize(ty);
                const double dY = ty - double(iY);
                const double dInvY = 1.0 - dY;

                const double dInvYInvZ = dInvY * dInvZ;
                const double dYdInvZ = dY * dInvZ;
                const double dInvYdZ = dInvY * dZ;
                const double dYdZ = dY * dZ;
                for (int xOffset = start; xOffset <= m_samples; ++xOffset)
                {
                    const double tx = x + xOffset * step;
                    if (tx < 0.0 || tx > maxX)
                        continue;

                    // take interpolated voxel value for given coordinates
                    vpl::tSize iX = vpl::tSize(tx);                                        
                    double dX = tx - double(iX);                                        
                    double dInvX = 1.0 - dX;                  

                    vpl::tSize idx = m_volume->getIdx(iX, iY, iZ);

                    // Interpolate voxel value
                    T Value = T(
                                        dInvX * (dInvYInvZ * m_volume->at(idx) +                                         
                                                dYdInvZ * m_volume->at(idx + volYOffset) +
                                                dInvYdZ * m_volume->at(idx + volZOffset) +
                                                dYdZ * m_volume->at(idx + volYOffset + volZOffset))
                                        +
                                        dX * (dInvYInvZ * m_volume->at(idx + 1) +
                                              dYdInvZ * m_volume->at(idx + 1 + volYOffset) +
                                              dInvYdZ * m_volume->at(idx + 1 + volZOffset) +
                                              dYdZ * m_volume->at(idx + 1 + volYOffset + volZOffset))
                                );

                    // test volume value to threshold
                    if ((Value >= m_low_threshold) && (Value <= m_high_threshold))
                    {
                        sum++;
                    }

                    if (sum > sampleCount * m_limit)
                    {
                        return 1;
                    }
                }
            }
        }
#endif
        return 0;
    }

    //! Set volume
    void SetVolume(V *volume, MV *mask_volume)
    {
        m_volume = volume;
        m_mask_volume = mask_volume;
    }

    virtual vpl::img::CSize3i getVolumeDimensions() const
    {
        return m_volume->getSize();
    }

    virtual vpl::img::CSize3d getVoxelSize() const
    {
        return m_voxelSize;
    }
};

#endif // CMarchingCubes_H

////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
