#include <iostream>
#include <fstream>

#ifndef COMMDOM_CS_H
#define COMMDOM_CS_H 

class CS
{
  public:
   ~CS()
    {
     
    }

//    CS(fvm_nodal_t  *_CsMesh )
    CS() 
    {
//      CsMesh = _CsMesh;  

      CsMesh = fvm_nodal_create("COMMDOM", 3);  // fvm/fvm_nodal_priv.h  
      CsMesh->n_cells    = 0;
      CsMesh->n_sections = 0;
      CsMesh->n_vertices = 0; 

      CsMesh->_vertex_coords    = NULL;
      CsMesh->parent_vertex_num = NULL;
      CsMesh->global_vertex_num = NULL;

      __set_dict__(); 

      _debug = true;
    }


    void create_sections( int n_sections, int n_coords )
    {
      __error__( n_coords==0, "n_coords==0");

      CsMesh->n_vertices = n_coords;
      CsMesh->n_sections = n_sections;
      CsMesh->sections   = (fvm_nodal_section_t**) malloc( n_sections );

      if(_debug)
        cout<<" [create_sections] '" 
            <<", n_sections='"<< n_sections <<"'"
            <<", n_coords='"<< n_coords <<"'"
            <<" \n";
    }


    void allocate_coords( void* coords )
    {
      __error__( coords==NULL, "coords==NULL");

      int n_coords = CsMesh->n_vertices;
      int n_dims   = CsMesh->dim;

      CsMesh->_vertex_coords = new cs_coord_t[n_dims * n_coords];
      CsMesh->vertex_coords = CsMesh->_vertex_coords;

      cs_coord_t* l_coords = (cs_coord_t*) coords;
      for(int i=0; i < n_dims*n_coords; i++) CsMesh->_vertex_coords[i] = l_coords[i];
    }


    void create_section(int type, int idx, int n_elements, int n_faces=0)
    {
      __error__( n_elements==0, "n_elements==0");

      fvm_element_t cell = (fvm_element_t) type;  
      CsMesh->sections[idx]             = fvm_nodal_section_create( cell );     // creating 'section type cell' 
      CsMesh->sections[idx]->n_elements = n_elements;                           // setting  'n_elements'
      CsMesh->sections[idx]->n_faces    = n_faces;                              // Number of faces defining polyhedra

      int stride = -1;
      int connectivity_size = -1;  
      if(n_faces == 0)
      {
        stride            = CsMesh->sections[idx]->stride;                    // <- fvm_nodal_n_vertices_element[type] (fvm_nodal_section_create)
        connectivity_size = stride * n_elements;
        CsMesh->sections[idx]->connectivity_size = connectivity_size;             // setting 'connectivity_size = n_elements * nodal_n_vertices_element'
      }

      if(_debug) 
        cout<<" [create_section] '" << CS_DICT[ type] 
            <<", n_elements='"<< n_elements <<"'"
            <<", n_faces='"<< n_faces <<"'"
            <<", connectivity_size='"<< connectivity_size <<"'"
            <<" \n"; 
    }


    void allocate_regular_section(int idx, int* elements) 
    {
      __error__(!(0 < idx < CsMesh->n_sections), "0 < idx <= CsMesh->n_sections");
      __error__( elements==NULL, "elements==NULL");
 
      int connectivity_size = -1;  

      if( (CsMesh->sections[idx]->n_faces == 0) )
      {
        connectivity_size = CsMesh->sections[idx]->connectivity_size;   
        CsMesh->sections[idx]->_vertex_num = new cs_lnum_t[ connectivity_size ];
        CsMesh->sections[idx]->vertex_num  = CsMesh->sections[idx]->_vertex_num;

        for(int j=0; j<connectivity_size; j++)  CsMesh->sections[idx]->_vertex_num[j] = elements[j]; // fill section with the vertices from cell i  
      }

      if(_debug)
        cout<<" [allocate_regular_section] '"<<" idx='"<< idx <<"'"
            <<", connectivity_size='"<< connectivity_size <<"'"
            <<" \n";

    }


    void allocate_irregular_section(int  idx, 
                                    int  n_vertices,
                                    int *vertex_num,   
                                    int *vertex_index, 
                                    int *face_num,  
                                    int *face_index     
                                   ) 
    {
      __error__(!(0 < idx < CsMesh->n_sections), "0 < idx <= CsMesh->n_sections");
      __error__(   (face_index==NULL)||(face_num==NULL)  ,   "face_XXX==NULL");
      __error__( (vertex_index==NULL)||(vertex_num==NULL), "vertex_XXX==NULL");


      // fvm/fvm_nodal.c:        fvm_nodal_section_copy_on_write
      // fvm/fvm_nodal_append.c: _transfer_to_section 

      // 'faces_index'.  size: n_elements + 1 
      int n_elements  = CsMesh->sections[idx]->n_elements;
      int n_faces     = CsMesh->sections[idx]->n_faces;

      if(n_faces > 0) 
      {
        __error__( n_vertices==0, "n_vertices==0");
        __error__( n_vertices!= vertex_index[n_faces], "n_vertices!= vertex_index[n_faces]"); 

        // 'face_index'.   size: n_elements +1 
        CsMesh->sections[idx]->_face_index   = new cs_lnum_t[n_elements + 1];  
        CsMesh->sections[idx]->face_index    = CsMesh->sections[idx]->_face_index;
        for(int j=0; j<n_elements + 1; j++)  CsMesh->sections[idx]->_face_index[j] = face_index[j]; 

        // 'faces_num'.    size: n_faces  
        CsMesh->sections[idx]->_face_num     = new cs_lnum_t[n_faces]; 
        CsMesh->sections[idx]->face_num      = CsMesh->sections[idx]->_face_num;
        for(int j=0; j<n_faces; j++)  CsMesh->sections[idx]->_face_num[j] = face_num[j]; 

        // 'vertex_index'. size: n_faces+1  
        CsMesh->sections[idx]->_vertex_index = new cs_lnum_t[n_faces + 1]; 
        CsMesh->sections[idx]->vertex_index  = CsMesh->sections[idx]->_vertex_index;
        for(int j=0; j<n_faces+1; j++)  CsMesh->sections[idx]->_vertex_index[j] = vertex_index[j];           

        // 'vertex_num'.   size: n_vertices 
        CsMesh->sections[idx]->_vertex_num   = new cs_lnum_t[n_vertices];
        CsMesh->sections[idx]->vertex_num    = CsMesh->sections[idx]->_vertex_num;
        for(int j=0; j<n_vertices; j++)  CsMesh->sections[idx]->_vertex_num[j] = vertex_num[j]; 

        //_fvm_nodal_section_dump
        if(false)
        if(vertex_index != NULL) 
        {
        int* idx = vertex_index;
        int* num = vertex_num;
        for (int i = 0; i < n_faces; i++) 
        {
          cout<< i+1 <<" (idx ="<< idx[i] <<") "<< num[ idx[i] ] <<" "; 
          for(int j = idx[i] + 1; j < idx[i + 1]; j++) cout<< num[j] <<" ";
          cout<<" \n";
        }
        cout<<" end  (idx ="<< idx[n_faces] <<") \n";
        }

        if(false)
        if(face_index != NULL)
        {
        int* idx = face_index;
        int* num = face_num;
        for (int i = 0; i < n_elements; i++)
        {
          cout<< i+1 <<" (idx ="<< idx[i] <<") "<< num[ idx[i] ] <<" ";
          for(int j = idx[i] + 1; j < idx[i + 1]; j++) cout<< num[j] <<" ";
          cout<<" \n";
        }
        cout<<" end  (idx ="<< idx[n_elements] <<") \n";
        }

      } // n_faces 

      if(_debug)
        cout<<" [allocate_irregular_section] '"<<" idx='"<< idx <<"'"
            <<", n_faces='"<< n_faces <<"'"
            <<", n_vertices='"<< vertex_index[n_faces] <<"'"
            <<" \n";
    }


    void print_section( int id, string name_file ) 
    {
//      fvm_element_t cell = (fvm_element_t) type;

//      fvm_nodal_section_t*  section = NULL;

//      idx          = TypeId[cell];
//      section      = CsMesh->sections[id]; 

      const cs_lnum_t *idx, *num;
//      int n_elements = section->n_elements;
//      int n_faces    = section->n_faces;  

      int n_elements  = CsMesh->sections[id]->n_elements;
      int n_faces     = CsMesh->sections[id]->n_faces;
/*
      if(_debug)
        cout<<" [print_section] '"<<" idx='"<< id <<"'"
            <<", n_elements='"<< n_elements <<"'"
            <<", n_faces='"<< n_faces <<"'"
            <<" \n";
*/
      if(true)
      if( CsMesh->sections[id]->vertex_num != NULL)
      {
        std::string        filename(name_file);
        std::stringstream  srut;
        srut<< std::setfill('0') << std::setw(3) << id;
        filename += "_"+srut.str();
        filename += "_regular.dat";

        ofstream myfile ( filename.c_str() );
        myfile.is_open(); 

        int stride = CsMesh->sections[id]->stride; 
        num =  CsMesh->sections[id]->vertex_num;
        for (int i = 0, k=0; i < n_elements; i++)
        {
          myfile<< i+1 <<""; 
          for(int j = 0; j < stride; j++) myfile<<" "<< num[k++] <<"";
          myfile<<"\n";
        }

        myfile.close();

        cout<<" [print_section] '"<<" idx='"<< id <<"'"
            <<", n_elements='"<< n_elements <<"'"
            <<", n_faces='"<< n_faces <<" '"
            << filename <<"' "
            <<" \n";

      }


      if(false)
      if( CsMesh->sections[id]->vertex_index != NULL)
      {
        std::string        filename(name_file);
        std::stringstream  srut;
        srut<< std::setfill('0') << std::setw(3) << id;
        filename += "_"+srut.str();
        filename += "_irregular.dat";

        ofstream myfile ( filename.c_str() );
        myfile.is_open();

        idx =  CsMesh->sections[id]->vertex_index;
        num =  CsMesh->sections[id]->vertex_num;
        for (int i = 0; i < n_faces; i++)
        {
          myfile<< i+1 <<" (idx ="<< idx[i] <<") "<< num[ idx[i] ] <<" ";
          for(int j = idx[i] + 1; j < idx[i + 1]; j++) myfile<< num[j] <<" ";
          myfile<<" \n";
        }
        myfile<<" end  (idx ="<< idx[n_faces] <<") \n";

        myfile.close();

        cout<<" [print_section] '"<<" idx='"<< id <<"'"
            <<", n_elements='"<< n_elements <<"'"
            <<", n_faces='"<< n_faces <<" '"
            << filename <<"' "
            <<" \n";
      }

      if(false)
      if ( CsMesh->sections[id]->face_index != NULL)
      {
        idx =  CsMesh->sections[id]->face_index;
        num =  CsMesh->sections[id]->face_num;
        for (int i = 0; i < n_elements; i++)
        {
          cout<< i+1 <<" (idx ="<< idx[i] <<") "<< num[ idx[i] ] <<" ";
          for(int j = idx[i] + 1; j < idx[i + 1]; j++) cout<< num[j] <<" ";
          cout<<" \n";
        }
        cout<<" end  (idx ="<< idx[n_elements] <<") \n";
      }

    }


    void __set_dict__()
    {
      CS_DICT[1] = "FVM_FACE_TRIA"; 
      CS_DICT[2] = "FVM_FACE_QUAD";
    //CS_DICT[3] = --- 
      CS_DICT[4] = "FVM_CELL_TETRA"; 
      CS_DICT[5] = "FVM_CELL_PYRAM"; 
      CS_DICT[6] = "FVM_CELL_PRISM"; 
      CS_DICT[7] = "FVM_CELL_HEXA"; 
      CS_DICT[8] = "FVM_CELL_POLY"; 
    }


   void __error__(bool ok, string str)
   {
     if(ok)
     {
       cout<<" ERROR! '"<< str <<"' ";
       cout<<" line:'"<< __LINE__ <<"' ";
       cout<<" file:'"<< __FILE__  <<"' ";
       cout<<" exit...\n\n";
       exit(1);
     } 
   }


   fvm_nodal_t  *CsMesh;


  private: 

//   fvm_nodal_t  *CsMesh;
   map<int,string> CS_DICT;              

   bool _debug;
}; 


#endif // COMMDOM_CS_H
