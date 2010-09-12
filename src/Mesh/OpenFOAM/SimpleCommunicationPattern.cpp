#include "SimpleCommunicationPattern.hpp"

#include "Common/MPI/PEInterface.hpp"

#include "Mesh/CTable.hpp"
#include "Mesh/CMesh.hpp"

namespace CF {
namespace Mesh {
namespace OpenFOAM {

SimpleCommunicationPattern::SimpleCommunicationPattern()  : 
    receive_dist(PEInterface::instance().size()+1, 0),
    send_dist(PEInterface::instance().size()+1, 0)
{
}

void SimpleCommunicationPattern::update_send_lists()
{
  boost::mpi::communicator& world = PEInterface::instance();
  const Uint nb_procs = world.size();
  
  IndicesT receive_counts(nb_procs);
  IndicesT send_counts(nb_procs);
  
  for(Uint i = 0; i != nb_procs; ++i)
    receive_counts[i] = receive_dist[i+1] - receive_dist[i];
  
  boost::mpi::all_to_all(world, receive_counts, send_counts);
  
  for(Uint i = 0; i != nb_procs; ++i)
    send_dist[i+1] = send_dist[i] + send_counts[i];
  
  send_list.resize(send_dist.back());
  
  
  std::vector<boost::mpi::request> reqs;
  reqs.reserve(nb_procs*2);
  for(Uint i = 0; i != nb_procs; ++i)
  {
    reqs.push_back(world.isend(i, 0, &receive_list[receive_dist[i]], receive_dist[i+1]-receive_dist[i]));
    reqs.push_back(world.irecv(i, 0, &send_list[send_dist[i]], send_dist[i+1]-send_dist[i]));
  }
  
  boost::mpi::wait_all(reqs.begin(), reqs.end());
}

void make_receive_lists(const SimpleCommunicationPattern::IndicesT& nodes_dist, CMesh& mesh, SimpleCommunicationPattern& comms_pattern)
{
  std::vector<Uint> missing_nodes;
  const Uint nodes_begin = nodes_dist[PEInterface::instance().rank()];
  const Uint nodes_end = nodes_dist[PEInterface::instance().rank()+1];
  
  // Find the nodes that are not stored locally
  BOOST_FOREACH(const CElements& celements, recursive_range_typed<CElements>(mesh))
  {
    const CTable::ArrayT& connectivity_table = celements.connectivity_table().array();
    BOOST_FOREACH(const CTable::ConstRow row, connectivity_table)
    {
      BOOST_FOREACH(const Uint node_idx, row)
      {
        if(node_idx >= nodes_end || node_idx < nodes_begin)
        {
          missing_nodes.push_back(node_idx);
        }
      }
    }
  }
  
  std::sort(missing_nodes.begin(), missing_nodes.end());
  
  comms_pattern.receive_list.clear();
  comms_pattern.receive_targets.clear();
  
  // set the communications pattern
  const Uint nb_missing_nodes = missing_nodes.size();
  for(Uint i = 0; i != nb_missing_nodes;)
  {
    const Uint node_idx = missing_nodes[i];
    const Uint node_processor = std::upper_bound(nodes_dist.begin(), nodes_dist.end(), node_idx) - 1 - nodes_dist.begin();
    
    comms_pattern.receive_targets.push_back(nodes_end - nodes_begin + comms_pattern.receive_list.size());
    comms_pattern.receive_list.push_back(node_idx - nodes_dist[node_processor]);
    comms_pattern.receive_dist[node_processor+1] = comms_pattern.receive_list.size();
    
    while(i != nb_missing_nodes && node_idx == missing_nodes[i])
    {
      missing_nodes[i] = comms_pattern.receive_targets.back();
      ++i;
    }
  }
  
  // Fill missing receive dists
  for(Uint i = 1; i != comms_pattern.receive_dist.size(); ++i)
  {
    if(comms_pattern.receive_dist[i] < comms_pattern.receive_dist[i-1])
      comms_pattern.receive_dist[i] = comms_pattern.receive_dist[i-1];
  }
  
  // Update connectivity data to point to ghosts where data is missing
  Uint missing_idx = 0;
  BOOST_FOREACH(CElements& celements, recursive_range_typed<CElements>(mesh))
  {
    CTable::ArrayT& connectivity_table = celements.connectivity_table().array();
    BOOST_FOREACH(CTable::Row row, connectivity_table)
    {
      BOOST_FOREACH(Uint& node_idx, row)
      {
        if(node_idx >= nodes_end || node_idx < nodes_begin)
        {
          node_idx = missing_nodes[missing_idx];
          cf_assert(node_idx < (nodes_end - nodes_begin + comms_pattern.receive_list.size()));
          ++missing_idx;
        }
      }
    }
  }
}

std::ostream& operator<<(std::ostream& os, const SimpleCommunicationPattern& pattern)
{
  os << "send dist for rank " << PEInterface::instance().rank() << ":" << "\n";
  for(Uint i = 0; i != (pattern.send_dist.size()-1); ++i)
    os << "  " << pattern.send_dist[i+1] - pattern.send_dist[i] << " to process " << i << "\n";
  
  os << "recv dist for rank " << PEInterface::instance().rank() << ":" << "\n";
  for(Uint i = 0; i != (pattern.receive_dist.size()-1); ++i)
    os << "  " << pattern.receive_dist[i+1] - pattern.receive_dist[i] << " from process " << i << "\n";
  
  return os;
}



} // namespace OpenFOAM
} // namespace Mesh
} // namespace CF