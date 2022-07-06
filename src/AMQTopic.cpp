#include "AMQTopic.h"
#include "JsonFormatter.h"

AMQTopic::AMQTopic(std::string const& topicName, 
        ActiveMQSession* const& pSession, 
        Logger* pLogger) : 
    pTopic_(pSession->createTopic(topicName)), 
    pSession_(pSession),
    pLogger_(pLogger) 
{
    pLogger_->Info( g_strdup_printf( "Created topic [%s]", get_topic_name().c_str() ) );
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

    pLogger_->Info( g_strdup_printf( "Sent text message for topic [%s]: %s", get_topic_name().c_str(), textMessage.c_str() ) );

    delete _textMessage;
}

void AMQTopic::subscribe()
{
    if ( pConsumer_ == nullptr )
    {
        pConsumer_ = pSession_->createConsumer( pTopic_ );
        pConsumer_->setMessageListener( static_cast<MessageListener*>(this) );
        
        pLogger_->Info( g_strdup_printf( "Subscribed to topic [%s]", get_topic_name().c_str() ) );
    }
    else
    {
        pLogger_->Info( g_strdup_printf( "Already subscribed to topic [%s]", get_topic_name().c_str() ) );
    }
}

cms::MessageProducer* AMQTopic::get_producer_ptr()
{
    if ( pProducer_ == nullptr )
    {
        pProducer_ = pSession_->createProducer( pTopic_ );
        pProducer_->setDeliveryMode( DeliveryMode::NON_PERSISTENT );

        pLogger_->Info( g_strdup_printf( "Created producer for topic [%s]", get_topic_name().c_str() ) );
    }

    return pProducer_;
}

void AMQTopic::onMessage( const Message* message ) 
{   
    try        
    {   
        messageCount_++;

        const TextMessage* _textMessage = static_cast< const TextMessage* >( message );
        if( _textMessage != NULL ) 
        {  
            pLogger_->Info( g_strdup_printf( "[%s] Message #%d received:", get_topic_name().c_str(), messageCount_ ) );
            pLogger_->InfoStr( _textMessage->getText() );
        } 
        else 
        {                
            pLogger_->Error( g_strdup_printf( "[%s] Message #%d is not a text message", get_topic_name().c_str(), messageCount_ ) );          
        }
        
        message->acknowledge();  
    }
    catch (CMSException& e) 
    {
        pLogger_->Error( g_strdup_printf( "[%s] Error reading message #%d: %s", get_topic_name().c_str(), messageCount_, e.what() ) );
    } 
 
}