#include "AMQTopic.h"
#include "JsonFormatter.h"

AMQTopic::AMQTopic(std::string const& topicName, 
        ActiveMQSession* const& pSession, 
        std::shared_ptr<Logger> const& pLogger) : 
    pTopic_(pSession->createTopic(topicName)), 
    pSession_(pSession),
    pLogger_(pLogger) 
    {
        char _buffer[BUFFERLEN];     
        snprintf( _buffer, BUFFERLEN, "Created topic [%s]", get_topic_name().c_str());
        pLogger_->Info(_buffer);
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

void AMQTopic::send_text_message(std::string const& textMessage) 
{
    auto _textMessage = pSession_->createTextMessage();
    _textMessage->setText(textMessage);
    get_producer_ptr()->send(_textMessage);

    char _buffer[BUFFERLEN];     
    snprintf( _buffer, BUFFERLEN, "Sent text message for topic [%s]: %s", get_topic_name().c_str(), textMessage.c_str());
    pLogger_->Info(_buffer);

    delete _textMessage;
}

void AMQTopic::subscribe()
{
    char _buffer[BUFFERLEN]; 
    if (pConsumer_ == nullptr)
    {
        pConsumer_ = pSession_->createConsumer(pTopic_);
        pConsumer_->setMessageListener(static_cast<MessageListener*>(this));
        
        snprintf( _buffer, BUFFERLEN, "Subscribed to topic [%s]", get_topic_name().c_str());
        pLogger_->Info(_buffer);
    }
    else
    {
        snprintf( _buffer, BUFFERLEN, "Already subscribed to topic [%s]", get_topic_name().c_str());
        pLogger_->Info(_buffer);
    }
}

cms::MessageProducer* AMQTopic::get_producer_ptr()
{
    if (pProducer_ == nullptr)
    {
        pProducer_ = pSession_->createProducer(pTopic_);
        pProducer_->setDeliveryMode( DeliveryMode::NON_PERSISTENT );

        char _buffer[BUFFERLEN]; 
        snprintf( _buffer, BUFFERLEN, "Created producer for topic [%s]", get_topic_name().c_str());
        pLogger_->Info(_buffer);
    }

    return pProducer_;
}

void AMQTopic::onMessage( const Message* message ) 
{ 
    char _buffer[BUFFERLEN];  
  
    try        
    {   
        messageCount_++;

        const TextMessage* _textMessage = static_cast< const TextMessage* >( message );
        if( _textMessage != NULL ) 
        {  
            snprintf(_buffer, BUFFERLEN, "[%s] Message #%d received:", get_topic_name().c_str(), messageCount_);
            pLogger_->Info(_buffer);
            pLogger_->Info(_textMessage->getText());
//            auto _formattedString = JsonFormatter::format(_textMessage->getText());
//            pLogger_->Info(_formattedString);
        } 
        else 
        {                
            snprintf( _buffer, BUFFERLEN, "[%s] Message #%d is not a text message", get_topic_name().c_str(), messageCount_);
            pLogger_->Error(_buffer);          
        }
        
        message->acknowledge();  
    }
    catch (CMSException& e) 
    {
        snprintf( _buffer, BUFFERLEN, "[%s] Error reading message #%d: %s", get_topic_name().c_str(), messageCount_, e.what());
        pLogger_->Error(_buffer);
    }   
}