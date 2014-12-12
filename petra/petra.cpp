#include <iostream>
#include <osmium/osm.hpp>
#include <osmium/io/any_input.hpp>
#include <string>
#include <vector>
#include <map>
#include <queue>
#include <set>
#include <osmium/geom/haversine.hpp>
#include <cstdlib>
#include <list>

using osmium::geom::haversine::distance;

using namespace std;
struct csomopont_adat{
	osmium::object_id_type id;
	osmium::Location loc;
	std::set<osmium::object_id_type> szomszedok;
	
csomopont_adat(osmium::Node& node){
	id=node.id();
	loc=node.location();
}
csomopont_adat()=default;
~csomopont_adat()=default;
csomopont_adat(const csomopont_adat&)=default;
};

std::map<osmium::object_id_type,csomopont_adat> terkep;

int main(int argc, char** argv)
{
      std::cout.setf(std::ios::fixed);
      std::cout.precision(8);
  
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
				  terkep.insert({n.id(), csomopont_adat(n)});
				  continue;
			  }
			  
		  }
	  }
	  
	  reader.close();
      }
	
      {
	osmium::io::Reader reader(argv[1]);
	osmium::memory::Buffer buff;

	while(buff=reader.read())
	{
		for (auto& item:buff)
		{
			if(item.type() == osmium::item_type::way)
			{
				osmium::Way& r = static_cast<osmium::Way&>(item);
				for(int i=1;i<r.nodes().size();i++)
				{
				    csomopont_adat& cs=terkep[r.nodes()[i].ref()];
				    csomopont_adat& cs2=terkep[r.nodes()[i-1].ref()];
				    cs.szomszedok.insert(cs2.id);
				    cs2.szomszedok.insert(cs.id);
				} 
				
			}
		}
	}
      }
	
	osmium::Location kezdop(atof(argv[2]),atof(argv[3]));
	osmium::Location vegp(atof(argv[4]),atof(argv[5]));
	
	double dp=INFINITY;
	double dp2=INFINITY;
	osmium::object_id_type legkozelebbi;
	osmium::object_id_type legkozelebbiveg;
	double dk;
	double dv;
	for(auto& item:terkep)
	{
	   dk=distance(kezdop,item.second.loc);
	 
	  if (dk < dp)
	  {
	    dp = dk;
	    legkozelebbi=item.second.id;
	  }
	  
	  
	}
	for(auto& item:terkep)
	{
	  if (dv < dp2)
	  {
	    dv=distance(vegp,item.second.loc);
	    dp2=dv;
	    legkozelebbiveg=item.second.id;
	  }
	}
	
	std::cout << legkozelebbi << " / " << legkozelebbiveg << std::endl;
	
	std::queue<osmium::object_id_type> to_visit;
	std::map<osmium::object_id_type,double> tav;
	
	tav.insert({legkozelebbi,0});
	for(auto& item:terkep[legkozelebbi].szomszedok)
	  {
	      to_visit.push(legkozelebbi);
	      to_visit.push(item);
	  }
	  
	  
	while (!to_visit.empty())
	  {
	      osmium::object_id_type szulo;
	      osmium::object_id_type gyerek;
	      
	      szulo=to_visit.front();
	      to_visit.pop();
	      gyerek=to_visit.front();
	      to_visit.pop();
	      if (tav.count(gyerek)) //itt meg nem jartunk
		  {
		    continue;
		  }
	      tav.insert({gyerek,tav[szulo]+distance(terkep[szulo].loc,terkep[gyerek].loc)});
	      for(auto& item:terkep[gyerek].szomszedok)
		{
		  to_visit.push(gyerek);
		  to_visit.push(item);
		}
	  }
	  
	cout<<tav[legkozelebbiveg]<<std::endl;
	osmium::object_id_type aktualis;
	aktualis=legkozelebbiveg;
	std::list<osmium::object_id_type> lista;
	for (;;)
	{   
	    osmium::object_id_type kozeli_csp=*terkep[aktualis].szomszedok.begin();
	    double min=tav[kozeli_csp];
	    lista.push_front(aktualis);
	    if (aktualis==legkozelebbi) //ha visszaertunk a kezdopontba
	      break;
	    for (auto& item:terkep[aktualis].szomszedok)
	    {
		if (tav[item]<min)
		{
		  min=tav[item];
		  kozeli_csp=item;
		}
	    }
	    aktualis=kozeli_csp;
	}
	int db=0;
	for (auto& item:lista)
	{
	    db++;
	    cout<<terkep[item].loc<<std::endl;
	}
	cout<<"DB:"<<db;
	
	return 0;
}