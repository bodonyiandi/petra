#include <iostream>
#include <osmium/osm.hpp>
#include <osmium/io/any_input.hpp>
#include <string>
#include <vector>
#include <map>

using namespace std;
struct utvonal {
    string nev;
    string honnan;
    string hova;
    vector<osmium::object_id_type> megallok;
};

struct megallo{
  string nev;
  osmium::Location hol_van;
  
};

std::map<osmium::object_id_type,megallo> terkep;

int main(int argc, char** argv)
{ 
  vector<utvonal> utvonalak;
  if(argc<2)
	{
		cout << "tul keves argumentum" << endl;
		return 0;
	}
	
	{
	  osmium::io::Reader reader(argv[1]);
	  osmium::memory::Buffer buff;

	  while(buff=reader.read())
	  {
		  for (auto& item:buff)
		  {
			  if(item.type() == osmium::item_type::node)
			  {
				  
				  osmium::Node& n = static_cast<osmium::Node&>(item);
				  if(n.tags().get_value_by_key("name")){
				    megallo megallo2;  
				    megallo2.nev=n.tags().get_value_by_key("name");
				    megallo2.hol_van=n.location();
				    terkep.insert({n.id(), megallo2});
				  }
			  }
			  if (item.type()==osmium::item_type::relation)
			  {
				  osmium::Relation& rel=static_cast<osmium::Relation&>(item);
				  if (string(rel.tags().get_value_by_key("route",""))=="bus")
				  {
				    utvonal ut;
				    if (rel.tags()["ref"])
				      ut.nev=rel.tags()["ref"];
				    else if (rel.tags()["name"])
				      ut.nev=rel.tags()["name"];
				    else ut.nev="???";
				    ut.honnan=rel.tags()["from"];
				    ut.hova=rel.tags()["to"];
				    for (auto& item:rel.members())
				    {
					if (item.type()==osmium::item_type::node && string(item.role())=="stop")
					  ut.megallok.push_back(item.ref());
				    }
				    utvonalak.push_back(ut);
				  }
				  
			  }
			  
		  }
	  }
	  
	  reader.close();
	}
    for (auto& item:utvonalak)
    {
	static string asd=string(64,'*');
	cout<<asd<<endl;
	cout<<"Busz szama:"<<item.nev<<", "<<item.honnan<<"-->"<<item.hova<<endl;
	cout<<asd<<endl;
	for(auto& megallo:item.megallok)
	{
	  cout<<terkep[megallo].nev<<", "<<terkep[megallo].hol_van<<endl;
	}
    }
  cout<<"ennyit talaltam: "<<utvonalak.size()<<endl;
  return 0;
}








