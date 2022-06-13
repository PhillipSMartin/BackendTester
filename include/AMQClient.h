#pragma once

#include "activemq.h"
#include "AMQTopic.h"
#include "Logger.h"

#include <unordered_map>
#include <memory>

class AMQClient : public ExceptionListener, 
    public DefaultTransportListener
{
    #define BUFFERLEN 512

    private:
        ActiveMQConnection* pConnection_;
        ActiveMQSession* pSession_;
        bool started_ = false;

        std::shared_ptr<Logger> pLogger_;
        std::unordered_map<std::string, std::shared_ptr<AMQTopic>> topics_;
    
    public:
        AMQClient(std::shared_ptr<Logger> const& pLogger);

        void start();
        void stop();
        void send_text_message(std::string const& topicName, 
            std::string const& textMessage);
        void subscribe(std::string const& topicName);

        virtual void onException( const CMSException& e);
        virtual void transportInterrupted(); 
        virtual void transportResumed(); 

    private:
        std::shared_ptr<AMQTopic> get_topic_ptr(std::string const& topicName);
};