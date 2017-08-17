
# ifndef CLUSTERCOMMANDS_H_
# define CLUSTERCOMMANDS_H_
# include "Util.h"
namespace ClusterCommands
{
	// CLUSTER ADDSLOTS slot [slot ...]
	// Assign new hash slots to receiving node
	void ClusterAddslots(std::ostream& out, int slot, const std::vector<int>& slots = {})
	{
		GenCmdWithSize(out, 3 + slots.size(), "CLUSTER", "ADDSLOTS", slot, slots);
	}

	// CLUSTER COUNT-FAILURE-REPORTS node-id
	// Return the number of failure reports active for a given node
	void ClusterCountFailureReports(std::ostream& out, const std::string& nodeId)
	{
		GenCmd(out, "CLUSTER", "COUNT-FAILURE-REPORTS", nodeId);
	}



	// CLUSTER COUNT-FAILURE-REPORTS node-id
	// Return the number of failure reports active for a given node
	void ClusterCountFailureReports(std::ostream& out, const std::vector<char>& nodeId)
	{
		GenCmd(out, "CLUSTER", "COUNT-FAILURE-REPORTS", nodeId);
	}

	// CLUSTER COUNTKEYSINSLOT slot
	// Return the number of local keys in the specified hash slot
	void ClusterCountkeysinslot(std::ostream& out, int slot)
	{
		GenCmd(out, "CLUSTER", "COUNTKEYSINSLOT", slot);
	}

	// CLUSTER DELSLOTS slot [slot ...]
	// Set hash slots as unbound in receiving node
	void ClusterDelslots(std::ostream& out, int slot, const std::vector<int>& slots = {})
	{
		GenCmdWithSize(out, 3 + slots.size(), "CLUSTER", "DELSLOTS", slot, slots);

	}

	// CLUSTER FAILOVER [FORCE|TAKEOVER]
	// Forces a slave to perform a manual failover of its master.
	void ClusterFailover(std::ostream& out)
	{
		GenCmd(out, "CLUSTER", "FAILOVER");
	}

	// CLUSTER FAILOVER [FORCE|TAKEOVER]
	// Forces a slave to perform a manual failover of its master.
	void ClusterFailover(std::ostream& out, FailOverModes mode)
	{
		GenCmd(out, "CLUSTER", "FAILOVER", mode);
	}
	// CLUSTER FORGET node-id
	// Remove a node from the nodes table
	void ClusterForget(std::ostream& out, const std::string& nodeId)
	{
		GenCmd(out, "CLUSTER", "FORGET", nodeId);
	}



	// CLUSTER FORGET node-id
	// Remove a node from the nodes table
	void ClusterForget(std::ostream& out, const std::vector<char>& nodeId)
	{
		GenCmd(out, "CLUSTER", "FORGET", nodeId);
	}

	// CLUSTER GETKEYSINSLOT slot count
	// Return local key names in the specified hash slot
	void ClusterGetkeysinslot(std::ostream& out, int slot, int count)
	{
		GenCmd(out, "CLUSTER", "GETKEYSINSLOT", slot, count);
	}

	// CLUSTER INFO 
	// Provides info about Redis Cluster node state
	void ClusterInfo(std::ostream& out)
	{
		GenCmd(out, "CLUSTER", "INFO");
	}

	// CLUSTER KEYSLOT key
	// Returns the hash slot of the specified key
	void ClusterKeyslot(std::ostream& out, const std::string& key)
	{
		GenCmd(out, "CLUSTER", "KEYSLOT", key);
	}

	// CLUSTER MEET ip port
	// Force a node cluster to handshake with another node
	void ClusterMeet(std::ostream& out, const std::string& ip, unsigned short port)
	{
		GenCmd(out, "CLUSTER", "MEET", ip, port);
	}

	// CLUSTER NODES 
	// Get Cluster config for the node
	void ClusterNodes(std::ostream& out)
	{
		GenCmd(out, "CLUSTER", "NODES");
	}

	// CLUSTER REPLICATE node-id
	// Reconfigure a node as a slave of the specified master node
	void ClusterReplicate(std::ostream& out, const std::string& nodeId)
	{
		GenCmd(out, "CLUSTER", "REPLICATE", nodeId);
	}



	// CLUSTER REPLICATE node-id
	// Reconfigure a node as a slave of the specified master node
	void ClusterReplicate(std::ostream& out, const std::vector<char>& nodeId)
	{
		GenCmd(out, "CLUSTER", "REPLICATE", nodeId);
	}

	// CLUSTER RESET [HARD|SOFT]
	// Reset a Redis Cluster node
	void ClusterReset(std::ostream& out)
	{
		GenCmd(out, "CLUSTER", "RESET");
	}

	// CLUSTER RESET [HARD|SOFT]
	// Reset a Redis Cluster node
	void ClusterReset(std::ostream& out, ResetModes mode)
	{
		GenCmd(out, "CLUSTER", "RESET", mode);
	}
	// CLUSTER SAVECONFIG 
	// Forces the node to save cluster state on disk
	void ClusterSaveconfig(std::ostream& out)
	{
		GenCmd(out, "CLUSTER", "SAVECONFIG");
	}

	// CLUSTER SET-CONFIG-EPOCH config-epoch
	// Set the configuration epoch in a new node
	void ClusterSetConfigEpoch(std::ostream& out, int configEpoch)
	{
		GenCmd(out, "CLUSTER", "SET-CONFIG-EPOCH", configEpoch);
	}

	// CLUSTER SETSLOT slot IMPORTING|MIGRATING|STABLE|NODE [node-id]
	// Bind a hash slot to a specific node
	void ClusterSetslot(std::ostream& out, int slot, SlotModes mode)
	{
		GenCmd(out, "CLUSTER", "SETSLOT", slot, mode);
	}
	// CLUSTER SETSLOT slot IMPORTING|MIGRATING|STABLE|NODE [node-id]
	// Bind a hash slot to a specific node
	void ClusterSetslot(std::ostream& out, int slot, SlotModes mode, const std::vector<char>& nodeId)
	{
		GenCmd(out, "CLUSTER", "SETSLOT", slot, mode, nodeId);
	}

	// CLUSTER SLAVES node-id
	// List slave nodes of the specified master node
	void ClusterSlaves(std::ostream& out, const std::string& nodeId)
	{
		GenCmd(out, "CLUSTER", "SLAVES", nodeId);
	}



	// CLUSTER SLAVES node-id
	// List slave nodes of the specified master node
	void ClusterSlaves(std::ostream& out, const std::vector<char>& nodeId)
	{
		GenCmd(out, "CLUSTER", "SLAVES", nodeId);
	}

	// CLUSTER SLOTS 
	// Get array of Cluster slot to node mappings
	void ClusterSlots(std::ostream& out)
	{
		GenCmd(out, "CLUSTER", "SLOTS");
	}

	// READONLY 
	// Enables read queries for a connection to a cluster slave node
	void Readonly(std::ostream& out)
	{
		GenCmd(out, "READONLY");
	}

	// READWRITE 
	// Disables read queries for a connection to a cluster slave node
	void Readwrite(std::ostream& out)
	{
		GenCmd(out, "READWRITE");
	}

} // namespace ClusterCommands

# endif // CLUSTERCOMMANDS_H_
