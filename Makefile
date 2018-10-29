CXX			= g++
CXXFLAGS	= -std=c++11
PROGRAM		= pcd_splitter

all:
			$(CXX) $(PROGRAM).cpp $(CXXFLAGS) -o $(PROGRAM)    

clean:
			rm -f $(PROGRAM)
