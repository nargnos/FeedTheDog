
# ifndef PUBSUBCOMMANDS_H_
# define PUBSUBCOMMANDS_H_
# include "Util.h"
namespace PubsubCommands
{

	// PSUBSCRIBE pattern [pattern ...]
	// Listen for messages published to channels matching the given patterns
	void Psubscribe(std::ostream& out, const std::string& pattern,
		const std::vector<std::string>& patterns = {})
	{
		GenCmdWithSize(out, 2 + patterns.size(), "PSUBSCRIBE", pattern, patterns);
	}

	// PUBSUB subcommand [argument [argument ...]]
	// Inspect the state of the Pub/Sub subsystem
	void Pubsub(std::ostream& out, const std::string& subcommand,
		const std::vector<std::string>& arguments = {})
	{
		GenCmdWithSize(out, 2 + arguments.size(), "PUBSUB", subcommand, arguments);
	}

	// PUBLISH channel message
	// Post a message to a channel
	void Publish(std::ostream& out, const std::string& channel, const std::string& message)
	{
		GenCmd(out, "PUBLISH", channel, message);
	}



	// PUBLISH channel message
	// Post a message to a channel
	void Publish(std::ostream& out, const std::string& channel, const std::vector<char>& message)
	{
		GenCmd(out, "PUBLISH", channel, message);
	}

	// PUNSUBSCRIBE [pattern [pattern ...]]
	// Stop listening for messages posted to channels matching the given patterns
	void Punsubscribe(std::ostream& out,
		const std::vector<std::string>& patterns = {})
	{
		GenCmdWithSize(out, 1 + patterns.size(), "PUNSUBSCRIBE", patterns);
	}

	// SUBSCRIBE channel [channel ...]
	// Listen for messages published to the given channels
	void Subscribe(std::ostream& out, const std::string& channel,
		const std::vector<std::string>& channels = {})
	{
		GenCmdWithSize(out, 2 + channels.size(), "SUBSCRIBE", channel, channels);
	}

	// UNSUBSCRIBE [channel [channel ...]]
	// Stop listening for messages posted to the given channels
	void Unsubscribe(std::ostream& out,
		const std::vector<std::string>& channels = {})
	{
		GenCmdWithSize(out, 1 + channels.size(), "UNSUBSCRIBE", channels);
	}

} // namespace PubsubCommands

# endif // PUBSUBCOMMANDS_H_
