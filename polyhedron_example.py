import numpy as np
import sys 
sys.path.insert(0, "./")
import POLYHEDRON

Pts = [ (1.21412, 0, 1.58931),
(0.375185, 1.1547, 1.58931), 
(-0.982247, 0.713644, 1.58931), 
(-0.982247, -0.713644, 1.58931), 
(0.375185, -1.1547, 1.58931), 
(1.96449, 0, 0.375185), 
(0.607062, 1.86835, 0.375185), 
(-1.58931, 1.1547, 0.375185), 
(-1.58931, -1.1547, 0.375185), 
(0.607062, -1.86835, 0.375185), 
(1.58931, 1.1547, -0.375185), 
(-0.607062, 1.86835, -0.375185), 
(-1.96449, 0, -0.375185), 
(-0.607062, -1.86835, -0.375185), 
(1.58931, -1.1547, -0.375185), 
(0.982247, 0.713644, -1.58931), 
(-0.375185, 1.1547, -1.58931), 
(-1.21412, 0, -1.58931), 
(-0.375185, -1.1547, -1.58931), 
(0.982247, -0.713644, -1.58931) ] 

Dodecahedron = [
        [0, 1, 2, 3, 4],
        [0, 5, 10, 6, 1],
        [1, 6, 11, 7, 2],
        [2, 7, 12, 8, 3],
        [3, 8, 13, 9, 4],
        [4, 9, 14, 5, 0],
        [15, 10, 5, 14, 19],
        [16, 11, 6, 10, 15],
        [17, 12, 7, 11, 16],
        [18, 13, 8, 12, 17],
        [19, 14, 9, 13, 18],
        [19, 18, 17, 16, 15]
        ]

Pts = np.array(Pts) 
#
#for face in Dodecahedron: 
#  aver = np.zeros(3)
#  for idx in face:
#    pt =  Pts[idx]  
#    aver += pt   
#    print idx, 
#    
#  print aver/len(face) 
#print 



aux      = POLYHEDRON.ArrayD(3)
n_points = len(Pts) 
points   = POLYHEDRON.VectorV( n_points )
for i in range(n_points): 
  pt     = Pts[i]
  aux[0] = pt[0]
  aux[1] = pt[1]
  aux[2] = pt[2] 
  points[i].set(aux)

n_loops = [ len(face) for face in Dodecahedron ] 
#
n_faces = len(n_loops)
n_loops = sum(n_loops) 
#
faces = POLYHEDRON.VectorI(n_faces)
loops = POLYHEDRON.VectorI(n_loops) 
#
j = 0 
x = [] 
y = [] 
for i in range(n_faces): 
  faces[i] = j 
  x.append( j )
  face     = Dodecahedron[i]
  for f in face: 
    loops[j] = f 
    y.append(f)
    j += 1 


P = POLYHEDRON.PL()
P.setPositions( points ) 
P.setLoopsFaces(loops, faces)

#
Centroids = [] 
centroid = POLYHEDRON.ArrayD(3)
for i in range(3): centroid[i] = 0.0
#
aux = P.centroid()
#aux.vprint()
#print
aux.get( centroid )
Centroids.append( [centroid[0], centroid[1], centroid[2]] )
#
for iface in range(n_faces):
  aux = P.faceCentroid( iface )
  #aux.vprint() 
  #print 
  aux.get( centroid )
  Centroids.append( [centroid[0], centroid[1], centroid[2]] )
#print 


for pt in Centroids: 
  aux = POLYHEDRON.VC(pt[0], pt[1], pt[2]) 
  inside = False
  inside = P.isPointInside( aux )
  print inside,  

  aux = P.getFaceVectorError()
  #aux.vprint()
  #print 

  aux = POLYHEDRON.ArrayD(n_points)
  P.get_lambda( aux ) 
  Ws = [ aux[i] for i in range(n_points) ]
  print sum(Ws)


n_faces += 1
HEADER = """# vtk DataFile Version 2.0
CHINGON!! 
ASCII
DATASET POLYDATA
POINTS %d float"""
np.savetxt("aux.vtk", Centroids, header=HEADER%(n_faces), comments='',  fmt='%12.7f', ) 

