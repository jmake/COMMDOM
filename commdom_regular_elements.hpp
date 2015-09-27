#include <limits>
#include <numeric>
#include <ctime>

#ifndef COMMDOM_REGULARCELLS_H 
#define COMMDOM_REGULARCELLS_H  

class Regular 
{
  public:
   ~Regular()
    {
      if(_run)
      {
      }
    };


    Regular()
    {
       _cell      = -1; 
       _irregular =  false; 
       _run       =  false;  
       debug      =  true; 
       name       = "Regular";

       n_total_cells    = 0; 
//       n_total_vertex   = 0;

      _n_total_vertices = 0; 

         _max_n_total_vertices =  numeric_limits<int>::min();
      _id_max_n_total_vertices = -1;

         _min_n_total_vertices =  numeric_limits<int>::max();
      _id_min_n_total_vertices = -1;

      _n_global_cells      = 0;
      _n_global_vertices = 0; 

      __set_time__(); 
    };


    void allocate_cells( int cell, int n_cells )
    {
      _cell  = cell; 

      if(n_cells>0) _run = true;  

      if(_run)
      {
         _idx_cell = vector<int>(n_cells+0,-1);  //         Id/cell  
        _size_cell = vector<int>(n_cells+0,-1);  //       size/cell  
        _init_cell = vector<int>(n_cells+0,-1);  // vertices/cell  ¿Where the vertices start? 
            _index = vector<int>(n_cells+1,-1); 
            _index[0] = 0; 
      }

      if(debug) cout<<"["<< name <<"."<< _cell<<"] total_number_of_cells_type='"<< _idx_cell.size() <<"' \n";

      __set_time__(); 
    }


    void count_faces( int cell_type, int cell_idx, int cell_size , int* dummy=NULL)
    {

      if( (cell_type==_cell)&&(_run) )
      {
        if(_max_n_total_vertices   < cell_size)
        {  
             _max_n_total_vertices = cell_size;
          _id_max_n_total_vertices = cell_idx; 
        }

        if(_min_n_total_vertices   > cell_size)
        { 
             _min_n_total_vertices = cell_size;
          _id_min_n_total_vertices = cell_idx;
        }

        _idx_cell[n_total_cells] =  cell_idx;  
       _size_cell[n_total_cells] =  cell_size;
       _init_cell[n_total_cells] = _n_global_vertices;  // first vertex position (global) 
                  n_total_cells++;
      }

      _n_global_vertices += cell_size;
      _n_global_cells    += 1; 
    } 


    void allocate_connectivity()
    {
      int stride = _max_n_total_vertices - _min_n_total_vertices; 
      if(stride != 0)   
      {
         _irregular = true;
               name = "Irregular"; 
 
         cout<<"\n ["<< name <<"."<< _cell <<"] stride="<< stride <<" "; 
      } 


      if(_run)
      {
        int n_local_vertices = std::accumulate( _size_cell.begin(), _size_cell.end(), 0); 
        _connectivity = vector<int>( n_local_vertices, -1); 
      }

      if(debug) cout<<"["<< name <<"."<< _cell <<"] allocate_connectivity='"<< _connectivity.size() <<"'  \n";

      __set_time__(); 
    }


    void set_cells_by_ptr(int* vertices_ptr)
    {
      debug = false;

      if(_run)
      {
        int  n_vertices_before_me = 0;
        for(int i=0; i<_idx_cell.size(); i++)
        {
          if(debug) cout<< i <<"  "<< _idx_cell[i] <<" ";

          int  n_vertices_in_cell = _size_cell[i]; 
          int  first_vertex       = _init_cell[i];
          int *first_vertex_ptr   =  vertices_ptr + first_vertex; 

          if(_max_n_total_vertices < n_vertices_in_cell)
          {
            cout<< i <<"  "<< _idx_cell[i] <<" ";
            cout<< "_max_n_total_vertices="<< _max_n_total_vertices <<", ";
            cout<< "n_vertices_in_cell="<< n_vertices_in_cell <<" \n";
            __error__("_max_n_total_vertices < n_vertices");
          }

          // from 'local' to 'global' numeration  
          int  n_faces_in_cell = -1;
          set_connectivity_by_cell( i, n_vertices_in_cell, first_vertex_ptr, &n_faces_in_cell, NULL);   

          n_vertices_before_me += n_vertices_in_cell; 

          if(debug) cout<<"\n";          
        }
        // the last one is take into account HERE
        _vertex_index.push_back( _n_total_vertices ); 
//        face_index

      //if(debug) 
          cout<<"["<< name <<"."<< _cell <<"] n_vertices='" << n_vertices_before_me <<"' "; 
          cout<<"_vertex_index.back='"<< _vertex_index.back() <<"' "; 
          cout<<"_index.back='"<< _index.back() <<"' ";
          cout<<" \n";

        if(_index.back() != n_vertices_before_me) 
        {
          cout<<"\n\n["<< name <<"."<< _cell <<"] "; 
          cout<<"_index.back:"<< _index.back() <<" ";
          cout<<"n_vertices_before_me:"<< n_vertices_before_me  <<" \n";
          if(!_irregular) __error__("_index[local_idx] != n_vertices_before_me");    
        }

      }// _run  

      __set_time__(); 
    }


    void set_connectivity_by_cell(int  local_idx,     int local_n_vertices, int* ptIds,
                                  int *n_local_faces, int* local_faces=NULL)
    {
      if(_run)
      {
        int local_first_vertex =  _index[local_idx];
 
        if(_irregular) 
        {
          int n_vertices_in_cell = -1; 
          int    n_faces_in_cell = -1;
 
          int * first_vertex_in_cell_ptr = new int[_max_n_total_vertices];
          int *   first_face_in_cell_ptr = new int[_max_n_total_vertices];
          for(int j=0; j<_id_max_n_total_vertices; j++) first_vertex_in_cell_ptr[j] = -1;
          for(int j=0; j<_id_max_n_total_vertices; j++)   first_face_in_cell_ptr[j] = -1;

          set_faces_by_cell( ptIds,
                            &n_vertices_in_cell, first_vertex_in_cell_ptr,
                            &n_faces_in_cell,      first_face_in_cell_ptr);
  
          if(_max_n_total_vertices < n_vertices_in_cell) __error__("_max_n_total_vertices < n_vertices_in_cell");
          if(local_n_vertices      < n_vertices_in_cell) __error__("local_n_vertices      < n_vertices_in_cell");

          for(int j=0; j<n_vertices_in_cell; j++) _connectivity[_n_total_vertices+j] = first_vertex_in_cell_ptr[j]; // _vertex_num 
          _index[local_idx+1] = _index[local_idx] + n_faces_in_cell; // <-- _face_index 

          for(int j=0; j<n_faces_in_cell;    j++) first_face_in_cell_ptr[j] += _n_total_vertices;

          _vertex_index.insert( _vertex_index.end(), first_face_in_cell_ptr, first_face_in_cell_ptr + n_faces_in_cell); 

           n_local_faces[0]  = n_faces_in_cell; 
          _n_total_vertices += n_vertices_in_cell; 
        } 
        else
        {
          for(int j=0; j<local_n_vertices; j++) _connectivity[local_first_vertex+j] = ptIds[j];
          _index[local_idx+1] = local_first_vertex + local_n_vertices;
  
          n_local_faces[0] = 0;
        } 

        if(debug)
        {
          cout<<" | ";
          for(int i=0; i<local_n_vertices; i++) cout<< _connectivity[local_first_vertex+i] <<" ";
          cout<<"| ";
        }
      }
    }


    void set_faces_by_ptr(int* ptr)
    {
      /*
  _connectivity == _vertex_num <- set_connectivity_by_cell    
         _index == _face_index  <- set_connectivity_by_cell 
  _vertex_index ->    
      */
      if(_run)
      {
        if(_irregular)
        {
/*
          int n_faces = _index.back(); ; // <-- _face_index  
          cout<<"["<< name <<"] n_faces["<< _cell <<"]='" << n_faces <<"' \n";

          _vertex_index = vector<int>(n_faces+1,-1);  

          for(int j=0; j<n_faces_in_cell;    j++, l++) _vertex_index[l] = first_face_in_cell_ptr[j] + n_vertices_before_me;
*/
        }
      }
    }


    void set_faces_by_cell(int* ptIds, int* n_vertices, int* vertices, int* n_faces, int* faces)
    {
      /*
          ptIds = [numCellFaces, numFace0Pts, a01, a02, a03, numFace1Pts,b01, b02, b03, ...]   

                                       cell 
                   |<--------------------------------------->| 

           face = [  1             2                  n_faces ]
                   |            |             |    |  ...    |
     vertex_num = [ a01 a02 a03   b01 b02 b03   ...           ]  
   vertex_index = [  0             4                n_vertices]  
                                                            \_ vertex_index[-1] = total_num_of_vertices  
      */
        int numCellFaces = ptIds[0];

        if(debug) cout<<"numCellFaces="<< numCellFaces <<" ";

        vector<int>      face_num(numCellFaces  , 0);
        vector<int>  vertex_index(numCellFaces+1, 0);
        vector<int>    vertex_num;

        int m=0;
        for(int i=0; i<numCellFaces; i++)
        {
          m++;
          int numFaceiPts = ptIds[m];
        //if(debug) cout<<"[ "; 
          for(int k=0; k<numFaceiPts; k++)
          {
            m++;
            int id_k = ptIds[m];
          //if(debug) cout<< id_k <<" "; 
            vertex_num.push_back( id_k+1 ); // Fortran Style (PLE) 
          }
        //if(debug) cout<<"] ";
          vertex_index[i+1] = vertex_index[i] + numFaceiPts;
              face_num[i  ] = i;
        }

        if(debug)
        {
          cout<<" [";
          for(int i=0; i<vertex_index.size()-1; i++)
          {
            cout<<" | ";
            for(int j=vertex_index[i]; j<vertex_index[i+1]; j++) cout<< vertex_num[j] <<" ";
            cout<<"|";
          }
          cout<<"] ";
          cout<<" \n ";
        }

           n_faces[0] = numCellFaces;
        n_vertices[0] = vertex_index[numCellFaces];

        for(int i=0; i<numCellFaces+1;             i++)    faces[i] = vertex_index[i];
        for(int i=0; i<vertex_index[numCellFaces]; i++) vertices[i] = vertex_num[i];

    } // set_faces_by_cell


    void print_cells()
    {
      if(_run)
      {

//debug = _irregular;
 
        if(debug) 
          for(int i=0; i<_idx_cell.size(); i++)
          {
            cout<< i <<") g_idx="<< _idx_cell[i] <<" ";
            int n_elements = _index[i+1] - _index[i];
            cout<<" n_elems="<< n_elements <<" ";

            cout<<"  [ ";
            for(int j=_index[i]; j<_index[i+1]; j++)
            {
              int n_pts = _vertex_index[j+1] - _vertex_index[j]; 
              if(_irregular) 
              { 
                cout<<"| "; //cout<<"("<< n_pts <<") "; 
                for(int k= _vertex_index[j]; k<_vertex_index[j+1]; k++) cout<< _connectivity[k] <<" ";
                cout<<"| "; 
              }
              else
              {
                cout<< _connectivity[j] <<" ";  
              } 
            }
            cout<<"] ";
            cout<<"\n";
          } // for 

      } // _run 
    } // print  


    // C++ NOTE: Passing an Argument as a Reference to a Pointer. 
    //           'vertex_num' is now a reference to pointer '&(*( _connectivity.begin()+0))'!
    //           This means if we CHANGE 'vertex_num', we change '&(*( _connectivity.begin()+0))'!
    void get_cells_ptr(int*& vertex_num, int*& vertex_index, int*& face_num, int*& face_index)
    {
        face_index =  NULL; 
          face_num =  NULL;
      vertex_index =  NULL; // &(*( _index.begin()+0)); // 'stride' is a constant! 
        vertex_num = &(*( _connectivity.begin()+0)); 

      if(_irregular)
      {
           _face_num = vector<int>(_index.back(), -1); 
           for(int i = 0; i<_face_num.size(); i++) _face_num[i] = -(i+1);  // Fortran Style (PLE)  

          face_index = &(*(        _index.begin()+0)); 
            face_num = &(*(     _face_num.begin()+0));
        vertex_index = &(*( _vertex_index.begin()+0)); 
          vertex_num = &(*( _connectivity.begin()+0)); 
      }
/*
cout<<"[get_cells_ptr] Connectivity_size="<< _connectivity.size() <<" ";
cout<< vertex_num[_connectivity.size()-1]  <<" ";
cout<<"\n"; 
*/
    }


    void get_sizes( int* n_vertex, int* n_cells, int* n_faces )
    {
       n_cells[0] = _idx_cell.size(); 
       n_faces[0] = 0;
      n_vertex[0] = _index.back();  

      if(_irregular)
      {
         n_faces[0] = _index.back(); 
        n_vertex[0] = _vertex_index.back(); 
      }
    }


  private:

   void __error__(string str)
   {
     cout<<" ERROR! '"<< str <<"' "; 
     cout<<" line:'"<< __LINE__ <<"' ";
     cout<<" file:'"<< __FILE__  <<"' "; 
     cout<<" exit...\n\n";
     exit(1);
   }

    void __set_time__()
    {
      _timer.push_back( clock()/CLOCKS_PER_SEC );  
    }

    int n_total_cells; 

    string name;
    bool debug;
    bool   _run;

    bool _irregular; 

    int _cell; 
    int _n_total_vertices; 

    int _n_global_cells; 
    int _n_global_vertices;  

    int    _max_n_total_vertices; 
    int _id_max_n_total_vertices; 

    int    _min_n_total_vertices;
    int _id_min_n_total_vertices;

    vector<int> _idx_cell;
    vector<int> _init_cell;
    vector<int> _size_cell;

    vector<int> _connectivity;
    vector<int> _index;  
    vector<int> _vertex_index; 
    vector<int>   _face_num;

    vector<clock_t> _timer; 
};


class RegularCells
{
  public: 
   ~RegularCells()
    {
//      delete[] _TYPES; 
    }

    RegularCells()
    {
       _run     =  false;
       _debug   =  true;
       _name    = "RegularCells";

      _n_global_cells = 0; 

        n_types = -1; 
    }

    // 1.0 count type of cells 
    void count_cells( int type )
    {
      _n_global_cells++;
      set_val_dict( type );
    }


    void set_val_dict( int x )
    {
      _typesIt it = _types.find(x);

      if( it != _types.end() )
      {
        _types[x] += 1;
      }
      else
      {
        _types[x]  = 1;
      }

    }


    void print_dict()
    {
      cout<<"\n";
      for(_typesIt it = _types.begin(); it != _types.end(); it++)
      {
        cout<<"["<< _name <<"] '"<< it->first <<"' = '"<< it->second <<"' \n";
      }
    }

    // 2.0 allocate memory for idx_cell, first_cell, size_cell (size: n_cell_for_type)  
    void allocate_cells()
    {
      print_dict();

      n_types = _types.size();  
      _TYPES  = new Regular[n_types];  

      int i = 0; 
      for(_typesIt it = _types.begin(); it != _types.end(); it++) 
        _TYPES[i++].allocate_cells( it->first, it->second); 
    } 


    void count_faces( int cell_type, int cell_idx, int cell_size , double* dummy=NULL)
    {
      int i = 0;
      for(_typesIt it = _types.begin(); it != _types.end(); it++) 
        _TYPES[i++].count_faces(cell_type, cell_idx, cell_size);
    }


    void set_cells_by_ptr( int* ptr )
    {
      int i = 0;
      for(_typesIt it = _types.begin(); it != _types.end(); it++)
      {
        _TYPES[i].allocate_connectivity();  
        _TYPES[i].set_cells_by_ptr( ptr );
      //_TYPES[i].set_faces_by_ptr( ptr );
         i++; 
      }
    }

    void print()  
    {
      int i = 0;
      for(_typesIt it = _types.begin(); it != _types.end(); it++)
        _TYPES[i++].print_cells();
    }


    void get_sizes( int idx, int* n_total_vertex, int* n_total_cells, int* n_total_faces )
    {
      _TYPES[idx].get_sizes(n_total_vertex, n_total_cells, n_total_faces );
    }


    void get_cells_ptr(int idx, int*& vertex_num, int*& vertex_index, int*& face_num, int*& face_index)
    {
      _TYPES[idx].get_cells_ptr( vertex_num, vertex_index, face_num, face_index );
    }


    int n_types; 
    Regular* _TYPES;
    map<int,int>           _types;
    typedef typename map<int,int>::iterator _typesIt;

    _typesIt IT; 
    string _name; 

  private: 
//    map<int,int>           _types;
//    typedef typename map<int,int>::iterator _typesIt;

     bool _run; 
     bool _debug; 
//     string _name; 

     int _n_global_cells; 

//     Regular* _TYPES; 
}; 

#endif // COMMDOM_REGULARCELLS_H 
