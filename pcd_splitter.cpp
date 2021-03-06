#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <cmath>

using namespace std;

vector<string> split(const string &s, char delim)
{
  vector<string> elems;
  stringstream ss(s);
  string item;
  while (getline(ss, item, delim))
  {
    if (!item.empty())
    {
      elems.push_back(item);
    }
  }
  return elems;
}

int main(int argc, char** argv)
{
  if(argc < 2)
  {
    cerr << "Invalid argument num" << endl;
    return 1;
  }

  string filename;
  filename = argv[1];

  cout << "filename: " << filename << endl;

  vector<string> filenameVec = split(filename, '.');
  if(filenameVec.size() < 2)
  {
    cerr << "Invalid file path" << endl;
    return 1;
  }

  string filetype = filenameVec[filenameVec.size() - 1];
  if(filetype != "pcd")
  {
    cerr << "Invalid file type" << endl;
    return 1;
  }
  cout << "file type: " << filetype << endl;

  ifstream ifs;
  ifs.open(filename.c_str());
  if (!ifs) {
    cerr << "File open error:" << filename << endl;
    return 1;
  }

  int max_points = 100000000;

  if(argc == 3)
  {
    max_points = stoi(argv[2]);
  }

  cout << "#############################################" << endl;

  string line;
  vector<string> header;
  int width, height, points;
  for(int i=0; i<11; i++)
  {
    getline(ifs, line);
    header.push_back(line);

    if (line.substr (0, 5) == "WIDTH")
    {
      vector<string> strvec = split(line, ' ');
      width = stoi(strvec[1]);
    }

    if (line.substr (0, 6) == "HEIGHT")
    {
      vector<string> strvec = split(line, ' ');
      height = stoi(strvec[1]);
    }

    if (line.substr (0, 6) == "POINTS")
    {
      vector<string> strvec = split(line, ' ');
      points = stoi(strvec[1]);
    }

    if (line.substr (0, 4) == "DATA")
    {
      vector<string> strvec = split(line, ' ');
      if(strvec[1] != "binary")
      {
        cerr << strvec[1] << " format is not supported... :(" << endl;
        return 1;
      }
    }
  }

  int header_count = ifs.tellg(); // size of header
  cout << "header has " << header_count << " bytes." << endl;

  ifs.close();

  cout << "width: " << width << endl;
  cout << "height: " << height << endl;
  cout << "points: " << points << endl;

  if(width * height != points)
  {
    cerr << "Invalid point num (width * height != points)" << endl;
  }

  int num_split_files = ceil((double)points/max_points);
  cout << "Split to " << num_split_files << " pcd files..." << endl;

  vector<char> buf;
  buf.resize(max_points * 12);

  ifstream bifs(filename.c_str(), ios_base::in | ios_base::binary);
  if (!bifs) {
    cerr << "Binary File open error: " << filename << endl;
    return 1;
  }

  bifs.seekg( header_count );

  for(int i=0; i<num_split_files; i++)
  {
    string output_filename_base = "";
    for(int i=0; i<filenameVec.size()-2; i++)
    {
      output_filename_base += (filenameVec[i] + ".");
    }
    output_filename_base += filenameVec[filenameVec.size()-2];

    string output_filename = output_filename_base + "_" + to_string(i) + ".pcd";

    int write_points = max_points;
    if(i == num_split_files - 1)
    {
      write_points = points - max_points * (num_split_files - 1);
    }
    cout << "Write " << write_points << " points to " << output_filename << " ..." << endl;

    ofstream ofs;
    ofs.open(output_filename.c_str());
    if (!ofs) {
      cerr << "File open error: " << output_filename << endl;
      return 1;
    }

    // PCD file generated by cartographer has the following format:
    ofs << "# generated by pcd_splitter" << endl;
    ofs << "VERSION 0.7" << endl;
    ofs << "FIELDS x y z" << endl;
    ofs << "SIZE 4 4 4" << endl;
    ofs << "TYPE F F F" << endl;
    ofs << "COUNT 1 1 1" << endl;
    ofs << "WIDTH " << write_points << endl;
    ofs << "HEIGHT 1" << endl;
    ofs << "VIEWPOINT 0 0 0 1 0 0 0" << endl;
    ofs << "POINTS " << write_points << endl;
    ofs << "DATA binary" << endl;

    ofs.close();
    ofstream bofs(output_filename.c_str(), ios_base::out | ios_base::binary | ios::app);
    if (!bofs) {
      cerr << "Binary File open error: " << output_filename << endl;
      return 1;
    }

    buf.clear();

    bifs.read( &buf[0], write_points*12 );

    bofs.write( &buf[0], write_points*12 );

    bofs.close();
  }

  bifs.close();

  return 0;
}
