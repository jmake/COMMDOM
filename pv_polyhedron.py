
#--------------------------------------------------------------------------||--#
"""
1) 
  Add Macro -> Add new macro
  Macros will be displayed in the macros menu and the macros toolbar
2)
  Select  vtkUnstructuredGrid or vtkPolyData 
3) 
  Check the directory, Alya files will be created 
  
Created: 
        miguel zavala, Oct 29, 2014, Barcelona

"""
#--------------------------------------------------------------------------||--#

Script03 = """
import numpy as np 
import vtk 
#--------------------------------------------------------------------------||--#
#--------------------------------------------------------------------------||--#
def get_vtk_CenterOfMass( points ):  
    polydata = vtk.vtkPolyData() 
    polydata.SetPoints( points )

    centerOfMassFilter = vtk.vtkCenterOfMass()  
    if vtk.VTK_MAJOR_VERSION <= 5: 
      centerOfMassFilter.SetInput( polydata )
    else: 
      centerOfMassFilter.SetInputData( polydata )

    centerOfMassFilter.SetUseScalarsAsWeights(False)
    centerOfMassFilter.Update()

    center = centerOfMassFilter.GetCenter()  
    #print "center:", center  

    return center 


def set_vtk_unstructured( vtk_data, n_pts=0, vtk_pts=[], n_cells=0, vtk_cells_type=[], vtk_cells_id=[] ): 
    vtk_data.SetPoints( vtk_pts )

    vtk_data.Allocate( n_cells, 1) #<-- paraview ?? 
    for cell_type, cell_ids in zip(vtk_cells_type, vtk_cells_id):
      vtk_data.InsertNextCell(cell_type, cell_ids)

    print "[set_vtk_unstructured] ", 
    print "pts/cells: %d %d" %( n_pts, n_cells )


def MakePolyhedron():
    ##  Make a regular dodecahedron. It consists of twelve regular pentagonal
    ##  faces with three faces meeting at each vertex.
    # numberOfVertices = 20
    numberOfFaces = 12
    # numberOfFaceVertices = 5

    points = vtk.vtkPoints()
    points.InsertNextPoint(1.21412, 0, 1.58931)
    points.InsertNextPoint(0.375185, 1.1547, 1.58931)
    points.InsertNextPoint(-0.982247, 0.713644, 1.58931)
    points.InsertNextPoint(-0.982247, -0.713644, 1.58931)
    points.InsertNextPoint(0.375185, -1.1547, 1.58931)
    points.InsertNextPoint(1.96449, 0, 0.375185)
    points.InsertNextPoint(0.607062, 1.86835, 0.375185)
    points.InsertNextPoint(-1.58931, 1.1547, 0.375185)
    points.InsertNextPoint(-1.58931, -1.1547, 0.375185)
    points.InsertNextPoint(0.607062, -1.86835, 0.375185)
    points.InsertNextPoint(1.58931, 1.1547, -0.375185)
    points.InsertNextPoint(-0.607062, 1.86835, -0.375185)
    points.InsertNextPoint(-1.96449, 0, -0.375185)
    points.InsertNextPoint(-0.607062, -1.86835, -0.375185)
    points.InsertNextPoint(1.58931, -1.1547, -0.375185)
    points.InsertNextPoint(0.982247, 0.713644, -1.58931)
    points.InsertNextPoint(-0.375185, 1.1547, -1.58931)
    points.InsertNextPoint(-1.21412, 0, -1.58931)
    points.InsertNextPoint(-0.375185, -1.1547, -1.58931)
    points.InsertNextPoint(0.982247, -0.713644, -1.58931)


    # Dimensions are [numberOfFaces][numberOfFaceVertices]
    dodechedronFace = [
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

    dodechedronFacesIdList = vtk.vtkIdList()
    dodechedronFacesIdList.InsertNextId(numberOfFaces)
    for face in dodechedronFace:
        dodechedronFacesIdList.InsertNextId(len(face))
        [dodechedronFacesIdList.InsertNextId(i) for i in face]

    COM = []  
    for face in dodechedronFace:
        pts = vtk.vtkPoints()
        pts.SetNumberOfPoints( len(face) ) 
        for i, ivertex in enumerate(face):
          pt = points.GetPoint(ivertex) 
          pts.SetPoint(i, pt)

        center = get_vtk_CenterOfMass( pts ) 
        COM.append( center )
        print center 

    n_COM  = len(COM) 
    n_COM += points.GetNumberOfPoints() 
    points.Resize( n_COM )

    for pt in COM:  points.InsertNextPoint( pt ) 
    
    #uGrid = vtk.vtkUnstructuredGrid()
    #uGrid.InsertNextCell(vtk.VTK_POLYHEDRON, dodechedronFacesIdList)
    #uGrid.SetPoints(points)
    #return uGrid

    n_cells = 1
    vtk_cells_type = [vtk.VTK_POLYHEDRON]
    vtk_cells_id=[dodechedronFacesIdList]

    set_vtk_unstructured( self.GetUnstructuredGridOutput(), 
                          points.GetNumberOfPoints(), points, 
                          n_cells, vtk_cells_type, vtk_cells_id) 


MakePolyhedron()  
#--------------------------------------------------------------------------||--#
#VTU = VTU_CREATOR( )
#
#for coord in ALYA_DATA['COORDINATES']: 
#  VTU.set_point( coord ) 
#
#VTK2ALYA = Vtk2Alya_cell()
#for cell in ALYA_DATA['ELEMENTS']: 
#  alya_cell = 30 
#  vtk_cell  = VTK2ALYA.get_vtk_cell( alya_cell ) 
#
#  cell -= 1 
#  VTU.set_cell( cell, vtk_cell[2] )
#
#VTU.set_vtk_unstructured( vtk_data=self.GetUnstructuredGridOutput() ) 
#--------------------------------------------------------------------------||--#
"""

#--------------------------------------------------------------------------||--#
#--------------------------------------------------------------------------||--#
import paraview.simple as Simple 


#-----------------------------------------------------| Programmable Source |--#
programmableSource1                          = Simple.ProgrammableSource()
programmableSource1.OutputDataSetType        = 'vtkUnstructuredGrid'
programmableSource1.Script                   = Script03 
programmableSource1.ScriptRequestInformation = ''  
programmableSource1.PythonPath               = ''

Simple.SetActiveSource( programmableSource1 )
Simple.Show( programmableSource1 ) 



#--------------------------------------------------------------------------||--#
import os
print "Directory: \'%s\'" % os.getcwd()

print "OK!\n"


#--------------------------------------------------------------------------||--#
#
# http://www.paraview.org/Wiki/Python_Programmable_Filter 
#
#--------------------------------------------------------------------------||--#
