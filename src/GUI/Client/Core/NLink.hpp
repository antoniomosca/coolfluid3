// Copyright (C) 2010 von Karman Institute for Fluid Dynamics, Belgium
//
// This software is distributed under the terms of the
// GNU Lesser General Public License version 3 (LGPLv3).
// See doc/lgpl.txt and doc/gpl.txt for the license text.

#ifndef CF_GUI_Client_NLink_hpp
#define CF_GUI_Client_NLink_hpp

//////////////////////////////////////////////////////////////////////////////

#include <QObject>

#include "GUI/Client/Core/CNode.hpp"

//////////////////////////////////////////////////////////////////////////////

namespace CF {

namespace Common { class CPath; }

namespace GUI {
namespace ClientCore {

  ////////////////////////////////////////////////////////////////////////////

  /// @brief Client corresponding component for @c CF::Common::CLink.
  class ClientCore_API NLink :
      public QObject,
      public CNode
  {
    Q_OBJECT

  public:

    typedef boost::shared_ptr<NLink> Ptr;
    typedef boost::shared_ptr<NLink const> ConstPtr;

    /// @brief Constructor
    /// @param name Link name
    /// @param targetPath Target path
    NLink(const QString & name);

    /// @brief Gives the icon associated to this node
    /// @return Returns the icon associated to this node
    /// @note This method should be reimplemented by all subclasses.
    virtual QIcon getIcon() const;

    /// @brief Gives the text to put on a tool tip
    /// @return The name of the class.
    virtual QString getToolTip() const;

    /// @brief Gives the target path
    /// @return Returns the target path.
    CF::Common::CPath getTargetPath() const;

    void setTargetPath(const CF::Common::CPath & path);

    void setTargetNode(const CNode::Ptr & node);

  public slots:

    /// @brief Slot called when user wants to change the target path
    void changeTarget();

    /// @brief Slot called when user wants to switch to the target
    void goToTarget();

  private :

    /// @brief Target path
    CNode::Ptr m_target;

    /// regists all the signals declared in this class
    static void regist_signals ( Component* self ) {}

    void change_link(CF::Common::XmlNode & node);

  }; // class NLink

//////////////////////////////////////////////////////////////////////////////

} // namespace ClientCore
} // namespace GUI
} // namespace CF

//////////////////////////////////////////////////////////////////////////////

#endif // CF_GUI_Client_NLink_hpp