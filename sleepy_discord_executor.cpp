#include <string>

#include "sleepy_discord/sleepy_discord.h"
#include "token.hpp"
#include "app.hpp"

class SleepyDiscordApplicationExecutor : public SleepyDiscord::DiscordClient,
                                         public Application< SleepyDiscordApplicationExecutor > {
  using SleepyDiscord::DiscordClient::DiscordClient;

  void onMessage( SleepyDiscord::Message message ) {
    on_message_reaction( message.content, message.author.username, message.channelID.number() );
  }

public:
  void send_message( std::string msg, std::any channel_descriptor ) {
    sendMessage( std::any_cast< int64_t >( channel_descriptor ), msg );
  }

  void upload_file( std::string file_name, std::string msg, std::any channel_descriptor ) {
    uploadFile( std::any_cast< int64_t >( channel_descriptor ), file_name, msg );
  }
};

int main() {
  SleepyDiscordApplicationExecutor client( token, SleepyDiscord::USER_CONTROLED_THREADS );
  client.setIntents( SleepyDiscord::Intent::SERVER_MESSAGES, SleepyDiscord::Intent::MESSAGE_CONTENT );
  client.run();
}
