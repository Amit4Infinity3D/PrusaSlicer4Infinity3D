#ifndef slic3r_SurfaceMesh_hpp_
#define slic3r_SurfaceMesh_hpp_

#include <admesh/stl.h>

namespace Slic3r {

class TriangleMesh;



enum Face_index : int;

class Halfedge_index {
    friend class SurfaceMesh;

public:
    Halfedge_index() : m_face(Face_index(-1)), m_side(0) {}
    Face_index face() const { return m_face; }
    bool is_invalid() const { return int(m_face) < 0; }
    bool operator!=(const Halfedge_index& rhs) const { return ! ((*this) == rhs); }
    bool operator==(const Halfedge_index& rhs) const { return m_face == rhs.m_face && m_side == rhs.m_side; }

private:
    Halfedge_index(int face_idx, unsigned char side_idx) : m_face(Face_index(face_idx)), m_side(side_idx) {}

    Face_index m_face;
    unsigned char m_side;
};



class Vertex_index {
    friend class SurfaceMesh;

public:
    Vertex_index() : m_face(Face_index(-1)), m_vertex_idx(0) {}
    bool is_invalid() const { return int(m_face) < 0; }
    bool operator==(const Vertex_index& rhs) const = delete; // Use SurfaceMesh::is_same_vertex.

private:
    Vertex_index(int face_idx, unsigned char vertex_idx) : m_face(Face_index(face_idx)), m_vertex_idx(vertex_idx) {}

    Face_index m_face;
    unsigned char m_vertex_idx;
};



class SurfaceMesh {
public:
    explicit SurfaceMesh(const indexed_triangle_set& its)
    : m_its(its),
      m_face_neighbors(its_face_neighbors_par(its))
    {}
    SurfaceMesh(const SurfaceMesh&)            = delete;
    SurfaceMesh& operator=(const SurfaceMesh&) = delete;

    Vertex_index source(Halfedge_index h) const { assert(! h.is_invalid()); return Vertex_index(h.m_face, h.m_side); }
    Vertex_index target(Halfedge_index h) const { assert(! h.is_invalid()); return Vertex_index(h.m_face, h.m_side == 2 ? 0 : h.m_side + 1); }

    Face_index face(Halfedge_index h) const { assert(! h.is_invalid()); return h.m_face; }

    Halfedge_index next(Halfedge_index h)     const { assert(! h.is_invalid()); h.m_side = (h.m_side + 1) % 3; return h; }
    Halfedge_index prev(Halfedge_index h)     const { assert(! h.is_invalid()); h.m_side = (h.m_side == 0 ? 2 : h.m_side - 1); return h; }
    Halfedge_index halfedge(Vertex_index v)   const { return Halfedge_index(v.m_face, (v.m_vertex_idx == 0 ? 2 : v.m_vertex_idx - 1)); }
    Halfedge_index halfedge(Face_index f)     const { return Halfedge_index(f, 0); }
    Halfedge_index opposite(Halfedge_index h) const;
    Halfedge_index next_around_target(Halfedge_index h) const { return opposite(next(h)); }
    Halfedge_index prev_around_target(Halfedge_index h) const { return prev(opposite(h)); }
    Halfedge_index next_around_source(Halfedge_index h) const { return next(opposite(h)); }
    Halfedge_index prev_around_source(Halfedge_index h) const { return opposite(prev(h)); }
    Halfedge_index halfedge(Vertex_index source, Vertex_index target) const;

    const stl_vertex& point(Vertex_index v) const { return m_its.vertices[m_its.indices[v.m_face][v.m_vertex_idx]]; }

    bool is_border(Halfedge_index h) const { return m_face_neighbors[h.m_face][h.m_side] == -1; }

    bool is_same_vertex(const Vertex_index& a, const Vertex_index& b) const { return m_its.indices[a.m_face][a.m_vertex_idx] == m_its.indices[b.m_face][b.m_vertex_idx]; }



private:
    const std::vector<Vec3i> m_face_neighbors;
    const indexed_triangle_set& m_its;
};

} //namespace Slic3r

#endif // slic3r_SurfaceMesh_hpp_
