#include <gtk/gtk.h>
#include "AMQClient.h"
  
AMQClient::AMQClient(Logger* pLogger) : pLogger_(pLogger)
{
    // initialize the activeMQ Library
    activemq::library::ActiveMQCPP::initializeLibrary();
  
    // create a connection
    auto _connectionFactory = new ActiveMQConnectionFactory("tcp://localhost:61616");
    pConnection_ = _connectionFactory->createConnection();
    delete _connectionFactory;

    pConnection_->addTransportListener(static_cast<TransportListener*>(this));
    pConnection_->setExceptionListener(static_cast<ExceptionListener*>(this));

    // create a session
    pSession_ = pConnection_->createSession( cms::Session::AUTO_ACKNOWLEDGE );
    pLogger_->Info( "Created ActiveMQ session" );
}


void AMQClient::send_text_message( std::string const& topicName, std::string const& textMessage )
{
    if (started_)
    {
        get_topic_ptr( topicName )->send_text_message( textMessage );
    }
    else
    {
        pLogger_->Error( "Cannot send text message before client is started" );
    }
}

void AMQClient::subscribe( std::string const& topicName )
{
    get_topic_ptr( topicName )->subscribe();
}

AMQTopic* AMQClient::get_topic_ptr( std::string const& topicName )
{
 
    auto _it = topics_.find( topicName );
    if ( _it != topics_.end() )
    {
        return _it->second;
    }
    else
    {
        topics_.emplace( topicName,  new AMQTopic(topicName, pSession_, pLogger_) );
        return topics_[topicName];
    }
}

void AMQClient::start()
{
    pConnection_->start();
    started_ = true;

    pLogger_->Info( "ActiveMQ connection started" );
}

void AMQClient::stop()
{
    started_ = false;
    for ( auto _topic : topics_ )
    {
        _topic.second->stop();
        delete _topic.second;
    }
    topics_.clear();
    delete pSession_;
    delete pConnection_;
    activemq::library::ActiveMQCPP::shutdownLibrary();

    pLogger_->Info( "ActiveMQ connection stopped" );
}

void AMQClient::onException( const CMSException& e ) 
{   
    gchar* _buffer = g_strdup_printf( "Shutting down client. CMS Exception occurred: %s", e.what() ) ;   
    pLogger_->Fatal( _buffer ); 
    g_free( _buffer ) ;      
    exit( 1 );    
}

void AMQClient::transportInterrupted() 
{        
    pLogger_->Info( "The connection's transport has been interrupted" );    
}

void AMQClient::transportResumed() 
{        
    pLogger_->Info( "The connection's transport has been restarted" );    
}
