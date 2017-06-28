
#ifndef PUBSUBCOMMANDS_H_
#define PUBSUBCOMMANDS_H_
#include <initializer_list>
#include <string>
#include <ostream>
#include "Util.h"
namespace PubsubCommands
{

    // PSUBSCRIBE pattern [pattern ...]
    // Listen for messages published to channels matching the given patterns
	void Psubscribe(std::ostream& out)
	{
        // TODO: PSUBSCRIBE
		assert(false);
	}

    // PUBSUB subcommand [argument [argument ...]]
    // Inspect the state of the Pub/Sub subsystem
	void Pubsub(std::ostream& out)
	{
        // TODO: PUBSUB
		assert(false);
	}

    // PUBLISH channel message
    // Post a message to a channel
	void Publish(std::ostream& out)
	{
        // TODO: PUBLISH
		assert(false);
	}

    // PUNSUBSCRIBE [pattern [pattern ...]]
    // Stop listening for messages posted to channels matching the given patterns
	void Punsubscribe(std::ostream& out)
	{
        // TODO: PUNSUBSCRIBE
		assert(false);
	}

    // SUBSCRIBE channel [channel ...]
    // Listen for messages published to the given channels
	void Subscribe(std::ostream& out)
	{
        // TODO: SUBSCRIBE
		assert(false);
	}

    // UNSUBSCRIBE [channel [channel ...]]
    // Stop listening for messages posted to the given channels
	void Unsubscribe(std::ostream& out)
	{
        // TODO: UNSUBSCRIBE
		assert(false);
	}

} // namespace PubsubCommands

#endif // PUBSUBCOMMANDS_H_
