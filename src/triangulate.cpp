#include "triangulate.h"

using namespace std;
using namespace cv;

// External classes and structs
typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
typedef CGAL::Triangulation_vertex_base_2<K> Vb;
typedef CGAL::Delaunay_mesh_face_base_2<K> Fb;
typedef CGAL::Triangulation_data_structure_2<Vb, Fb> Tds;
typedef CGAL::Constrained_Delaunay_triangulation_2<K, Tds> CDT;
typedef CGAL::Delaunay_mesh_size_criteria_2<CDT> Criteria;
typedef CGAL::Delaunay_mesher_2<CDT, Criteria> Mesher;

typedef CDT::Vertex_handle Vertex_handle;
typedef K::Point_2 CGPoint;

void triangulate(vector<Point>& points, vector<int>& tris, const Mat& mask) {
    // Copy points to equivalent CGAL vector
    vector<CGPoint> vertices;
    int nPoints = points.size();
    for (int i = 0; i < nPoints; i++)
        vertices.push_back(CGPoint(points[i].x, points[i].y));
    
    // Prepare constraints for triangulation
    CDT cdt;
    for (int i = 0; i < nPoints; i++) {
        Vertex_handle point1 = cdt.insert(vertices[i]);
        Vertex_handle point2 = cdt.insert(vertices[i == nPoints - 1 ? 0 : i + 1]);
        cdt.insert_constraint(point1, point2);
    }

    // Validate current CDT
    assert(cdt.is_valid());

    // Start constrained Delaunay triangulation
    Mesher mesher(cdt);
    mesher.refine_mesh();

    // Collect and save results
    CDT::Finite_vertices_iterator vit;
    points.clear();
    for (vit = cdt.finite_vertices_begin(); vit != cdt.finite_vertices_end(); vit++)
        points.push_back(Point(vit->point().hx(), vit->point().hy()));
    
    CDT::Finite_faces_iterator fit;
    for (fit = cdt.finite_faces_begin(); fit != cdt.finite_faces_end(); fit++) {
        // Filter invalid triangles (vertices or gravity center out of mask)
        double cx = 0, cy = 0;
        bool flag = false;
        for (int i = 0; i < 3; i++) {
            Point cur(fit->vertex(i)->point().hx(), fit->vertex(i)->point().hy());
            cx += cur.x, cy += cur.y;
            if (mask.at<uchar>(cur.y, cur.x) == 0)
                flag = true;
        }
        cx /= 3, cy /= 3;
        if (mask.at<uchar>(cvRound(cy), cvRound(cx)) == 0)
            flag = true;
        if (flag)
            continue;
        
        // Save results
        for (int i = 0; i < 3; i++) {
            Point cur(fit->vertex(i)->point().hx(), fit->vertex(i)->point().hy());
            int index = find(points.begin(), points.end(), cur) - points.begin();
            tris.push_back(index);
        }
    }
}
