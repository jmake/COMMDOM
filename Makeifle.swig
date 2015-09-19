

##
## Where is Python.h?
##   sudo find / -name 'Python.h'
##   python-config --ldflags      <- compilation options 
##  
INC_PY=-I/System/Library/Frameworks/Python.framework/Versions/2.7/include/python2.7/
LIB_PY=-L/System/Library/Frameworks/Python.framework/Versions/2.7/lib/ -lpython
##
CPP=c++ -w

all: 


shared: swig
        $(CPP) -shared  read_file_wrap.o -o _POLYHEDRON.so $(LIB_PY) 
        python ../example01.py 

swig: 
        swig -python -c++ read_file.i  
        $(CPP) -c -fPIC read_file_wrap.cxx $(INC_PY) 


clean:
        rm -rf *~ *.o *.x *.mod *.so *.a *.pyc *_wrap.cxx *.py *.gch  


#
# apt-get install python-dev swig python-mpi4py
# sudo port install swig
#
# +fatal error: 'Python.h' file not found
#
#  sudo find / -name 'Python.h'
# 
~          
