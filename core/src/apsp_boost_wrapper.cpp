/*
 * Shortest path algorithm for PostgreSQL
 *
 * Copyright (c) 2011 Jay Mahadeokar
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 *
 */

#include <boost/lexical_cast.hpp>
#include <boost/config.hpp>

#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/floyd_warshall_shortest.hpp>
#include "string"
#include "apsp.h"
#include "set"
#include "map"
using namespace std;
using namespace boost;


// Maximal number of nodes in the path (to avoid infinite loops)
#define MAX_NODES 100000000




typedef adjacency_list<vecS, vecS, directedS, no_property,property<edge_weight_t, double, property<edge_weight2_t, double> > > Graph;

int 
boost_apsp(edge_t *edges, unsigned int edge_count, const int node_cnt,   //TODO deal with this later - node_count
	       bool directed, bool has_reverse_cost,
	       apsp_element_t **pair, int *pair_count, char **err_msg)
{
	string msg;
	msg.append("\nStarting with boost_apsp");
	msg.append("\nEdge count is: ");
	msg.append(boost::lexical_cast<std::string>(edge_count));
	
	set<int> vertices;
	typedef std::pair<int,int> Edge;
	map<int,int> vertex_map;
	map<int,int> echo_map;
	int map_counter = 0;

	//cout<<edge_count;
	Edge *edge_array = (Edge *) malloc(sizeof(Edge) * (edge_count));   
	//int *weights = (int*) malloc(sizeof(int) * (edge_count));
	double *weights = (double*) malloc(sizeof(double) * (edge_count));
	
	
	for(int i=0;i<edge_count;i++)
	{
		//edge_array[i].first = edges[i].source;  
		//edge_array[i].second = edges[i].target;
		//weights[i] = (int)(edges[i].cost * 10000);
		weights[i] = (edges[i].cost);
		vertices.insert(edges[i].source);
		vertices.insert(edges[i].target);
		
		if(vertex_map.find(edges[i].source) == vertex_map.end())
		{
		  vertex_map[edges[i].source] = map_counter;
		  echo_map[map_counter++] = edges[i].source;
		}
		if(vertex_map.find(edges[i].target) == vertex_map.end())
		{
		  vertex_map[edges[i].target] = map_counter;
		  echo_map[map_counter++] = edges[i].target;
		}
		
		edge_array[i].first = vertex_map.find(edges[i].source)->second;
		edge_array[i].second = vertex_map.find(edges[i].target)->second;
	
	}
	
	
	
	msg.append("\nPopulated edge_array and vertices");
	//*err_msg = (char*)msg.c_str();
	/*for(int i=0;i<edge_count;i++)
	{
	    cout<<edge_array[i].first<<" "<<edge_array[i].second<<"\t";
	    cout<<weights[i]<<endl;
	}*/
	int node_count = vertices.size();
	
	msg.append("\nNode_Count is: ");
	msg.append(boost::lexical_cast<std::string>(node_count));
	
	
	Graph g(edge_array, edge_array + edge_count, weights, node_count);
	
	std::vector<double> d(node_count, std::numeric_limits<double>::max());
	double ** D = (double**) malloc(sizeof(double*) * node_count);
	//std::vector<int> d(node_count, std::numeric_limits<int>::max());
	//float8 ** D = (float8**) malloc(sizeof(float8*) * node_count);
	
	
	for(int i = 0; i < node_count; i++)
		{
		//D[i] = (int*)malloc(node_count * sizeof(int));
		D[i] = (double*)malloc(node_count * sizeof(double));
		if(D[i] == NULL)
			{
			//fprintf(stderr, "out of memory\n");
			msg.append("\nout of memory");
			//*err_msg = (char*)msg.c_str();
			//*err_msg = (char*) "out of memory";
			return -1;
			}
		}


	msg.append("\nNow calling actual boost function");

	floyd_warshall_all_pairs_shortest_paths(g, D, distance_map(&d[0]));

	//msg.append("\nactual boost function call successfull");
	//*err_msg = (char*)msg.c_str();
	
	/*for(int i=0;i<node_count;i++)
	{
		for(int j=0;j<node_count;j++)
		{
			std::cout<<D[i][j]<<" ";
			
		}
		std::cout<<std::endl;
	} */
	
	*pair = (apsp_element_t *) malloc(sizeof(apsp_element_t) * (node_count*node_count));
	//pair_count = (int*)malloc(sizeof(int));
	*pair_count = 0;
	
	graph_traits<Graph>::vertex_iterator vi1, vi_end1, vi2, vi_end2;
	
	tie(vi1,vi_end1)= boost::vertices(g);
	
	for(int i = 0; vi1 != vi_end1; vi1++,i++)
	{
	    
	    tie(vi2,vi_end2) = boost::vertices(g);
	    for(int j = 0; vi2 != vi_end2; vi2++,j++)
	    {
		//(*pair)[*pair_count].src_vertex_id = *vi1;
		//(*pair)[*pair_count].dest_vertex_id = *vi2;
		
		(*pair)[*pair_count].src_vertex_id = echo_map.find(*vi1)->second;
		(*pair)[*pair_count].dest_vertex_id = echo_map.find(*vi2)->second;
		(*pair)[*pair_count].cost = D[i][j];
		(*pair_count)++;
		//cout<<" "<<(*pair_count);
	    }
	}
	//cout<<"Number of pairs: "<<(*pair_count)<<endl;

    return EXIT_SUCCESS;
}
