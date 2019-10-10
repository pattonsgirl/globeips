#include <vtkSmartPointer.h>
#include <vtkOBJReader.h>

//includes from ReadOBJ
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkProperty.h>
#include <vtkCamera.h>
#include <vtkNamedColors.h>
//includes from GeoAssignCoordinates
#include <vtkDataSetAttributes.h>
#include <vtkGeoAssignCoordinates.h>
#include <vtkDoubleArray.h>
#include <vtkGraphMapper.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkMutableDirectedGraph.h>
#include <vtkTransform.h>
#include <vtkTransformPolyDataFilter.h>
#include <vtkSphereSource.h>
#include <vtkGlyph3D.h>
#include <vtkGraph.h>
#include <vtkGraphToPolyData.h>

//includes for C++
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
using namespace std;

//int main(int, char* [])
int main(int argc, char* argv[])
{

  //read in globe with OBJ Reader tools
  string filename = "../globe.obj";
  vtkOBJReader *globe = vtkOBJReader::New();
  globe->SetFileName(filename.c_str());
  //need to load model now to find center later
  globe->Update();

  //calculate center of the globe
  //lat lon coordinates are not off, just the globe
  double* globe_center = globe->GetOutput()->GetCenter();
  //double* angle = 90.00;
  //define our transform closer to origin
  vtkTransform *globe_to_origin = vtkTransform::New();
  //Wischgoll used 0, 1, 2, here - I'm a trusting person
  globe_to_origin->Translate(-globe_center[0],-globe_center[1],-globe_center[2]);
  //globe_to_origin->RotateY(90.0);
  //now implement the transform
  vtkTransformPolyDataFilter *globe_shifted = vtkTransformPolyDataFilter::New();
  globe_shifted->SetInputConnection(globe->GetOutputPort());
  globe_shifted->SetTransform(globe_to_origin);
  globe_shifted->Update();

  //initiatize a mutable graph
  vtkMutableDirectedGraph *latlon_graph = vtkMutableDirectedGraph::New();
  //initialize a latitude array
  vtkDoubleArray *latitude = vtkDoubleArray::New();
  latitude->SetName("latitude");
  //initialize a longitude array
  vtkDoubleArray *longitude = vtkDoubleArray::New();
  longitude->SetName("longitude");

  
  //open our data file (hardcoded)
  ifstream data;
  string line;
  float lat,lon;
  data.open("../locations.dat");
  //data.open("../dayton-orient.dat");
  //check if file opened:
  if(data.is_open())
  {
    //insert our data from the file
    cout << "Reading from lat/long coordinates from file" << endl;
    while(getline(data,line))
    {
      //cout << line << endl; 
      istringstream ss(line);
      //data is given in order of "longitude latitude"
      //needs to be parsed
      ss >> lon >> lat;
      //cout << lat << "\t" << lon << endl;
      latlon_graph->AddVertex();
      latitude->InsertNextValue(lat);
      longitude->InsertNextValue(lon);
    }
  }
  else
  {
    cout << "Data file failed to open" << endl;
    return EXIT_FAILURE;
  }
  
  data.close();
  

  /*
  //putting this back to test globe orientation
  for(int lat = -90; lat <= 90; lat += 10)
  {
    for(int lon = -180; lon <= 180; lon += 20)
    {
      latlon_graph->AddVertex();
      latitude->InsertNextValue(lat);
      longitude->InsertNextValue(lon);
    }
  }
  */


  latlon_graph->GetVertexData()->AddArray(latitude);
  latlon_graph->GetVertexData()->AddArray(longitude);

  //set colors - background, globe, ips
  vtkNamedColors *colors = vtkNamedColors::New();
  vtkColor3d backgroundColor = colors->GetColor3d("black");
  vtkColor3d globeColor = colors->GetColor3d("blue");
  vtkColor3d pinColor = colors->GetColor3d("yellow");


  vtkGeoAssignCoordinates *assign = vtkGeoAssignCoordinates::New();
  assign->SetInputData(latlon_graph);
  assign->SetLatitudeArrayName("latitude");
  assign->SetLongitudeArrayName("longitude");
  assign->SetGlobeRadius(62.0);
  assign->Update();

  //for glyphs, we need to convert assign to PolyData
  vtkGraphToPolyData *latlon_poly_graph = vtkGraphToPolyData::New();
  latlon_poly_graph->SetInputData(assign->GetOutput());
  latlon_poly_graph->Update();


  //mapper / actor for globe - updated to map the shifted (transformed globe)
  vtkPolyDataMapper *poly_mapper = vtkPolyDataMapper::New();
  //poly_mapper->SetInputConnection(globe->GetOutputPort());
  poly_mapper->SetInputConnection(globe_shifted->GetOutputPort());
  vtkActor *globe_actor = vtkActor::New();
  globe_actor->SetMapper(poly_mapper);
  globe_actor->GetProperty()->SetDiffuseColor(globeColor.GetData());

  //mapper / actor for lat long *** Not used once we create "pins"
  vtkGraphMapper *latlon_mapper = vtkGraphMapper::New();
  latlon_mapper->SetInputConnection(assign->GetOutputPort());
  vtkActor *latlong_actor = vtkActor::New();
  latlong_actor->SetMapper(latlon_mapper);
  //latlong_actor->GetProperty()->SetDiffuseColor(pinColor.GetData());

  //we want to use a glyph to map spheres to our lat lon squares
  vtkSphereSource *pin = vtkSphereSource::New();
  pin->SetThetaResolution(8);
  pin->SetPhiResolution(8);
  
  vtkGlyph3D *pin_glyph = vtkGlyph3D::New();
  pin_glyph->SetInputConnection(latlon_poly_graph->GetOutputPort());
  pin_glyph->SetSourceConnection(pin->GetOutputPort());
  pin_glyph->SetVectorModeToUseNormal();
  pin_glyph->SetScaleModeToScaleByVector();
  //set scale factor to big enough to see (1 or 2 works nicely)
  pin_glyph->SetScaleFactor(1.5);
  pin_glyph->Update();

  vtkTransform *rotate_pins = vtkTransform::New();
  //north/south orientation - 
  //-90 on X appeared correct (more north hem lat lons than south)
  rotate_pins->RotateX(-95.00);
  rotate_pins->RotateY(-5.0);
  rotate_pins->RotateZ(125.0);
  //now implement the transform
  vtkTransformPolyDataFilter *pins_shifted = vtkTransformPolyDataFilter::New();
  pins_shifted->SetInputConnection(pin_glyph->GetOutputPort());
  pins_shifted->SetTransform(rotate_pins);
  pins_shifted->Update();

  //create another mapper / actor to hold our glyph
  vtkPolyDataMapper *pin_mapper = vtkPolyDataMapper::New();
  pin_mapper->SetInputConnection(pins_shifted->GetOutputPort());
  vtkActor *pin_actor = vtkActor::New();
  pin_actor->SetMapper(pin_mapper);
  pin_actor->GetProperty()->SetDiffuseColor(pinColor.GetData());
  //pin_actor->Update();



  //set what to render here?
  vtkRenderer *ren = vtkRenderer::New();
  //ren->AddActor(latlong_actor);
  ren->AddActor(globe_actor);
  ren->AddActor(pin_actor);
  //ren->AddActor(pins_shifted);
  ren->SetBackground(backgroundColor.GetData());
  /*
  ren->ResetCamera();
  ren->GetActiveCamera()->Azimuth(30);
  ren->GetActiveCamera()->Elevation(30);
  ren->GetActiveCamera()->Dolly(1.5);
  ren->ResetCameraClippingRange();
  */

  //set up interactive window
  vtkRenderWindowInteractor *iren = vtkRenderWindowInteractor::New();
  //set render window
  vtkRenderWindow *win = vtkRenderWindow::New();
  win->AddRenderer(ren);
  win->SetInteractor(iren);
  ren->ResetCamera();

  win->SetSize(640,480);
  win->Render();
  iren->Initialize();
  iren->Start();

  return EXIT_SUCCESS;
}
