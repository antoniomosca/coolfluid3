// Copyright (C) 2010 von Karman Institute for Fluid Dynamics, Belgium
//
// This software is distributed under the terms of the
// GNU Lesser General Public License version 3 (LGPLv3).
// See doc/lgpl.txt and doc/gpl.txt for the license text.

#include <QtCore>

#include "rapidxml/rapidxml.hpp"

#include "Common/CHistory.hpp"
#include "Common/Core.hpp"

#include "Common/CRoot.hpp"
#include "Common/NotificationQueue.hpp"
#include "Common/XML/Protocol.hpp"

#include "Mesh/CTable.hpp"

#include "Solver/CMethod.hpp"

#include "GUI/Network/ComponentNames.hpp"

#include "GUI/Server/Notifier.hpp"
#include "GUI/Server/ProcessingThread.hpp"

#include "GUI/Server/ServerRoot.hpp"

using namespace std;
using namespace CF::Common;
using namespace CF::Common::XML;
using namespace CF::GUI::Server;
using namespace CF::Mesh;

NotificationQueue * ServerRoot::m_queue;
Notifier * ServerRoot::m_notifier;

ProcessingThread * ServerRoot::m_thread = nullptr;
QMutex ServerRoot::m_mutex;
SignalCatcher * ServerRoot::m_catcher = new SignalCatcher();
XmlDoc::Ptr ServerRoot::m_doc;

void SignalCatcher::finished()
{
  XmlNode nodedoc = Protocol::goto_doc_node(*ServerRoot::m_doc.get());
  SignalFrame frame(nodedoc.content->first_node());

  SignalFrame reply = frame.get_reply();

  if( reply.node.is_valid() )
  {
    ServerRoot::core()->sendSignal(*ServerRoot::m_doc.get());
    ServerRoot::journal()->add_signal( reply );
  }


  delete ServerRoot::m_thread;
  ServerRoot::m_thread = nullptr;
  ServerRoot::m_doc.reset();
  ServerRoot::m_mutex.unlock();

  ServerRoot::m_queue->flush();
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

CRoot::Ptr ServerRoot::root()
{
  static bool created = false;
  CRoot::Ptr root = Core::instance().root();

  if(!created)
  {
    CCore::Ptr core(new CCore());

    m_queue = nullptr;
    m_notifier = nullptr;
    m_thread = nullptr;
    root->add_component(core);

    created = true;

    Component::Ptr tools = root->get_child("Tools");

    tools->create_component<CJournal>("Journal")->mark_basic();
    tools->create_component<CHistory>("History")->mark_basic();

    CTable::Ptr table = tools->create_component<CTable>("MyTable");

    table->mark_basic();
  }

  return root;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void ServerRoot::processSignal(const string & target,
                               const URI & receiver,
                               const string & clientid,
                               const string & frameid,
                               Signal::arg_t & signal)
{
  if(m_mutex.tryLock())
  {
    m_doc.swap(signal.xml_doc);
    Component::Ptr receivingCompo = root()->access_component(receiver);

    m_thread = new ProcessingThread(signal, target, receivingCompo);
    QObject::connect(m_thread, SIGNAL(finished()), m_catcher, SLOT(finished()));
    journal()->add_signal( signal );
    m_thread->start();
  }
  else
  {
    try
    {
      Component::Ptr comp = root()->access_component(receiver);

      if( comp->signal(target).is_read_only )
      {
        comp->call_signal(target, signal );
        journal()->add_signal(signal);

        SignalFrame reply = signal.get_reply();

        if( reply.node.is_valid() )
        {
          core()->sendSignal(*m_doc.get());
          journal()->add_signal( reply );
        }
      }
      else
        core()->sendFrameRejected(clientid, frameid, SERVER_CORE_PATH, "Server is busy.");
    }
    catch(CF::Common::Exception & cfe)
    {
      core()->sendException(cfe.what());
    }
    catch(std::exception & stde)
    {
      core()->sendException(stde.what());
    }
    catch(...)
    {
      CFerror << "Unknown exception thrown during execution of action [" << target
          << "] on component " << " [" << receiver.path() << "]." << CFendl;
    }
  }
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

CCore::Ptr ServerRoot::core()
{
  return root()->access_component<CCore>(SERVER_CORE_PATH);
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

CJournal::Ptr ServerRoot::journal()
{
  return root()->access_component<CJournal>(SERVER_JOURNAL_PATH);
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void ServerRoot::listenToEvents()
{
  if(m_queue == nullptr)
  {
    m_queue = new NotificationQueue(root());
    m_notifier = new Notifier(m_queue);

    m_notifier->listenToEvent("tree_updated", true);

    QObject::connect(m_notifier, SIGNAL(eventOccured(std::string,CF::Common::URI)),
                     core().get(), SLOT(newEvent(std::string,CF::Common::URI)));
  }
}
