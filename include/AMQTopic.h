#pragma once

#include "activemq.h"
#include "Logger.h"

#include <memory>
#include <string>

class AMQTopic : public MessageListener 
{
    #define BUFFERLEN 512

    private:
        ActiveMQSession* pSession_; // AMQClient is responsible for deletion
        cms::Topic* pTopic_; 
        std::shared_ptr<Logger> pLogger_;
        cms::MessageProducer* pProducer_ = nullptr;
        cms::MessageConsumer* pConsumer_ = nullptr;
        int messageCount_  = 0;

    public:
        AMQTopic(std::string const& topicName,
            ActiveMQSession* const& pSession,
            std::shared_ptr<Logger> const& pLogger);

        std::string get_topic_name() const 
        { 
            return pTopic_->getTopicName(); 
        }
        void send_text_message(std::string const& textMessage);
        void subscribe();
        void stop();

        virtual void onMessage( const Message* message );
        
    private:
        cms::MessageProducer* get_producer_ptr();
};