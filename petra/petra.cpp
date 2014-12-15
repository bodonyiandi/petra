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
			  
				const char* highway = r.tags() ["highway"];
				if(!highway)
				  continue;
				
				if ( !strcmp ( highway, "footway" )
				      || !strcmp ( highway, "cycleway" )
				      || !strcmp ( highway, "bridleway" )
				      || !strcmp ( highway, "steps" )
				      || !strcmp ( highway, "path" )
				      || !strcmp ( highway, "construction" ) )
				  continue;
				
				
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
	  dv=distance(vegp,item.second.loc);
	  if (dv < dp2)
	  {
	    
	    dp2=dv;
	    legkozelebbiveg=item.second.id;
	  }
	}
	
	std::list<osmium::object_id_type> utvonal;

	{
	  std::map<osmium::object_id_type, double> dstmap;
	  std::map<osmium::object_id_type, osmium::object_id_type> previous;
	  std::set<std::pair<double, osmium::object_id_type>> to_visit;
	  
	  for(auto& np : terkep)
	  {
	    osmium::object_id_type n = np.first;
	    
	    if(n == legkozelebbi)
		dstmap.insert({legkozelebbi, 0.0});
	    else
		dstmap.insert({n, INFINITY});
	  }
	
	  to_visit.insert({0.0, legkozelebbi});
	  
	  while(!to_visit.empty())
	  {
	    osmium::object_id_type u = to_visit.begin()->second;
	    double u_dst = to_visit.begin()->first;
	    to_visit.erase(to_visit.begin());
	   
	    
	    for(auto& v: terkep[u].szomszedok)
	    {
	      double cost = osmium::geom::haversine::distance(terkep[u].loc, terkep[v].loc);
	      double dst = u_dst + cost;
	     
	      if(dst < dstmap[v])
	      {
		if(dstmap[v] != INFINITY)
		  to_visit.erase({dstmap[v], v});
		
		dstmap[v] = dst;
		previous.insert({v,u});
		previous[v] = u;
		
		to_visit.insert({dst, v});
	      }
	    }

	  }
	  
	  osmium::object_id_type at = legkozelebbiveg;
	  for(osmium::object_id_type at=legkozelebbiveg; at!=0; at = previous[at])
	  {
	     //cout << terkep[at].loc.lat() << " " << terkep[at].loc.lon() <<std::endl;
	     utvonal.push_front(at);
	  }
	  for (auto& it:utvonal)
	    std::cout<<terkep[it].loc.lat()<<" "<<terkep[it].loc.lon()<<std::endl;

	}
	
	return 0;
}
