#include "activemq.h"
#include "AMQClient.h"
#include "JsonFormatter.h"
#include "Logger.h"
#include "Parameters.h"

#include <iostream>
#include <memory>


int main(int argc, char** argv) 
{
     
    // build Parameters instance from command-line arguments
    auto _pParms = std::make_shared<Parameters>();
    if (0 != _pParms->ParseArguments(argc, argv))
    {
        exit(0);
    }

    // start Logger
    auto _pLogger = std::make_shared<Logger>(_pParms);
    _pLogger->Info("Backend Tester started");

    std::string _testString = "{table:{auction:[1H,P,1N,P,P,P],board:{cards:[0,0,0,0,0],id:[],number:0}}}";
    auto _pFormattedString = JsonFormatter::format(_testString);
    for (std::string const& s : *_pFormattedString)
    {
        _pLogger->Info(s);
    }
 //   exit(0);


    // start ActiveMQ session
    AMQClient _session(_pLogger);
//  auto _tableEventProducer( _session.createProducer("Event.tb=lh-test-1") );
//  auto _tourneyStateProducer( _session.createProducer("State.tm=lh-test-0") );

 //   auto tableState( _session.get_session_ptr()->createTopic( "State.tb=lh-test-1") );

    _session.subscribe("State.tb=lh-test-1");

    // start the connection
    _session.start();

#if 0
 
    // use session to create one consumer
    auto tableStateConsumer( _session.get_session_ptr()->createConsumer( tableState ) );
    AMQTopicListener tl(tableState->getTopicName(), _pLogger);
    tableStateConsumer->setMessageListener(static_cast<MessageListener*>(&tl));
    _pLogger->Debug("Created Consumer for " + tableState->getTopicName());

   // use session to create a message
    auto textMessage( _session.get_session_ptr()->createTextMessage() );
    _pLogger->Debug("Created text message");

    // set payload
    textMessage->setText("text exiting");
#endif

    // wait to receive message
    // will send message just before quitting
    std::cout << "Press 'q' to quit" << std::endl;    
    while( std::cin.get() != 'q') {}

    // send the message and delete it
    _session.send_text_message("Event.tb=lh-test-1", "Test text message");
 //   _tableEventProducer->send( textMessage );
  //  _pLogger->Debug("Sent message");
  //  delete textMessage;

    // clean up
    /*
    if (connection != NULL)
    {
       delete tableEventProducer;
       delete tourneyStateProducer;
       delete tableStateConsumer;
       delete tableEvent;
       delete tourneyState;
       delete tableState;
    } 
    activemq::library::ActiveMQCPP::shutdownLibrary();
    _pLogger->Debug("Shut down library"); */
    _session.stop();
    _pLogger->Info("Backend Tester exited");
}