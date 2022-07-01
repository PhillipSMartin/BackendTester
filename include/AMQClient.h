#pragma once

#include <unordered_map>

#include "activemq.h"
#include "AMQTopic.h"
#include "Logger.h"

class AMQClient : public ExceptionListener, 
    public DefaultTransportListener
{
    private:
        ActiveMQConnection* pConnection_;
        ActiveMQSession* pSession_;
        bool started_ = false;

        Logger* pLogger_;
        std::unordered_map<std::string, AMQTopic*> topics_;
    
    public:
        AMQClient(Logger* pLogger);

        void start();
        void stop();
        void send_text_message( std::string const& topicName, std::string const& textMessage );
        void subscribe( std::string const& topicName );

        virtual void onException( const CMSException& e );
        virtual void transportInterrupted(); 
        virtual void transportResumed(); 

    private:
        AMQTopic* get_topic_ptr( std::string const& topicName );
};