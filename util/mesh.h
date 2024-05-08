/// @file mesh.h
///
/// @brief This file defines the mesh API.
///
/// The mesh format does not use compression because files are typically kilobytes, not megabytes, and there is
/// therefore no reason to compress them.
///
/// @copyright (c) 2024 Randomcode Developers

#pragma once

#include "purpl/purpl.h"

#include "common/common.h"
#include "common/filesystem.h"
#include "common/log.h"

/// @brief Mesh file magic string
#define MESH_MAGIC "PMSH"
/// @brief Mesh file magic number (little endian)
#define MESH_MAGIC_NUMBER 'HSMP'
/// @brief Mesh file magic signature length
#define MESH_MAGIC_LENGTH 4

/// @brief Mesh format version
#define MESH_FORMAT_VERSION 5

/// @brief Vertex
PURPL_MAKE_TAG(struct, VERTEX, {
    vec3 Position;
    vec4 Colour;
    vec2 TextureCoordinate;
    vec3 Normal;
})

/// @brief A mesh
PURPL_MAKE_TAG(struct, MESH, {
    union {
        CHAR MagicBytes[4];
        UINT32 Magic;
    };

    UINT32 Version;

    CHAR Material[32];

    UINT64 VertexCount;
    UINT64 IndexCount;

    /// @brief Indicates that the vertices and indices are dynamically allocated
    /// with CmnAlloc
    BOOLEAN DataSeparate;
    PVERTEX Vertices;
    ivec3 *Indices;
})

/// @brief Mesh file header size
#define MESH_HEADER_SIZE offsetof(MESH, DataSeparate)

/// @brief Check a mesh's validity
///
/// @param Mesh The mesh to check
///
/// @return Whether the mesh can be used
#define ValidateMesh(Mesh)                                                                                             \
    ((Mesh) && memcmp((Mesh)->MagicBytes, MESH_MAGIC, MESH_MAGIC_LENGTH) == 0 && (Mesh)->Version == MESH_FORMAT_VERSION)

/// @brief Create a mesh
///
/// @param Material The name of the material for the mesh
/// @param Vertices The vertex data of the mesh
/// @param VertexCount The number of vertices
/// @param Indices The index data of the mesh
/// @param IndexCount The number of indices
///
/// @return A mesh which can be freed with CmnFree.
extern PMESH CreateMesh(_In_z_ PCSTR Material, _In_ PCVERTEX Vertices, _In_ SIZE_T VertexCount,
                        _In_ CONST ivec3 *CONST Indices, _In_ SIZE_T IndexCount);

/// @brief Load a mesh from a file
///
/// @param Path The path of the mesh file
///
/// @return The loaded mesh, which can be freed with CmnFree.
extern PMESH LoadMesh(_In_z_ PCSTR Path);

/// @brief Write a mesh
///
/// @param Path The path to write the mesh into
/// @param Mesh The mesh to write
///
/// @return Whether the mesh could be written
extern BOOLEAN WriteMesh(_In_z_ PCSTR Path, _In_ PCMESH Mesh);
