#include "AMQTopic.h"
#include "JsonFormatter.h"

AMQTopic::AMQTopic(std::string const& topicName, 
        ActiveMQSession* const& pSession, 
        Logger* pLogger) : 
    pTopic_(pSession->createTopic(topicName)), 
    pSession_(pSession),
    pLogger_(pLogger) 
{
    gchar* _buffer = g_strdup_printf( "Created topic [%s]", get_topic_name().c_str() );
    pLogger_->Info( _buffer );
    g_free( _buffer );
}

void AMQTopic::stop() 
{ 
    if (pProducer_ != nullptr) 
    {
        delete pProducer_; 
    }
    if (pConsumer_ != nullptr) 
    {
        delete pConsumer_; 
    }
   delete pTopic_; 
}

void AMQTopic::send_text_message( std::string const& textMessage ) 
{
    auto _textMessage = pSession_->createTextMessage();
    _textMessage->setText( textMessage );
    get_producer_ptr()->send( _textMessage );

    gchar* _buffer = g_strdup_printf( "Sent text message for topic [%s]: %s", get_topic_name().c_str(), textMessage.c_str() );
    pLogger_->Info( _buffer );
    g_free( _buffer );

    delete _textMessage;
}

void AMQTopic::subscribe()
{
    gchar* _buffer = NULL; 
    if ( pConsumer_ == nullptr )
    {
        pConsumer_ = pSession_->createConsumer( pTopic_ );
        pConsumer_->setMessageListener( static_cast<MessageListener*>(this) );
        
        _buffer = g_strdup_printf( "Subscribed to topic [%s]", get_topic_name().c_str() );
        pLogger_->Info( _buffer );
    }
    else
    {
        _buffer = g_strdup_printf( "Already subscribed to topic [%s]", get_topic_name().c_str() );
        pLogger_->Info( _buffer );
    }
    g_free( _buffer );
}

cms::MessageProducer* AMQTopic::get_producer_ptr()
{
    if ( pProducer_ == nullptr )
    {
        pProducer_ = pSession_->createProducer( pTopic_ );
        pProducer_->setDeliveryMode( DeliveryMode::NON_PERSISTENT );

        gchar* _buffer = g_strdup_printf( "Created producer for topic [%s]", get_topic_name().c_str() );
        pLogger_->Info( _buffer );
        g_free( _buffer );
    }

    return pProducer_;
}

void AMQTopic::onMessage( const Message* message ) 
{ 
    gchar* _buffer = NULL;
  
    try        
    {   
        messageCount_++;

        const TextMessage* _textMessage = static_cast< const TextMessage* >( message );
        if( _textMessage != NULL ) 
        {  
            _buffer = g_strdup_printf( "[%s] Message #%d received:", get_topic_name().c_str(), messageCount_ );
            pLogger_->Info( _buffer );
            pLogger_->Info( _textMessage->getText() );
        } 
        else 
        {                
            _buffer = g_strdup_printf( "[%s] Message #%d is not a text message", get_topic_name().c_str(), messageCount_ );
            pLogger_->Error( _buffer );          
        }
        
        message->acknowledge();  
    }
    catch (CMSException& e) 
    {
        _buffer = g_strdup_printf( "[%s] Error reading message #%d: %s", get_topic_name().c_str(), messageCount_, e.what() );
        pLogger_->Error( _buffer );
    } 

    g_free( _buffer );  
}