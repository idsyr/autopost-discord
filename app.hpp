#include <string>
#include <any>

template < class ConcreteExecutor > class Application {
  void send_message( std::string msg, std::any channel_descriptor );
  void upload_file( std::string file_name, std::string msg, std::any channel_descriptor );

public:
  void on_message_reaction( std::string msg, std::string msg_username, std::any channel_descriptor );
};

template < typename ConcreteExecutor >
void Application< ConcreteExecutor >::send_message( std::string msg, std::any channel_descriptor ) {
  static_cast< ConcreteExecutor * >( this )->send_message( msg, channel_descriptor );
}

template < typename ConcreteExecutor >
void Application< ConcreteExecutor >::upload_file( std::string file_name, std::string msg, std::any channel_descriptor ) {
  static_cast< ConcreteExecutor * >( this )->upload_file( msg, channel_descriptor );
}

template < typename ConcreteExecutor >
void Application< ConcreteExecutor >::on_message_reaction( std::string msg, std::string msg_username,
                                                           std::any channel_descriptor ) {
  if ( msg.rfind( "say_hello;", 0 ) == 0 ) {
    Application::send_message( "hello " + msg_username, channel_descriptor );
  }
}
